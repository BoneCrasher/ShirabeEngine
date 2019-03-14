#ifndef   _SHIRABE_SHADERPRECOMP_HELPERS_H_
#define   _SHIRABE_SHADERPRECOMP_HELPERS_H_

#include <string>
#include <filesystem>
#include <memory>
#include <unordered_map>

#include <log/log.h>
#include "result.h"
#include "enginestatus.h"
#include "databuffer.h"

namespace engine
{
    namespace helpers
    {
        SHIRABE_DECLARE_LOG_TAG(ShirabeHelpers);
    }

    /**
     * Execute an external command and fetch the result.
     *
     * @param aCommand
     * @return
     */
    CEngineResult<std::string> executeCmd(std::string const &aCommand);

    /**
     * Helper to extract the numeric representation of the provided flag value
     * of an arbitrary enum class type.
     *
     * @param aEnumFlag The flag to convert to it's numeric representation.
     * @return          See brief.
     */
    template <typename TEnum>
    std::underlying_type_t<TEnum> EnumValueOf(TEnum const &aEnumFlag)
    {
        return static_cast<std::underlying_type_t<TEnum>>(aEnumFlag);
    }

    /**
     * Accept an inline list of options and check if a specific value is contained in this list.
     *
     * @param aOptions
     * @param aCompare
     * @return
     */
    template <typename TValue>
    static bool anyOf(std::vector<TValue> const &&aOptions, TValue const &aCompare)
    {
        return (aOptions.end() != std::find(aOptions.begin(), aOptions.end(), aCompare));
    }

    /**
     * Accept an inline assignment and check, whether an assignment for a specific key exists.
     * Return it's value.
     *
     * @param aExtension
     * @param aOptions
     * @return
     */
    template <typename TKey, typename TValue>
    static std::enable_if_t<std::is_default_constructible_v<TValue>, TValue> const mapValue(TKey const &aExtension, std::unordered_map<TKey, TValue> const &&aOptions)
    {
        bool const contained = (aOptions.end() != aOptions.find(aExtension));
        if(not contained)
        {
            return TValue();
        }
        else
        {
            return aOptions.at(aExtension);
        }
    }

    /**
     * Read a file into a string.
     *
     * @param aFileName Filename of the file to read.
     * @return          See brief.
     */
    std::string readFile(std::string const &aFileName);

    /**
     * Write a string to a file.
     *
     * @param aFilename The filename of the file to write to. Will be overwritten, if extist.
     * @param aData     The data to write.
     * @return          EResult::Success, if successful.
     * @return          EResult::WriteFailed, on error.
     */
    CEngineResult<> writeFile(std::string const &aFilename, std::string const &aData);

    /**
     * Write a byte vector to a file.
     *
     * @param aFilename The filename of the file to write to. Will be overwritten, if extist.
     * @param aData     The data to write.
     * @return          EResult::Success, if successful.
     * @return          EResult::WriteFailed, on error.
     */
    CEngineResult<> writeFile(std::string const &aFilename, std::vector<int8_t> const &aData);

    /**
     * Write a byte buffer to a file.
     *
     * @param aFilename The filename of the file to write to. Will be overwritten, if extist.
     * @param aData     The data to write.
     * @return          EResult::Success, if successful.
     * @return          EResult::WriteFailed, on error.
     */
    CEngineResult<> writeFile(std::string const &aFilename, ByteBuffer const &aData);
}

#endif // _SHIRABE_SHADERPRECOMP_HELPERS_H_
