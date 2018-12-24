#ifndef __SHIRABE_VULKAN_FRAMEBUFFER_RESOURCE_H__
#define __SHIRABE_VULKAN_FRAMEBUFFER_RESOURCE_H__

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
        struct SVulkanFrameBufferResource
        {
        public_members:
            VkFramebuffer handle;
        };
    }
}

#endif // VULKANFRAMEBUFFERRESOURCE_H
