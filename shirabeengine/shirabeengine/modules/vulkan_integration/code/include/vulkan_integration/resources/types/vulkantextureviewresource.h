#ifndef __SHIRABE_VULKAN_TEXTUREVIEW_RESOURCE_H__
#define __SHIRABE_VULKAN_TEXTUREVIEW_RESOURCE_H__

#include <vulkan/vulkan.h>

#include <base/declaration.h>

namespace engine
{
    namespace vulkan
    {
        /**
         * The SVulkanTextureViewResource struct describes all data to
         * deal with image views inside the vulkan API.
         */
        struct SVulkanTextureViewResource
        {
            VkImageView handle;
        };

    }
}

#endif
