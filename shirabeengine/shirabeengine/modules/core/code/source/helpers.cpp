#include <fstream>
#include <cstring>
#include "core/helpers.h"

namespace engine
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<std::string> executeCmd(std::string const &aCommand)
    {
        std::array<char, 128> buffer {};
        std::string           result {};

        std::string const cmd = aCommand + " 2>&1";

        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
        if (nullptr == pipe)
        {
            CLog::Error(helpers::logTag(), CString::format("Failed to open command pipe for command '%0'", cmd));
            return { EEngineStatus::Error };
        }

        while(not feof(pipe.get()))
        {
            if(nullptr != fgets(buffer.data(), buffer.size(), pipe.get()))
            {
                result.append(buffer.data());
            }
        }

        return { EEngineStatus::Ok, result };
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
    CEngineResult<> writeFile(std::string const &aFilename, std::string const &aData)
    {
        std::ofstream output;

        try
        {
            output.open(aFilename, std::ofstream::out);
            if(not output.good())
            {
                CLog::Error(helpers::logTag(), "Failed to open file '%0' for writing. Error %1.", aFilename, std::strerror(errno));
                return { EEngineStatus::Error };
            }

            output << aData;
            output.close();

            return { EEngineStatus::Ok };
        }
        catch (...)
        {
            output.close();
            return { EEngineStatus::Error };
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
    CEngineResult<> writeFile(std::string const &aFilename, std::vector<int8_t> const &aData)
    {
        std::ofstream output(aFilename, std::ofstream::binary);
        if(output.bad() || output.fail() || output.eof())
        {
            return { EEngineStatus::Error };
        }

        try
        {
            output.write(reinterpret_cast<char const *>(&aData[0]),
                         static_cast<int64_t>(aData.size() * sizeof(int8_t)));
            output.close();

            return { EEngineStatus::Ok };
        }
        catch (...)
        {
            return { EEngineStatus::Error };
        }
    }
    //<-----------------------------------------------------------------------------
}
