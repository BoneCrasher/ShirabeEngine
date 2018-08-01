#ifndef __SHIRABE_LOG_H__
#define __SHIRABE_LOG_H__

#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <iomanip>
#include <fstream>

#include "Platform/Platform.h"
#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#endif

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#include <math.h>
#include <cmath>

// #include "Platform/ApplicationEnvironment.h"
#include "Core/EngineTypeHelper.h"
#include "Core/BitField.h"

#include "Core/String.h"

namespace Engine {

  enum class ELogLevel {
    Verbose,
    Debug,
    Status,
    Warning,
    Error,
    WTF
  };

  static std::string ELogLevelToString(const ELogLevel& level) {
    if(level == ELogLevel::Verbose) return "VERBOSE";
    if(level == ELogLevel::Debug)   return "DEBUG";
    if(level == ELogLevel::Status)  return "STATUS";
    if(level == ELogLevel::Warning) return "WARNING";
    if(level == ELogLevel::Error)   return "ERROR";
    if(level == ELogLevel::WTF)     return "WTF";
    return "UNKNOWN";
  }

  #ifdef SHIRABE_DEBUG
  #if defined LOG_USE_VERBOSE || defined SHIRABE_TEST
  #define MinimumLogLevel ELogLevel::Verbose
  #else 
  #define MinimumLogLevel ELogLevel::Debug 
  #endif
  #else 
  #ifdef LOG_IGNORE_STATUS
  #ifdef LOG_IGNORE_WARNINGS
  #define MinimumLogLevel ELogLevel::Error
  #else 
  #define MinimumLogLevel ELogLevel::Warning
  #endif
  #else
  #define MinimumLogLevel ELogLevel::Status
  #endif
  #endif

  #define SHIRABE_DECLARE_LOG_TAG(className) constexpr static const char* const kLogTag = #className;         \
								 constexpr static inline const char* logTag() { return kLogTag; }

  using Engine::Core::BitField;

  class SHIRABE_LIBRARY_EXPORT Log {
  public:
    enum class Style
      : unsigned int {
      NO_HEADER = 1,
      LEFT      = 2,
      CENTRE    = 4,
      RIGHT     = 8,
      TRUNCATE  = 16
    };

    static void Verbose(
      std::string     const&tag,
      std::string     const&message);

    static void Status(
      std::string     const&tag,
      std::string     const&message);

    static void Debug(
      std::string     const&tag,
      std::string     const&message);

    static void Warning(
      std::string     const&tag,
      std::string     const&message);

    static void Error(
      std::string     const&tag,
      std::string     const&message);

    static void WTF(
      std::string     const&tag,
      std::string     const&message);

    static unsigned int lineWidth();
    static void setLineWidth(unsigned int width);

  private:
    static void LogImpl(
      ELogLevel       const&type,
      std::string     const&tag,
      std::string     const&message);

    static unsigned int* lineWidthInternal();
  };


  // maximum mumber of lines the output console should have
  constexpr static const WORD MAX_CONSOLE_LINES = 500;

  #ifdef SHIRABE_DEBUG
  class SHIRABE_LIBRARY_EXPORT Console {
  public:
    static void InitializeConsole();
    static void DeinitializeConsole();
  };
  #endif
}

#endif