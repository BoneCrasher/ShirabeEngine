#include <core/enginetypehelper.h>
#include <core/helpers.h>
#include <asset/assetstorage.h>
#include <resources/resourcedescriptions.h>
#include <resources/resourcetypes.h>
#include <util/crc32.h>
#include <util/documents/json.h>

#include "textures/declaration.h"
#include "textures/loader.h"

namespace engine
{
    namespace textures
    {
        using namespace asset;

        //<-----------------------------------------------------------------------------
        //;---------------------------------------------------------
        CTextureLoader::CTextureLoader()
        {
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CEngineResult<T> readFile(  std::string                  const &aLogTag
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
        CEngineResult<STextureAsset> readTextureFile(std::string const &aLogTag, Shared<asset::IAssetStorage> const &aAssetStorage, asset::AssetId_t const &aAssetUID)
        {
            return readFile<STextureAsset>(aLogTag, aAssetStorage, aAssetUID);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<Shared<CTextureInstance>> CTextureLoader::createInstance(asset::AssetID_t const &aAssetId)
        {
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<> CTextureLoader::destroyInstance(asset::AssetID_t const &aAssetId)
        {
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<STextureAsset> CTextureLoader::loadTextureFile(Shared<asset::IAssetStorage> const &aAssetStorage, engine::asset::AssetID_t const &aAssetId)
        {
            return readTextureFile(logTag(), aAssetStorage, aAssetId);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<Shared<CTextureInstance>> CTextureLoader::loadInstance( Shared<asset::IAssetStorage> const &aAssetStorage
                                                                            , asset::AssetID_t             const &aAssetId)
        {
            CEngineResult<ByteBuffer> data = {};

            //
            // If the material has been loaded already, return it!
            //
            Shared<CTextureInstance> instance = nullptr;

            if(mInstances.end() != mInstances.find(aAssetId))
            {
                instance = mInstances.at(aAssetId);
            }
            else
            {
                auto const [textureFileFetchResult, textureFile] = readTextureFile(logTag(), aAssetStorage, aAssetId);
                {
                    PrintEngineError(textureFileFetchResult, logTag(), "Could not fetch master meta data.");
                    SHIRABE_RETURN_RESULT_ON_ERROR(textureFileFetchResult);
                }

                static uint64_t sInstanceIndex = 0;
                std::string instanceName = fmt::format("{}_instance_{}", textureFile.name, ++sInstanceIndex);

                instance = CTextureInstance::fromAsset(textureFile);
                mInstances[aAssetId] = instance;
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
