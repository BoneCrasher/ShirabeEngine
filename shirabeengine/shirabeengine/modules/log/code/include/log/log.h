#ifndef __SHIRABE_LOG_H__
#define __SHIRABE_LOG_H__

#include <cstdio>
#include <fcntl.h>
//#include <io.h>
#include <iostream>
#include <iomanip>
#include <fstream>

#include <platform/platform.h>
#include <base/declaration.h>
#include <base/string.h>

#ifdef SHIRABE_PLATFORM_WINDOWS
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

namespace engine
{
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

    #define SHIRABE_DECLARE_LOG_TAG(className)                                 \
        constexpr static  char const* const kLogTag = #className;              \
        constexpr static inline char const* logTag() { return kLogTag; }

    /**
     * Static entry point for any kind of log-calls.
     */
    class SHIRABE_LIBRARY_EXPORT CLog
    {
        public_enums:
        /**
         * The ELogLevel describes the specific kind of severity of the log
         * output in increasing order from Verbose to WTF.
         */
        enum class ELogLevel
                : int8_t
        {
            Verbose =  1,
            Debug   =  2,
            Status  =  4,
            Warning =  8,
            Error   = 16,
            WTF     = 32
        };

    public_static_functions:
        /**
         * Convert a log level to it's string representation.
         *
         * @param aLevel The loglevel to convert.
         * @return       A string representation of the provided log level.
         */
        static std::string ELogLevelToString(ELogLevel const &aLevel)
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
            return "UNKNOWN";
        }

        /**
         * Log-call for very descriptive output, usually just for
         * testing or profiling.
         *
         * @param aLogTag  The tag of the source of the log call to associate with.
         * @param aMessage -
         */
        template <typename... TArguments>
        static void Verbose(
                std::string const &aLogTag,
                std::string const &aFormat,
                TArguments    &&...aArguments)
        {
            LogImpl(ELogLevel::Verbose, aLogTag, aFormat, std::forward<TArguments>(aArguments)...);
        }

        /**
         * Log-call to trace events and status in the system.
         *
         * @param aLogTag  The tag of the source of the log call to associate with.
         * @param aMessage -
         */
        template <typename... TArguments>
        static void Status(
                std::string const &aLogTag,
                std::string const &aFormat,
                TArguments    &&...aArguments)
        {
            LogImpl(ELogLevel::Status, aLogTag, aFormat, std::forward<TArguments>(aArguments)...);
        }

        /**
         * Log-call for debug output, usally printing state and
         * values or intermediate positional output.
         *
         * @param aLogTag  The tag of the source of the log call to associate with.
         * @param aMessage -
         */
        template <typename... TArguments>
        static void Debug(
                std::string const &aLogTag,
                std::string const &aFormat,
                TArguments    &&...aArguments)
        {
            LogImpl(ELogLevel::Debug, aLogTag, aFormat, std::forward<TArguments>(aArguments)...);
        }

        /**
         * Log-call for abnormal state or behaviour, which can be
         * caught or recovered but require attention.
         *
         * @param aLogTag  The tag of the source of the log call to associate with.
         * @param aMessage -
         */
        template <typename... TArguments>
        static void Warning(
                std::string const &aLogTag,
                std::string const &aFormat,
                TArguments    &&...aArguments)
        {
            LogImpl(ELogLevel::Warning, aLogTag, aFormat, std::forward<TArguments>(aArguments)...);
        }

        /**
         * Log-call for abnormal state or behaviour, which requires
         * handling or cleanup and abort.
         *
         * @param aLogTag  The tag of the source of the log call to associate with.
         * @param aMessage -
         */
        template <typename... TArguments>
        static void Error(
                std::string const &aLogTag,
                std::string const &aFormat,
                TArguments    &&...aArguments)
        {
            LogImpl(ELogLevel::Error, aLogTag, aFormat, std::forward<TArguments>(aArguments)...);
        }

        /**
         * Log-call for errors that should never have happened.
         *
         * @param aLogTag  The tag of the source of the log call to associate with.
         * @param aMessage -
         */
        template <typename... TArguments>
        static void WTF(
                std::string const &aLogTag,
                std::string const &aFormat,
                TArguments    &&...aArguments)
        {
            LogImpl(ELogLevel::WTF, aLogTag, aFormat, std::forward<TArguments>(aArguments)...);
        }

    private_static_functions:
        template <typename... TArguments>
        static void LogImpl(
                ELogLevel       const &aLevel,
                std::string     const &aLogTag,
                std::string     const &aFormat,
                TArguments       &&...aArguments)
        {
            std::string const message = CString::format(aFormat, std::forward<TArguments>(aArguments)...);
            LogImpl(aLevel, aLogTag, message);
        }

        /**
         * Common implementation of log output generation.
         * Forwarded to by all other log-calls.
         *
         * @param aLevel   The specific severity/type of output.
         * @param aLogTag  The tag of the source of the log call to associate with.
         * @param aMessage -
         */
        static void LogImpl(
                ELogLevel       const &aLevel,
                std::string     const &aLogTag,
                std::string     const &aMessage)
        {
            if(MinimumLogLevel > aLevel)
                // Only print in case of a valid loglevel
                return;

            std::stringstream ss;
            ss
                << std::setw(7)
                << ELogLevelToString(aLevel) << ": "
                << "[" << aLogTag << "]"     << " --> \n"
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
    };

    /**
     * The CConsole class provides static entry points to create
     * and shutdown additional console instances in case of a GUI
     * application, which requires STDOUT, STDIN and STDERR output.
     */
    class SHIRABE_LIBRARY_EXPORT CConsole
    {
    public:
        /**
         * Create, initialize and bind a console instance to the running
         * application and prepare it for STDOUT/IN/ERR I/O.
         */
        static void InitializeConsole() { }
        /**
         * Unbind, deinitialize and destroy a console instance from a running
         * application, if any.
         */
        static void DeinitializeConsole() { }
    };
}

#endif
