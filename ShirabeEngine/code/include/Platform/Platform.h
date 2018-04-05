#ifndef __SHIRABE_PLATFORM_H__
#define __SHIRABE_PLATFORM_H__

// #define SHIRABE_LIBRARY_EXPORT
// #define SHIRABE_TEST_EXPORT

#if defined _WIN32 && defined _WINDOWS
#  define PLATFORM_WINDOWS

// Are we building a DLL?
#  ifdef _WINDLL 
#    ifndef SHIRABE_BUILD_DLL
#     define SHIRABE_BUILD_DLL
#    endif
#  endif
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
#    ifdef SHIRABE_BUILD_DLL
#      define SHIRABE_LIBRARY_EXPORT __declspec(dllexport)
#    else 
#      define SHIRABE_LIBRARY_EXPORT __declspec(dllimport)
#    endif
// This is for test-only exports
#    if defined SHIRABE_BUILD_DLL && defined SHIRABE_TEST
#      define SHIRABE_TEST_EXPORT __declspec(dllexport)
#    else 
#      define SHIRABE_TEST_EXPORT __declspec(dllimport)
#    endif
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