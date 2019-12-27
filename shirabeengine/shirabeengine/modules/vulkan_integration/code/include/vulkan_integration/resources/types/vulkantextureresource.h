#ifndef __SHIRABE_VULKAN_TEXTURE_RESOURCE_H__
#define __SHIRABE_VULKAN_TEXTURE_RESOURCE_H__

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <base/declaration.h>
#include <resources/resourcetypes.h>
#include <resources/agpuapiresourceobject.h>
#include <resources/iloadablegpuapiresourceobject.h>
#include <resources/itransferrablegpuapiresourceobject.h>
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
                : public CVkApiResource<STexture>
                , public ILoadableGpuApiResourceObject
                , public ITransferrableGpuApiResourceObject
        {
            SHIRABE_DECLARE_LOG_TAG(CVulkanTextureResource);

        public_constructors:
            using CVkApiResource<STexture>::CVkApiResource;

        public_methods:
            // AGpuApiResourceObject
            CEngineResult<> create(  STextureDescription          const &aDescription
                                   , SNoDependencies              const &aDependencies
                                   , GpuApiResourceDependencies_t const &aResolvedDependencies) final;
            CEngineResult<> destroy()  final;

            // ILoadableGpuApiResourceObject
            CEngineResult<> load()     final;
            CEngineResult<> unload()   final;

            // ITransferrableGpuApiResourceObject
            CEngineResult<> transfer() final;

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
