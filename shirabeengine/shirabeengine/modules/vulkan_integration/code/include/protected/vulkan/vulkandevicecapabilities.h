#ifndef __SHIRABE_VULKAN_DEVICECAPABILITIES_H__
#define __SHIRABE_VULKAN_DEVICECAPABILITIES_H__

#include <vulkan/vulkan.h>

#include "Core/BasicTypes.h"
#include "Core/EngineStatus.h"
#include "Core/EngineTypeHelper.h"
#include "Core/BitField.h"
#include "Log/Log.h"

#include "GraphicsAPI/Resources/Types/Definition.h"

// #include "GFXAPI/Capabilities.h"


namespace Engine {
  namespace Vulkan {

    using Engine::Resources::Format;
    using Engine::Core::BitField;

    class VulkanDeviceCapsHelper {
      DeclareLogTag(VulkanDeviceCapsHelper)
    public:

      static VkFormat convertFormatToVk(const Format& fmt);
      static Format   convertFormatFromVk(const VkFormat& fmt);
      static uint32_t determineMemoryType(
        VkPhysicalDevice      const&physicalDevice,
        uint32_t              const&requiredType,
        VkMemoryPropertyFlags const&properties)
      {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        for(uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
          uint32_t typeFlag = (requiredType & (1 << i));
          bool     propFlag = ((memProperties.memoryTypes[i].propertyFlags & properties) == properties);
          if(typeFlag && propFlag)
            return i;
        }

        throw std::runtime_error("failed to find suitable memory type!");
      }
    };

  }
}

#endif