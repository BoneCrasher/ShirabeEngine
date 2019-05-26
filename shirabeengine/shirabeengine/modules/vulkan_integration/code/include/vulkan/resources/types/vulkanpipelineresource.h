#ifndef __SHIRABE_VULKAN_TEXTURE_RESOURCE_H__
#define __SHIRABE_VULKAN_TEXTURE_RESOURCE_H__

#include <vector>

#include <vulkan/vulkan.h>

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
            VkPipeline                  pipeline;
            std::vector<VkShaderModule> shaderModules;
        };
    }
}

#endif
