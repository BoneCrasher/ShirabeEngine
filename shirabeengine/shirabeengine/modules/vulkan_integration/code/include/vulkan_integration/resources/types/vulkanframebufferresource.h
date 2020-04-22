#ifndef __SHIRABE_VULKAN_FRAMEBUFFER_RESOURCE_H__
#define __SHIRABE_VULKAN_FRAMEBUFFER_RESOURCE_H__

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <base/declaration.h>
#include <resources/resourcetypes.h>
#include <resources/extensibility.h>

namespace engine
{
    namespace vulkan
    {
        struct SVulkanFrameBufferResource;
    }

    namespace resources
    {
        template <> struct SLogicalToGpuApiResourceTypeMap<SFrameBuffer> { using TGpuApiResource = struct vulkan::SVulkanFrameBufferResource;  };
    }

    namespace vulkan
    {
        using namespace resources;

        /**
         * The SVulkanTextureResource struct describes the relevant data to deal
         * with textures inside the vulkan API.
         */
        struct SVulkanFrameBufferResource
        {
            struct Handles_t
            {
                VkFramebuffer handle;
            };
        };
        using FrameBufferResourceState_t = SResourceState<SFrameBuffer>;
    }
}

#endif // VULKANFRAMEBUFFERRESOURCE_H
