#ifndef __SHIRABE_PLATFORM_H__
#define __SHIRABE_PLATFORM_H__

//#ifdef SHIRABE_BUILD_DLL
#include "ShirabeEngine_Export.h"

#ifdef SHIRABE_TEST
#include "ShirabeEngine_TestExport.h"
#endif
//#endif

#ifndef SHIRABEENGINE_TEST_EXPORT 
  #define SHIRABE_TEST_EXPORT
#else 
  #define SHIRABE_TEST_EXPORT SHIRABEENGINE_TEST_EXPORT
#endif

#ifndef SHIRABEENGINE_LIBRARY_EXPORT 
  #define SHIRABE_LIBRARY_EXPORT
#else 
  #define SHIRABE_LIBRARY_EXPORT SHIRABEENGINE_LIBRARY_EXPORT
#endif

// #define SHIRABE_LIBRARY_EXPORT
// #define SHIRABE_TEST_EXPORT

#if defined _WIN32
#  define SHIRABE_PLATFORM_WINDOWS

// Check for address model
#  ifdef _WIN64
#    define TARGET_64BIT
#  else 
#    define TARGET_32BIT
#  endif
// Go on and check for the compiler and define related settings
#  ifdef _MSC_VER
#    define COMPILER_MSVC
// This is for public exports
#  else
#    ifdef __linux__
#      define PLATFORM_LINUX
#      ifdef __GNUC__
#        define COMPILER_GCC
#      endif
#    endif
#  endif

#  ifdef _DEBUG
#    define CONFIGURATION_DEBUG
#  else 
#    define CONFIGURATION_RELEASE
#  endif
#endif

#endif