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

  #define DeclareLogTag(className) constexpr static const char* const kLogTag = #className;         \
								 constexpr static inline const char* logTag() { return kLogTag; }

  using Engine::Core::BitField;

  class Log {
  public:
    enum class Style
      : unsigned int {
      NO_HEADER = 1,
      LEFT      = 2,
      CENTRE    = 4,
      RIGHT     = 8,
      TRUNCATE  = 16
    };

    inline static void Verbose(
      std::string     const&tag,
      std::string     const&message,
      BitField<Style> const&style = Style::LEFT) {
      LogImpl(ELogLevel::Verbose, tag, message, style);
    }

    inline static void Status(
      std::string     const&tag,
      std::string     const&message,
      BitField<Style> const&style = Style::LEFT) {
      LogImpl(ELogLevel::Status, tag, message, style);
    }

    inline static void Debug(
      std::string     const&tag,
      std::string     const&message,
      BitField<Style> const&style = Style::LEFT) {
      LogImpl(ELogLevel::Debug, tag, message, style);
    }

    inline static void Warning(
      std::string     const&tag,
      std::string     const&message,
      BitField<Style> const&style = Style::LEFT) {
      LogImpl(ELogLevel::Warning, tag, message, style);
    }

    inline static void Error(
      std::string     const&tag,
      std::string     const&message,
      BitField<Style> const&style = Style::LEFT) {
      LogImpl(ELogLevel::Error, tag, message, style);
    }

    inline static void WTF(
      std::string     const&tag,
      std::string     const&message,
      BitField<Style> const&style = Style::LEFT) {
      LogImpl(ELogLevel::WTF, tag, message, style);
    }

    static unsigned int lineWidth() { return *lineWidthInternal(); }
    static void setLineWidth(unsigned int width) { *lineWidthInternal() = width; }

  private:
    inline static void LogImpl(
      ELogLevel       const&type,
      std::string     const&tag,
      std::string     const&message,
      BitField<Style> const&style);

    inline static unsigned int* lineWidthInternal() {
      static unsigned int m_lineWidth = 80;

      return &m_lineWidth;
    }
  };

  void Log::LogImpl(
    ELogLevel       const&type,
    std::string     const&tag,
    std::string     const&message,
    BitField<Style> const&style) {
    if(MinimumLogLevel > type)
      // Only print in case of a valid loglevel
      return;

    std::stringstream ss;
    if(!style.check(Style::NO_HEADER)) {
      std::stringstream t; t << "[" << tag << "]";
      ss << "Log::" << std::setw(7) << ELogLevelToString(type) << ": " << std::setw(20) << t.str() << " --> ";
    }

    if(style.check(Style::CENTRE)) {
      uint64_t offset = std::max<uint64_t>(0, ((lineWidth() / 2) - (message.size() / 2)));
      ss << std::setw(offset);
    }
    ss << message << "\n";

    std::string msg = ss.str();

    #ifdef PLATFORM_WINDOWS
    std::wstring wmsg = String::toWideString(msg);
    DWORD       written = 0;
    WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), wmsg.c_str(), ((DWORD)msg.size()), &written, nullptr);
    #else 
    std::cout << msg << std::endl; 
    #endif
  }

  // maximum mumber of lines the output console should have
  constexpr static const WORD MAX_CONSOLE_LINES = 500;
  #ifdef _DEBUG
  static void InitializeConsole() {

    DWORD err = 0;

    int                         consoleHandle;
    HANDLE                      stdHandle;
    CONSOLE_SCREEN_BUFFER_INFO  consoleInformation;
    FILE                       *fp;

    // Allocate a console for this app
    if(!AllocConsole()) {
      // Console already allocated!
      return;
    }

    // if (!AttachConsole(GetCurrentProcessId())) {
    // 	// Failed to attach!
    // 	err = GetLastError();
    // 	return;
    // }

    // Set the screen buffer to be big enough to let us scroll text
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleInformation);
    consoleInformation.dwSize.Y = MAX_CONSOLE_LINES;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), consoleInformation.dwSize);
    SetConsoleActiveScreenBuffer(GetStdHandle(STD_OUTPUT_HANDLE));


    // Redirect unbuffered STDOUT to the console
    stdHandle     = GetStdHandle(STD_OUTPUT_HANDLE);
    consoleHandle = _open_osfhandle(intptr_t(stdHandle), _O_TEXT);
    // Open file to output console buffer
    fp = _fdopen(consoleHandle, "w");
    *stdout = *fp;
    setvbuf(stdout, NULL, _IONBF, 0);

    // Redirect unbuffered STDIN to the console
    stdHandle = GetStdHandle(STD_INPUT_HANDLE);
    consoleHandle = _open_osfhandle(intptr_t(stdHandle), _O_TEXT);
    // Open file to input console buffer
    fp = _fdopen(consoleHandle, "r");
    *stdin = *fp;
    setvbuf(stdin, NULL, _IONBF, 0);

    // Redirect unbuffered STDERR to the console
    stdHandle = GetStdHandle(STD_ERROR_HANDLE);
    consoleHandle = _open_osfhandle(intptr_t(stdHandle), _O_TEXT);
    // Open file to error output console buffer
    fp = _fdopen(consoleHandle, "w");
    *stderr = *fp;
    setvbuf(stderr, NULL, _IONBF, 0);

    // Make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
    // point to console as well
    std::ios::sync_with_stdio(true);
  }

  static void DeinitializeConsole() {

    if(!FreeConsole()) {
      // There was no console...
    }
  }

  #endif
}

#endif