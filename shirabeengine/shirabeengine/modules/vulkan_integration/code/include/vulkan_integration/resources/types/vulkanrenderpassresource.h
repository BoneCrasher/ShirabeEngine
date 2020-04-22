#ifndef __SHIRABE_VULKAN_RENDERPASS_RESOURCE_H__
#define __SHIRABE_VULKAN_RENDERPASS_RESOURCE_H__

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <base/declaration.h>
#include <resources/resourcetypes.h>
#include <resources/extensibility.h>

namespace engine
{
    namespace vulkan
    {
        struct SVulkanRenderPassResource;
    }

    namespace resources
    {
        template <> struct SLogicalToGpuApiResourceTypeMap<SRenderPass> { using TGpuApiResource = struct vulkan::SVulkanRenderPassResource; };
    }

    namespace vulkan
    {
        using namespace resources;

        /**
         * The SVulkanTextureResource struct describes the relevant data to deal
         * with textures inside the vulkan API.
         */
        struct SVulkanRenderPassResource
        {
            struct Handles_t
            {
                VkRenderPass handle;
            };
        };

        using RenderPassResourceState_t = SResourceState<SRenderPass>;
    }
}


#endif // VULKANRENDERPASSRESOURCE_H
