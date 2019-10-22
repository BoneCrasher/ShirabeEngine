#ifndef __SHIRABE_VULKAN_IMPORT_H__
#define __SHIRABE_VULKAN_IMPORT_H__

#include <platform/platform.h>

#if defined SHIRABE_PLATFORM_WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR
#elif defined SHIRABE_PLATFORM_LINUX
#define VK_USE_PLATFORM_XLIB_KHR
#endif

#include <vulkan/vulkan.h>

#endif
