#ifndef __SHIRABE_MATERIAL_SERIALIZATION_H__
#define __SHIRABE_MATERIAL_SERIALIZATION_H__

#include <sstream>
#include <optional>
#include <stack>

#include <nlohmann/json.hpp>

#include <log/log.h>
#include <core/enginetypehelper.h>

#include "declaration.h"

namespace engine
{
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


    }
}

#endif
