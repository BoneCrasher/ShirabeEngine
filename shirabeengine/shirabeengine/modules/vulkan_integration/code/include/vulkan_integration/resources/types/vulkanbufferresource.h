#ifndef __SHIRABE_VULKAN_BUFFER_RESOURCE_H__
#define __SHIRABE_VULKAN_BUFFER_RESOURCE_H__

#include <vulkan/vulkan.h>

#include <base/declaration.h>
#include <resources/resourcetypes.h>
#include <resources/agpuapiresourceobject.h>

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

        static CEngineResult<SVulkanBufferCreationResult> __createVkBuffer(  VkPhysicalDevice      const &aPhysicalDevice
                                                                           , VkDevice              const &aLogicalDevice
                                                                           , VkDeviceSize          const &aBufferSize
                                                                           , VkBufferUsageFlags    const &aBufferUsage
                                                                           , VkMemoryPropertyFlags const &aBufferMemoryProperties)
        {
            VkBuffer       vkBuffer       = VK_NULL_HANDLE;
            VkDeviceMemory vkBufferMemory = VK_NULL_HANDLE;

            VkBufferCreateInfo vkBufferCreateInfo = {};
            vkBufferCreateInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            vkBufferCreateInfo.pNext       = nullptr;
            vkBufferCreateInfo.size        = aBufferSize;
            vkBufferCreateInfo.usage       = aBufferUsage;
            vkBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            VkResult result = vkCreateBuffer(aLogicalDevice, &vkBufferCreateInfo, nullptr, &vkBuffer);
            if(VkResult::VK_SUCCESS != result)
            {
                return EEngineStatus::Error;
            }

            VkMemoryRequirements vkBufferMemoryRequirements {};
            vkGetBufferMemoryRequirements(aLogicalDevice, vkBuffer, &vkBufferMemoryRequirements);

            VkMemoryAllocateInfo vkMemoryAllocationInfo = {};
            vkMemoryAllocationInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            vkMemoryAllocationInfo.pNext           = nullptr;
            vkMemoryAllocationInfo.allocationSize  = vkBufferMemoryRequirements.size;
            vkMemoryAllocationInfo.memoryTypeIndex = CVulkanDeviceCapsHelper::determineMemoryType(aPhysicalDevice, vkBufferMemoryRequirements.memoryTypeBits, aBufferMemoryProperties).data();

            result = vkAllocateMemory(aLogicalDevice, &vkMemoryAllocationInfo, nullptr, &vkBufferMemory)
            if(VkResult::VK_SUCCESS != result)
            {
                vkDestroyBuffer(aLogicalDevice, vkBuffer, nullptr);
                return EEngineStatus::Error;
            }

            vkBindBufferMemory(aLogicalDevice, vkBuffer, vkBufferMemory, 0);
            return { EEngineStatus::Ok, SVulkanBufferCreationResult {vkBuffer, vkBufferMemory}};
        }

        /**
         * The SVulkanTextureResource struct describes the relevant data to deal
         * with textures inside the vulkan API.
         */
        class CVulkanBufferResource
            : public CVkApiResource<SBufferDescription>
        {
            SHIRABE_DECLARE_LOG_TAG(CVulkanBufferResource);

        public_constructors:
            using CVkApiResource<SBufferDescription>::AGpuApiResourceObject;

        public_methods:
            CEngineResult<> create(CGpiApiDependencyCollection const &aDependencies)   final;
            CEngineResult<> load()     final;
            CEngineResult<> unload()   final;
            CEngineResult<> destroy()  final;
            CEngineResult<> bind()     final;
            CEngineResult<> transfer() final;
            CEngineResult<> unbind()   final;

        public_members:
            VkBuffer       handle;
            VkDeviceMemory attachedMemory;
        };
    }
}

#endif
