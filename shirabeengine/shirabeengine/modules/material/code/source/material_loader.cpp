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
        //;---------------------------------------------------------
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
        CEngineResult<CStdSharedPtr_t<CMaterialInstance>> CMaterialLoader::loadMaterialInstance(asset::AssetID_t const &aMaterialInstanceAssetId)
        {
            CEngineResult<ByteBuffer> data = {};

            AssetID_t instanceAssetId = aMaterialInstanceAssetId;
            if(0_uid == instanceAssetId)
            {
                return { EEngineStatus::Error };
            }

            CEngineResult<SAsset> const assetFetch = mStorage->loadAsset(instanceAssetId);
            if(not assetFetch.successful())
            {
                return { assetFetch.result() };
            }

            //--------------------------------------------------------------------------------------------------------------------
            // Fetch indices
            //--------------------------------------------------------------------------------------------------------------------
            bool processInstance = false;

            AssetID_t                       masterIndexId      = 0; // Needs to be here, since it will be shared across both case-blocks.
            CResult<SMaterialMasterIndex>   masterIndexFetch   = {};
            CResult<SMaterialInstanceIndex> instanceIndexFetch = {};

            SAsset const asset = assetFetch.data();
            //switch(asset.type)
            //{
            //case asset::EAssetType::MaterialInstance:
            //{
            //    processInstance = true;

            // AssetID_t const instanceIndexId  = rootAssetId;

                instanceIndexFetch = readMaterialInstanceIndexFile(logTag(), mStorage.get(), instanceAssetId);
                if(not instanceIndexFetch.successful())
                {
                    return { EEngineStatus::Error };
                }

                // Make sure to fetch master index w/ the corresponding asset UID as well ( fallthrough case ).
                SMaterialInstanceIndex const instanceIndex = instanceIndexFetch.data();

                masterIndexId = asset.parent;
                if(0_uid == masterIndexId)
                {
                    return { EEngineStatus::Error };
                }
            // }
            // [[fallthrough]];
            // case asset::EAssetType::MaterialMaster:
            // {
                masterIndexFetch = readMaterialMasterIndexFile(logTag(), mStorage.get(), masterIndexId);
                if(not masterIndexFetch.successful())
                {
                    return { EEngineStatus::Error };
                }
            //    break;
            // }
            // default:
            //     return { EEngineStatus::Error };
            // }

            //--------------------------------------------------------------------------------------------------------------------
            // Process master
            //--------------------------------------------------------------------------------------------------------------------
            SMaterialMasterIndex const &masterIndex = masterIndexFetch.data();

            // Fetch signature
            CEngineResult<SAsset> signatureAssetFetch  = mStorage->assetFromUri(masterIndex.signatureFilename);
            if(not signatureAssetFetch.successful())
            {
                return { signatureAssetFetch.result() };
            }

            CEngineResult<SAsset> baseConfigAssetFetch = mStorage->assetFromUri(masterIndex.baseConfigurationFilename);
            if(not baseConfigAssetFetch.successful())
            {
                return { baseConfigAssetFetch.result() };
            }

            CResult<SMaterialSignature> signatureFetch = readMaterialSignature(logTag(), mStorage.get(), signatureAssetFetch.data().id);
            if(not signatureFetch.successful())
            {
                return { EEngineStatus::Error };
            }

            CResult<CMaterialConfig> masterConfigFetch = readMaterialConfig(logTag(), mStorage.get(), baseConfigAssetFetch.data().id);
            if(not masterConfigFetch.successful())
            {
                return { EEngineStatus::Error };
            }

            std::string        const  masterName      = masterIndex.name;
            SMaterialSignature const &masterSignature = signatureFetch.data();
            CMaterialConfig    const &masterConfig    = masterConfigFetch.data();
            std::string        const  instanceName    = instanceIndex.name;
            CMaterialConfig          &instanceConfig  = masterConfigFetch.data(); // Not const. Possibly overwritten.

            //--------------------------------------------------------------------------------------------------------------------
            // If we process an instance...
            //--------------------------------------------------------------------------------------------------------------------
            if(processInstance)
            {
                // Fetch instance configuration override
                CEngineResult<SAsset> configAssetFetch = mStorage->assetFromUri(masterIndex.signatureFilename);
                if(not configAssetFetch.successful())
                {
                    return { configAssetFetch.result() };
                }


                CResult<CMaterialConfig> const configFetch = readMaterialConfig(logTag(), mStorage.get(), configAssetFetch.data().id);
                if(not configFetch.successful())
                {
                    return { EEngineStatus::Error };
                }

                // Copy master to instance.
                instanceConfig = masterConfig;
                // And override values.
                CMaterialConfig const &overrideConfig = configFetch.data();
                instanceConfig.override(overrideConfig);
            }

            //--------------------------------------------------------------------------------------------------------------------
            // Create Material instance
            //--------------------------------------------------------------------------------------------------------------------
            CStdSharedPtr_t<CMaterialMaster>   master   = makeCStdSharedPtr<CMaterialMaster>(masterIndexId, masterName, std::move(masterSignature), std::move(masterConfig));
            CStdSharedPtr_t<CMaterialInstance> instance = makeCStdSharedPtr<CMaterialInstance>(instanceAssetId, instanceName, std::move(instanceConfig));


            // CEngineResult<CMaterialLayer *> layerAddition = material->addLayer("Default");
            // if(not layerAddition.successful())
            // {
            //     return { EEngineStatus::Error };
            // }
            // CMaterialLayer *layer = layerAddition.data();
            // layer->assignMaterialInstance(instance);

            return { EEngineStatus::Ok, instance };
        }
        //<-----------------------------------------------------------------------------

    }
}
