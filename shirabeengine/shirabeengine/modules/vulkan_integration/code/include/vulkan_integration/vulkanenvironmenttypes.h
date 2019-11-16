//
// Created by dotti on 02.11.19.
//

#ifndef __SHIRABEDEVELOPMENT_VULKANENVIRONMENTTYPES_H__
#define __SHIRABEDEVELOPMENT_VULKANENVIRONMENTTYPES_H__

#include <vector>

#include <log/log.h>
#include <core/enginestatus.h>
#include <os/applicationenvironment.h>
#include <math/geometric/rect.h>
#include <graphicsapi/definitions.h>
#include <wsi/windowhandlewrapper.h>
#include <wsi/windowhandlewrapper.h>
#include "vulkan_integration/vulkanimport.h"

namespace engine::vulkan
{

    /**
     * The SVulkanQueueFamilyRegistry struct describes all
     * supported queue families of the selected device separated
     * into categories/contexts.
     */
    struct SVulkanQueueFamilyRegistry
    {
    public_constructors:
       SVulkanQueueFamilyRegistry();

    public_members:
        std::vector<uint32_t> supportingQueueFamilyIndices;
        std::vector<uint32_t> graphicsQueueFamilyIndices;
        std::vector<uint32_t> computeQueueFamilyIndices;
        std::vector<uint32_t> transferQueueFamilyIndices;
        std::vector<uint32_t> presentQueueFamilyIndices;
    };

    /**
     * The SVulkanPhysicalDevice struct describes the selected
     * physical device and it's data and state.
     */
    struct SVulkanPhysicalDevice
    {
    public_constructors:
        SVulkanPhysicalDevice();

    public_members:
        VkPhysicalDevice                 handle;
        VkPhysicalDeviceProperties       properties;
        VkPhysicalDeviceFeatures         features;
        VkPhysicalDeviceMemoryProperties memoryProperties;
        SVulkanQueueFamilyRegistry       queueFamilies;
    };

    /**
     * The SVulkanSwapChain struct describes the swapchain created, if any.
     */
    struct SVulkanSwapChain
    {
    public_constructors:
        SVulkanSwapChain();

    public_members:
        VkSwapchainKHR                  handle;
        VkSurfaceCapabilitiesKHR        capabilities;
        std::vector<VkSurfaceFormatKHR> supportedFormats;
        std::vector<VkPresentModeKHR>   supportedPresentModes;
        VkExtent2D                      selectedExtents;
        VkSurfaceFormatKHR              selectedFormat;
        VkPresentModeKHR                selectedPresentMode;
        std::vector<VkImage>            swapChainImages;
        uint32_t                        currentSwapChainImageIndex;
        VkSemaphore                     imageAvailableSemaphore;
        VkSemaphore                     renderCompletedSemaphore;
        // Creation config
        math::CRect                     requestedBackBufferSize;
        VkFormat                        requestedFormat;
        VkColorSpaceKHR                 colorSpace;
    };

    /**
     * The SVulkanState struct describes all relevant vulkan API data
     * and state of the current vulkan instantiation, including
     * layers, extensions, debug capabilities, instances, devices,
     * etc... etc...
     */
    struct SVulkanState
    {
    public_constructors:
        SVulkanState();

    public_members:
        // Instance
        std::vector<char const*>      instanceLayers;
        std::vector<char const*>      instanceExtensions;
        VkInstanceCreateInfo          instanceCreateInfo;
        VkInstance                    instance;
        // Debug
        VkDebugReportCallbackEXT      debugReportCallback;
        // Surface
        VkSurfaceKHR                  surface;
        // Physical Device
        std::vector<char const*>      deviceLayers;
        std::vector<char const*>      deviceExtensions;
        Vector<SVulkanPhysicalDevice> supportedPhysicalDevices;
        uint32_t                      selectedPhysicalDevice;
        // Logical Device
        VkDevice                      selectedLogicalDevice;
        // Swap Chain
        SVulkanSwapChain              swapChain;
        // Command Pool & Buffer
        VkCommandPool                 commandPool;
        std::vector<VkCommandBuffer>  commandBuffers;
    };

    /**
     * The CVulkanError class defines an std::runtime_error compatible
     * type to provide any kind of error related to the vulkan API integration.
     */
    class CVulkanError
            : public std::runtime_error
    {
    public_constructors:
        /**
         * Construct a new vulkan error.
         *
         * @param aMessage      Message of the error.
         * @param aVulkanResult VkResult value of the error.
         */
        CVulkanError(
                std::string const &aMessage,
                VkResult    const &aVulkanResult);

    public_methods:
        /**
         * Return the VkResult value of the error.
         *
         * @return See brief.
         */
        VkResult vulkanResult() const;

    private_members:
        VkResult mVkResult;
    };
}

#endif //__SHIRABEDEVELOPMENT_VULKANENVIRONMENTTYPES_H__
