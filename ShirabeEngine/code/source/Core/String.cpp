#include "Core/String.h"

namespace Engine {
	std::wstring String::toWideString(const std::string& in) {
		std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>> converter;
		std::wstring wide = converter.from_bytes(in);
		return wide;
	}

	std::string String::toNarrowString(const std::wstring& in) {
		std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>> converter;
		std::string narrow = converter.to_bytes(in);
		return narrow;
	}
}
