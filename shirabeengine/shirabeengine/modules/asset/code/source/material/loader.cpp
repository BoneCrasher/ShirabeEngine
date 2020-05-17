#include <core/enginetypehelper.h>
#include <core/helpers.h>
#include <asset/assetstorage.h>
#include <util/crc32.h>
#include <util/documents/json.h>

#include "asset/material/declaration.h"
#include "asset/material/serialization.h"
#include "asset/material/loader.h"

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
            using namespace documents;

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
        CEngineResult<CMaterialConfig> readMaterialConfig(std::string const &aLogTag, Shared<asset::IAssetStorage> const &aAssetStorage, asset::AssetId_t const &aAssetUID)
        {
            return readMaterialFile<CMaterialConfig>(aLogTag, aAssetStorage, aAssetUID);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SMaterialAsset> readMaterialAsset(std::string const &aLogTag, Shared<asset::IAssetStorage> const &aAssetStorage, asset::AssetId_t const &aAssetUID)
        {
            return readMaterialFile<SMaterialAsset>(aLogTag, aAssetStorage, aAssetUID);
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
        using MasterMaterialReturn_t = std::tuple<bool, std::string, SMaterialAsset, CMaterialConfig>;

        static
        auto loadMasterMaterialFiles(std::string                                    const &aLogTag,
                                     Shared<asset::IAssetStorage>                   const &aAssetStorage,
                                     Map<asset::AssetID_t, Shared<CSharedMaterial>>       &aMasterMaterialIndex,
                                     asset::AssetID_t                               const &aMasterMaterialAssetId)
            -> MasterMaterialReturn_t
        {
            SHIRABE_UNUSED(aMasterMaterialIndex);

            AssetID_t              const masterIndexId    = aMasterMaterialAssetId; // Needs to be here, since it will be shared across both case-blocks.
            CResult<SMaterialAsset> masterIndexFetch = {};

            MasterMaterialReturn_t const failureReturnValue = { false, {}, {}, {} };

            auto const [assetDataFetchResult, assetData] = readMaterialAsset(aLogTag, aAssetStorage, masterIndexId);
            {
                PrintEngineError(assetDataFetchResult, aLogTag, "Could not fetch sharedMaterial asset data.");
                SHIRABE_RETURN_VALUE_ON_ERROR(assetDataFetchResult, failureReturnValue);
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

            std::string        const  masterName      = assetData.name;
            CMaterialConfig    const &masterConfig    = {}; // config;

            return { true, masterName, assetData, masterConfig };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<Shared<CMaterialInstance>> CMaterialLoader::createInstance(asset::AssetID_t const &aMasterMaterialAssetId)
        {
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMaterialLoader::destroyInstance(asset::AssetID_t const &aMaterialInstanceAssetId)
        {
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<Shared<CMaterialInstance>> CMaterialLoader::loadInstance( std::string                  const &aMaterialInstanceId
                                                                              , Shared<asset::IAssetStorage> const &aAssetStorage
                                                                              , asset::AssetID_t             const &aMaterialInstanceAssetId
                                                                              , bool                                aAutoCreateConfiguration)
        {
            if(mMaterialInstances.end() != mMaterialInstances.find(aMaterialInstanceId))
            {
                return { EEngineStatus::Ok, mMaterialInstances.at(aMaterialInstanceId) };
            }

            return loadInstance(aAssetStorage, aMaterialInstanceAssetId, aAutoCreateConfiguration);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<Shared<CMaterialInstance>> CMaterialLoader::loadInstance( Shared<asset::IAssetStorage> const &aAssetStorage
                                                                              , asset::AssetID_t             const &aMaterialInstanceAssetId
                                                                              , bool                                aAutoCreateConfiguration
                                                                              , bool                                aIncludeSystemBuffers)
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
            // Fetch sharedMaterial data
            //--------------------------------------------------------------------------------------------------------------------
            AssetID_t masterIndexId = aMaterialInstanceAssetId; // instanceIndexAsset.parent;
            // if(0_uid == masterIndexId)
            // {
            //     return { EEngineStatus::Error };
            // }

            //
            // If the material has been loaded already, return it!
            //
            Shared<CSharedMaterial> master = nullptr;

            if(mSharedMaterials.end() != mSharedMaterials.find(masterIndexId))
            {
                master = mSharedMaterials.at(masterIndexId);
            }
            else
            {
                auto[successful, masterName, masterAsset, masterConfig] = loadMasterMaterialFiles(logTag(), aAssetStorage, mSharedMaterials, masterIndexId);
                {
                    PrintEngineError(not successful, logTag(), "Couldn't fetch sharedMaterial material data.");
                    SHIRABE_RETURN_RESULT_ON_ERROR(not successful);
                }

                master = CSharedMaterial::fromAsset(masterAsset);
                mSharedMaterials[masterAsset.uid] = master;
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
                instance->createConfiguration(*master, aIncludeSystemBuffers);
            }

            mMaterialInstances.insert({instanceName, instance });

            return { EEngineStatus::Ok, instance };
        }
        //<-----------------------------------------------------------------------------

    }
}
