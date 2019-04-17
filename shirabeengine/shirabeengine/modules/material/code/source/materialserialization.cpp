#include "material/materialserialization.h"

namespace engine
{
    namespace serialization
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        VkPipelineStageFlagBits stageFromString(std::string const &aString)
        {
            if(0 == aString.compare("vertex"))             return VkPipelineStageFlagBits::VK_PIPELINE_STAGE_VERTEX_SHADER_BIT                 ;
            if(0 == aString.compare("tess_control_point")) return VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT   ;
            if(0 == aString.compare("tess_evaluation"))    return VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
            if(0 == aString.compare("geometry"))           return VkPipelineStageFlagBits::VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT               ;
            if(0 == aString.compare("fragment"))           return VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT               ;
            if(0 == aString.compare("compute"))            return VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT                ;

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
    }
}
