#ifndef __SHIRABE_VULKAN_MESH_RESOURCE_H__
#define __SHIRABE_VULKAN_MESH_RESOURCE_H__

#include <vector>

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <resources/extensibility.h>
#include <resources/resourcetypes.h>

#include <base/declaration.h>

namespace engine
{
    namespace vulkan
    {
        struct SVulkanMeshResource;
    }

    namespace resources
    {
        template <> struct SLogicalToGpuApiResourceTypeMap<SMesh> { using TGpuApiResource = vulkan::SVulkanMeshResource;  };
    }

    namespace vulkan
    {
        /**
         * The SVulkanTextureResource struct describes the relevant data to deal
         * with textures inside the vulkan API.
         */
        struct SVulkanMeshResource
        {
            static constexpr bool is_loadable      = false;
            static constexpr bool is_unloadable    = false;
            static constexpr bool is_transferrable = false;

            struct Handles_t
            {
            };
        };
        using MeshResourceState_t = SResourceState<SMesh>;
    }
}

#endif
