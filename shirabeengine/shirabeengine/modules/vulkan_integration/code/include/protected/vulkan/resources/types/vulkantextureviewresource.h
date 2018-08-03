#ifndef __SHIRABE_VULKAN_TEXTUREVIEW_RESOURCE_H__
#define __SHIRABE_VULKAN_TEXTUREVIEW_RESOURCE_H__

#include <vulkan/vulkan.h>

namespace engine {
  namespace Vulkan {

    struct VulkanTextureViewResource {
      VkImageView handle;
    };

  }
}

#endif