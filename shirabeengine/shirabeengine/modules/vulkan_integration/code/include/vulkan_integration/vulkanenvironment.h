#ifndef __SHIRABE_VULKAN_ENVIRONMENT_H__
#define __SHIRABE_VULKAN_ENVIRONMENT_H__

#include "vulkan_integration/vulkanenvironmenttypes.h"
#include "vulkan_integration/resources/ivkglobalcontext.h"

namespace engine::vulkan
{
    using os::SApplicationEnvironment;
    using engine::wsi::CWindowHandleWrapper;
    using engine::graphicsapi::EFormat;

    /**
     * The CVulkanEnvironment class encapsulates all vulkan API related
     * base state & information required to use the graphics card and the
     * vulkan API in the engine.
     */
    class CVulkanEnvironment
        : public IVkGlobalContext
    {
        SHIRABE_DECLARE_LOG_TAG(CVulkanEnvironment);

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
        EEngineStatus initialize(SApplicationEnvironment        const &aApplicationEnvironment
                               , Shared<CGpuApiResourceStorage>       &aStorage);

        /**
         * Stop and clean up all vulkan API related functionality.
         *
         * @return EEngineStatus::Ok, if successful. An error code otherwise.
         */
        EEngineStatus deinitialize();

        SHIRABE_INLINE
        void registerDebugObjectName(uint64_t const &aHandle, VkObjectType const &aObjectType, std::string const &aObjectName) final
        {
            // static auto sDefinedDebugUtilsObjectNameFn = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(mVkState.instance, "vkSetDebugUtilsObjectNameEXT");
            // if(nullptr != sDefinedDebugUtilsObjectNameFn)
            // {
            //     VkDebugUtilsObjectNameInfoEXT name = {
            //             .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            //             .pNext = nullptr,
            //             .objectType   = aObjectType,
            //             .objectHandle = aHandle,
            //             .pObjectName  = aObjectName.c_str()
            //     };
            //     sDefinedDebugUtilsObjectNameFn(mVkState.selectedLogicalDevice, &name);
            // }
        }

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

    public_api:
        Shared<IVkFrameContext> getVkCurrentFrameContext() final;

        VkDevice         getLogicalDevice()  final;
        VkPhysicalDevice getPhysicalDevice() final;

        Shared<CGpuApiResourceStorage> getResourceStorage() final;

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

        void recreateCommandBuffers(uint32_t const &aBufferCount);

        void destroyCommandBuffers();

        /**
         * Cleanup all swapchain resources.
         */
        void destroySwapChain();

    private_members:
        SVulkanState                   mVkState;
        Shared<CGpuApiResourceStorage> mResourceStorage;
    };

}

#endif
