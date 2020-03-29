//
// Created by dottideveloper on 29.10.19.
//
#include "vulkan_integration/resources/types/vulkanbufferresource.h"
#include "vulkan_integration/vulkandevicecapabilities.h"

namespace engine::vulkan
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<SVulkanBufferCreationResult> __createVkBuffer(  VkPhysicalDevice      const &aPhysicalDevice
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

        result = vkAllocateMemory(aLogicalDevice, &vkMemoryAllocationInfo, nullptr, &vkBufferMemory);
        if(VkResult::VK_SUCCESS != result)
        {
            vkDestroyBuffer(aLogicalDevice, vkBuffer, nullptr);
            return EEngineStatus::Error;
        }

        vkBindBufferMemory(aLogicalDevice, vkBuffer, vkBufferMemory, 0);
        return { EEngineStatus::Ok, SVulkanBufferCreationResult {vkBuffer, vkBufferMemory}};
    }
    //<-----------------------------------------------------------------------------
}
