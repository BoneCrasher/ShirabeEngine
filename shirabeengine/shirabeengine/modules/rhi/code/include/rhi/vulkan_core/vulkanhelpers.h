#ifndef __SHIRABE_VULKAN_HELPERS_H__
#define __SHIRABE_VULKAN_HELPERS_H__

#include <platform/platform.h>
#include <vulkan/vulkan.h>

namespace engine::vulkan
{
    VkShaderStageFlagBits const shaderStageFromPipelineStage(VkPipelineStageFlags const &aFlags);
}

#endif
