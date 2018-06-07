#include <functional>
#include <set>


#include "Core/String.h"
#include "Resources/Subsystems/GFXAPI/Types/SwapChain.h"
#include "GFXAPI/Capabilities.h"
#include "GFXAPI/Vulkan/Environment.h"
#include "GFXAPI/Vulkan/DeviceCapabilities.h"

namespace Engine {
  namespace Vulkan {
    using namespace Engine::GFXAPI;

    VulkanError::VulkanError(
      std::string const&message,
      VkResult    const&vkResult)
      : std::exception(message.c_str())
      , m_vkResult(vkResult)
    {}

    VkResult
      VulkanError::vulkanResult() const
    {
      return m_vkResult;
    }

    VulkanEnvironment::VulkanEnvironment()
    {}

    void VulkanEnvironment::createVulkanInstance(std::string const&name)
    {
      VkApplicationInfo vkApplicationInfo{ };
      vkApplicationInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
      vkApplicationInfo.pApplicationName   = name.c_str();
      vkApplicationInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
      vkApplicationInfo.pEngineName        = "Shirabe Engine";
      vkApplicationInfo.engineVersion      = VK_MAKE_VERSION(0, 0, 1);
      vkApplicationInfo.apiVersion         = VK_API_VERSION_1_1;

      std::vector<char const*> layers{ };
      std::vector<char const*> extensions ={
        VK_KHR_SURFACE_EXTENSION_NAME
      };

      uint32_t instanceLayerCount     = 0;
      uint32_t instanceExtensionCount = 0;

      //
      // Check Layer support
      // 
      std::vector<VkLayerProperties> supportedLayers{ };

      vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);
      supportedLayers.resize(instanceExtensionCount);
      vkEnumerateInstanceLayerProperties(&instanceLayerCount, supportedLayers.data());

      std::set<char const*> layersCopy(layers.begin(), layers.end());
      for(VkLayerProperties const&properties : supportedLayers)
        layersCopy.erase(properties.layerName);

      if(!layersCopy.empty())
        throw VulkanError("Not all required layers supported.", VkResult::VK_ERROR_LAYER_NOT_PRESENT);

      //
      // Check Extension support
      // 
      std::vector<VkExtensionProperties> supportedExtensions{};

      vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);
      supportedExtensions.resize(instanceExtensionCount);
      vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, supportedExtensions.data());

      std::set<char const*> extensionsCopy(extensions.begin(), extensions.end());
      for(VkExtensionProperties const&extensions : supportedExtensions)
        extensionsCopy.erase(extensions.extensionName);

      if(!extensionsCopy.empty())
        throw VulkanError("Not all required extensions supported.", VkResult::VK_ERROR_LAYER_NOT_PRESENT);

      //
      // Create
      // 
      VkInstanceCreateInfo vkInstanceCreatInfo{ };
      vkInstanceCreatInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
      vkInstanceCreatInfo.pApplicationInfo        = &vkApplicationInfo;
      vkInstanceCreatInfo.enabledLayerCount       = layers.size();
      vkInstanceCreatInfo.ppEnabledLayerNames     = layers.data();
      vkInstanceCreatInfo.enabledExtensionCount   = extensions.size();
      vkInstanceCreatInfo.ppEnabledExtensionNames = extensions.data();
      vkInstanceCreatInfo.flags                   = VkInstanceCreateFlags(0);
      vkInstanceCreatInfo.pNext                   = nullptr;

      VkResult result = vkCreateInstance(&vkInstanceCreatInfo, nullptr, &(m_vkState.instance));
      if(VK_SUCCESS != result) {
        throw VulkanError("Failed to create VkInstance.", result);
      }
    }

    void VulkanEnvironment::createVulkanSurface(
      Platform::ApplicationEnvironment const&appEnvironment,
      WindowHandleWrapper              const&handleWrapper)
    {
      VkSurfaceKHR surface{};

      #if defined PLATFORM_WINDOWS
      VkWin32SurfaceCreateInfoKHR vkWin32SurfaceCreateInfo{};
      vkWin32SurfaceCreateInfo.sType     = VK_STRUCTURE_TYPE_DISPLAY_SURFACE_CREATE_INFO_KHR;
      vkWin32SurfaceCreateInfo.hwnd      = handleWrapper.handle;
      vkWin32SurfaceCreateInfo.hinstance = appEnvironment.instanceHandle;

      auto CreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)vkGetInstanceProcAddr(m_vkState.instance, "vkCreateWin32SurfaceKHR");
      if(!CreateWin32SurfaceKHR)
        throw VulkanError("Cannot find vulkan function 'vkCreateWin32SurfaceKHR'.", VkResult::VK_ERROR_INITIALIZATION_FAILED);

      VkResult result = CreateWin32SurfaceKHR(m_vkState.instance, &vkWin32SurfaceCreateInfo, nullptr, &surface);
      if(VkResult::VK_SUCCESS != result) {
        throw VulkanError("Failed to create window surface!", result);
      }
      #endif
    }

    void VulkanEnvironment::determinePhysicalDevices(Format const&requiredBackBufferFormat)
    {
      VkResult result = VkResult::VK_SUCCESS;

      VkFormat requiredFormat = VulkanDeviceCapsHelper::convertFormatToVk(requiredBackBufferFormat);

      std::vector<VkPhysicalDevice> vkPhysicalDevices;
      uint32_t                      vkPhysicalDeviceCount = 0;

      result = vkEnumeratePhysicalDevices(m_vkState.instance, &vkPhysicalDeviceCount, nullptr);
      if(VkResult::VK_SUCCESS == result) {
        vkPhysicalDevices.resize(vkPhysicalDeviceCount);
        vkEnumeratePhysicalDevices(m_vkState.instance, &vkPhysicalDeviceCount, vkPhysicalDevices.data());

        for(uint64_t k = 0; k < vkPhysicalDevices.size(); ++k) {
          VkPhysicalDevice const&vkPhysicalDevice = vkPhysicalDevices.at(k);

          //
          // Check Physical Device Props and Features
          // 
          VkPhysicalDeviceProperties vkPhysicalDeviceProperties{};
          vkGetPhysicalDeviceProperties(vkPhysicalDevice, &vkPhysicalDeviceProperties);

          VkPhysicalDeviceFeatures vkPhysicalDeviceFeatures{};
          vkGetPhysicalDeviceFeatures(vkPhysicalDevice, &vkPhysicalDeviceFeatures);

          bool isDiscreteGPU              = (vkPhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
          bool supportsGeometryShaders    = vkPhysicalDeviceFeatures.geometryShader;
          bool supportsTesselationShaders = vkPhysicalDeviceFeatures.tessellationShader;
          bool supportsLogicalBlendOps    = vkPhysicalDeviceFeatures.logicOp;
          bool supportsNonSolidDrawing    = vkPhysicalDeviceFeatures.fillModeNonSolid;
          bool supportsAnisotropicSampler = vkPhysicalDeviceFeatures.samplerAnisotropy;
          bool supportsBlockCompressedFmt = vkPhysicalDeviceFeatures.textureCompressionBC;

          bool allFeaturesSupported = (
            isDiscreteGPU              &&
            supportsGeometryShaders    &&
            supportsTesselationShaders &&
            supportsLogicalBlendOps    &&
            supportsNonSolidDrawing    &&
            supportsAnisotropicSampler &&
            supportsBlockCompressedFmt);

          if(!allFeaturesSupported)
            continue;

          //
          // Check Extension support
          // 
          std::vector<VkExtensionProperties> supportedExtensions{ };
          uint32_t                           supportedExtensionCount = 0;

          result = vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &supportedExtensionCount, nullptr);
          if(VkResult::VK_SUCCESS == result) {
            supportedExtensions.resize(supportedExtensionCount);
            vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &supportedExtensionCount, supportedExtensions.data());
          }

          std::set<std::string> requiredExtensions ={
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
          };

          for(VkExtensionProperties const&extension : supportedExtensions)
            requiredExtensions.erase(extension.extensionName);

          if(!requiredExtensions.empty()) {
            // Not all extensions supported, skip
            continue;
          }

          // 
          // Check Queue Families for support
          // 
          std::vector<VkQueueFamilyProperties> vkQueueFamilies{};

          uint32_t vkQueueFamilyCount = 0;
          vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &vkQueueFamilyCount, nullptr);
          vkQueueFamilies.resize(vkQueueFamilyCount);
          vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &vkQueueFamilyCount, vkQueueFamilies.data());

          VulkanQueueFamilyRegistry supportingQueueFamilies{};

          for(uint32_t k=0; k<vkQueueFamilyCount; ++k)
          {
            VkQueueFamilyProperties const&properties = vkQueueFamilies.at(k);

            bool enoughQueues     = (properties.queueCount > 0);
            bool supportsGraphics = (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT);
            bool supportsCompute  = (properties.queueFlags & VK_QUEUE_COMPUTE_BIT);
            bool supportsTransfer = (properties.queueFlags & VK_QUEUE_TRANSFER_BIT);

            // TODO: Perform check for transfer only queue family, so that we can speed up transfer ops!

            bool queueValid = (
              enoughQueues     &&
              supportsGraphics &&
              supportsCompute);

            if(!queueValid)
              continue;

            // Store queue family in common lists and specific lists
            supportingQueueFamilies.supportingQueueFamilyIndices.push_back(k);
            if(supportsGraphics)
              supportingQueueFamilies.graphicsQueueFamilyIndices.push_back(k);
            if(supportsCompute)
              supportingQueueFamilies.computeQueueFamilyIndices.push_back(k);
            if(supportsTransfer)
              supportingQueueFamilies.transferQueueFamilyIndices.push_back(k);
          }

          if(supportingQueueFamilies.supportingQueueFamilyIndices.empty())
            // Physical device provides no single supporting queue family
            continue;

          // 
          // Check Memory support
          // 
          VkPhysicalDeviceMemoryProperties vkPhysicalDeviceMemoryProperties{};
          vkGetPhysicalDeviceMemoryProperties(vkPhysicalDevice, &vkPhysicalDeviceMemoryProperties);

          //
          // Create and store collection of information for this device 
          //  
          VulkanPhysicalDevice physicalDevice{};
          physicalDevice.handle           = vkPhysicalDevice;
          physicalDevice.properties       = vkPhysicalDeviceProperties;
          physicalDevice.features         = vkPhysicalDeviceFeatures;
          physicalDevice.memoryProperties = vkPhysicalDeviceMemoryProperties;
          physicalDevice.queueFamilies    = supportingQueueFamilies;

          m_vkState.supportedPhysicalDevices.push_back(physicalDevice);
        }
      }

      if(m_vkState.supportedPhysicalDevices.empty())
        throw VulkanError("No supporting physical devices found.", VkResult::VK_SUBOPTIMAL_KHR);
    }

    void VulkanEnvironment::selectPhysicalDevice(uint32_t index)
    {
      m_vkState.selectedPhysicalDevice = index;

      VulkanPhysicalDevice physicalDevice = m_vkState.supportedPhysicalDevices.at(m_vkState.selectedPhysicalDevice);

      std::vector<float> queuePriorities{};
      queuePriorities.push_back(1.0f);

      VkDeviceQueueCreateInfo vkDeviceQueueCreateInfo{};
      vkDeviceQueueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      vkDeviceQueueCreateInfo.queueFamilyIndex = physicalDevice.queueFamilies.graphicsQueueFamilyIndices.at(0);
      vkDeviceQueueCreateInfo.queueCount       = queuePriorities.size();
      vkDeviceQueueCreateInfo.pQueuePriorities = queuePriorities.data();
      vkDeviceQueueCreateInfo.flags            = 0;
      vkDeviceQueueCreateInfo.pNext            = nullptr;

      VkDeviceCreateInfo vkDeviceCreateInfo{};
      vkDeviceCreateInfo.sType                = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
      vkDeviceCreateInfo.pQueueCreateInfos    = &vkDeviceQueueCreateInfo;
      vkDeviceCreateInfo.queueCreateInfoCount = 1;
      vkDeviceCreateInfo.pNext                = nullptr;
    }

    EEngineStatus
      VulkanEnvironment::initialize(
        ApplicationEnvironment const& applicationEnvironment,
        WindowHandleWrapper    const& windowHandle)
    {
      try {
        EEngineStatus status = EEngineStatus::Ok;

        createVulkanInstance("ShirabeEngine Demo");
        createVulkanSurface(applicationEnvironment, windowHandle);
        determinePhysicalDevices(Format::R8G8B8A8_UNORM);
        selectPhysicalDevice(0);

        return status;
      }
      catch(VulkanError const&ve) {
        Log::Error(logTag(), String::format("VulkanError (VkResult: %0):\n%1", ve.vulkanResult(), ve.what()));
        return EEngineStatus::Error;
      }
      catch(EngineException const e) {
        Log::Error(logTag(), e.message());
        return e.status();
      }
      catch(std::exception const stde) {
        Log::Error(logTag(), stde.what());
        return EEngineStatus::Error;
      }
      catch(...) {
        Log::Error(logTag(), "Unknown error occurred.");
        return EEngineStatus::Error;
      }
    }

    EEngineStatus
      VulkanEnvironment::deinitialize()
    {

      return EEngineStatus::Ok;
    }



  }
}
