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

#include <fmt/format.h>
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

    #define SHIRABE_DECLARE_LOG_TAG(className)                                       \
        constexpr static String::value_type const* const kLogTag = TEXT(#className); \
        constexpr static inline String::value_type const* logTag() { return kLogTag; }

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
        static String ELogLevelToString(ELogLevel const &aLevel)
        {
            switch(aLevel)
            {
            case CLog::ELogLevel::Verbose: return TEXT("VERBOSE");
            case CLog::ELogLevel::Debug:   return TEXT("DEBUG");
            case CLog::ELogLevel::Status:  return TEXT("STATUS");
            case CLog::ELogLevel::Warning: return TEXT("WARNING");
            case CLog::ELogLevel::Error:   return TEXT("ERROR");
            case CLog::ELogLevel::WTF:     return TEXT("WTF");
            }
            return TEXT("UNKNOWN");
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
                String const   &aLogTag,
                String const   &aFormat,
                TArguments &&...aArguments)
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
            String const   &aLogTag,
            String const   &aFormat,
            TArguments &&...aArguments)
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
            String const   &aLogTag,
            String const   &aFormat,
            TArguments &&...aArguments)
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
            String const   &aLogTag,
            String const   &aFormat,
            TArguments &&...aArguments)
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
            String const   &aLogTag,
            String const   &aFormat,
            TArguments &&...aArguments)
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
            String const   &aLogTag,
            String const   &aFormat,
            TArguments &&...aArguments)
        {
            LogImpl(ELogLevel::WTF, aLogTag, aFormat, std::forward<TArguments>(aArguments)...);
        }

    private_static_functions:
        template <typename... TArguments>
        static void LogImpl(
            ELogLevel       const &aLevel,
            String const   &aLogTag,
            String const   &aFormat,
            TArguments &&...aArguments)
        {
            std::basic_string_view<String::value_type> view = { aFormat.c_str(), aFormat.size() };
            UnencodedString const message = fmt::vformat(view, fmt::make_format_args(std::forward<TArguments>(aArguments)...));
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
                ELogLevel  const &aLevel,
                String     const &aLogTag,
                String     const &aMessage)
        {
            if(MinimumLogLevel > aLevel)
                // Only print in case of a valid loglevel
                return;

            std::basic_stringstream<String::value_type> ss;
            ss
                << std::setw(7)
                << ELogLevelToString(aLevel) << ": "
                << "[" << aLogTag << "]"     << " --> \n"
                << aMessage      << "\n";

            String const formatted = ss.str();

            #ifdef SHIRABE_PLATFORM_WINDOWS
            #ifdef _UNICODE
                    std::wstring wmsg = StaticStringHelpers::toWideString(msg);
                    DWORD       written = 0;
                    WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), wmsg.c_str(), ((DWORD)msg.size()), &written, nullptr);
            #else
                    DWORD       written = 0;
                    WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), msg.c_str(), ((DWORD)msg.size()), &written, nullptr);
            #endif
            #else
            #if SHIRABEENGINE_STRING_IS_UTF8
            std::cout << reinterpret_cast<const char*>(formatted.c_str()) << std::endl;
            #else
            std::cout << formatted << std::endl;
            #endif
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
