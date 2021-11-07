//
// Created by dottideveloper on 29.10.19.
//
#include "rhi/vulkan_resources/resources/types/vulkanmemoryresource.h"

namespace engine::vulkan
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<VkDeviceMemory> __createVkMemory(  VkPhysicalDevice      const &aPhysicalDevice
                                                   , VkDevice              const &aLogicalDevice
                                                   , VkMemoryRequirements  const &aMemoryRequirements
                                                   , VkMemoryPropertyFlags const &aMemoryProperties)
    {
        VkDeviceMemory memory = VK_NULL_HANDLE;

        VkMemoryAllocateInfo memoryAllocateInfo = {};
        memoryAllocateInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.pNext           = nullptr;
        memoryAllocateInfo.allocationSize  = aMemoryRequirements.size;
        memoryAllocateInfo.memoryTypeIndex = CVulkanDeviceCapsHelper::determineMemoryType(aPhysicalDevice, aMemoryRequirements.memoryTypeBits, aMemoryProperties).data();

        VkResult const result = vkAllocateMemory(aLogicalDevice, &memoryAllocateInfo, nullptr, &memory);
        if(VkResult::VK_SUCCESS != result)
        {
            return EEngineStatus::Error;
        }

        return { EEngineStatus::Ok, memory};
    }
    //<-----------------------------------------------------------------------------
}
