#ifndef __SHIRABE_LOG_H__
#define __SHIRABE_LOG_H__

#include <stdio.h>
#include <fcntl.h>
//#include <io.h>
#include <iostream>
#include <iomanip>
#include <fstream>

#include <platform/platform.h>
#include <base/declaration.h>

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
        constexpr static inline char const* const logTag() { return kLogTag; }

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
        static std::string ELogLevelToString(
                ELogLevel const &aLevel);

        /**
         * Log-call for very descriptive output, usually just for
         * testing or profiling.
         *
         * @param aLogTag  The tag of the source of the log call to associate with.
         * @param aMessage -
         */
        static void Verbose(
                std::string const &aLogTag,
                std::string const &aMessage);

        /**
         * Log-call to trace events and status in the system.
         *
         * @param aLogTag  The tag of the source of the log call to associate with.
         * @param aMessage -
         */
        static void Status(
                std::string const &aLogTag,
                std::string const &aMessage);
        /**
         * Log-call for debug output, usally printing state and
         * values or intermediate positional output.
         *
         * @param aLogTag  The tag of the source of the log call to associate with.
         * @param aMessage -
         */
        static void Debug(
                std::string const &aLogTag,
                std::string const &aMessage);
        /**
         * Log-call for abnormal state or behaviour, which can be
         * caught or recovered but require attention.
         *
         * @param aLogTag  The tag of the source of the log call to associate with.
         * @param aMessage -
         */
        static void Warning(
                std::string const &aLogTag,
                std::string const &aMessage);
        /**
         * Log-call for abnormal state or behaviour, which requires
         * handling or cleanup and abort.
         *
         * @param aLogTag  The tag of the source of the log call to associate with.
         * @param aMessage -
         */
        static void Error(
                std::string const &aLogTag,
                std::string const &aMessage);
        /**
         * Log-call for errors that should never have happened.
         *
         * @param aLogTag  The tag of the source of the log call to associate with.
         * @param aMessage -
         */
        static void WTF(
                std::string const &aLogTag,
                std::string const &aMessage);

    private_static_functions:
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
                std::string     const &aMessage);
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
        static void InitializeConsole();
        /**
         * Unbind, deinitialize and destroy a console instance from a running
         * application, if any.
         */
        static void DeinitializeConsole();
    };
}

#endif
