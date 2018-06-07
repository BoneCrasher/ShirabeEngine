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
      };

      struct VulkanPhysicalDevice {
        VkPhysicalDevice                 handle;
        VkPhysicalDeviceProperties       properties;
        VkPhysicalDeviceFeatures         features;
        VkPhysicalDeviceMemoryProperties memoryProperties; 
        VulkanQueueFamilyRegistry        queueFamilies;
      };

      struct VulkanState {
        // Instance
        std::vector<char const*>     instanceLayers;
        std::vector<char const*>     instanceExtensions;
        VkInstanceCreateInfo         instanceCreateInfo;
        VkInstance                   instance;
        // Surface
        VkSurfaceKHR                 surface;
        // Physical Device
        std::vector<char const*>     deviceLayers;
        std::vector<char const*>     deviceExtensions;
        Vector<VulkanPhysicalDevice> supportedPhysicalDevices;
        uint32_t                     selectedPhysicalDevice;
        // Logical Device
        VkDevice                     selectedLogicalDevice;
      };
            
      VulkanEnvironment();

      EEngineStatus initialize(
        ApplicationEnvironment const& applicationEnvironment);

      EEngineStatus deinitialize();
      
      VkQueue getGraphicsQueue();

    private:
      void createVulkanInstance(std::string const&name);
      void createVulkanSurface(
        Platform::ApplicationEnvironment const&);
      void determinePhysicalDevices(Format const&);
      void selectPhysicalDevice(uint32_t);

      VulkanState m_vkState;
    };


  }
}

#endif