#ifndef __SHIRABE_VULKAN_BUFFER_RESOURCE_H__
#define __SHIRABE_VULKAN_BUFFER_RESOURCE_H__

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <base/declaration.h>
#include <resources/resourcetypes.h>
#include <resources/agpuapiresourceobject.h>
#include <resources/iloadablegpuapiresourceobject.h>
#include <resources/itransferrablegpuapiresourceobject.h>

#include "vulkan_integration/vulkandevicecapabilities.h"
#include "vulkan_integration/resources/cvkapiresource.h"

namespace engine
{
    namespace vulkan
    {
        using namespace resources;

        struct
             [[nodiscard]]
             SHIRABE_TEST_EXPORT SVulkanBufferCreationResult
        {
            VkBuffer       buffer;
            VkDeviceMemory attachedMemory;
        };

        /**
         *
         * @param aPhysicalDevice
         * @param aLogicalDevice
         * @param aBufferSize
         * @param aBufferUsage
         * @param aBufferMemoryProperties
         * @return
         */
        CEngineResult<SVulkanBufferCreationResult> __createVkBuffer(  VkPhysicalDevice      const &aPhysicalDevice
                                                                    , VkDevice              const &aLogicalDevice
                                                                    , VkDeviceSize          const &aBufferSize
                                                                    , VkBufferUsageFlags    const &aBufferUsage
                                                                    , VkMemoryPropertyFlags const &aBufferMemoryProperties);

        /**
         * The SVulkanTextureResource struct describes the relevant data to deal
         * with textures inside the vulkan API.
         */
        class CVulkanBufferResource
            : public CVkApiResource<SBuffer>
            , public ILoadableGpuApiResourceObject
            , public ITransferrableGpuApiResourceObject
        {
            SHIRABE_DECLARE_LOG_TAG(CVulkanBufferResource);

        public_constructors:
            using CVkApiResource<SBuffer>::CVkApiResource;

        public_methods:
            // AGpuApiResourceObject
            CEngineResult<> create(  SBufferDescription           const &aDescription
                                   , SNoDependencies              const &aDependencies
                                   , GpuApiResourceDependencies_t const &aResolvedDependencies) final;
            CEngineResult<> destroy() final;

            // ILoadableGpuApiResourceObject
            CEngineResult<> load()     const final;
            CEngineResult<> unload()   const final;

            // ITransferrableGpuApiResourceObject
            CEngineResult<> transfer() const final;

        public_members:
            VkBuffer       handle;
            VkDeviceMemory attachedMemory;
        };
    }
}

#endif
