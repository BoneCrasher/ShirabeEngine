#ifndef __SHIRABE_VULKAN_ENVIRONMENT_H__
#define __SHIRABE_VULKAN_ENVIRONMENT_H__

#include "Platform/ApplicationEnvironment.h"
#include "Platform/WindowHandleWrapper.h"

#include "Core/EngineStatus.h"
#include "Log/Log.h"

#include "Platform/Vulkan/VulkanImport.h"
#include "Resources/Subsystems/GFXAPI/Types/Definition.h"

namespace Engine {
  namespace Vulkan {

    using Platform::ApplicationEnvironment;
    using Platform::Window::WindowHandleWrapper;
    using Engine::Resources::Format;


    /**
     * \class VulkanError
     *
     * \brief A vulkan error.
     **************************************************************************************************/
    class VulkanError
      : public std::exception
    {
    public:
      VulkanError(std::string const&, VkResult const&);

      VkResult vulkanResult() const;

    private:
      VkResult m_vkResult;
    };

    /**
     * \class VulkanEnvironment
     *
     * \brief A vulkan environment.
     **************************************************************************************************/
    class VulkanEnvironment {
      DeclareLogTag(VulkanEnvironment);

    public:
      struct VulkanQueueFamilyRegistry {
        std::vector<uint32_t> supportingQueueFamilyIndices;
        std::vector<uint32_t> graphicsQueueFamilyIndices;
        std::vector<uint32_t> computeQueueFamilyIndices;
        std::vector<uint32_t> transferQueueFamilyIndices;
        std::vector<uint32_t> presentQueueFamilyIndices;

        VulkanQueueFamilyRegistry();
      };

      struct VulkanPhysicalDevice {
        VkPhysicalDevice                 handle;
        VkPhysicalDeviceProperties       properties;
        VkPhysicalDeviceFeatures         features;
        VkPhysicalDeviceMemoryProperties memoryProperties;
        VulkanQueueFamilyRegistry        queueFamilies;

        VulkanPhysicalDevice();
      };

      struct VulkanSwapChain {
        VkSwapchainKHR                  handle;
        VkSurfaceCapabilitiesKHR        capabilities;
        std::vector<VkSurfaceFormatKHR> supportedFormats;
        std::vector<VkPresentModeKHR>   supportedPresentModes;
        VkExtent2D                      selectedExtents;
        VkSurfaceFormatKHR              selectedFormat;
        VkPresentModeKHR                selectedPresentMode;
        std::vector<VkImage>            swapChainImages;

        VulkanSwapChain();
      };

      struct VulkanState {
        // Instance
        std::vector<char const*>     instanceLayers;
        std::vector<char const*>     instanceExtensions;
        VkInstanceCreateInfo         instanceCreateInfo;
        VkInstance                   instance;
        // Debug
        VkDebugReportCallbackEXT     debugReportCallback;
        // Surface
        VkSurfaceKHR                 surface;
        // Physical Device
        std::vector<char const*>     deviceLayers;
        std::vector<char const*>     deviceExtensions;
        Vector<VulkanPhysicalDevice> supportedPhysicalDevices;
        uint32_t                     selectedPhysicalDevice;
        // Logical Device
        VkDevice                     selectedLogicalDevice;
        // Swap Chain 
        VulkanSwapChain              swapChain;

        VulkanState();
      };

      VulkanEnvironment();

      EEngineStatus initialize(
        ApplicationEnvironment const& applicationEnvironment);

      EEngineStatus deinitialize();

      VkQueue      getGraphicsQueue();
      VulkanState& getState();

    private:
      void createVulkanInstance(std::string const&name);
      void createVulkanSurface(
        Platform::ApplicationEnvironment const&);
      void determinePhysicalDevices();
      void selectPhysicalDevice(uint32_t);
      void createSwapChain(
        Rect            const&requestedBackBufferSize,
        VkFormat        const&requestedFormat,
        VkColorSpaceKHR const&colorSpace);

      VulkanState m_vkState;
    };


  }
}

#endif