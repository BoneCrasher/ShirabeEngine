#ifndef __SHIRABE_X11_DISPLAY_H__
#define __SHIRABE_X11_DISPLAY_H__

#include <platform/platform.h>
#include <log/log.h>
#include <core/enginetypehelper.h>
#include <os/osdisplay.h>
#include "wsi/display.h"

namespace engine
{
    namespace wsi
    {

        class CX11Display
                : public CWSIDisplay
        {
            SHIRABE_DECLARE_LOG_TAG(CX11Display);

        public_methods:
            /**
             * Initialize the display, binding to the operating system mechanics for displays,
             * trying to fetch information about the available screens of the display,
             * i.e. connected and activated monitors, which are integrated in the current desktop.
             *
             * @return EEngineStatus::Ok, if successful. An error otherwise.
             */
            virtual EEngineStatus initialize() final;

            /**
             * Clear resources and unbind from the operation system mechanics for displayes.
             *
             * @return EEngineStatus::Ok, if successful. An error otherwise.
             */
            virtual EEngineStatus deinitialize() final;

        private_methods:
            /**
             * Determine the screens of the operating system.
             *
             * @param aOutPrimaryDisplayIndex The primary screen handle will be stored in this variable.
             * @return                        A list of display descriptors, if any.
             */
            virtual Vector<os::SOSDisplayDescriptor> getScreenInfo(uint32_t &aOutPrimaryDisplayIndex) final;
        };

    }
}

#endif
