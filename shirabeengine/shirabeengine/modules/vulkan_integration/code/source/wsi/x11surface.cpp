#include "vulkan/wsi/x11surface.h"

namespace engine
{
    namespace vulkan
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<VkSurfaceKHR> CX11VulkanSurface::create(
                CStdSharedPtr_t<CVulkanEnvironment> const &aVulkanEnvironment,
                CStdSharedPtr_t<CX11Display>        const &aDisplay,
                CStdSharedPtr_t<CX11Window>         const &aWindow)
        {
            Display      *x11Display = reinterpret_cast<Display*>(aDisplay->displayHandle());
            Window const  x11Window  = aWindow->handle();

            CVulkanEnvironment::SVulkanState &state = aVulkanEnvironment->getState();

            VkXlibSurfaceCreateInfoKHR vkXlibSurfaceCreateInfo{};
            vkXlibSurfaceCreateInfo.sType  = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
            vkXlibSurfaceCreateInfo.dpy    = x11Display;
            vkXlibSurfaceCreateInfo.window = x11Window;
            vkXlibSurfaceCreateInfo.flags  = 0;
            vkXlibSurfaceCreateInfo.pNext  = nullptr;

            VkSurfaceKHR surface = VK_NULL_HANDLE;

            auto CreateXLibSurfaceKHR = (PFN_vkCreateXlibSurfaceKHR)vkGetInstanceProcAddr(state.instance, "vkCreateXlibSurfaceKHR");
            if(!CreateXLibSurfaceKHR)
            {
                CLog::Error(logTag(), "Cannot find vulkan function 'vkCreateXlibSurfaceKHR'.");
                return { EEngineStatus::InitializationError };
            }

            VkResult result = CreateXLibSurfaceKHR(state.instance, &vkXlibSurfaceCreateInfo, nullptr, &surface);
            if(VkResult::VK_SUCCESS != result)
            {
                CLog::Error(logTag(), "Failed to create window surface!");
                return { EEngineStatus::InitializationError };
            }

            return { EEngineStatus::Ok, surface };
        }
        //<-----------------------------------------------------------------------------

    }
}
