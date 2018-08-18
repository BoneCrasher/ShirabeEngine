#ifndef __SHIRABE_OS_APPLICATIONENVIRONMENT_H__
#define __SHIRABE_OS_APPLICATIONENVIRONMENT_H__

#include <stdint.h>

#include <core/enginetypehelper.h>
#include "os/osdisplay.h"
#include "os/osdef.h"

//#ifdef SHIRABE_PLATFORM_WINDOWS
//    #ifndef STRICT
//      #define STRICT // Enhanced type safety including windows header
//    #endif
//    #define WIN32_LEAN_AND_MEAN // Exclude unnecessary sh**
//    #include <Windows.h>
//#endif

namespace engine
{
    namespace os
    {
        /**
         * The SApplicationEnvironment struct encapsulates information about the
         * running process, operatin system integration as well as current operating
         * system hardware state like monitors, selected resolutions etc...
         */
        struct SApplicationEnvironment
        {
        public_methods:
            SHIRABE_INLINE SOSDisplayDescriptor const &primaryDisplay() const
            {
                return osDisplays[primaryDisplayIndex];
            }

        public_members:
            OSHandle_t instanceHandle;
            OSHandle_t previousInstanceHandle;

            uint32_t                     primaryDisplayIndex;
            Vector<SOSDisplayDescriptor> osDisplays;
        };

    }
}

#endif
