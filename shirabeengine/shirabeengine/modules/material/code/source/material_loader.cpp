#include <core/enginetypehelper.h>
#include <core/helpers.h>
#include <asset/assetstorage.h>
#include <resources/resourcedescriptions.h>
#include <resources/resourcetypes.h>
#include <util/crc32.h>

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
        CMaterialLoader::CMaterialLoader()
        {
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CEngineResult<T> readMaterialFile(  std::string                  const &aLogTag
                                          , Shared<asset::IAssetStorage> const &aAssetStorage
                                          , asset::AssetId_t             const &aAssetUID)
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
        CEngineResult<SMaterialMasterIndex> readMaterialMasterIndexFile(std::string const &aLogTag, Shared<asset::IAssetStorage> const &aAssetStorage, asset::AssetId_t const &aAssetUID)
        {
            return readMaterialFile<SMaterialMasterIndex>(aLogTag, aAssetStorage, aAssetUID);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SMaterialInstanceIndex> readMaterialInstanceIndexFile(std::string const &aLogTag, Shared<asset::IAssetStorage> const &aAssetStorage, asset::AssetId_t const &aAssetUID)
        {
            return readMaterialFile<SMaterialInstanceIndex>(aLogTag, aAssetStorage, aAssetUID);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SMaterialSignature> readMaterialSignature(std::string const &aLogTag, Shared<asset::IAssetStorage> const &aAssetStorage, asset::AssetId_t const &aAssetUID)
        {
            return readMaterialFile<SMaterialSignature>(aLogTag, aAssetStorage, aAssetUID);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<CMaterialConfig> readMaterialConfig(std::string const &aLogTag, Shared<asset::IAssetStorage> const &aAssetStorage, asset::AssetId_t const &aAssetUID)
        {
            return readMaterialFile<CMaterialConfig>(aLogTag, aAssetStorage, aAssetUID);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SMaterialMeta> readMaterialMeta(std::string const &aLogTag, Shared<asset::IAssetStorage> const &aAssetStorage, asset::AssetId_t const &aAssetUID)
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
        //
        //<-----------------------------------------------------------------------------
        using MasterMaterialReturn_t = std::tuple<bool, std::string, SMaterialMeta, SMaterialSignature, CMaterialConfig>;

        static
        auto loadMasterMaterialFiles(std::string                                    const &aLogTag,
                                     Shared<asset::IAssetStorage>                   const &aAssetStorage,
                                     Map<asset::AssetID_t, Shared<CMaterialMaster>>       &aMasterMaterialIndex,
                                     asset::AssetID_t                               const &aMasterMaterialAssetId)
            -> MasterMaterialReturn_t
        {
            SHIRABE_UNUSED(aMasterMaterialIndex);

            AssetID_t              const masterIndexId    = aMasterMaterialAssetId; // Needs to be here, since it will be shared across both case-blocks.
            CResult<SMaterialMeta>       masterIndexFetch = {};

            MasterMaterialReturn_t const failureReturnValue = { false, {}, {}, {}, {} };

            auto const [metaDataFetchResult, metaData] = readMaterialMeta(aLogTag, aAssetStorage, masterIndexId);
            {
                PrintEngineError(metaDataFetchResult, aLogTag, "Could not fetch master meta data.");
                SHIRABE_RETURN_VALUE_ON_ERROR(metaDataFetchResult, failureReturnValue);
            }

            auto const [signatureAssetFetchResult, signatureAsset] = aAssetStorage->loadAsset(metaData.signatureAssetUid);
            {
                PrintEngineError(signatureAssetFetchResult, aLogTag, "Could not fetch signature asset data.");
                SHIRABE_RETURN_VALUE_ON_ERROR(signatureAssetFetchResult, failureReturnValue);
            }

            auto const [signatureFetchResult, signature] = readMaterialSignature(aLogTag, aAssetStorage, signatureAsset.id);
            {
                PrintEngineError(signatureFetchResult, aLogTag, "Could not fetch signature data.");
                SHIRABE_RETURN_VALUE_ON_ERROR(signatureFetchResult, failureReturnValue);
            }

            // auto const [configAssetFetchResult, configAsset] = aAssetStorage->loadAsset(metaData.configurationAssetUid);
            // {
            //     PrintEngineError(configAssetFetchResult, aLogTag, "Could not fetch configuration asset data.");
            //     SHIRABE_RETURN_VALUE_ON_ERROR(configAssetFetchResult, failureReturnValue);
            // }

            //auto const [configFetchResult, config] = readMaterialConfig(aLogTag, aAssetStorage, configAsset.id);
            //{
            //    PrintEngineError(configFetchResult, aLogTag, "Could not fetch configuration data.");
            //    SHIRABE_RETURN_VALUE_ON_ERROR(configFetchResult, failureReturnValue);
            //}

            std::string        const  masterName      = metaData.name;
            SMaterialSignature const &masterSignature = signature;
            CMaterialConfig    const &masterConfig    = {}; // config;

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
        CEngineResult<SMaterialMeta> CMaterialLoader::loadMaterialMeta(Shared<asset::IAssetStorage> const &aAssetStorage, engine::asset::AssetID_t const &aMaterialInstanceAssetId)
        {
            return readMaterialMeta(logTag(), aAssetStorage, aMaterialInstanceAssetId);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<Shared<CMaterialInstance>> CMaterialLoader::loadMaterialInstance( std::string                  const &aMaterialInstanceId
                                                                                      , Shared<asset::IAssetStorage> const &aAssetStorage
                                                                                      , asset::AssetID_t             const &aMaterialInstanceAssetId
                                                                                      , bool                                aAutoCreateConfiguration)
        {
            if(mInstantiatedMaterialInstances.end() != mInstantiatedMaterialInstances.find(aMaterialInstanceId))
            {
                return { EEngineStatus::Ok, mInstantiatedMaterialInstances.at(aMaterialInstanceId) };
            }

            return loadMaterialInstance(aAssetStorage, aMaterialInstanceAssetId, aAutoCreateConfiguration);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<Shared<CMaterialInstance>> CMaterialLoader::loadMaterialInstance( Shared<asset::IAssetStorage> const &aAssetStorage
                                                                                      , asset::AssetID_t             const &aMaterialInstanceAssetId
                                                                                      , bool                                aAutoCreateConfiguration)
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
            //
            Shared<CMaterialMaster> master = nullptr;

            if(mInstantiatedMaterialMasters.end() != mInstantiatedMaterialMasters.find(masterIndexId))
            {
                master = mInstantiatedMaterialMasters.at(masterIndexId);
            }
            else
            {
                auto[successful, masterName, masterMeta, masterSignature, masterConfig] = loadMasterMaterialFiles(logTag(), aAssetStorage, mInstantiatedMaterialMasters, masterIndexId);
                {
                    PrintEngineError(not successful, logTag(), "Couldn't fetch master material data.");
                    SHIRABE_RETURN_RESULT_ON_ERROR(not successful);
                }

                master = makeShared<CMaterialMaster>(masterIndexId, masterName, std::move(masterSignature), std::move(masterConfig));
                mInstantiatedMaterialMasters[master->getAssetId()] = master;
            }

            if(nullptr == master)
            {
                return { EEngineStatus::Error, nullptr };
            }

            static uint64_t sInstanceIndex = 0;
            std::string instanceName = fmt::format("{}_instance_{}", master->name(), ++sInstanceIndex);

            Shared<CMaterialInstance> instance = makeShared<CMaterialInstance>(instanceName, master);

            if(aAutoCreateConfiguration)
            {
                instance->createConfiguration();
            }

            mInstantiatedMaterialInstances.insert({ instanceName, instance });

            return { EEngineStatus::Ok, instance };
        }
        //<-----------------------------------------------------------------------------

    }
}
