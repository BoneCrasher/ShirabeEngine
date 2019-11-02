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
    CEngineResult<> CVulkanBufferViewResource::create(CGpiApiDependencyCollection const &aDependencies)
    {
        CVulkanBufferResource const *const bufferResource = aDependencies.extract<CVulkanBufferResource>(getDescription().bufferId);
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
            CLog::Error(logTag(), CString::format("Failed to create buffer view. Vulkan error: %0", result));
            return { EEngineStatus::Error };
        }

        this->handle = vkBufferView;

        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanBufferViewResource::load()
    {
        // Nothing to be done...
        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanBufferViewResource::unload()
    {
        // Nothing to be done...
        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanBufferViewResource::destroy()
    {
        VkBufferView vkBufferView    = this->handle;
        VkDevice     vkLogicalDevice = mVulkanEnvironment->getState().selectedLogicalDevice;

        vkDestroyBufferView(vkLogicalDevice, vkBufferView, nullptr);

        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanBufferViewResource::bind()
    {
        // Nothing to be done as buffer views are not explicitly bound...
        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanBufferViewResource::transfer()
    {
        // Nothing to be done...
        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanBufferViewResource::unbind()
    {
        // Nothing to be done as buffer views are not explicitly unbound...
        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------
}
