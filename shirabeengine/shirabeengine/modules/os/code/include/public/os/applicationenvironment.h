#ifndef __SHIRABE_OS_APPLICATIONENVIRONMENT_H__
#define __SHIRABE_OS_APPLICATIONENVIRONMENT_H__

#include <stdint.h>

#include <core/enginetypehelper.h>
#include "os/osdisplay.h"
#include "os/osdef.h"

//#ifdef PLATFORM_WINDOWS
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
        struct SApplicationEnvironment
        {
            OSHandle_t instanceHandle;
            OSHandle_t previousInstanceHandle;
            OSHandle_t primaryWindowHandle;

            uint32_t                     primaryDisplayIndex;
            Vector<SOSDisplayDescriptor> osDisplays;

            SHIRABE_INLINE SOSDisplayDescriptor const &primaryDisplay() const
            {
                return osDisplays[primaryDisplayIndex];
            }
        };

    }
}

#endif 
