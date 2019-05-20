#include <unordered_map>
#include <filesystem>
#include <functional>

#include <log/log.h>
#include <core/helpers.h>
#include <util/crc32.h>

namespace Main
{
    SHIRABE_DECLARE_LOG_TAG(ApplicationMain)
}

#if defined SHIRABE_PLATFORM_WINDOWS
int WINAPI WinMain(
        HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        PSTR      szCmdLine,
        int       iCmdShow)
#elif defined SHIRABE_PLATFORM_LINUX
int main(int aArgC, char **aArgV)
#endif
{
    using namespace engine;

  #ifdef _DEBUG
    CConsole::InitializeConsole();
  #endif

    std::vector<std::string> usableArguments(aArgV + 1, aArgV + aArgC);
    std::filesystem::path    path = {};

    //
    // Process all options provided to the application.
    //
    auto const processor = [&] (std::string const &aArgument) -> bool
    {
        //
        // All options do have the format: <option>[=|:]<value>
        //

        auto const extract = [&aArgument] () -> std::tuple<bool, std::string, std::string>
        {
            std::string option = std::string();
            std::string value  = std::string();

            std::string::size_type const separatorPosition = aArgument.find_first_of("=:");
            if(not (std::string::npos == separatorPosition))
            {
                value = aArgument.substr(separatorPosition + 1, std::string::npos);
            }

            option = aArgument.substr(0, separatorPosition);

            return { true, option, value };
        };

        auto const [valid, option, value] = extract();
        if(not valid)
        {
            return false;
        }

        std::string const referencableValue = value;

        std::unordered_map<std::string, std::function<bool()>> handlers =
        {
            { "-i", [&] () { path = referencableValue; return true; }},
        };

        auto const fn = mapValue<std::string, std::function<bool()>>(option, std::move(handlers));
        if(fn)
        {
            return fn();
        }
        else
        {
            return false;
        }
    };
    std::for_each(usableArguments.begin(), usableArguments.end(), processor);

    std::cout << util::crc32FromString(path) << "\n";

  #ifdef _DEBUG
    CConsole::DeinitializeConsole();
  #endif

    return 0;
}
