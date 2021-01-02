//
// Created by dottideveloper on 29.10.19.
//
#include "renderer/vulkan_resources/resources/types/vulkanbufferresource.h"

namespace engine::vulkan
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<std::tuple<VkBuffer, VkMemoryRequirements>>
        __createVkBuffer(  VkPhysicalDevice   const &aPhysicalDevice
                         , VkDevice           const &aLogicalDevice
                         , VkDeviceSize       const &aBufferSize
                         , VkBufferUsageFlags const &aBufferUsage)
    {
        VkBuffer vkBuffer = VK_NULL_HANDLE;

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

        return { EEngineStatus::Ok, {vkBuffer, vkBufferMemoryRequirements}};
    }
    //<-----------------------------------------------------------------------------
}
