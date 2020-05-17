#include <core/enginetypehelper.h>
#include <core/helpers.h>
#include <asset/assetstorage.h>
#include <util/crc32.h>
#include <util/documents/json.h>

#include "asset/buffers/declaration.h"
#include "asset/buffers/loader.h"

namespace engine
{
    namespace buffers
    {
        using namespace asset;
        using namespace engine::documents;

        //<-----------------------------------------------------------------------------
        CBufferLoader::CBufferLoader()
        {
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CEngineResult<T> readAssetFile(  std::string                  const &aLogTag
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
        CEngineResult<SBufferAsset> readBufferAsset(std::string                  const &aLogTag
                                                  , Shared<asset::IAssetStorage> const &aAssetStorage
                                                  , asset::AssetId_t             const &aAssetUID)
        {
            return readAssetFile<SBufferAsset>(aLogTag, aAssetStorage, aAssetUID);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        AssetID_t getAssetUIDForInstanceID(Map<std::string, AssetID_t> const &aRegistry, std::string const &aInstanceId)
        {
            if(aRegistry.end() == aRegistry.find(aInstanceId))
            {
                return 0_uid;
            }

            return aRegistry.at(aInstanceId);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<Shared<CBufferInstance>> CBufferLoader::createInstance(asset::AssetID_t const &aAssetId)
        {
            SHIRABE_UNUSED(aAssetId);
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<> CBufferLoader::destroyInstance(engine::asset::AssetID_t const &aAssetId)
        {
            SHIRABE_UNUSED(aAssetId);
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<Shared<CBufferInstance>> CBufferLoader::loadInstance( std::string                  const &aInstanceId
                                                                          , Shared<asset::IAssetStorage> const &aAssetStorage
                                                                          , asset::AssetID_t             const &aAssetId)
        {
            if(mInstances.end() != mInstances.find(aAssetId))
            {
                return { EEngineStatus::Ok, mInstances.at(aAssetId) };
            }

            return loadInstance(aAssetStorage, aAssetId);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<Shared<CBufferInstance>> CBufferLoader::loadInstance( Shared<asset::IAssetStorage> const &aAssetStorage
                                                                          , asset::AssetID_t             const &aAssetId)
        {
            CEngineResult<ByteBuffer> data = {};

            //--------------------------------------------------------------------------------------------------------------------
            // Fetch instance data
            //--------------------------------------------------------------------------------------------------------------------
            if(0_uid == aAssetId)
            {
                return { EEngineStatus::Error };
            }

            //
            // If the material has been loaded already, return it!
            //
            Shared<CBufferInstance> instance = nullptr;

            if(mInstances.end() != mInstances.find(aAssetId))
            {
                instance = mInstances.at(aAssetId);
            }
            else
            {

                auto [result, asset] = readBufferAsset(logTag(), aAssetStorage, aAssetId);
                {
                    PrintEngineError(CheckEngineError(result), logTag(), "Couldn't fetch sharedMaterial material data.");
                    SHIRABE_RETURN_RESULT_ON_ERROR(CheckEngineError(result));
                }

                instance = CBufferInstance::fromAsset(asset);
                mInstances[asset.uid] = instance;
            }

            if(nullptr == instance)
            {
                return { EEngineStatus::Error, nullptr };
            }

            return { EEngineStatus::Ok, instance };
        }
        //<-----------------------------------------------------------------------------

    }
}
