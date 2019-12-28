//
// Created by dotti on 02.11.19.
//

#include "vulkan_integration/vulkanenvironmenttypes.h"

namespace engine::vulkan
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CVulkanError::CVulkanError(
            std::string const&message,
            VkResult    const&vkResult)
            : std::runtime_error(message.c_str())
              , mVkResult(vkResult)
    {}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    VkResult CVulkanError::vulkanResult() const
    {
        return mVkResult;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    SVulkanQueueFamilyRegistry::SVulkanQueueFamilyRegistry()
            : supportingQueueFamilyIndices()
              , graphicsQueueFamilyIndices()
              , computeQueueFamilyIndices()
              , transferQueueFamilyIndices()
              , presentQueueFamilyIndices()
    {}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    SVulkanPhysicalDevice::SVulkanPhysicalDevice()
            : handle(VK_NULL_HANDLE)
              , properties({})
              , features({})
              , memoryProperties({})
              , queueFamilies()
    {}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    SVulkanSwapChain::SVulkanSwapChain()
            : handle(VK_NULL_HANDLE)
              , capabilities({})
              , supportedFormats()
              , supportedPresentModes()
              , selectedExtents({ 0, 0 })
              , selectedFormat()
              , selectedPresentMode()
              , swapChainImages()
              , currentSwapChainImageIndex(0)
              , requestedBackBufferSize()
              , requestedFormat(VkFormat::VK_FORMAT_R8G8B8A8_UNORM)
              , colorSpace(VkColorSpaceKHR::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
    {}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    SVulkanState::SVulkanState()
            : instanceLayers()
              , instanceExtensions()
              , instanceCreateInfo()
              , instance(VK_NULL_HANDLE)
              , surface(VK_NULL_HANDLE)
              , deviceLayers()
              , deviceExtensions()
              , supportedPhysicalDevices()
              , selectedPhysicalDevice(0)
              , selectedLogicalDevice(VK_NULL_HANDLE)
              , swapChain()
    {}
    //<-----------------------------------------------------------------------------
}

