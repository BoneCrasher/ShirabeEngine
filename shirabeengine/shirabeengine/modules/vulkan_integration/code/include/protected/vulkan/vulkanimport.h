#ifndef __SHIRABE_VULKAN_IMPORT_H__
#define __SHIRABE_VULKAN_IMPORT_H__

#include "Platform/Platform.h"

#ifdef PLATFORM_WINDOWS 
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan/vulkan.h>

namespace engine {
  namespace Vulkan {

  }
}

#endif