#ifndef __SHIRABE_X11SURFACE_H__
#define __SHIRABE_X11SURFACE_H__

#include <base/declaration.h>
#include <core/enginetypehelper.h>
#include <wsi/x11/x11display.h>
#include <wsi/x11/x11window.h>
#include <vulkan/vulkanimport.h>
#include <vulkan/vulkanenvironment.h>

namespace engine
{
    namespace vulkan
    {
        using engine::wsi::x11::CX11Display;
        using engine::wsi::x11::CX11Window;

        /**
         * The CX11VulkanSurface class wraps the create method to create a VkSurfaceKHR from a X11 environment.
         */
        class CX11VulkanSurface
        {
        public_static_functions:
            /**
             * Create an X11 based VkSurfaceKHR.
             *
             * @param aVulkanEnvironment The Vulkan Instance and environment to construct the surface in.
             * @param aDisplay           The X11 display to bind to.
             * @param aWindow            The X11 window handle to bind to.
             * @return                   A VkSurfaceKHR handle, successful. Throws otherwise.
             * @throw CVulkanError       An instance of CVulkanError containing an error message and VkResult code.
             */
            static VkSurfaceKHR create(
                    CStdSharedPtr_t<CVulkanEnvironment> const &aVulkanEnvironment,
                    CStdSharedPtr_t<CX11Display>        const &aDisplay,
                    CStdSharedPtr_t<CX11Window>         const &aWindow);

        private_constructors:
            CX11VulkanSurface() = delete;
            CX11VulkanSurface(CX11VulkanSurface const&) = delete;
            CX11VulkanSurface(CX11VulkanSurface &&)     = delete;
        private_destructors:
            ~CX11VulkanSurface() = delete;

         private_operators:
            CX11VulkanSurface &operator=(CX11VulkanSurface const&) = delete;
            CX11VulkanSurface &operator=(CX11VulkanSurface &&)     = delete;
        };
    }
}

#endif // X11SURFACE_H
