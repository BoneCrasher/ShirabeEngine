#ifndef __SHIRABE_VULKAN_ENVIRONMENT_H__
#define __SHIRABE_VULKAN_ENVIRONMENT_H__

#include <vector>

#include <log/log.h>
#include <core/enginestatus.h>
#include <os/applicationenvironment.h>
#include <math/geometric/rect.h>
#include <graphicsapi/resources/types/definition.h>
#include <wsi/windowhandlewrapper.h>
#include <wsi/windowhandlewrapper.h>
#include "vulkan/vulkanimport.h"

namespace engine
{
    namespace vulkan
    {
        using os::SApplicationEnvironment;
        using engine::wsi::CWindowHandleWrapper;
        using engine::resources::EFormat;

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

        /**
         * The CVulkanEnvironment class encapsulates all vulkan API related
         * base state & information required to use the graphics card and the
         * vulkan API in the engine.
         */
        class CVulkanEnvironment
        {
            SHIRABE_DECLARE_LOG_TAG(CVulkanEnvironment);

        public_structs:
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

        public_constructors:
            /**
             * Default-Construct a vulkan environment.
             */
            CVulkanEnvironment();

        public_methods:
            /**
             * Initialize the vulkan environment from the current application's environment.
             *
             * @param aApplicationEnvironment The application environment to attach to.
             * @return                        EEngineStatus::Ok, if successful. An error code otherwise.
             */
            EEngineStatus initialize(SApplicationEnvironment const &aApplicationEnvironment);

            /**
             * Stop and clean up all vulkan API related functionality.
             *
             * @return EEngineStatus::Ok, if successful. An error code otherwise.
             */
            EEngineStatus deinitialize();

            /**
             * Return the currently selected graphics queue, if any.
             *
             * @return A graphics queue, if available or VK_NULL_HANDLE.
             */
            VkQueue getGraphicsQueue();

            /**
             * Return the currently selected present queue, if any.
             *
             * @return A present queue, if available or VK_NULL_HANDLE.
             */
            VkQueue getPresentQueue();

            /**
             * If the Vulkan-Environment should work upon a surface, provide it here.
             *
             * @param aSurface The surface to bind to the Vk-Environment.
             */
            void setSurface(VkSurfaceKHR const &aSurface);

            /**
             * Return the current vulkan internal state mutably.
             *
             * @return See brief.
             */
            SVulkanState &getState();

            /**
             * Create and bind a vulkan swapchain.
             *
             * @param aRequestedBackBufferSize The max-size of the back buffers to be created.
             * @param aRequestedFormat         The requested back buffer format.
             * @param aColorSpace              The color space for operating system output.
             */
            void createSwapChain(
                    math::CRect     const &aRequestedBackBufferSize,
                    VkFormat        const &aRequestedFormat,
                    VkColorSpaceKHR const &aColorSpace);

            /**
             * Try to recreate the swapchain with the last known configuration.
             */
            void recreateSwapChain();

        private_methods:
            /**
             * Create and initialize the vulkan instance, including determinition of all
             * vulkan capable devices and their capabilities as well as device selection
             * and binding.
             *
             * @param aInstanceName The name of the instance to create.
             */
            void createVulkanInstance(std::string const &aInstanceName);

            // /**
            //  * Create a vulkan surface and swapchain.
            //  *
            //  * @param aApplicationEnvironment The application environment to attach to.
            //  */
            // void createVulkanSurface(os::SApplicationEnvironment const &aApplicationEnvironment);

            /**
             * Find all physical devices and required capabilities in the system.
             */
            void determinePhysicalDevices();

            /**
             * Select a specific physical device previously discovered by the vulkan API.
             *
             * @param aDeviceIndex Index of the device to select.
             */
            void selectPhysicalDevice(uint32_t const &aDeviceIndex);

            void createCommandPool();

            /**
             * Cleanup all swapchain resources.
             */
            void destroySwapChain();

        private_members:
            SVulkanState mVkState;
        };


    }
}

#endif
