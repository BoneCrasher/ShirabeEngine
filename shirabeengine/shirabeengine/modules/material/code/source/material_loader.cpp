#include <core/enginetypehelper.h>
#include <core/helpers.h>
#include <asset/assetstorage.h>

#include "material/material_declaration.h"
#include "material/materialserialization.h"
#include "material/material_loader.h"

namespace engine
{
    namespace material
    {
        using namespace asset;

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CMaterialLoader::CMaterialLoader(CStdSharedPtr_t<asset::IAssetStorage> &aAssetStorage)
            : mStorage(aAssetStorage)
        {
            assert(nullptr != aAssetStorage);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CResult<T> readMaterialFile(std::string const &aLogTag, asset::IAssetStorage *aAssetStorage, asset::AssetId_t const &aAssetUID)
        {
            using namespace serialization;

            CEngineResult<ByteBuffer> const dataFetch = aAssetStorage->loadAssetData(aAssetUID);
            if(not dataFetch.successful())
            {
                return { false };
            }

            std::string rawInput(reinterpret_cast<char const *>(dataFetch.data().data()),
                                                                dataFetch.data().size());

            CJSONDeserializer<T> deserializer {};
            deserializer.initialize();

            CResult<CStdSharedPtr_t<typename IDeserializer<T>::IResult>> deserialization = deserializer.deserialize(rawInput);
            if(not deserialization.successful())
            {
                CLog::Error(aLogTag, "Could not load material file '%0'", aAssetUID);
            }

            CResult<T> const index = deserialization.data()->asT();

            deserializer.deinitialize();

            return index;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CResult<SMaterialMasterIndex> readMaterialMasterIndexFile(std::string const &aLogTag, asset::IAssetStorage *aAssetStorage, asset::AssetId_t const &aAssetUID)
        {
            return readMaterialFile<SMaterialMasterIndex>(aLogTag, aAssetStorage, aAssetUID);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CResult<SMaterialInstanceIndex> readMaterialInstanceIndexFile(std::string const &aLogTag, asset::IAssetStorage *aAssetStorage, asset::AssetId_t const &aAssetUID)
        {
            return readMaterialFile<SMaterialInstanceIndex>(aLogTag, aAssetStorage, aAssetUID);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CResult<SMaterialSignature> readMaterialSignature(std::string const &aLogTag, asset::IAssetStorage *aAssetStorage, asset::AssetId_t const &aAssetUID)
        {
            return readMaterialFile<SMaterialSignature>(aLogTag, aAssetStorage, aAssetUID);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CResult<CMaterialConfig> readMaterialConfig(std::string const &aLogTag, asset::IAssetStorage *aAssetStorage, asset::AssetId_t const &aAssetUID)
        {
            return readMaterialFile<CMaterialConfig>(aLogTag, aAssetStorage, aAssetUID);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        AssetID_t getAssetUIDForMaterialID(Map<std::string, AssetID_t> const &aRegistry, std::string const &aMaterialId)
        {
            if(aRegistry.end() == aRegistry.find(aMaterialId))
            {
                return 0_uid;
            }

            return aRegistry.at(aMaterialId);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<CStdSharedPtr_t<CMaterial>> CMaterialLoader::loadMaterial(std::string const &aMaterialId)
        {
            CEngineResult<ByteBuffer> data = {};

            AssetID_t rootAssetId = getAssetUIDForMaterialID(mMaterial2AssetMapping, aMaterialId);
            if(0_uid == rootAssetId)
            {
                return { EEngineStatus::Error };
            }

            CEngineResult<SAsset> const assetFetch = mStorage->loadAsset(rootAssetId);
            if(not assetFetch.successful())
            {
                return { assetFetch.result() };
            }

            //--------------------------------------------------------------------------------------------------------------------
            // Fetch indices
            //--------------------------------------------------------------------------------------------------------------------
            bool processInstance = false;

            AssetID_t                       masterIndexId      = rootAssetId; // Needs to be here, since it will be shared across both case-blocks.
            CResult<SMaterialMasterIndex>   masterIndexFetch   = {};
            CResult<SMaterialInstanceIndex> instanceIndexFetch = {};

            SAsset const asset = assetFetch.data();
            switch(asset.type)
            {
            case asset::EAssetType::MaterialInstance:
            {
                processInstance = true;

                AssetID_t const instanceIndexId  = rootAssetId;

                instanceIndexFetch = readMaterialInstanceIndexFile(logTag(), mStorage.get(), instanceIndexId);
                if(not instanceIndexFetch.successful())
                {
                    return { EEngineStatus::Error };
                }

                // Make sure to fetch master index w/ the corresponding asset UID as well ( fallthrough case ).
                SMaterialInstanceIndex const instanceIndex = instanceIndexFetch.data();

                masterIndexId = getAssetUIDForMaterialID(mMaterial2AssetMapping, instanceIndex.masterIndexFilename);
                if(0_uid == masterIndexId)
                {
                    return { EEngineStatus::Error };
                }
            }
            [[fallthrough]];
            case asset::EAssetType::MaterialMaster:
            {
                masterIndexFetch = readMaterialMasterIndexFile(logTag(), mStorage.get(), masterIndexId);
                if(not masterIndexFetch.successful())
                {
                    return { EEngineStatus::Error };
                }
                break;
            }
            default:
                return { EEngineStatus::Error };
            }

            //--------------------------------------------------------------------------------------------------------------------
            // Process master
            //--------------------------------------------------------------------------------------------------------------------
            SMaterialMasterIndex const &masterIndex = masterIndexFetch.data();

            // Fetch signature
            AssetID_t const signatureFileId = getAssetUIDForMaterialID(mMaterial2AssetMapping, masterIndex.signatureFilename);
            if(0_uid == signatureFileId)
            {
                return { EEngineStatus::Error };
            }

            CResult<SMaterialSignature> signatureFetch = readMaterialSignature(logTag(), mStorage.get(), signatureFileId);
            if(not signatureFetch.successful())
            {
                return { EEngineStatus::Error };
            }

            // Fetch base configuration
            AssetID_t const baseConfigFileId = getAssetUIDForMaterialID(mMaterial2AssetMapping, masterIndex.baseConfigurationFilename);
            if(0_uid == baseConfigFileId)
            {
                return { EEngineStatus::Error };
            }

            CResult<CMaterialConfig> baseConfigFetch = readMaterialConfig(logTag(), mStorage.get(), baseConfigFileId);
            if(not baseConfigFetch.successful())
            {
                return { EEngineStatus::Error };
            }

            std::string         name      = masterIndex.name;
            SMaterialSignature &signature = signatureFetch.data();
            CMaterialConfig    &config    = baseConfigFetch.data(); // Not const. Possibly overwritten.

            //--------------------------------------------------------------------------------------------------------------------
            // If we process an instance...
            //--------------------------------------------------------------------------------------------------------------------
            if(processInstance)
            {
                SMaterialInstanceIndex const &instanceIndex = instanceIndexFetch.data();

                name = instanceIndex.name;

                // Fetch instance configuration override
                AssetID_t const configFileId = getAssetUIDForMaterialID(mMaterial2AssetMapping, instanceIndex.configurationFilename);
                if(0_uid == configFileId)
                {
                    return { EEngineStatus::Error };
                }

                CResult<CMaterialConfig> const configFetch = readMaterialConfig(logTag(), mStorage.get(), configFileId);
                if(not configFetch.successful())
                {
                    return { EEngineStatus::Error };
                }

                CMaterialConfig const &overrideConfig = configFetch.data();
                config.override(overrideConfig);
            }

            //--------------------------------------------------------------------------------------------------------------------
            // Create Material instance
            //--------------------------------------------------------------------------------------------------------------------
            CStdSharedPtr_t<CMaterial> material = makeCStdSharedPtr<CMaterial>();

            CEngineResult<CMaterialLayer *> layerAddition = material->addLayer("Default");
            if(not layerAddition.successful())
            {
                return { EEngineStatus::Error };
            }

            CStdSharedPtr_t<CMaterialInstance> instance = makeCStdSharedPtr<CMaterialInstance>(name, std::move(signature), std::move(config));

            CMaterialLayer *layer = layerAddition.data();
            layer->assignMaterialInstance(instance);

            return { EEngineStatus::Ok, material };
        }
        //<-----------------------------------------------------------------------------

    }
}
