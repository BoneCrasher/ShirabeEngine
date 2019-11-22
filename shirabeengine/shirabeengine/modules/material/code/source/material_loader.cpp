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
        CMaterialLoader::CMaterialLoader(Shared<asset::IAssetStorage> const &aAssetStorage)
            : mStorage(aAssetStorage)
        {
            assert(nullptr != aAssetStorage);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CEngineResult<T> readMaterialFile(  std::string          const &aLogTag
                                          , asset::IAssetStorage       *aAssetStorage
                                          , asset::AssetId_t     const &aAssetUID)
        {
            using namespace serialization;

            CJSONDeserializer<T> deserializer {};
            deserializer.initialize();

            auto const [dataFetchResult, dataBuffer] = aAssetStorage->loadAssetData(aAssetUID);
            {
                PrintEngineError(dataFetchResult, aLogTag, "Could not load asset data for asset {}", aAssetUID);
                SHIRABE_RETURN_RESULT_ON_ERROR(dataFetchResult);
            }

            std::string rawInput(reinterpret_cast<char const *>(dataBuffer.data()),
                                                                dataBuffer.size());

            auto const [deserializationSuccessful, resultData] = deserializer.deserialize(rawInput);
            {
                PrintEngineError(not deserializationSuccessful, aLogTag, "Could not load material file '{}'", aAssetUID);
                SHIRABE_RETURN_RESULT_ON_ERROR(not deserializationSuccessful);
            }

            CEngineResult<T> const index = { EEngineStatus::Ok, resultData->asT().data() };

            deserializer.deinitialize();

            return index;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SMaterialMasterIndex> readMaterialMasterIndexFile(std::string const &aLogTag, asset::IAssetStorage *aAssetStorage, asset::AssetId_t const &aAssetUID)
        {
            return readMaterialFile<SMaterialMasterIndex>(aLogTag, aAssetStorage, aAssetUID);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SMaterialInstanceIndex> readMaterialInstanceIndexFile(std::string const &aLogTag, asset::IAssetStorage *aAssetStorage, asset::AssetId_t const &aAssetUID)
        {
            return readMaterialFile<SMaterialInstanceIndex>(aLogTag, aAssetStorage, aAssetUID);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SMaterialSignature> readMaterialSignature(std::string const &aLogTag, asset::IAssetStorage *aAssetStorage, asset::AssetId_t const &aAssetUID)
        {
            return readMaterialFile<SMaterialSignature>(aLogTag, aAssetStorage, aAssetUID);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<CMaterialConfig> readMaterialConfig(std::string const &aLogTag, asset::IAssetStorage *aAssetStorage, asset::AssetId_t const &aAssetUID)
        {
            return readMaterialFile<CMaterialConfig>(aLogTag, aAssetStorage, aAssetUID);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SMaterialMeta> readMaterialMeta(std::string const &aLogTag, asset::IAssetStorage *aAssetStorage, asset::AssetId_t const &aAssetUID)
        {
            return readMaterialFile<SMaterialMeta>(aLogTag, aAssetStorage, aAssetUID);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
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
        using MasterMaterialReturn_t = std::tuple<bool, std::string, SMaterialMeta, SMaterialSignature, CMaterialConfig>;

        static
        auto loadMasterMaterial(std::string                                    const &aLogTag,
                                Shared<asset::IAssetStorage>                         &aAssetStorage,
                                Map<asset::AssetID_t, Shared<CMaterialMaster>>       &aMasterMaterialIndex,
                                asset::AssetID_t                               const &aMasterMaterialAssetId)
            -> MasterMaterialReturn_t
        {
            AssetID_t              const masterIndexId    = aMasterMaterialAssetId; // Needs to be here, since it will be shared across both case-blocks.
            CResult<SMaterialMeta>       masterIndexFetch = {};

            MasterMaterialReturn_t const failureReturnValue = { false, {}, {}, {}, {} };

            auto const [metaDataFetchResult, metaData] = readMaterialMeta(aLogTag, aAssetStorage.get(), masterIndexId);
            {
                PrintEngineError(metaDataFetchResult, aLogTag, "Could not fetch master meta data.");
                SHIRABE_RETURN_VALUE_ON_ERROR(metaDataFetchResult, failureReturnValue);
            }

            auto const [signatureAssetFetchResult, signatureAsset] = aAssetStorage->loadAsset(metaData.signatureAssetUid);
            {
                PrintEngineError(signatureAssetFetchResult, aLogTag, "Could not fetch signature asset data.");
                SHIRABE_RETURN_VALUE_ON_ERROR(signatureAssetFetchResult, failureReturnValue);
            }

            auto const [signatureFetchResult, signature] = readMaterialSignature(aLogTag, aAssetStorage.get(), signatureAsset.id);
            {
                PrintEngineError(signatureFetchResult, aLogTag, "Could not fetch signature data.");
                SHIRABE_RETURN_VALUE_ON_ERROR(signatureFetchResult, failureReturnValue);
            }

            auto const [configAssetFetchResult, configAsset] = aAssetStorage->loadAsset(metaData.configurationAssetUid);
            {
                PrintEngineError(configAssetFetchResult, aLogTag, "Could not fetch configuration asset data.");
                SHIRABE_RETURN_VALUE_ON_ERROR(configAssetFetchResult, failureReturnValue);
            }

            auto const [configFetchResult, config] = readMaterialConfig(aLogTag, aAssetStorage.get(), configAsset.id);
            {
                PrintEngineError(configFetchResult, aLogTag, "Could not fetch configuration data.");
                SHIRABE_RETURN_VALUE_ON_ERROR(configFetchResult, failureReturnValue);
            }

            std::string        const  masterName      = metaData.name;
            SMaterialSignature const &masterSignature = signature;
            CMaterialConfig    const &masterConfig    = config;



            return { true, masterName, metaData, masterSignature, masterConfig };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<Shared<CMaterialInstance>> CMaterialLoader::createMaterialInstance(asset::AssetID_t const &aMasterMaterialAssetId)
        {
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMaterialLoader::destroyMaterialInstance(asset::AssetID_t const &aMaterialInstanceAssetId)
        {
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SMaterialMeta> CMaterialLoader::loadMaterialMeta(engine::asset::AssetID_t const &aMaterialInstanceAssetId)
        {
            return readMaterialMeta(logTag(), mStorage.get(), aMaterialInstanceAssetId);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<Shared<CMaterialMaster>> CMaterialLoader::loadMaterialInstance(asset::AssetID_t const &aMaterialInstanceAssetId)
        {
            CEngineResult<ByteBuffer> data = {};

            //--------------------------------------------------------------------------------------------------------------------
            // Fetch instance data
            //--------------------------------------------------------------------------------------------------------------------            
            AssetID_t instanceIndexAssetId = aMaterialInstanceAssetId;
            if(0_uid == instanceIndexAssetId)
            {
                return { EEngineStatus::Error };
            }

            // auto const [instanceIndexAssetFetchResult, instanceIndexAsset] = mStorage->loadAsset(instanceIndexAssetId);
            // {
            //     PrintEngineError(instanceIndexAssetFetchResult, logTag(), "Couldn't fetch material instance index asset.");
            //     SHIRABE_RETURN_RESULT_ON_ERROR(instanceIndexAssetFetchResult);
            // }

            // auto const [instanceIndexFetchResult, instanceIndex] = readMaterialInstanceIndexFile(logTag(), mStorage.get(), instanceIndexAssetId);
            // {
            //     PrintEngineError(instanceIndexFetchResult, logTag(), "Couldn't fetch material instance index for ID {}", instanceIndexAssetId);
            //     SHIRABE_RETURN_RESULT_ON_ERROR(instanceIndexFetchResult);
            // }
//
            // auto const [instanceConfigAssetFetchResult, instanceConfigAsset] = mStorage->loadAsset(instanceIndex.configurationAssetId);
            // {
            //     PrintEngineError(instanceConfigAssetFetchResult, logTag(), "Couldn't fetch instance config asset for ID {}.", instanceIndex.configurationAssetId);
            //     SHIRABE_RETURN_RESULT_ON_ERROR(instanceConfigAssetFetchResult);
            // }
//
            // auto [instanceConfigFetchResult, instanceConfig] = readMaterialConfig(logTag(), mStorage.get(), instanceConfigAsset.id);
            // {
            //     PrintEngineError(instanceConfigFetchResult, logTag(), "Couldn't fetch instance config for ID {}.", instanceConfigAsset.id);
            //     SHIRABE_RETURN_RESULT_ON_ERROR(instanceConfigFetchResult);
            // }
//
            // std::string const  instanceName = instanceIndex.name;

            //--------------------------------------------------------------------------------------------------------------------
            // Fetch master data
            //--------------------------------------------------------------------------------------------------------------------
            AssetID_t masterIndexId = aMaterialInstanceAssetId; // instanceIndexAsset.parent;
            // if(0_uid == masterIndexId)
            // {
            //     return { EEngineStatus::Error };
            // }

            //
            // If the material has been loaded already, return it!
            // TODO: Material reload on filesystem change?
            //
            if(mInstantiatedMaterialMasters.end() != mInstantiatedMaterialMasters.find(masterIndexId))
            {
                return { EEngineStatus::Ok, mInstantiatedMaterialMasters.at(masterIndexId) };
            }

            auto [successful, masterName, masterMeta, masterSignature, masterConfig] = loadMasterMaterial(logTag(), mStorage, mInstantiatedMaterialMasters, masterIndexId);
            {
                PrintEngineError(not successful, logTag(), "Couldn't fetch master material data.");
                SHIRABE_RETURN_RESULT_ON_ERROR(not successful);
            }

            //--------------------------------------------------------------------------------------------------------------------
            // Override instance with master config.
            //--------------------------------------------------------------------------------------------------------------------
            // instanceConfig.override(masterConfig);

            //--------------------------------------------------------------------------------------------------------------------
            // Create Material instance
            //--------------------------------------------------------------------------------------------------------------------
            Shared<CMaterialMaster> master = makeShared<CMaterialMaster>  (masterIndexId, masterName, std::move(masterSignature), std::move(masterConfig));
            // Shared<CMaterialInstance> instance = makeShared<CMaterialInstance>(instanceIndexAssetId, instanceName, std::move(instanceConfig),  master);

            mInstantiatedMaterialMasters  [master->getAssetId()] = master;
            // mInstantiatedMaterialInstances[instance->getAssetId()] = instance;

            return { EEngineStatus::Ok, master };
        }
        //<-----------------------------------------------------------------------------

    }
}
