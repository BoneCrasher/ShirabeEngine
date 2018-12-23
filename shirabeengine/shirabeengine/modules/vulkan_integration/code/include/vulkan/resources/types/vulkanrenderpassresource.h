#ifndef __SHIRABE_VULKAN_RENDERPASS_RESOURCE_H__
#define __SHIRABE_VULKAN_RENDERPASS_RESOURCE_H__

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
        struct SVulkanRenderPassResource
        {
        public_members:
            VkRenderPass handle;
        };
    }
}

#endif // VULKANRENDERPASSRESOURCE_H
