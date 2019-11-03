#ifndef __SHIRABE_VULKAN_PIPELINE_RESOURCE_H__
#define __SHIRABE_VULKAN_PIPELINE_RESOURCE_H__

#include <vector>

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <base/declaration.h>

namespace engine
{
    namespace vulkan
    {
        /**
         * The SVulkanTextureResource struct describes the relevant data to deal
         * with textures inside the vulkan API.
         */
        struct SVulkanPipelineResource
        {
        public_members:
            VkPipeline                   pipeline;
            std::vector<VkShaderModule>  shaderModules;
            VkDescriptorPool             descriptorPool;
            std::vector<VkDescriptorSet> descriptorSets;
        };
    }
}

#endif
