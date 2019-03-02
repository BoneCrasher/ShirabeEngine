#include <fstream>
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

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    EResult writeFile(std::string const &aFilename, std::string const &aData)
    {
        std::ofstream output;

        try
        {
            output.open(aFilename, std::ofstream::out);
            output << aData;
            output.close();

            return EResult::Success;
        }
        catch (...)
        {
            output.close();
            return EResult::WriteFailed;
        }
    }

    /**
     * Write a byte vector to a file.
     *
     * @param aFilename The filename of the file to write to. Will be overwritten, if extist.
     * @param aData     The data to write.
     * @return          EResult::Success, if successful.
     * @return          EResult::WriteFailed, on error.
     */
    EResult writeFile(std::string const &aFilename, std::vector<int8_t> const &aData)
    {
        std::ofstream output(aFilename, std::ofstream::binary);
        if(output.bad() || output.fail() || output.eof())
        {
            return EResult::WriteFailed;
        }

        try
        {
            output.write(reinterpret_cast<char const *>(&aData[0]),
                         static_cast<int64_t>(aData.size() * sizeof(int8_t)));
            output.close();

            return EResult::Success;
        }
        catch (...)
        {
            return EResult::WriteFailed;
        }
    }
    //<-----------------------------------------------------------------------------
}
