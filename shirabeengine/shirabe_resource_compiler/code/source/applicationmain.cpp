#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iterator>
#include <filesystem>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <cmath>
#include <array>
#include <map>
#include <memory>
#include <thread>
#include <functional>
#include <sstream>

#include <fx/gltf.h>

#include <platform/platform.h>

#ifdef SHIRABE_PLATFORM_LINUX
#include <execinfo.h>
#include <cstdlib>
#include <unistd.h>
#endif

#include <log/log.h>
#include <base/string.h>
#include <core/bitfield.h>
#include <core/enginetypehelper.h>
#include <core/result.h>
#include <core/helpers.h>
#include <material/serialization.h>
#include <material/declaration.h>
#include <util/documents/json.h>

#include "common/config.h"
#include "common/definition.h"
#include "materials/materialprocessor.h"

using namespace engine;
using namespace engine::material;
using namespace engine::documents;
using engine::CResult;
using namespace resource_compiler;

//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------

/**
 * Print the proper usage of this tool.
 */
[[noreturn]]
void usage()
{
    using namespace engine;

    std::string const usageMessage =
            "Usage:                                                                                                  \n"
            "  ./shirabe_resource_compiler <options>                                                                 \n"
            "                                                                                                        \n"
            "Options:                                                                                                \n"
            "  --verbose                                                                                             \n"
            "      Values: true or false                                                                             \n"
            "      Effect: Enables verbose output while doing the job.                                               \n"
            "  --debug                                                                                               \n"
            "      Effect: Enable debug output.                                                                      \n"
            "  --optimize                                                                                            \n"
            "      Effect: Optimize the shader.                                                                      \n"
            "  --recursive_scan                                                                                      \n"
            "      Effect: If any of the paths in the -i option is a directory, include                              \n"
            "              all subdirectories in the input file search.                                              \n"
            "  -o=<dirpath>                                                                                          \n"
            "      Effect: Specifies the path of a directory where all output files should be stored relatively      \n"
            "  -i=<filepath>                                                                                         \n"
            "      Effect: Specifies the path of the index to write out.                                             \n"
            "                                                                                                        \n"
            " Example:                                                                                               \n"
            "                                                                                                        \n";

    CLog::Warning(resource_compiler::logTag(), usageMessage);

    ::exit(EnumValueOf(EResult::WrongUsage));
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------

/**
 * @brief The CPrecompiler class
 */
class CCompiler
{
    SHIRABE_DECLARE_LOG_TAG(CCompiler);
public_methods:

    CResult<EResult> initialize()
    {
        return EResult::Success;
    }

    /**
     * @brief ProcessArguments
     * @param aArgC
     * @param aArgV
     * @return
     */
    CResult<EResult> processArguments(uint32_t const aArgC, char **aArgV)
    {
        std::vector<std::string> usableArguments(aArgV + 1, aArgV + aArgC);

        core::CBitField<EOptions>          options        = {};
        std::vector<std::filesystem::path> includePaths   = {};
        std::vector<std::filesystem::path> filesToProcess = {};
        std::filesystem::path              inputPath      = {};
        std::filesystem::path              outputPath     = {};

        // std::string                        dataFile                = {};
        // std::vector<std::filesystem::path> includePaths            = {};
        // std::filesystem::path              inputIndexPath          = {};
        // std::filesystem::path              inputPath               = {};
        // std::filesystem::path              outputPath              = {};
        // std::filesystem::path              outputModulePath        = {};
        // std::filesystem::path              outputIndexPath         = {};
        // std::filesystem::path              outputSignaturePath     = {};
        // std::filesystem::path              outputConfigurationPath = {};

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
                { "--help",           [&] () { usage();                                                return true; }},
                { "--verbose",        [&] () { options.set(EOptions::VerboseOutput);                return true; }},
                { "--debug",          [&] () { options.set(EOptions::DebugMode);                    return true; }},
                { "--optimize",       [&] () { options.set(EOptions::OptimizationEnabled);          return true; }},
                { "--recursive_scan", [&] () { options.set(EOptions::RecursiveScan);                return true; }},
                // { "-I",               [&] () { includePaths.push_back(referencableValue);                  return true; }},
                { "-i" ,              [&] () { inputPath  = referencableValue;                          return true; }},
                { "-o",               [&] () { outputPath = referencableValue;                          return true; }},
                // { "-om",              [&] () { outputModulePath        = referencableValue;                return true; }},
                // { "-oi",              [&] () { outputIndexPath         = referencableValue;                return true; }},
                // { "-os",              [&] () { outputSignaturePath     = referencableValue;                return true; }},
                // { "-oc",              [&] () { outputConfigurationPath = referencableValue;                return true; }},
                // { "-i",               [&] () { inputIndexPath          = referencableValue;                return true; }},
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

        //
        // Format the absolute input path used to scan for resources...
        //
        std::filesystem::path const inputPathAbs = (std::filesystem::current_path() / inputPath).lexically_normal();
        bool const inputDirectoryExists = std::filesystem::exists(inputPathAbs);
        if(not inputDirectoryExists)
        {
            return false;
        }

        //
        // Format the absolute output path used to save out resources to...
        //
        std::filesystem::path const outputPathAbs = (std::filesystem::current_path() / outputPath).lexically_normal();
        bool const outputDirectoryExists = std::filesystem::exists(outputPathAbs);
        if(not outputDirectoryExists)
        {
            std::filesystem::create_directories(outputPathAbs);
        }

        //
        // Recursively scan the input path for all files, filter them against a list of valid files and
        // store their relative paths for processing...
        //
        auto const dirIterator = std::filesystem::recursive_directory_iterator(inputPathAbs);
        for(auto const &file : dirIterator)
        {
            // We can't process directories, can we?
            bool const isDirectory = std::filesystem::is_directory(file);
            if(isDirectory)
            {
                continue;
            }

            std::filesystem::path const extension = file.path().extension();

            // We only accept files with extension
            bool const extensionIsEmpty = (extension.string().empty());
            if(extensionIsEmpty)
            {
                continue;
            }

            // Check for supported extensions
            std::vector<std::filesystem::path> validExtensions =
                                                       {
                                                              ".material"
                                                            , ".materialinstance"
                                                            , ".gltf"
                                                       };
            if(validExtensions.end() == std::find(validExtensions.begin(), validExtensions.end(), extension))
            {
                continue;
            }

            std::filesystem::path const relativePath = std::filesystem::relative(file.path().lexically_normal());
            filesToProcess.push_back(relativePath);
        }

        //
        // Make sure to properly prepend the include paths
        //
        includePaths.emplace_back(std::filesystem::path("materials/include"));
        for(auto &includePath : includePaths)
        {
            includePath = std::filesystem::current_path()/inputPath/includePath;
        }

        SConfiguration config {};
        config.options             = options;
        config.inputPath           = inputPath .lexically_normal();
        config.outputPath          = outputPath.lexically_normal();
        config.includePaths        = includePaths;
        config.filesToProcess      = filesToProcess;
        // config.indexFile           = index;
        // config.inputPaths          = inputFiles;
        // config.moduleOutputPath    = outputModulePath       .lexically_normal();
        // config.indexOutputFile     = outputIndexPath        .lexically_normal();
        // config.signatureOutputFile = outputSignaturePath    .lexically_normal();
        // config.configOutputFile    = outputConfigurationPath.lexically_normal();
//
        // config.indexFile.signatureAssetUid     = asset::assetIdFromUri(outputSignaturePath);
        // config.indexFile.configurationAssetUid = asset::assetIdFromUri(outputConfigurationPath);

        mConfig = config;

        return EResult::Success;
    }


    /**
     * Run the shader precompiler on the identified input items.
     *
     * @return EResult::Success      if successful.
     * @return EResult::InputInvalid on error.
     */
    CResult<EResult> run()
    {
        for(auto const &file : mConfig.filesToProcess)
        {
            std::filesystem::path const extension = file.extension();
            if(".material" == extension)
            {
                auto const &[result, code] = materials::processMaterial(file);
                if(EResult::Success != code)
                {
                    CLog::Error(logTag(), "Failed to process material file w/ name {}", file.string());
                    continue;
                }
            }
            else if(".materialinstance" == extension)
            {
                // auto const &[result, code] = processMaterialInstance(file);
                // if(EResult::Success != code)
                // {
                //     CLog::Error(logTag(), "Failed to process material instance w/ name {}", file.string());
                //     continue;
                // }
            }
            else if(".gltf" == extension)
            {

            }
            else
            {
                continue; // ...
            }
        }

        // Regenerate asset index...
        std::vector<asset::SAsset> processedAssets {};
        auto const processedFilesIterator = std::filesystem::recursive_directory_iterator(mConfig.outputPath);
        for(auto const &file : processedFilesIterator)
        {
            std::filesystem::path const filePath  = file.path();
            std::filesystem::path const extension = filePath.extension();

            asset::SAsset a {};

            std::cout << extension.string() << std::endl;

            if(".meta" == extension)
            {
                a.type    = asset::EAssetType::Material;
                a.subtype = asset::EAssetSubtype::Meta;
                a.uri     = std::filesystem::relative(filePath, (std::filesystem::current_path() / mConfig.outputPath));
                a.id      = asset::assetIdFromUri(a.uri);
                processedAssets.push_back(a);
            }
            else if(".signature" == extension)
            {
                a.type    = asset::EAssetType::Material;
                a.subtype = asset::EAssetSubtype::Signature;
                a.uri = std::filesystem::relative(filePath, (std::filesystem::current_path() / mConfig.outputPath));
                a.id  = asset::assetIdFromUri(a.uri);
                processedAssets.push_back(a);
            }
            else if(".config" == extension)
            {
                a.type    = asset::EAssetType::Material;
                a.subtype = asset::EAssetSubtype::Config;
                a.uri = std::filesystem::relative(filePath, (std::filesystem::current_path() / mConfig.outputPath));
                a.id  = asset::assetIdFromUri(a.uri);
                processedAssets.push_back(a);
            }
            else if(".spv" == extension)
            {
                a.type    = asset::EAssetType::Material;
                a.subtype = asset::EAssetSubtype::SPVModule;
                a.uri     = std::filesystem::relative(filePath, (std::filesystem::current_path() / mConfig.outputPath));
                a.id      = asset::assetIdFromUri(a.uri);
                processedAssets.push_back(a);
            }
        }

        std::stringstream ss;
        ss << "<Index>\n";
        for(auto const &a : processedAssets)
        {
            ss << CString::format("<Asset aid=\"{}\" parent_aid=\"0\" type=\"{}\" subtype=\"{}\" uri=\"{}\"></Asset>\n"
                                  , a.id
                                  , convert_to_string<asset::EAssetType>(a.type)
                                  , convert_to_string<asset::EAssetSubtype>(a.subtype)
                                  , a.uri.string());
        }
        ss << "</Index>";
        writeFile(mConfig.outputPath / "game.assetindex.xml", ss.str());

        return EResult::Success;
    }

    EResult deinitialize()
    {
        return EResult::Success;
    }

private_methods:
    fx::gltf::Document const __loadFromData(ByteBuffer const &aInputBuffer)
    {
        try
        {
            nlohmann::json json;
            {
                std::string const  data = std::string(reinterpret_cast<char const*>(aInputBuffer.data()));
                std::istringstream stream(data);
                stream >> json;
            }

            return fx::gltf::detail::Create(json, { });
        }
        catch (fx::gltf::invalid_gltf_document &)
        {
            throw;
        }
        catch (std::system_error &)
        {
            throw;
        }
        catch (...)
        {
            std::throw_with_nested(fx::gltf::invalid_gltf_document("Invalid glTF document. See nested exception for details."));
        }
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    struct SBufferInfo
    {
        fx::gltf::Accessor const *accessor;

        uint8_t const *data;
        uint32_t       dataStride;
        uint32_t       totalSize;

        [[nodiscard]]
        bool hasData() const noexcept
        {
            return (nullptr != data);
        }
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    static uint32_t __calculateDataTypeSize(fx::gltf::Accessor const &aAccessor) noexcept
    {
        uint32_t elementSize = 0;
        switch (aAccessor.componentType)
        {
            case fx::gltf::Accessor::ComponentType::Byte:
            case fx::gltf::Accessor::ComponentType::UnsignedByte:
                elementSize = 1;
                break;
            case fx::gltf::Accessor::ComponentType::Short:
            case fx::gltf::Accessor::ComponentType::UnsignedShort:
                elementSize = 2;
                break;
            case fx::gltf::Accessor::ComponentType::Float:
            case fx::gltf::Accessor::ComponentType::UnsignedInt:
                elementSize = 4;
                break;
        }

        switch (aAccessor.type)
        {
            case fx::gltf::Accessor::Type::Mat2:
                return (4 * elementSize);
            case fx::gltf::Accessor::Type::Mat3:
                return (9 * elementSize);
            case fx::gltf::Accessor::Type::Mat4:
                return (16 * elementSize);
            case fx::gltf::Accessor::Type::Scalar:
                return (elementSize);
            case fx::gltf::Accessor::Type::Vec2:
                return (2 * elementSize);
            case fx::gltf::Accessor::Type::Vec3:
                return (3 * elementSize);
            case fx::gltf::Accessor::Type::Vec4:
                return (4 * elementSize);
        }

        return 0;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    static SBufferInfo __getData(  fx::gltf::Document const &aDocument
                                 , fx::gltf::Accessor const &aAccessor)
    {
        fx::gltf::BufferView const &bufferView = aDocument.bufferViews[aAccessor.bufferView];
        fx::gltf::Buffer     const &buffer     = aDocument.buffers    [bufferView.buffer];

        uint32_t const dataTypeSize = __calculateDataTypeSize(aAccessor);
        return SBufferInfo
                {
                        &(aAccessor)
                        , &(buffer.data[static_cast<uint64_t>(bufferView.byteOffset) + aAccessor.byteOffset])
                        , dataTypeSize
                        , (aAccessor.count * dataTypeSize)
                };
    }


    /**
     * @brief ReadInputPaths
     * @param aPathsString
     * @return
     */
    CResult<std::vector<std::string>> readInputPaths(std::string const &aPathsString)
    {
        if(aPathsString.empty())
        {
            return { false };
        }

        std::vector<std::string> paths = CString::split(aPathsString, ',');
        if(paths.empty())
        {
            return { false };
        }

        return { paths };
    }

private_members:

    SConfiguration mConfig;
};

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

    #ifdef SHIRABE_DEBUG
    CConsole::InitializeConsole();
    #endif

    if(1 >= aArgC)
    {
        usage();
        return -1;
    }

    try
    {
        std::filesystem::path exec_dir = std::filesystem::current_path();
        CLog::Error(logTag(), exec_dir);

        std::shared_ptr<CCompiler> precompiler = std::make_shared<CCompiler>();

        // Read all shader-files and convert them to spirv.
        // Then go for SPIRV-cross, perform reflection and generate headers
        // for all shaders.

        try {
            precompiler->initialize();
            precompiler->processArguments(static_cast<uint32_t>(aArgC), aArgV);
            precompiler->run();
            precompiler->deinitialize();
            precompiler.reset();
        } catch (std::exception &e) {
            CLog::Error(logTag(), "Failed to run precompiler. Error: {}", e.what());

            void *array[20];
            size_t size;

            // get void*'s for all entries on the stack
            size = backtrace(array, 20);

            // print out all the frames to stderr
            backtrace_symbols_fd(array, size, STDERR_FILENO);
        }
    }
    catch (...)
    {
    }

    #ifdef SHIRABE_DEBUG
    CConsole::DeinitializeConsole();
    #endif

    return 0;
}
