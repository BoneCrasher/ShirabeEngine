#ifndef __SHIRABE_PLATFORM_H__
#define __SHIRABE_PLATFORM_H__

#ifdef _WIN32 || _WINDOWS
    #define PLATFORM_WINDOWS
    #ifdef _WIN64
        #define TARGET_64BIT
    #endif
    #ifdef _MSC_VER
        #define COMPILER_MSVC
    #endif
#else
    #ifdef __linux__
        #define PLATFORM_LINUX
		#ifdef __GNUC__
			#define COMPILER_GCC
		#endif
    #endif
#endif

#ifdef _DEBUG
    #define CONFIGURATION_DEBUG
#else 
    #define CONFIGURATION_RELEASE
#endif

#endif