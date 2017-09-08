#ifndef __SHIRABE_STRING_H__
#define __SHIRABE_STRING_H__

#include <type_traits>
#include <string>
#include <locale>
#include <codecvt>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <array>

namespace Engine {

	class String {
	public:

		/**********************************************************************************************//**
		 * \fn	template <typename... TArgs> static std::string String::format(const std::string& format, TArgs&&... args);
		 *
		 * \brief	Formats a string using format and a variadic number of arguments 
		 * 			with different types.
		 *
		 * \tparam	TArgs	Type of the arguments.
		 * \param	format	Describes the format to use.
		 * \param	args  	Variable arguments providing [in,out] The arguments.
		 *
		 * \return	The formatted value.
		 **************************************************************************************************/
		template <typename... TArgs>
		static std::string format(const std::string& format, TArgs&&... args);

		/**********************************************************************************************//**
		 * \fn	static std::wstring String::toWideString(const std::string& in);
		 *
		 * \brief	Converts a narrow char-based string into its wide wchar_t based version.
		 *
		 * \param	in	The in.
		 *
		 * \return	In as a std::wstring.
		 **************************************************************************************************/
		static std::wstring toWideString(const std::string& in);

		/**********************************************************************************************//**
		 * \fn	static std::string String::toNarrowString(const std::wstring& in);
		 *
		 * \brief	Converts a wide wchar_t-based string into its narrow char based version, omitting
		 * 			unsupported wide-characters.
		 *
		 * \param	in	The in.
		 *
		 * \return	In as a std::string.
		 **************************************************************************************************/
		static std::string  toNarrowString(const std::wstring& in);

	private:
		template <typename TArg, typename enable = void>
		inline static std::string toString(const TArg& arg) {
			std::ostringstream stream;
			stream << arg;
			return stream.str();
		}

		template <
			typename TArg, 
			typename std::enable_if<std::is_floating_point<TArg>::value, TArg>::type
		>
		inline static std::string toString(const float& arg) {
			std::ostringstream stream;
			stream << std::setprecision(12) << arg;
			return stream.str();
		}

		template <std::size_t argCount>
		inline static void formatArguments(std::array<std::string, argCount>&) {
			// No forwarded arguments. ArgCount should be 0.
		}

		template <std::size_t argCount, typename TArg, typename... TArgs>
		inline static void formatArguments(
			std::array<std::string, argCount>     &outFormatted,
			TArg                                 &&inArg,
			TArgs                             &&...inArgs) {
			// Executed for each, recursively until there's no param left.
			outFormatted[argCount - 1 - sizeof...(TArgs)] = toString(inArg);
			formatArguments(outFormatted, std::forward<TArgs>(inArgs)...);
		}
	};

	template <typename... TArgs>
	std::string String::format(
		const std::string     &format,
		TArgs             &&...args) 
	{
		std::array<std::string, sizeof...(args)> formattedArguments;
		formatArguments(formattedArguments, std::forward<TArgs>(args)...);

		std::ostringstream stream;
		for (std::size_t k = 0; k < format.size(); ++k) {
			switch (format[k]) {
			case '%':
				// Can handle up to 10 entries indexed from 0..9! TODO: Replace with arbitrary number functionality
				if ((k + 1) < format.size()                                          // Next index within format string?
					&& std::size_t(format[k + 1] - '0') < formattedArguments.size()) // Provided argument index after % within passed argument count (0..9)?
				{
					stream << formattedArguments[std::size_t(format[++k] - '0')];
					break;
				}
			default:
				stream << format[k];
				break;
			}
		}

		return stream.str();
	}
}

#endif