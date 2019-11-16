//
// Created by dottideveloper on 29.10.19.
//
#include "vulkan_integration/resources/types/vulkanbufferviewresource.h"
#include "vulkan_integration/resources/types/vulkanbufferresource.h"
#include "vulkan_integration/vulkandevicecapabilities.h"

namespace engine::vulkan
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanBufferViewResource::create(GpuApiResourceDependencies_t const &aDependencies)
    {
        GpuApiHandle_t        const        bufferGpuApiId = aDependencies.at(getDescription().bufferId);
        CVulkanBufferResource const *const bufferResource = getVkContext()->getResourceStorage()->extract<CVulkanBufferResource>(bufferGpuApiId);

        if(nullptr == bufferResource)
        {
            return { EEngineStatus::Error };
        }

        VkBufferViewCreateInfo createInfo = getDescription().createInfo;
        createInfo.buffer =  bufferResource->handle;

        VkBufferView vkBufferView = VK_NULL_HANDLE;

        VkResult result = vkCreateBufferView(getVkContext()->getLogicalDevice(), &(createInfo), nullptr, &vkBufferView);
        if(VkResult::VK_SUCCESS != result)
        {
            CLog::Error(logTag(), CString::format("Failed to create buffer view. Vulkan error: {}", result));
            return { EEngineStatus::Error };
        }

        this->handle = vkBufferView;

        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanBufferViewResource::destroy()
    {
        VkBufferView vkBufferView    = this->handle;
        VkDevice     vkLogicalDevice = getVkContext()->getLogicalDevice();

        vkDestroyBufferView(vkLogicalDevice, vkBufferView, nullptr);

        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------
}
