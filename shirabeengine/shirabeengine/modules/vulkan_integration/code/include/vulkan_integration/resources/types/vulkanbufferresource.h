#ifndef __SHIRABE_VULKAN_BUFFER_RESOURCE_H__
#define __SHIRABE_VULKAN_BUFFER_RESOURCE_H__

#include <vulkan/vulkan.h>

#include <base/declaration.h>
#include <resources/resourcetypes.h>
#include <resources/agpuapiresourceobject.h>
#include "vulkan_integration/resources/cvkapiresource.h"

namespace engine
{
    namespace vulkan
    {
        using namespace resources;

        /**
         * The SVulkanTextureResource struct describes the relevant data to deal
         * with textures inside the vulkan API.
         */
        class CVulkanBufferResource
            : public CVkApiResource<SBufferDescription>
        {
        public_constructors:
            using CVkApiResource<SBufferDescription>::AGpuApiResourceObject;

        public_methods:
            // CEngineResult<> create()  override;
            // CEngineResult<> load()    override;
            // CEngineResult<> unload()  override;
            // CEngineResult<> destroy() override;

        public_members:
            VkBuffer       handle;
            VkDeviceMemory attachedMemory;
        };
    }
}

#endif
