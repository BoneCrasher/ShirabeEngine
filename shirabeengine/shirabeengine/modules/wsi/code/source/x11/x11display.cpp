#include "wsi/x11/x11display.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

namespace engine
{
    namespace wsi
    {
        namespace x11
        {
            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            EEngineStatus CX11Display::initialize()
            {
                Display *x11Display = XOpenDisplay(nullptr);
                if(!x11Display)
                {
                    return EEngineStatus::Error;
                }

                uint64_t const handle = reinterpret_cast<uint64_t>(x11Display);
                setDisplayHandle(handle);

                uint32_t primaryScreenIndex = 0;

                Vector<os::SOSDisplayDescriptor> const screens = getScreenInfo(primaryScreenIndex);
                setScreenInfo(screens);
                setPrimaryScreenIndex(primaryScreenIndex);

                return EEngineStatus::Ok;
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            EEngineStatus CX11Display::deinitialize()
            {
                Display *display = reinterpret_cast<Display*>(displayHandle());
                if(!display)
                {
                    return EEngineStatus::Error;
                }

                XCloseDisplay(display);
                return EEngineStatus::Ok;
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            Vector<os::SOSDisplayDescriptor> CX11Display::getScreenInfo(uint32_t &aOutPrimaryDisplayIndex)
            {
                using namespace engine::os;

                aOutPrimaryDisplayIndex = 0;

                Vector<SOSDisplayDescriptor> displayDescriptors;

                Display *x11Display = reinterpret_cast<Display*>(displayHandle());
                if(!x11Display)
                {
                    return displayDescriptors;
                }

                // Only primary screen
                // int screenWidth       = GetSystemMetrics(SM_CXSCREEN);
                // int screenHeight      = GetSystemMetrics(SM_CYSCREEN);
                // int windowBorderWidth = GetSystemMetrics(SM_CXBORDER);

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
                }

                return displayDescriptors;
            }
            //<-----------------------------------------------------------------------------
        }
    }
}
