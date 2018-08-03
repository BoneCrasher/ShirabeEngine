#include <functional>
#include <set>

#include "Platform/Platform.h"
#include "OS/ApplicationEnvironment.h"
#include "Core/String.h"
#include "GraphicsAPI/Resources/Types/SwapChain.h"
#include "Vulkan/VulkanEnvironment.h"
#include "Vulkan/VulkanDeviceCapabilities.h"
#include "Vulkan/VulkanImport.h"

namespace engine {
  namespace Vulkan {
    using namespace engine::OS;
    using namespace engine::GFXAPI;

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

    VulkanEnvironment::VulkanQueueFamilyRegistry::VulkanQueueFamilyRegistry()
      : supportingQueueFamilyIndices()
      , graphicsQueueFamilyIndices()
      , computeQueueFamilyIndices()
      , transferQueueFamilyIndices()
      , presentQueueFamilyIndices()
    {}

    VulkanEnvironment::VulkanPhysicalDevice::VulkanPhysicalDevice()
      : handle(VK_NULL_HANDLE)
      , properties({})
      , features({})
      , memoryProperties({})
      , queueFamilies()
    {}

    VulkanEnvironment::VulkanSwapChain::VulkanSwapChain()
      : handle(VK_NULL_HANDLE)
      , capabilities({})
      , supportedFormats()
      , supportedPresentModes()
      , selectedExtents({ 0, 0 })
      , selectedFormat()
      , selectedPresentMode()
    {}

    VulkanEnvironment::VulkanState::VulkanState()
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

    namespace Debug {
      SHIRABE_DECLARE_LOG_TAG(ValidationLayers);
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL __vkValidationLayerReportCallback(
      VkDebugReportFlagsEXT      flags,
      VkDebugReportObjectTypeEXT objType,
      uint64_t                   obj,
      size_t                     location,
      int32_t                    code,
      const char*                layerPrefix,
      const char*                msg,
      void*                      userData) 
    {

      std::string message =
        String::format(
          "[%0][%1(%2)]@'%3' -> (%4) in layer %5:\n%6",
          flags, objType, (void*)obj, location, code, layerPrefix, msg
        );
      Log::Debug(Debug::logTag(), message);

      return VK_FALSE;
    }

    /**
     * \fn  void VulkanEnvironment::createVulkanInstance(std::string const&name)
     *
     * \brief Creates vulkan instance
     *
     * \exception VulkanError Raised when a Vulkan error condition occurs.
     *
     * \param name  The name.
     **************************************************************************************************/
    void VulkanEnvironment::createVulkanInstance(std::string const&name)
    {
      VkApplicationInfo vkApplicationInfo{ };
      vkApplicationInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
      vkApplicationInfo.pApplicationName   = name.c_str();
      vkApplicationInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
      vkApplicationInfo.pEngineName        = "Shirabe Engine";
      vkApplicationInfo.engineVersion      = VK_MAKE_VERSION(0, 0, 1);
      vkApplicationInfo.apiVersion         = VK_API_VERSION_1_1;

      std::vector<char const*> layers{
        #ifdef SHIRABE_DEBUG
        "VK_LAYER_LUNARG_standard_validation"
        #endif
      };
      std::vector<char const*> extensions ={
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
        #ifdef SHIRABE_DEBUG 
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME
        #endif
      };

      uint32_t instanceLayerCount     = 0;
      uint32_t instanceExtensionCount = 0;

      //
      // Check Layer support
      // 
      std::vector<VkLayerProperties> supportedLayers{ };

      vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);
      supportedLayers.resize(instanceLayerCount);
      vkEnumerateInstanceLayerProperties(&instanceLayerCount, supportedLayers.data());

      std::set<std::string> layersCopy(layers.begin(), layers.end());
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

      std::set<std::string> extensionsCopy(extensions.begin(), extensions.end());
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

      VkInstance instance = VK_NULL_HANDLE;

      VkResult result = vkCreateInstance(&vkInstanceCreatInfo, nullptr, &instance);
      if(VK_SUCCESS != result) {
        throw VulkanError("Failed to create VkInstance.", result);
      }

      m_vkState.instanceLayers      = layers;
      m_vkState.instanceExtensions  = extensions;
      m_vkState.instanceCreateInfo  = vkInstanceCreatInfo;
      m_vkState.instance            = instance;

      //
      // In Debug-mode, hook-in a validation layer report callback
      // 
      #ifdef SHIRABE_DEBUG
      int reportFlags =
        VK_DEBUG_REPORT_ERROR_BIT_EXT       | 
        VK_DEBUG_REPORT_WARNING_BIT_EXT     |
        VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
        VK_DEBUG_REPORT_DEBUG_BIT_EXT       |
        VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT
        ;

      VkDebugReportCallbackCreateInfoEXT vkDebugReportCallbackCreateInfo ={ };
      vkDebugReportCallbackCreateInfo.sType       = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
      vkDebugReportCallbackCreateInfo.flags       = reportFlags;
      vkDebugReportCallbackCreateInfo.pfnCallback = __vkValidationLayerReportCallback;

      PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT =
        (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
      if(!vkCreateDebugReportCallbackEXT)
        throw VulkanError("Cannot load vulkan extension function 'vkCreateDebugReportCallbackEXT'", VkResult::VK_ERROR_INITIALIZATION_FAILED);

      VkDebugReportCallbackEXT vkDebugReportCallback = 0;
      result = vkCreateDebugReportCallbackEXT(instance, &vkDebugReportCallbackCreateInfo, nullptr, &vkDebugReportCallback);
      if(VkResult::VK_SUCCESS != result)
        throw VulkanError("Cannot hook-in debug report callback.", VkResult::VK_ERROR_INITIALIZATION_FAILED);

      m_vkState.debugReportCallback = vkDebugReportCallback;
      #endif
    }

    /**
     * \fn  void VulkanEnvironment::createVulkanSurface( Platform::ApplicationEnvironment const&appEnvironment, WindowHandleWrapper const&handleWrapper)
     *
     * \brief Creates vulkan surface
     *
     * \exception VulkanError Raised when a Vulkan error condition occurs.
     *
     * \param appEnvironment  The application environment.
     * \param handleWrapper   The handle wrapper.
     **************************************************************************************************/
    void VulkanEnvironment::createVulkanSurface(
      OS::ApplicationEnvironment const&appEnvironment)
    {
      VkSurfaceKHR surface{};

      #if defined PLATFORM_WINDOWS
      VkWin32SurfaceCreateInfoKHR vkWin32SurfaceCreateInfo{};
      vkWin32SurfaceCreateInfo.sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
      vkWin32SurfaceCreateInfo.hwnd      = reinterpret_cast<HWND>(appEnvironment.primaryWindowHandle);
      vkWin32SurfaceCreateInfo.hinstance = reinterpret_cast<HINSTANCE>(appEnvironment.instanceHandle);
      vkWin32SurfaceCreateInfo.flags     = 0;
      vkWin32SurfaceCreateInfo.pNext     = nullptr;

      auto CreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)vkGetInstanceProcAddr(m_vkState.instance, "vkCreateWin32SurfaceKHR");
      if(!CreateWin32SurfaceKHR)
        throw VulkanError("Cannot find vulkan function 'vkCreateWin32SurfaceKHR'.", VkResult::VK_ERROR_INITIALIZATION_FAILED);

      VkResult result = CreateWin32SurfaceKHR(m_vkState.instance, &vkWin32SurfaceCreateInfo, nullptr, &surface);
      if(VkResult::VK_SUCCESS != result) {
        throw VulkanError("Failed to create window surface!", result);
      }
      #endif

      m_vkState.surface = surface;
    }

    /**
     * \fn  void VulkanEnvironment::determinePhysicalDevices(Format const&requiredBackBufferFormat)
     *
     * \brief Determine physical devices
     *
     * \exception VulkanError Raised when a Vulkan error condition occurs.
     *
     * \param requiredBackBufferFormat  The required back buffer format.
     **************************************************************************************************/
    void VulkanEnvironment::determinePhysicalDevices()
    {
      VkResult result = VkResult::VK_SUCCESS;

      std::vector<char const*> requiredLayers{};
      std::vector<char const*> requiredExtensions{};
      requiredExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

      std::vector<VulkanPhysicalDevice> supportedPhysicalDevices{ };

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

          std::set<std::string> extensions(requiredExtensions.begin(), requiredExtensions.end());

          for(VkExtensionProperties const&extension : supportedExtensions)
            extensions.erase(extension.extensionName);

          if(!extensions.empty()) {
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

          supportedPhysicalDevices.push_back(physicalDevice);
        }
      }

      if(supportedPhysicalDevices.empty())
        throw VulkanError("No supporting physical devices found.", VkResult::VK_SUBOPTIMAL_KHR);

      m_vkState.deviceLayers             = requiredLayers;
      m_vkState.deviceExtensions         = requiredExtensions;
      m_vkState.supportedPhysicalDevices = supportedPhysicalDevices;
    }

    /**
     * \fn  void VulkanEnvironment::selectPhysicalDevice(uint32_t index)
     *
     * \brief Select physical device
     *
     * \param index Zero-based index of the.
     **************************************************************************************************/
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
      vkDeviceCreateInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
      vkDeviceCreateInfo.enabledLayerCount       = m_vkState.deviceLayers.size();
      vkDeviceCreateInfo.ppEnabledLayerNames     = m_vkState.deviceLayers.data();
      vkDeviceCreateInfo.enabledExtensionCount   = m_vkState.deviceExtensions.size();
      vkDeviceCreateInfo.ppEnabledExtensionNames = m_vkState.deviceExtensions.data();
      vkDeviceCreateInfo.pEnabledFeatures        = &(physicalDevice.features);
      vkDeviceCreateInfo.pQueueCreateInfos       = &vkDeviceQueueCreateInfo;
      vkDeviceCreateInfo.queueCreateInfoCount    = 1;
      vkDeviceCreateInfo.flags                   = 0;
      vkDeviceCreateInfo.pNext                   = nullptr;

      VkDevice vkLogicalDevice = VK_NULL_HANDLE;
      VkResult result = vkCreateDevice(physicalDevice.handle, &vkDeviceCreateInfo, nullptr, &vkLogicalDevice);
      if(VkResult::VK_SUCCESS != result)
        throw VulkanError(String::format("Failed to create logical device for physical device at index %0.", index), result);

      m_vkState.selectedLogicalDevice = vkLogicalDevice;
    }

    void VulkanEnvironment::createSwapChain(
      Rect            const&requestedBackBufferSize,
      VkFormat        const&requestedFormat,
      VkColorSpaceKHR const&colorSpace)
    {
      VulkanPhysicalDevice const&vkPhysicalDevice = m_vkState.supportedPhysicalDevices.at(m_vkState.selectedPhysicalDevice);
      VkSurfaceKHR         const&vkSurface        = m_vkState.surface;

      // 
      // Extract capabilities
      // 
      VkSurfaceCapabilitiesKHR vkSurfaceCapabilities{};

      VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevice.handle, vkSurface, &vkSurfaceCapabilities);
      if(VkResult::VK_SUCCESS != result)
        throw VulkanError("Failed to get surface capabilities for physical device and surface.", result);
      // 
      // Determine backbuffer extents
      // 
      VkExtent2D vkBackBufferExtents{};
      // Test, whether either the width or height in currentExtent were set to uint32_t::max.
      // In this case, we can differ from the window size and enforce our requested backbuffer size.
      if(vkSurfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        vkBackBufferExtents = vkSurfaceCapabilities.currentExtent;
      }
      else {
        bool requestedBackBufferSizeSupported =
          (vkSurfaceCapabilities.minImageExtent.width  <= requestedBackBufferSize.size.x()) &&
          (vkSurfaceCapabilities.minImageExtent.height <= requestedBackBufferSize.size.y()) &&
          (vkSurfaceCapabilities.maxImageExtent.width  >= requestedBackBufferSize.size.x()) &&
          (vkSurfaceCapabilities.maxImageExtent.height >= requestedBackBufferSize.size.y());
        if(!requestedBackBufferSizeSupported)
          Log::Warning(
            logTag(),
            String::format(
              "Requested backbuffer extents %0 x %1 unsupported. Clamping the extents.",
              requestedBackBufferSize.size.x(),
              requestedBackBufferSize.size.y()));

        VkExtent2D actualExtent ={ requestedBackBufferSize.size.x(), requestedBackBufferSize.size.y() };
        actualExtent.width  = std::max(vkSurfaceCapabilities.minImageExtent.width, std::min(vkSurfaceCapabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(vkSurfaceCapabilities.minImageExtent.height, std::min(vkSurfaceCapabilities.maxImageExtent.height, actualExtent.height));

        vkBackBufferExtents = actualExtent;
      }

      //
      // Extract Formats
      //
      uint32_t                        vkSurfaceFormatCount = 0;
      std::vector<VkSurfaceFormatKHR> vkSurfaceFormats{};

      result = vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice.handle, vkSurface, &vkSurfaceFormatCount, nullptr);
      if(VkResult::VK_SUCCESS != result)
        throw VulkanError("Failed to query the supported surface formats.", result);

      if(vkSurfaceFormatCount == 0)
        throw VulkanError("No supported surface formats.", VkResult::VK_ERROR_INITIALIZATION_FAILED);

      vkSurfaceFormats.resize(vkSurfaceFormatCount);
      vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice.handle, vkSurface, &vkSurfaceFormatCount, vkSurfaceFormats.data());

      //
      // Extract PresentModes
      //
      uint32_t                      vkSurfacePresentModeCount = 0;
      std::vector<VkPresentModeKHR> vkSurfacePresentModes{ };

      result = vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice.handle, vkSurface, &vkSurfacePresentModeCount, nullptr);
      if(VkResult::VK_SUCCESS != result)
        throw VulkanError("Failed to query the supported surface formats.", result);

      if(vkSurfacePresentModeCount == 0)
        throw VulkanError("No supported surface present modes.", VkResult::VK_ERROR_INITIALIZATION_FAILED);

      vkSurfacePresentModes.resize(vkSurfacePresentModeCount);
      vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice.handle, vkSurface, &vkSurfacePresentModeCount, vkSurfacePresentModes.data());

      // 
      // Determine format, color space
      // 
      VkSurfaceFormatKHR vkSelectedFormat{};

      if(vkSurfaceFormats.size() == 1 && vkSurfaceFormats.at(0).format == VK_FORMAT_UNDEFINED) {
        // We can use any format desired.
        vkSelectedFormat ={ requestedFormat, VK_COLORSPACE_SRGB_NONLINEAR_KHR };
      }
      else {
        bool found = false;
        for(VkSurfaceFormatKHR const&surfaceFormat : vkSurfaceFormats) {
          found = (surfaceFormat.format == requestedFormat);
          if(found) {
            vkSelectedFormat = surfaceFormat;
            break;
          }
        }

        // No immediately suitable format found. Take the first available.
        if(!found)
          vkSelectedFormat = vkSurfaceFormats.at(0);
      }

      // 
      // Determine present mode 
      // 
      VkPresentModeKHR vkSelectedPresentMode = VK_PRESENT_MODE_FIFO_KHR; // Guaranteed to be available.
      for(VkPresentModeKHR const&presentMode : vkSurfacePresentModes) {
        // Try to find a mailbox present mode, as it will allow us to implement triple buffering
        if(presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
          vkSelectedPresentMode = presentMode;
          break;
        }
        // Although Khronos recommends to fall back to immediate mode, let's try FIFO. 
        // Maybe the drivers support it properly by now. Nobody wants visible tearing...
        //   else {
        //     vkSelectedPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        //   }
      }

      // 
      // Create the swap chain
      // 
      VkSwapchainKHR vkSwapChain = VK_NULL_HANDLE;

      // Should give us triple buffering with fallback to double buffering...
      uint32_t swapChainImageCount = (vkSurfaceCapabilities.minImageCount + 1);
      if(vkSurfaceCapabilities.maxImageCount > 0) // We have a limited amount of images possible. Clamp!
        swapChainImageCount = std::min(vkSurfaceCapabilities.maxImageCount, swapChainImageCount);

      VkSwapchainCreateInfoKHR vkSwapChainCreateInfo{};
      vkSwapChainCreateInfo.sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
      vkSwapChainCreateInfo.surface               = m_vkState.surface;
      vkSwapChainCreateInfo.minImageCount         = swapChainImageCount;
      vkSwapChainCreateInfo.imageFormat           = vkSelectedFormat.format;
      vkSwapChainCreateInfo.imageColorSpace       = vkSelectedFormat.colorSpace;
      vkSwapChainCreateInfo.imageExtent           = vkBackBufferExtents;
      vkSwapChainCreateInfo.imageArrayLayers      = 1;
      vkSwapChainCreateInfo.imageUsage            = VK_IMAGE_USAGE_TRANSFER_DST_BIT; // Image will be moved into...
      vkSwapChainCreateInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
      vkSwapChainCreateInfo.queueFamilyIndexCount = 0; // Optional due to sharing mode exclusive!
      vkSwapChainCreateInfo.pQueueFamilyIndices   = nullptr;
      vkSwapChainCreateInfo.preTransform          = vkSurfaceCapabilities.currentTransform;
      vkSwapChainCreateInfo.compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
      vkSwapChainCreateInfo.presentMode           = vkSelectedPresentMode;
      vkSwapChainCreateInfo.flags                 = 0;
      vkSwapChainCreateInfo.clipped               = VK_TRUE;
      vkSwapChainCreateInfo.oldSwapchain          = m_vkState.swapChain.handle;
      vkSwapChainCreateInfo.pNext                 = nullptr;
      

      VulkanQueueFamilyRegistry const&queueFamilies = vkPhysicalDevice.queueFamilies;

      std::vector<uint32_t> supportedGraphicsQueueFamilyIndices{};

      VkBool32 supported = VK_FALSE;
      for(uint32_t k=0; k<queueFamilies.graphicsQueueFamilyIndices.size(); ++k)
      {
        uint32_t index  = queueFamilies.graphicsQueueFamilyIndices.at(k);
        VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice.handle, index, vkSurface, &supported);
        if(VkResult::VK_SUCCESS != result)
          throw VulkanError("Failed to check for surface support.", result);

        if(VK_TRUE == supported) {
          supportedGraphicsQueueFamilyIndices.push_back(index);
          supported = VK_FALSE;
        }
      }

      if(supportedGraphicsQueueFamilyIndices.empty())
        throw VulkanError("No supported queue family indices found which support the swapchain on the given surface.", VkResult::VK_ERROR_INITIALIZATION_FAILED);

      result = vkCreateSwapchainKHR(m_vkState.selectedLogicalDevice, &vkSwapChainCreateInfo, nullptr, &vkSwapChain);
      if(VkResult::VK_SUCCESS != result)
        throw VulkanError("Failed to create swapchain.", result);
      
      // 
      // Finally: Extract SwapChain images
      //
      uint32_t             createdSwapChainImageCount = 0;
      std::vector<VkImage> swapChainImages;

      result = vkGetSwapchainImagesKHR(m_vkState.selectedLogicalDevice, vkSwapChain, &createdSwapChainImageCount, nullptr);
      if(VkResult::VK_SUCCESS != result)
        throw VulkanError("Failed to fetch swapchain image handles.", result);
      
      swapChainImages.resize(createdSwapChainImageCount);
      vkGetSwapchainImagesKHR(m_vkState.selectedLogicalDevice, vkSwapChain, &createdSwapChainImageCount, swapChainImages.data());
      
      // 
      // Apply to state 
      //
      VulkanSwapChain swapChain{ };
      swapChain.capabilities          = vkSurfaceCapabilities;
      swapChain.supportedFormats      = vkSurfaceFormats;
      swapChain.supportedPresentModes = vkSurfacePresentModes;
      swapChain.selectedExtents       = vkBackBufferExtents;
      swapChain.selectedFormat        = vkSelectedFormat;
      swapChain.selectedPresentMode   = vkSelectedPresentMode;
      swapChain.swapChainImages       = swapChainImages;
      swapChain.handle                = vkSwapChain;

      m_vkState.swapChain = swapChain;
    }

    EEngineStatus
      VulkanEnvironment::initialize(
        ApplicationEnvironment const& applicationEnvironment)
    {
      try {
        EEngineStatus status = EEngineStatus::Ok;

        VkFormat requiredFormat = VulkanDeviceCapsHelper::convertFormatToVk(Format::R8G8B8A8_UNORM);

        createVulkanInstance("ShirabeEngine Demo");
        createVulkanSurface(applicationEnvironment);
        determinePhysicalDevices();
        selectPhysicalDevice(0);
        createSwapChain(
          applicationEnvironment.primaryDisplay().bounds, 
          requiredFormat, 
          VK_COLORSPACE_SRGB_NONLINEAR_KHR);

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
      // Wait for the logical device to finish up all work.
      vkDeviceWaitIdle(m_vkState.selectedLogicalDevice);
      // Kill it with fire...
      vkDestroyDevice(m_vkState.selectedLogicalDevice, nullptr);

      #ifdef SHIRABE_DEBUG
      // Remember to destroy the debug report callback...
      PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT =
        (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(m_vkState.instance, "vkDestroyDebugReportCallbackEXT");
      if(!vkDestroyDebugReportCallbackEXT)
        Log::Warning(logTag(), "Failed to fetch vulkan extension function 'vkDestroyDebugReportCallbackEXT'");
      else
        vkDestroyDebugReportCallbackEXT(m_vkState.instance, m_vkState.debugReportCallback, nullptr);
      #endif

      vkDestroyInstance(m_vkState.instance, nullptr);

      return EEngineStatus::Ok;
    }

    VkQueue VulkanEnvironment::getGraphicsQueue()
    {
      VulkanPhysicalDevice const&physicalDevice = m_vkState.supportedPhysicalDevices.at(m_vkState.selectedPhysicalDevice);

      VkQueue queue = VK_NULL_HANDLE;
      vkGetDeviceQueue(m_vkState.selectedLogicalDevice, physicalDevice.queueFamilies.graphicsQueueFamilyIndices.at(0), 0, &queue);

      return queue;
    }
    
    VulkanEnvironment::VulkanState& 
      VulkanEnvironment::getState()
    {
      return m_vkState;
    }
  }
}
