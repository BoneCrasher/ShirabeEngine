#ifndef __SHIRABE_STRING_H__
#define __SHIRABE_STRING_H__

#include <type_traits>
#include <string>
// codecvt: Safe due to http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0618r0.html
// "Move to Annex D, until safe replacement is provided"
// #define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include <codecvt>
#include <locale>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <array>

#include <base/declaration.h>

namespace engine
{
    /**
     * The CString class provides helpers to convert 8 and 16-bit
     * strings to each other or format a string with a variadic number
     * of arguments.
     */
    class CString
    {
        public_static_functions:
        /**
         * Format a string based on 'aFormat' with a variadic number of arbitrarily typed arguments.
         *
         * @param aFormat
         * @param aArguments
         * @return
         */
        template <typename... TArgs>
        static std::string format(
                std::string const&aFormat,
                TArgs        &&...aArguments);

        /**
         * Convert an 8-bit char array to a 16-bit string.
         *
         * @param aInput
         * @return
         */
        static std::wstring widen(char *aInput);
        /**
         * Convert an 8-bit string to a 16-bit string.
         *
         * @param aInput
         * @return
         */
        static std::wstring widen(std::string const &aInput);

        /**
         * Convert a 16-bit char array to an 8-bit string.
         *
         * @param aInput
         * @return
         */
        static std::string  narrow(wchar_t *aInput);
        /**
         * Convert a 16-bit string to an 8-bit string.
         *
         * @param aInput
         * @return
         */
        static std::string  narrow(std::wstring const &aInput);

        private_static_functions:
        /**
         * Accept an arbitrarily typed argument and convert it to it's proper
         * string representation.
         *
         * @tparam TArg
         * @tparam TEnable
         * @param aArg
         * @return
         */
        template <
                typename TArg,
                typename TEnable = void
                >
        static std::string toString(TArg const &aArg);

        /**
         * Accept a float argument and convert it to it's proper string representation.
         *
         * @tparam TArg
         * @param arg
         * @return
         */
        template <
                typename TArg,
                typename std::enable_if<std::is_floating_point<TArg>::value, TArg>::type
                >
        static std::string toString(const float& arg);

        /**
         * Format a list of arguments. In this case zero arguments as the abort-condition
         * of the recursive expansion of the parameter pack.
         *
         * @param aArguments
         */
        template <std::size_t NArgs>
        static void formatArguments(std::array<std::string, NArgs> const &aArguments);

        /**
         * Format a list of arguments of arbitrary type and expand recursively.
         *
         * @param outFormatted
         * @param inArg
         * @param inArgs
         */
        template <
                std::size_t NArgs,
                typename    TArg,
                typename... TArgs
                >
        static void formatArguments(
                std::array<std::string, NArgs>     &aOutFormatted,
                TArg                              &&aInArg,
                TArgs                          &&...aInArgs);
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <typename... TArgs>
    std::string CString::format(
            const std::string     &aFormat,
            TArgs             &&...aArgs)
    {
        std::array<std::string, sizeof...(aArgs)> formattedArguments{};

        formatArguments(formattedArguments, std::forward<TArgs>(aArgs)...);

        if constexpr (sizeof...(aArgs) == 0)
        {
            return aFormat;
        }
        else {
            uint32_t number     = 0;
            bool     readNumber = false;

            std::ostringstream stream;

            for(std::size_t k = 0; k < aFormat.size(); ++k)
            {
                switch(aFormat[k])
                {
                case '%':
                    readNumber = true;
                    break;
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    if(readNumber)
                    {
                        number *= 10;
                        number += static_cast<uint32_t>(aFormat[k] - '0');
                        break;
                    }
                default:
                    if(readNumber)
                    {
                        stream << formattedArguments[std::size_t(number)];
                        readNumber = false;
                        number     = 0;
                    }

                    stream << aFormat[k];
                    break;
                }
            }

            if(readNumber)
            {
                stream << formattedArguments[std::size_t(number)];
                readNumber = false;
                number     = 0;
            }

            return stream.str();
        }
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <typename TArg, typename enable>
    std::string CString::toString(TArg const &aArg)
    {
        std::ostringstream stream;
        stream << aArg;
        return stream.str();
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <
            typename TArg,
            typename std::enable_if<std::is_floating_point<TArg>::value, TArg>::type
            >
    std::string CString::toString(const float& arg) {
        std::ostringstream stream;
        stream << std::setprecision(12) << arg;
        return stream.str();
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <std::size_t argCount>
    void CString::formatArguments(std::array<std::string, argCount> const&aArgs)
    {
        SHIRABE_UNUSED(aArgs);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <std::size_t argCount, typename TArg, typename... TArgs>
    void CString::formatArguments(
            std::array<std::string, argCount>     &outFormatted,
            TArg                                 &&inArg,
            TArgs                             &&...inArgs)
    {
        // Executed for each, recursively until there's no param left.
        uint32_t const index = (argCount - 1 - sizeof...(TArgs));
        outFormatted[index] = toString(inArg);

        formatArguments(outFormatted, std::forward<TArgs>(inArgs)...);
    }
    //<-----------------------------------------------------------------------------
}

#endif
