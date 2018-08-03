#ifndef __SHIRABE_OS_APPLICATIONENVIRONMENT_H__
#define __SHIRABE_OS_APPLICATIONENVIRONMENT_H__

#include <stdint.h>

#include "Core/EngineTypeHelper.h"
#include "OS/OSDisplay.h"
#include "OS/OSDef.h"

//#ifdef PLATFORM_WINDOWS
//    #ifndef STRICT 
//      #define STRICT // Enhanced type safety including windows header
//    #endif           
//    #define WIN32_LEAN_AND_MEAN // Exclude unnecessary sh**
//    #include <Windows.h>
//#endif 

namespace engine {
  namespace OS {


    struct ApplicationEnvironment {
      OSHandle instanceHandle;
      OSHandle previousInstanceHandle;
      OSHandle primaryWindowHandle;

      uint32_t                    primaryDisplayIndex;
      Vector<OSDisplayDescriptor> osDisplays;

      inline
        OSDisplayDescriptor const&primaryDisplay() const
      {
        return osDisplays[primaryDisplayIndex];
      }
    };

  }
}

#endif 