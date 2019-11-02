#ifndef __SHIRABE_VULKAN_TEXTURE_RESOURCE_H__
#define __SHIRABE_VULKAN_TEXTURE_RESOURCE_H__

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
        class CVulkanTextureResource
                : public CVkApiResource<STextureDescription>
        {
            SHIRABE_DECLARE_LOG_TAG(CVulkanBufferResource);

        public_constructors:
            using CVkApiResource<STextureDescription>::AGpuApiResourceObject;

        public_methods:
            CEngineResult<> create(CGpiApiDependencyCollection const &aDependencies)   final;
            CEngineResult<> load()     final;
            CEngineResult<> unload()   final;
            CEngineResult<> destroy()  final;
            CEngineResult<> bind()     final;
            CEngineResult<> transfer() final;
            CEngineResult<> unbind()   final;

        public_members:
            VkBuffer       stagingBuffer;
            VkDeviceMemory stagingBufferMemory;
            VkImage        imageHandle;
            VkDeviceMemory imageMemory;
            VkSampler      attachedSampler;
        };
    }
}

#endif
