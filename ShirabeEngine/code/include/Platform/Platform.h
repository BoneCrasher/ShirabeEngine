#ifndef __SHIRABE_PLATFORM_H__
#define __SHIRABE_PLATFORM_H__

#define SHIRABE_LIBRARY_EXPORT

#ifdef _WIN32 || _WINDOWS
    #define PLATFORM_WINDOWS
    #ifdef _WIN64
        #define TARGET_64BIT
    #endif
    #ifdef _MSC_VER
        #define COMPILER_MSVC
        #ifdef _WINDLL
          #define SHIRABE_LIBRARY_EXPORT __declspec(dllexport)
        #else 
          #define SHIRABE_LIBRARY_EXPORT __declspec(dllimport)
        #endif
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