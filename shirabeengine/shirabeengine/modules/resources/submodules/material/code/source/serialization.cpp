#include "material/serialization.h"
#include <util/documents/json.h>

namespace engine
{
    namespace serialization
    {
        using namespace engine::documents;
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        VkPipelineStageFlagBits stageFromString(std::string const &aString)
        {
            if("vertex"             == aString) return VkPipelineStageFlagBits::VK_PIPELINE_STAGE_VERTEX_SHADER_BIT                 ;
            if("tess_control_point" == aString) return VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT   ;
            if("tess_evaluation"    == aString) return VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
            if("geometry"           == aString) return VkPipelineStageFlagBits::VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT               ;
            if("fragment"           == aString) return VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT               ;
            if("compute"            == aString) return VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT                ;

            return VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        std::string const stageToString(VkPipelineStageFlagBits const &aStage)
        {
            switch(aStage)
            {
            case VkPipelineStageFlagBits::VK_PIPELINE_STAGE_VERTEX_SHADER_BIT                 : return "vertex";
            case VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT   : return "tess_control_point";
            case VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT: return "tess_evaluation";
            case VkPipelineStageFlagBits::VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT               : return "geometry";
            case VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT               : return "fragment";
            case VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT                : return "compute";
            default:                                                                            return "unknown";
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        VkShaderStageFlagBits const shaderStageFromPipelineStage(VkPipelineStageFlags const &aFlags)
        {
            switch(aFlags)
            {
                case VkPipelineStageFlagBits::VK_PIPELINE_STAGE_VERTEX_SHADER_BIT                 : return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
                case VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT   : return VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
                case VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT: return VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
                case VkPipelineStageFlagBits::VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT               : return VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT;
                case VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT               : return VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
                case VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT                : return VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;
                default:                                                                            return VkShaderStageFlagBits::VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
            }
        }
        //<-----------------------------------------------------------------------------
    }
}
