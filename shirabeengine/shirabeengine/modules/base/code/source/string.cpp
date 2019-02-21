#include "base/string.h"

namespace engine
{

    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    std::wstring CString::widen(std::string const &aInput)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

        std::wstring const widened = converter.from_bytes(aInput);
        return widened;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    std::wstring CString::widen(char *aInput)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

        std::wstring const widened = converter.from_bytes(aInput);
        return widened;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    std::string CString::narrow(std::wstring const &aInput)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

        std::string const narrowed = converter.to_bytes(aInput);
        return narrowed;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    std::string CString::narrow(wchar_t *aInput)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

        std::string const narrowed = converter.to_bytes(aInput);
        return narrowed;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    std::vector<std::string> CString::split(std::string const &aInput, char const aDelimiter)
    {
        std::vector<std::string> tokens;
        std::string              token;

        std::istringstream tokenStream(aInput);
        while (std::getline(tokenStream, token, aDelimiter))
        {
           tokens.push_back(token);
        }

        return tokens;
    }
    //<-----------------------------------------------------------------------------
}
