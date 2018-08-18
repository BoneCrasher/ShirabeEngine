#ifndef __SHIRABE_WSI_DISPLAY_H__
#define __SHIRABE_WSI_DISPLAY_H__

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include <platform/platform.h>
#include <log/log.h>
#include <core/enginetypehelper.h>
#include <os/osdisplay.h>

namespace engine
{
    namespace wsi
    {

        class CX11Display
        {
            SHIRABE_DECLARE_LOG_TAG(WinAPIDisplay);

        public_methods:
            /**
             * Determine the displays/screens of the operating system.
             *
             * @param aOutPrimaryDisplayIndex The primary screen handle will be stored in this variable.
             * @return                        A list of display descriptors, if any.
             */
            static Vector<os::SOSDisplayDescriptor> getDisplays(uint32_t &aOutPrimaryDisplayIndex)
            {
                using namespace engine::os;

                // Only primary screen
                // int screenWidth       = GetSystemMetrics(SM_CXSCREEN);
                // int screenHeight      = GetSystemMetrics(SM_CYSCREEN);
                // int windowBorderWidth = GetSystemMetrics(SM_CXBORDER);

                Vector<SOSDisplayDescriptor> displayDescriptors;

                Display *x11Display = XOpenDisplay(nullptr);
                if(nullptr != x11Display)
                {
                    uint32_t const x11ScreenCount = XScreenCount(x11Display);

                    for(uint32_t k=0; k<x11ScreenCount; ++k)
                    {
                        int32_t const x11DefaultScreen = XDefaultScreen(x11Display);
                        int32_t const x11ScreenWidth   = XDisplayWidth(x11Display, k);
                        int32_t const x11ScreenHeight  = XDisplayHeight(x11Display, k);

                        SOSDisplayDescriptor descriptor = {};
                        descriptor.bounds.position.x(0);
                        descriptor.bounds.position.y(0);
                        descriptor.bounds.size.x(x11ScreenWidth);
                        descriptor.bounds.size.y(x11ScreenHeight);
                        descriptor.name          = "";
                        descriptor.isPrimary     = (x11DefaultScreen == k);
                        descriptor.monitorHandle = k;

                        if(descriptor.isPrimary)
                        {
                            aOutPrimaryDisplayIndex = k;
                        }

                        displayDescriptors.push_back(descriptor);
                    }

                    XCloseDisplay(x11Display);
                }

                return displayDescriptors;
            }
        };

    }
}

#endif
