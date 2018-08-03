#include <sstream>

#include "log/log.h"

namespace engine
{
    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    std::string ELogLevelToString(CLog::ELogLevel const &aLevel)
    {
        switch(aLevel)
        {
        case CLog::ELogLevel::Verbose: return "VERBOSE";
        case CLog::ELogLevel::Debug:   return "DEBUG";
        case CLog::ELogLevel::Status:  return "STATUS";
        case CLog::ELogLevel::Warning: return "WARNING";
        case CLog::ELogLevel::Error:   return "ERROR";
        case CLog::ELogLevel::WTF:     return "WTF";
        }
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    void CLog::Verbose(
            std::string const &aLogTag,
            std::string const &aMessage)
    {
        LogImpl(ELogLevel::Verbose, aLogTag, aMessage);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    void CLog::Status(
            std::string const &aLogTag,
            std::string const &aMessage)
    {
        LogImpl(ELogLevel::Status, aLogTag, aMessage);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    void CLog::Debug(
            std::string const &aLogTag,
            std::string const &aMessage)
    {
        LogImpl(ELogLevel::Debug, aLogTag, aMessage);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    void CLog::Warning(
            std::string const &aLogTag,
            std::string const &aMessage)
    {
        LogImpl(ELogLevel::Warning, aLogTag, aMessage);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    void CLog::Error(
            std::string const &aLogTag,
            std::string const &aMessage)
    {
        LogImpl(ELogLevel::Error, aLogTag, aMessage);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    void CLog::WTF(
            std::string const &aLogTag,
            std::string const &aMessage)
    {
        LogImpl(ELogLevel::WTF, aLogTag, aMessage);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    void CLog::LogImpl(
            ELogLevel   const &aLevel,
            std::string const &aLogTag,
            std::string const &aMessage)
    {
        if(MinimumLogLevel > aLevel)
            // Only print in case of a valid loglevel
            return;

        std::stringstream ss;
        ss
            << std::setw(7)  << ELogLevelToString(aLevel) << "): "
            << std::setw(20) << "[" << aLogTag << "]"     << " --> "
            << aMessage      << "\n";

        std::string const formatted = ss.str();

        #ifdef SHIRABE_PLATFORM_WINDOWS
        #ifdef _UNICODE
                std::wstring wmsg = String::toWideString(msg);
                DWORD       written = 0;
                WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), wmsg.c_str(), ((DWORD)msg.size()), &written, nullptr);
        #else
                DWORD       written = 0;
                WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), msg.c_str(), ((DWORD)msg.size()), &written, nullptr);
        #endif
        #else
        std::cout << formatted << std::endl;
        #endif
    }

    #ifdef SHIRABE_PLATFORM_WINDOWS
    void CConsole::InitializeConsole() {

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

    void CConsole::DeinitializeConsole() {

        if(!FreeConsole()) {
            // There was no console...
        }
    }

    #else
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    void CConsole::InitializeConsole()
    {
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    void CConsole::DeinitializeConsole()
    {
    }
    //<-----------------------------------------------------------------------------
    #endif
}


