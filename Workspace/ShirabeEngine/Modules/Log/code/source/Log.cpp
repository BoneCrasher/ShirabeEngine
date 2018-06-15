#include <sstream>

#include "Log/Log.h"

namespace Engine {

  void Log::Verbose(
    std::string     const&tag,
    std::string     const&message) {
    LogImpl(ELogLevel::Verbose, tag, message);
  }

  void Log::Status(
    std::string     const&tag,
    std::string     const&message) {
    LogImpl(ELogLevel::Status, tag, message);
  }

  void Log::Debug(
    std::string     const&tag,
    std::string     const&message) {
    LogImpl(ELogLevel::Debug, tag, message);
  }

  void Log::Warning(
    std::string     const&tag,
    std::string     const&message) {
    LogImpl(ELogLevel::Warning, tag, message);
  }

  void Log::Error(
    std::string     const&tag,
    std::string     const&message) {
    LogImpl(ELogLevel::Error, tag, message);
  }

  void Log::WTF(
    std::string     const&tag,
    std::string     const&message) {
    LogImpl(ELogLevel::WTF, tag, message);
  }

  unsigned int Log::lineWidth() { return *lineWidthInternal(); }
  void Log::setLineWidth(unsigned int width) { *lineWidthInternal() = width; }
  
  unsigned int* Log::lineWidthInternal() {
    static unsigned int m_lineWidth = 80;

    return &m_lineWidth;
  }

  void Log::LogImpl(
    ELogLevel       const&type,
    std::string     const&tag,
    std::string     const&message) 
  {
    if(MinimumLogLevel > type)
      // Only print in case of a valid loglevel
      return;

    std::stringstream ss;
    /*if(!style.check(Style::NO_HEADER)) {
      std::stringstream t; t << "[" << tag << "]";
      ss << "Log::" << std::setw(7) << ELogLevelToString(type) << ": " << std::setw(20) << t.str() << " --> ";
    }

    if(style.check(Style::CENTRE)) {
      uint64_t offset = std::max<uint64_t>(0, ((lineWidth() / 2) - (message.size() / 2)));
      ss << std::setw(offset);
    }*/
    ss << message << "\n";

    std::string msg = ss.str();

    #ifdef PLATFORM_WINDOWS
    #ifdef _UNICODE
    std::wstring wmsg = String::toWideString(msg);
    DWORD       written = 0;
    WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), wmsg.c_str(), ((DWORD)msg.size()), &written, nullptr);
    #else 
    DWORD       written = 0;
    WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), msg.c_str(), ((DWORD)msg.size()), &written, nullptr);
    #endif
    #else 
    std::cout << msg << std::endl;
    #endif
  }

   void Console::InitializeConsole() {

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

  void Console::DeinitializeConsole() {

    if(!FreeConsole()) {
      // There was no console...
    }
  }

}