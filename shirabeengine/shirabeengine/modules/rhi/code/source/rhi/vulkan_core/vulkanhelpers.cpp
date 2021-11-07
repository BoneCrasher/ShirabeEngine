#include "rhi/vulkan_core/vulkanhelpers.h"

namespace engine::vulkan
{
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
