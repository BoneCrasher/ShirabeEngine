#ifndef __SHIRABE_APPLICATIONENVIRONMENT_H__
#define __SHIRABE_APPLICATIONENVIRONMENT_H__

#include "Platform/OSDisplay.h"

#ifdef PLATFORM_WINDOWS
    #define STRICT              // Enhanced type safety including windows header
    #define WIN32_LEAN_AND_MEAN // Exclude unnecessary sh**
    #include <Windows.h>
#endif 

namespace Platform {

	struct ApplicationEnvironment {
#ifdef PLATFORM_WINDOWS
		HINSTANCE instanceHandle;
		HINSTANCE previousInstanceHandle;
		HWND      primaryWindowHandle;
#endif // PLATFORM_WINDOWS
		OSDisplayDescriptorList osDisplays;
	};

}

#endif 