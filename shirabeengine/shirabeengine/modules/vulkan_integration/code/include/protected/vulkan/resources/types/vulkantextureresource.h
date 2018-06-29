#ifndef __SHIRABE_VULKAN_TEXTURE_RESOURCE_H__
#define __SHIRABE_VULKAN_TEXTURE_RESOURCE_H__

#include <vulkan/vulkan.h>

namespace Engine {
  namespace Vulkan {

    struct VulkanTextureResource {
      VkImage        handle;
      VkDeviceMemory attachedMemory;
    };

  }
}

#endif