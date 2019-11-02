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
    CEngineResult<> CVulkanBufferResource::create(CGpiApiDependencyCollection const &aDependencies)
    {
        VkDevice         const &vkLogicalDevice  = mVulkanEnvironment->getState().selectedLogicalDevice;
        VkPhysicalDevice const &vkPhysicalDevice = mVulkanEnvironment->getState().supportedPhysicalDevices.at(mVulkanEnvironment->getState().selectedPhysicalDevice).handle;

        VkBufferCreateInfo createInfo = getDescription().createInfo;
        createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        // createInfo.queueFamilyIndexCount = ...;
        // createInfo.pQueueFamilyIndices   = ...;

        VkBuffer buffer = VK_NULL_HANDLE;

        VkResult result = vkCreateBuffer(vkLogicalDevice, &createInfo, nullptr, &buffer);
        if(VkResult::VK_SUCCESS != result)
        {
            CLog::Error(logTag(), CString::format("Failed to create buffer. Vulkan result: %0", result));
            return { EEngineStatus::Error };
        }

        VkMemoryRequirements vkMemoryRequirements ={ };
        vkGetBufferMemoryRequirements(vkLogicalDevice, buffer, &vkMemoryRequirements);

        VkMemoryAllocateInfo vkMemoryAllocateInfo ={ };
        vkMemoryAllocateInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        vkMemoryAllocateInfo.allocationSize  = vkMemoryRequirements.size;

        CEngineResult<uint32_t> memoryTypeFetch =
                CVulkanDeviceCapsHelper::determineMemoryType(
                        vkPhysicalDevice,
                        vkMemoryRequirements.memoryTypeBits,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        if(not memoryTypeFetch.successful())
        {
            CLog::Error(logTag(), "Could not determine memory type index.");
            return { EEngineStatus::Error };
        }

        vkMemoryAllocateInfo.memoryTypeIndex = memoryTypeFetch.data();

        VkDeviceMemory memory = VK_NULL_HANDLE;

        result = vkAllocateMemory(vkLogicalDevice, &vkMemoryAllocateInfo, nullptr, &memory);
        if(VkResult::VK_SUCCESS != result)
        {
            CLog::Error(logTag(), CString::format("Failed to allocate buffer memory on GPU. Vulkan error: %0", result));
            return { EEngineStatus::Error };
        }

        result = vkBindBufferMemory(vkLogicalDevice, buffer, memory, 0);
        if(VkResult::VK_SUCCESS != result)
        {
            CLog::Error(logTag(), CString::format("Failed to bind buffer memory on GPU. Vulkan error: %0", result));
            return { EEngineStatus::Error };
        }

        this->handle         = buffer;
        this->attachedMemory = memory;

        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanBufferResource::load()
    {

    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanBufferResource::unload()
    {

    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanBufferResource::destroy()
    {
        VkBuffer       vkBuffer        = handle;
        VkDeviceMemory vkDeviceMemory  = attachedMemory;
        VkDevice       vkLogicalDevice = mVulkanEnvironment->getState().selectedLogicalDevice;

        vkFreeMemory(vkLogicalDevice, vkDeviceMemory, nullptr);
        vkDestroyBuffer(vkLogicalDevice, vkBuffer, nullptr);

        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanBufferResource::bind()
    {

    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanBufferResource::transfer()
    {

    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanBufferResource::unbind()
    {

    }
    //<-----------------------------------------------------------------------------
}
