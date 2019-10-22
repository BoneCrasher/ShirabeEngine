#ifndef __SHIRABE_VULKAN_BUFFER_RESOURCE_H__
#define __SHIRABE_VULKAN_BUFFER_RESOURCE_H__

#include <vulkan/vulkan.h>

#include <base/declaration.h>
#include <resources/resourcetypes.h>
#include <resources/aprivateresourceobject.h>
#include "vulkan/resources/ivkapiresource.h"

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
            : public APrivateResourceObject<SBufferDescription>
        {
        public_constructors:
            using APrivateResourceObject<SBufferDescription>::APrivateResourceObject;

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
