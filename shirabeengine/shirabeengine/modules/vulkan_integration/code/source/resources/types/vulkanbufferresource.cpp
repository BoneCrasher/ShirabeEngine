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

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanBufferResource::create(  SBufferDescription           const &aDescription
                                                  , SNoDependencies              const &aDependencies
                                                  , GpuApiResourceDependencies_t const &aResolvedDependencies)

    {
        CVkApiResource<SBuffer>::create(aDescription, aDependencies, aResolvedDependencies);

        Shared<IVkGlobalContext> vkContext = getVkContext();

        VkDevice         const &vkLogicalDevice  = vkContext->getLogicalDevice();
        VkPhysicalDevice const &vkPhysicalDevice = vkContext->getPhysicalDevice();

        VkBufferCreateInfo createInfo = aDescription.createInfo;
        createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        // createInfo.queueFamilyIndexCount = ...;
        // createInfo.pQueueFamilyIndices   = ...;

        VkBuffer buffer = VK_NULL_HANDLE;

        VkResult result = vkCreateBuffer(vkLogicalDevice, &createInfo, nullptr, &buffer);
        if(VkResult::VK_SUCCESS != result)
        {
            CLog::Error(logTag(), CString::format("Failed to create buffer. Vulkan result: {}", result));
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
            CLog::Error(logTag(), CString::format("Failed to allocate buffer memory on GPU. Vulkan error: {}", result));
            return { EEngineStatus::Error };
        }

        result = vkBindBufferMemory(vkLogicalDevice, buffer, memory, 0);
        if(VkResult::VK_SUCCESS != result)
        {
            CLog::Error(logTag(), CString::format("Failed to bind buffer memory on GPU. Vulkan error: {}", result));
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
    CEngineResult<> CVulkanBufferResource::load() const
    {
        SBufferDescription const description = *getCurrentDescriptor();
        if(nullptr != description.dataSource)
        {
            VkDevice device = getVkContext()->getLogicalDevice();

            ByteBuffer const dataSource = description.dataSource();

            void *mappedData = nullptr;
            vkMapMemory(device, this->attachedMemory, 0, dataSource.size(), 0, &mappedData);
            memcpy(mappedData, dataSource.data(), dataSource.size());
            vkUnmapMemory(device, this->attachedMemory);
        }

        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanBufferResource::transfer() const
    {
        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanBufferResource::unload() const
    {
        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanBufferResource::destroy()
    {
        VkBuffer       vkBuffer        = handle;
        VkDeviceMemory vkDeviceMemory  = attachedMemory;
        VkDevice       vkLogicalDevice = getVkContext()->getLogicalDevice();

        vkFreeMemory   (vkLogicalDevice, vkDeviceMemory, nullptr);
        vkDestroyBuffer(vkLogicalDevice, vkBuffer, nullptr);

        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------
}
