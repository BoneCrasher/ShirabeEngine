#ifndef __SHIRABE_OS_DISPLAY_H__
#define __SHIRABE_OS_DISPLAY_H__

#include <stdint.h>
#include <string>

#include <platform/platform.h>
#include <math/geometric/rect.h>

#include "os/osdef.h"

namespace engine
{
    namespace os
    {
        using engine::math::CRect;

        /**
         * Describes an operatin system display by OS specific handle and other attributes.
         */
        struct SHIRABE_LIBRARY_EXPORT SOSDisplayDescriptor
        {
        public_constructors:
            SOSDisplayDescriptor();

        public_members:
            OSHandle_t   monitorHandle;
            std::string  name;
            CRect        bounds;
            bool         isPrimary;
        };

        /**
         * ...
         */
        template <typename TProvider>
        class OSDisplay
                : public TProvider
        {};
    }
}

#endif
