#ifndef __SHIRABE_MATERIAL_SERIALIZATION_H__
#define __SHIRABE_MATERIAL_SERIALIZATION_H__

#include <sstream>
#include <optional>
#include <stack>

#include <nlohmann/json.hpp>

#include <log/log.h>
#include <core/enginetypehelper.h>
#include <asset/assetstorage.h>
#include <util/crc32.h>
#include <util/documents/json.h>

namespace engine
{
    namespace asset
    {
        using AssetID_t = uint64_t;
        class IAssetStorage;
    }

    namespace serialization
    {
        using namespace engine::material;

        /**
         * Read a string and convert it to an EShaderStage value (if possible).
         *
         * @param aString
         * @return
         */
        VkPipelineStageFlagBits stageFromString(std::string const &aString);

        /**
         * Read an EShaderStage value and convert it to its string representation.
         *
         * @param aStage
         * @return
         */
        std::string const stageToString(VkPipelineStageFlagBits const &aStage);

        /**
         * Convert pipeline stage flags to shader stage flags
         *
         * @param aFlags Source flags
         * @return       Target flags
         */
        VkShaderStageFlagBits const shaderStageFromPipelineStage(VkPipelineStageFlags const &aFlags);

        template <typename T>
        CEngineResult<T> readMaterialFile(std::string                    const &aLogTag
                                          , Shared<asset::IAssetStorage> const &aAssetStorage
                                          , asset::AssetId_t             const &aAssetUID);

        CEngineResult<SMaterialMasterIndex> readMaterialMasterIndexFile(std::string const &aLogTag, Shared<asset::IAssetStorage> const &aAssetStorage, asset::AssetId_t const &aAssetUID);

        template <typename T>
        CEngineResult<T> readMaterialFile(std::string                    const &aLogTag
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
    }
}

#endif
