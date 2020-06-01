#include <core/enginetypehelper.h>
#include <core/helpers.h>
#include <asset/assetstorage.h>
#include <util/crc32.h>
#include <util/documents/json.h>

#include "asset/material/asset.h"
#include "asset/material/declaration.h"
#include "asset/material/source.h"
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
            AssetID_t materialAssetId = aMaterialInstanceAssetId;

            Shared<CSharedMaterial> sharedMaterial = nullptr;

            if(mSharedMaterials.end() != mSharedMaterials.find(materialAssetId))
            {
                sharedMaterial = mSharedMaterials.at(materialAssetId);
            }
            else
            {
                auto const [assetDataFetchResult, assetData] = readMaterialAsset(logTag(), aAssetStorage, materialAssetId);
                {
                    PrintEngineError(assetDataFetchResult, logTag(), "Could not fetch sharedMaterial asset data.");
                    SHIRABE_RETURN_RESULT_ON_ERROR(assetDataFetchResult)
                }

                sharedMaterial = CSharedMaterial::fromAsset(assetData);
                mSharedMaterials[assetData.uid] = sharedMaterial;
            }

            if(nullptr == sharedMaterial)
            {
                return { EEngineStatus::Error, nullptr };
            }

            static uint64_t sInstanceIndex = 0;
            std::string instanceName = fmt::format("{}_instance_{}", sharedMaterial->name(), ++sInstanceIndex);

            Shared<CMaterialInstance> instance = makeShared<CMaterialInstance>(instanceName, sharedMaterial);
            if(aAutoCreateConfiguration)
            {
                instance->createConfiguration(*sharedMaterial, aIncludeSystemBuffers);
            }

            mMaterialInstances.insert({instanceName, instance });

            return { EEngineStatus::Ok, instance };
        }
        //<-----------------------------------------------------------------------------

    }
}
