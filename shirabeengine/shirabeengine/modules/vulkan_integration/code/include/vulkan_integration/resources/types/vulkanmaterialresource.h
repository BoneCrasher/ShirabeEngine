#ifndef __SHIRABE_VULKAN_MATERIAL_RESOURCE_H__
#define __SHIRABE_VULKAN_MATERIAL_RESOURCE_H__

#include <vector>

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <resources/resourcetypes.h>
#include <resources/extensibility.h>

#include <base/declaration.h>

namespace engine
{
    namespace vulkan
    {
        struct SVulkanMaterialResource;
    }

    namespace resources
    {
        template <> struct SLogicalToGpuApiResourceTypeMap<SMaterial> { using TGpuApiResource = vulkan::SVulkanMaterialResource;  };
    }

    namespace vulkan
    {
        /**
         * The SVulkanTextureResource struct describes the relevant data to deal
         * with textures inside the vulkan API.
         */
        struct SVulkanMaterialResource
        {
            struct Handles_t
            { };
        };

        using MaterialResourceState_t = SResourceState<SMaterial>;
    }
}

#endif
