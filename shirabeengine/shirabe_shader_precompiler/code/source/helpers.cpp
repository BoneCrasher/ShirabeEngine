#include "helpers.h"

namespace shader_precompiler
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CResult<std::string> executeCmd(std::string const &aCommand)
    {
        std::array<char, 128> buffer {};
        std::string           result {};

        std::string const cmd = aCommand + " 2>&1";

        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
        if (nullptr == pipe)
        {
            CLog::Error(shader_precompiler::logTag(), CString::format("Failed to open command pipe for command '%0'", cmd));
            return { false };
        }

        while(not feof(pipe.get()))
        {
            if(nullptr != fgets(buffer.data(), buffer.size(), pipe.get()))
            {
                result.append(buffer.data());
            }
        }

        return result;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    std::string readFile(std::string const &aFileName)
    {
        bool const fileExists = std::filesystem::exists(aFileName);
        if(not fileExists)
        {
            return std::string();
        }

        std::ifstream inputFileStream(aFileName);
        bool const inputStreamOk = inputFileStream.operator bool();
        if(not inputStreamOk)
        {
            return std::string();
        }

        std::string inputData((std::istreambuf_iterator<char>(inputFileStream)),
                               std::istreambuf_iterator<char>());

        return inputData;
    }
    //<-----------------------------------------------------------------------------
}
