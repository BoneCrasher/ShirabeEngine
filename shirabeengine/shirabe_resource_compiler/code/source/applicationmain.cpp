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
#include <stdlib.h>
#include <unistd.h>
#endif

#include <log/log.h>
#include <base/string.h>
#include <core/bitfield.h>
#include <core/enginetypehelper.h>
#include <core/result.h>
#include <core/helpers.h>
#include <material/materialserialization.h>

#include "definition.h"
#include "extraction.h"

using namespace engine;
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

public_enums:
    /**
     * Enumeration describing various tool options to be respected while processing.
     */
    enum class EOptions
            : uint64_t
    {
        None                 = 0,
        VerboseOutput        = (1u << 1),
        DebugMode            = (1u << 2),
        OptimizationEnabled  = (1u << 3),
        MultiThreaded        = (1u << 4),
        RecursiveScan        = (1u << 5),
        DumpConfig           = (1u << 6),
        DumpReflection       = (1u << 7),
        DumpBareVersion      = (1u << 8),
    };

private_structs:

    /**
     * @brief The SConfiguration struct
     */
    struct SConfiguration
    {
        std::filesystem::path                inputPath;
        std::filesystem::path                outputPath;
        std::vector<std::filesystem::path>   includePaths;
        std::vector<std::filesystem::path>   filesToProcess;
        core::CBitField<CCompiler::EOptions> options;

        // SMaterialMasterIndex                    indexFile;
        // std::filesystem::path                   inputPath;
        // std::filesystem::path                   outputPath;
        // std::vector<std::filesystem::path>      includePaths;
        // std::vector<std::filesystem::path>      inputPaths;
        // std::filesystem::path                   moduleOutputPath;
        // std::filesystem::path                   indexOutputFile;
        // std::filesystem::path                   signatureOutputFile;
        // std::filesystem::path                   configOutputFile;
    };

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
                { "--verbose",        [&] () { options.set(CCompiler::EOptions::VerboseOutput);         return true; }},
                { "--debug",          [&] () { options.set(CCompiler::EOptions::DebugMode);             return true; }},
                { "--optimize",       [&] () { options.set(CCompiler::EOptions::OptimizationEnabled);   return true; }},
                { "--recursive_scan", [&] () { options.set(CCompiler::EOptions::RecursiveScan);         return true; }},
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

        std::filesystem::path const inputPathAbs = (std::filesystem::current_path() / inputPath).lexically_normal();
        bool const inputDirectoryExists = std::filesystem::exists(inputPathAbs);
        if(not inputDirectoryExists)
        {
            return false;
        }

        std::filesystem::path const outputPathAbs = (std::filesystem::current_path() / outputPath).lexically_normal();
        bool const outputDirectoryExists = std::filesystem::exists(outputPathAbs);
        if(not outputDirectoryExists)
        {
            std::filesystem::create_directories(outputPathAbs);
        }

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
            bool const extensionIsEmpty = ("" == extension.string());
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
        includePaths.push_back(std::filesystem::path("materials/include"));
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
/*
    static void foo()
    {
        auto const checkExtensionIsValid = [] (std::filesystem::path const &aPath, std::filesystem::path const &aExtension) -> bool
        {
            std::filesystem::path const &extension = aPath.extension();

            bool const isFile           = not std::filesystem::is_directory(aPath);
            bool const isExtensionValid = isFile && (aExtension == extension);

            return isExtensionValid;
        };

        bool outputFileConfigValid = true;
        outputFileConfigValid = outputFileConfigValid && checkExtensionIsValid(outputIndexPath,         ".index");
        outputFileConfigValid = outputFileConfigValid && checkExtensionIsValid(outputSignaturePath,     ".signature");
        outputFileConfigValid = outputFileConfigValid && checkExtensionIsValid(outputConfigurationPath, ".config");

        if(not outputFileConfigValid)
        {
            CLog::Error(logTag(), "Output file configuration is invalid. One or more output file's filenames do have invalid suffixes.");
        }

        // Make sure the output config is correct.
        std::filesystem::path const inputPathAbsolute               = (std::filesystem::current_path() / inputPath)                           .lexically_normal();
        std::filesystem::path const outputPathAbsolute              = (std::filesystem::current_path() / outputPath)                          .lexically_normal();
        std::filesystem::path const outputModulePathAbsolute        = (std::filesystem::current_path() / outputPath / outputModulePath)       .lexically_normal();
        std::filesystem::path const outputIndexPathAbsolute         = (std::filesystem::current_path() / outputPath / outputIndexPath)        .lexically_normal();
        std::filesystem::path const outputSignaturePathAbsolute     = (std::filesystem::current_path() / outputPath / outputSignaturePath)    .lexically_normal();
        std::filesystem::path const outputConfigurationPathAbsolute = (std::filesystem::current_path() / outputPath / outputConfigurationPath).lexically_normal();

        auto const checkPathExists = [] (std::filesystem::path const &aPath) -> void
        {
            std::filesystem::path path = aPath;

            bool const pathExists = std::filesystem::exists(path);
            if(not pathExists)
            {
                bool const pathIsFile = not std::filesystem::is_directory(path);
                if(pathIsFile)
                {
                    path = aPath.parent_path();
                }

                try
                {
                    bool const created = std::filesystem::create_directories(path);
                    if(not created)
                    {
                        CLog::Error(logTag(), "Can't create directory '%0'", path);
                    }
                }
                catch(std::filesystem::filesystem_error fserr)
                {
                    CLog::Error(logTag(), "Cant create directory '%0'. Error: %1", path, fserr.what());
                }
            }
        };

        checkPathExists(outputPath);
        checkPathExists(outputModulePathAbsolute);
        checkPathExists(outputIndexPathAbsolute);
        checkPathExists(outputSignaturePathAbsolute);
        checkPathExists(outputConfigurationPathAbsolute);


        std::filesystem::path const indexFilePath       = (std::filesystem::current_path()/inputPath/inputIndexPath).lexically_normal();
        std::filesystem::path const indexFileParentPath = (std::filesystem::current_path()/inputPath/indexFilePath.parent_path()).lexically_normal();
        std::filesystem::path const indexFileBaseName   = indexFilePath.stem();

        std::string const indexFileContents = readFile(indexFilePath);

        Shared<serialization::IJSONDeserializer<SMaterialMasterIndex>> indexDeserializer = makeShared<serialization::CJSONDeserializer<SMaterialMasterIndex>>();
        indexDeserializer->initialize();

        CResult<Shared<serialization::IDeserializer<SMaterialMasterIndex>::IResult>> serialization = indexDeserializer->deserialize(indexFileContents);
        if(not serialization.successful())
        {
            CLog::Error(logTag(), "Could not deserialize material index file.");
            return false;
        }

        indexDeserializer->deinitialize();

        std::vector<std::filesystem::path> inputFiles;

        SMaterialMasterIndex index = serialization.data()->asT().data();

        for(auto const &[stage, pathReferences] : index.stages)
        {
            if(not pathReferences.glslSourceFilename.empty())
            {
                inputFiles.push_back(std::filesystem::current_path()/inputPath/indexFileParentPath/pathReferences.glslSourceFilename);
            }
        }
    }
*/
    CResult<EResult> processMaterial(std::filesystem::path const &aMaterialFile)
    {
        std::filesystem::path const &materialPathAbs  = std::filesystem::current_path() / aMaterialFile;
        std::filesystem::path const &parentPath       = std::filesystem::relative(aMaterialFile, mConfig.inputPath).parent_path();
        std::filesystem::path const &materialID       = aMaterialFile.stem();

        // Make sure the output config is correct.
        std::filesystem::path const outputPath                      = ( parentPath)                                                              .lexically_normal();
        std::filesystem::path const outputModulePath                = ( parentPath / "modules")                                                  .lexically_normal();
        std::filesystem::path const outputIndexPath                 = ( parentPath / (std::filesystem::path(materialID.string() + ".instance"))) .lexically_normal();
        std::filesystem::path const outputSignaturePath             = ( parentPath / (std::filesystem::path(materialID.string() + ".signature"))).lexically_normal();
        std::filesystem::path const outputConfigurationPath         = ( parentPath / (std::filesystem::path(materialID.string() + ".config")))   .lexically_normal();
        std::filesystem::path const outputPathAbsolute              = (std::filesystem::current_path() / mConfig.outputPath / outputPath             ).lexically_normal();
        std::filesystem::path const outputModulePathAbsolute        = (std::filesystem::current_path() / mConfig.outputPath / outputModulePath       ).lexically_normal();
        std::filesystem::path const outputIndexPathAbsolute         = (std::filesystem::current_path() / mConfig.outputPath / outputIndexPath        ).lexically_normal();
        std::filesystem::path const outputSignaturePathAbsolute     = (std::filesystem::current_path() / mConfig.outputPath / outputSignaturePath    ).lexically_normal();
        std::filesystem::path const outputConfigurationPathAbsolute = (std::filesystem::current_path() / mConfig.outputPath / outputConfigurationPath).lexically_normal();

        auto const checkPathExists = [] (std::filesystem::path const &aPath) -> void
        {
            std::filesystem::path path = aPath;

            bool const pathExists = std::filesystem::exists(path);
            if(not pathExists)
            {
                bool const pathIsFile = not std::filesystem::is_directory(path);
                if(pathIsFile)
                {
                    path = aPath.parent_path();
                }

                try
                {
                    bool const created = std::filesystem::create_directories(path);
                    if(not created)
                    {
                        CLog::Error(logTag(), "Can't create directory '%0'", path);
                    }
                }
                catch(std::filesystem::filesystem_error fserr)
                {
                    CLog::Error(logTag(), "Cant create directory '%0'. Error: %1", path, fserr.what());
                }
            }
        };

        checkPathExists(outputPathAbsolute);
        checkPathExists(outputModulePathAbsolute);
        checkPathExists(outputIndexPathAbsolute);
        checkPathExists(outputSignaturePathAbsolute);
        checkPathExists(outputConfigurationPathAbsolute);

        std::string const indexFileContents = readFile(materialPathAbs);

        Shared<serialization::IJSONDeserializer<SMaterialMasterIndex>> indexDeserializer = makeShared<serialization::CJSONDeserializer<SMaterialMasterIndex>>();
        indexDeserializer->initialize();

        auto [success, index] = indexDeserializer->deserialize(indexFileContents);
        if(not success)
        {
            CLog::Error(logTag(), "Could not deserialize material index file.");
            return false;
        }

        indexDeserializer->deinitialize();

        std::vector<std::filesystem::path> inputFiles {};

        SMaterialMasterIndex indexData = *static_cast<SMaterialMasterIndex const *>(&(index->asT().data()));
        indexData.signatureAssetUid     = asset::assetIdFromUri(outputSignaturePath);
        indexData.configurationAssetUid = asset::assetIdFromUri(outputConfigurationPath);

        for(auto const &[stage, pathReferences] : indexData.stages)
        {
            if(not pathReferences.glslSourceFilename.empty())
            {
                inputFiles.push_back(mConfig.inputPath / parentPath / pathReferences.glslSourceFilename);
            }
        }

        // Determine compilation items and config.
        auto [generationSuccessful, unit] = generateCompilationUnit(inputFiles, outputModulePathAbsolute, indexData);
        if(not generationSuccessful)
        {
            CLog::Error(logTag(), "Failed to derive shader compilation units and configuration");
            return EResult::InputInvalid;
        }

        CResult<EResult> const glslangResult = runGlslang(mConfig, unit, true);
        if(not glslangResult.successful())
        {
            CLog::Error(logTag(), "Failed to run glslang.");
            return glslangResult;
        }

        CResult<SMaterialSignature> const extractionResult = spirvCrossExtract(unit);
        if(not extractionResult.successful())
        {
            CLog::Error(logTag(), "Failed to extract data from Spir-V file(s).");
            return EResult::ExtractionFailed;
        }

        std::string serializedData = {};

        // Rewrite index
        CResult<EResult> const indexSerializationResult = serializeMaterialIndex(indexData, serializedData);
        if(not indexSerializationResult.successful())
        {
            CLog::Error(logTag(), "Failed to serialize index data.");
            return EResult::SerializationFailed;
        }

        writeFile(outputIndexPathAbsolute, serializedData);

        CResult<EResult> const signatureSerializationResult = serializeMaterialSignature(extractionResult.data(), serializedData);
        if(not signatureSerializationResult.successful())
        {
            CLog::Error(logTag(), "Failed to serialize signature data.");
            return EResult::SerializationFailed;
        }

        writeFile(outputSignaturePathAbsolute, serializedData);

        CMaterialConfig        config                    = CMaterialConfig::fromMaterialDesc(extractionResult.data());
        CResult<EResult> const configSerializationResult = serializeMaterialConfig(config, serializedData);
        if(not configSerializationResult.successful())
        {
            CLog::Error(logTag(), "Failed to serialize config data.");
            return EResult::SerializationFailed;
        }

        writeFile(outputConfigurationPathAbsolute, serializedData);

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
                auto const &[result, code] = processMaterial(file);
                if(EResult::Success != code)
                {
                    CLog::Error(logTag(), "Failed to process material file w/ name %0", file.string());
                    continue;
                }
            }
            else if(".materialinstance" == extension)
            {
                // auto const &[result, code] = processMaterialInstance(file);
                // if(EResult::Success != code)
                // {
                //     CLog::Error(logTag(), "Failed to process material instance w/ name %0", file.string());
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

            if(".instance" == extension)
            {
                a.type    = asset::EAssetType::Material;
                a.subtype = asset::EAssetSubtype::Instance;
                a.uri = std::filesystem::relative(filePath, (std::filesystem::current_path() / mConfig.outputPath));
                a.id  = asset::assetIdFromUri(a.uri);
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
            ss << CString::format("<Asset aid=\"%0\" parent_aid=\"0\" type=\"%1\" subtype=\"%2\" uri=\"%3\"></Asset>\n"
                                  , a.id
                                  , to_string<asset::EAssetType>(a.type)
                                  , to_string<asset::EAssetSubtype>(a.subtype)
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

    static void foo()
    {
        return;

        // The buffer data contains the binary/text data of a .gltf-file.
        // Use fx-gltf to load the mesh data.
        // fx::gltf::Document const document = __loadFromData(data);
        // for(auto const &mesh : document.meshes)
        // {
        //     for(auto const &primitive : mesh.primitives)
        //     {
        //         SBufferInfo positionBufferInfo {};
        //         SBufferInfo normalBufferInfo   {};
        //         SBufferInfo tangentBufferInfo  {};
        //         SBufferInfo texcoordBufferInfo {};
        //         SBufferInfo indexBufferInfo    {};
//
        //         for(auto const &[attributeID, attributeIndex] : primitive.attributes)
        //         {
        //             if("POSITION" == attributeID)
        //             {
        //                 positionBufferInfo = __getData(document, document.accessors[attributeIndex]);
        //                 continue;
        //             }
//
        //             if("NORMAL" == attributeID)
        //             {
        //                 normalBufferInfo = __getData(document, document.accessors[attributeIndex]);
        //                 continue;
        //             }
//
        //             if("TANGENT" == attributeID)
        //             {
        //                 tangentBufferInfo = __getData(document, document.accessors[attributeIndex]);
        //                 continue;
        //             }
//
        //             if("TEXCOORD_0" == attributeID)
        //             {
        //                 texcoordBufferInfo = __getData(document, document.accessors[attributeIndex]);
        //                 continue;
        //             }
        //         }
//
        //         indexBufferInfo = __getData(document, document.accessors[primitive.indices]);
//
        //         // We got all buffers... Generate Vk-Structs out of it...
        //         auto const createBufferFn = [&] (SBufferInfo const &aBufferInfo) -> CEngineResult<std::tuple<VkBuffer, VkDeviceMemory>>
        //         {
        //             VkBufferCreateInfo bufferCreateInfo {};
        //             bufferCreateInfo.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        //             bufferCreateInfo.pNext                 = nullptr;
        //             bufferCreateInfo.flags                 = 0;
        //             bufferCreateInfo.usage                 = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        //             bufferCreateInfo.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
        //             bufferCreateInfo.size                  = aBufferInfo.totalSize;
        //             // bufferCreateInfo.pQueueFamilyIndices   = ;
        //             // bufferCreateInfo.queueFamilyIndexCount = ;
//
        //             VkBuffer buffer = VK_NULL_HANDLE;
//
        //             VkDevice         const &vkLogicalDevice  = mVulkanEnvironment->getState().selectedLogicalDevice;
        //             VkPhysicalDevice const &vkPhysicalDevice = mVulkanEnvironment->getState().supportedPhysicalDevices.at(mVulkanEnvironment->getState().selectedPhysicalDevice).handle;
//
        //             VkResult result = vkCreateBuffer(vkLogicalDevice, &bufferCreateInfo, nullptr, &buffer);
        //             if(VkResult::VK_SUCCESS != result)
        //             {
        //                 CLog::Error(logTag(), CString::format("Failed to create buffer. Vulkan result: %0", result));
        //                 return { EEngineStatus::Error };
        //             }
//
        //             VkMemoryRequirements vkMemoryRequirements ={ };
        //             vkGetBufferMemoryRequirements(vkLogicalDevice, buffer, &vkMemoryRequirements);
//
        //             VkMemoryAllocateInfo vkMemoryAllocateInfo ={ };
        //             vkMemoryAllocateInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        //             vkMemoryAllocateInfo.allocationSize  = vkMemoryRequirements.size;
//
        //             CEngineResult<uint32_t> memoryTypeFetch =
        //                                             CVulkanDeviceCapsHelper::determineMemoryType(
        //                                                     vkPhysicalDevice,
        //                                                     vkMemoryRequirements.memoryTypeBits,
        //                                                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
//
        //             if(not memoryTypeFetch.successful())
        //             {
        //                 CLog::Error(logTag(), "Could not determine memory type index.");
        //                 return { EEngineStatus::Error };
        //             }
//
        //             vkMemoryAllocateInfo.memoryTypeIndex = memoryTypeFetch.data();
//
        //             VkDeviceMemory bufferMemory = VK_NULL_HANDLE;
//
        //             result = vkAllocateMemory(vkLogicalDevice, &vkMemoryAllocateInfo, nullptr, &bufferMemory);
        //             if(VkResult::VK_SUCCESS != result)
        //             {
        //                 CLog::Error(logTag(), CString::format("Failed to allocate image memory on GPU. Vulkan error: %0", result));
        //                 return { EEngineStatus::Error };
        //             }
//
        //             result = vkBindBufferMemory(vkLogicalDevice, buffer, bufferMemory, 0);
        //             if(VkResult::VK_SUCCESS != result)
        //             {
        //                 CLog::Error(logTag(), CString::format("Failed to bind image memory on GPU. Vulkan error: %0", result));
        //                 return { EEngineStatus::Error };
        //             }
//
        //             // Copy vertex data over...
        //             void *data = nullptr;
        //             vkMapMemory(vkLogicalDevice, bufferMemory, 0, bufferCreateInfo.size, 0, &(data) );
        //             memcpy(data, aBufferInfo.data, static_cast<std::size_t>(aBufferInfo.totalSize));
        //             vkUnmapMemory(vkLogicalDevice, bufferMemory);
//
        //             return { EEngineStatus::Ok, { buffer, bufferMemory } };
        //         };
//
        //         auto position  = createBufferFn(positionBufferInfo);
        //         auto normal    = createBufferFn(normalBufferInfo);
        //         auto tangent   = createBufferFn(tangentBufferInfo);
        //         auto texcoord0 = createBufferFn(texcoordBufferInfo);
        //         auto index     = createBufferFn(indexBufferInfo);
//
        //         // All components are required for compatibility...
        //         if(not (   position .successful()
        //                    && normal   .successful()
        //                    && tangent  .successful()
        //                    && texcoord0.successful()
        //                    && index    .successful()))
        //         {
        //             return { EEngineStatus::Error };
        //         }
//
        //         vertexBuffers.push_back(std::get<0>(position .data()));
        //         vertexBuffers.push_back(std::get<0>(normal   .data()));
        //         vertexBuffers.push_back(std::get<0>(tangent  .data()));
        //         vertexBuffers.push_back(std::get<0>(texcoord0.data()));
        //         vertexBufferMemoryObjects.push_back(std::get<1>(position .data()));
        //         vertexBufferMemoryObjects.push_back(std::get<1>(normal   .data()));
        //         vertexBufferMemoryObjects.push_back(std::get<1>(tangent  .data()));
        //         vertexBufferMemoryObjects.push_back(std::get<1>(texcoord0.data()));
        //         indexBuffer       = std::get<0>(index.data());
        //         indexBufferMemory = std::get<1>(index.data());
//
        //         break; // For now, just process the first...
        //     }
//
        //     break; // For now, just process the first...
        // }
    };

    /**
     * Derive the shading language used and the stage to compile from the full shader file name.
     *
     * @param aFileName See brief.
     * @return          A tuple containing the language and stage information.
     */
    std::tuple<EShadingLanguage, VkPipelineStageFlagBits> const compileTargetFromShaderFilename(std::string const &aFileName)
    {
        VkPipelineStageFlagBits stage    = {};
        EShadingLanguage        language = EShadingLanguage::Unknown;

        // Possible filename variants:
        // 1. <basename>.<stage>.<language_unified_ext> ; e.g. awesomeShader.vert.glsl
        // 2. <basename>.<language_unified_ext>         ; e.g. awesomeShader.hlsl, awesomeShader.cg
        // 3. <basename>.<stage>                        ; e.g. awesomeShader.vert, awesomeShader.vs

        std::string const filename = std::filesystem::path(aFileName).filename();

        size_t const primaryExtensionPosition   = filename.find_last_of(".");
        size_t const secondaryExtensionPosition = filename.find_last_of(".", primaryExtensionPosition - 1);

        bool const hasPrimaryExtension   = (std::string::npos != primaryExtensionPosition);
        bool const hasSecondaryExtension = (std::string::npos != secondaryExtensionPosition);

        std::string primaryExtension   = std::string();
        std::string secondaryExtension = std::string();

        if(hasPrimaryExtension)
        {
            primaryExtension = filename.substr(primaryExtensionPosition + 1, std::string::npos);
        }

        if(hasSecondaryExtension)
        {
            secondaryExtension = filename.substr(secondaryExtensionPosition + 1, (primaryExtensionPosition - secondaryExtensionPosition - 1));
        }

        bool const usesUnifiedExtension = anyOf({ "cg", "hlsl", "glsl", "xs" }, primaryExtension);

        std::string stageName = std::string();
        if(hasPrimaryExtension and not usesUnifiedExtension)
        {
            stageName = primaryExtension;
        }
        else if(usesUnifiedExtension)
        {
            stageName = hasSecondaryExtension
                            ? secondaryExtension
                            : primaryExtension;
        }
        else
        {
            // Invalid
            usage();
            return { EShadingLanguage::Unknown, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM };
        }

        // Determine language
        language  = mapValue<std::string, EShadingLanguage>(primaryExtension, {{ "cg",   EShadingLanguage::CGLanguage },
                                                                               { "glsl", EShadingLanguage::GLSL       },
                                                                               { "vert", EShadingLanguage::GLSL       },
                                                                               { "tesc", EShadingLanguage::GLSL       },
                                                                               { "tese", EShadingLanguage::GLSL       },
                                                                               { "geom", EShadingLanguage::GLSL       },
                                                                               { "frag", EShadingLanguage::GLSL       },
                                                                               { "comp", EShadingLanguage::GLSL       },
                                                                               { "hlsl", EShadingLanguage::HLSL       },
                                                                               { "vs",   EShadingLanguage::HLSL       },
                                                                               { "hs",   EShadingLanguage::HLSL       },
                                                                               { "ds",   EShadingLanguage::HLSL       },
                                                                               { "gs",   EShadingLanguage::HLSL       },
                                                                               { "ps",   EShadingLanguage::HLSL       },
                                                                               { "cs",   EShadingLanguage::HLSL       },
                                                                               { "xs",   EShadingLanguage::XShade     }});

        if(EShadingLanguage::Unknown == language)
        {
            CLog::Error(logTag(), CString::format("Invalid extension '%0'. Cannot derive language.", stageName));
            return { EShadingLanguage::Unknown, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM };
        }

        // Determine stage
        stage = mapValue<std::string, VkPipelineStageFlagBits>(stageName, {{ "vert", VkPipelineStageFlagBits::VK_PIPELINE_STAGE_VERTEX_SHADER_BIT                  },
                                                                           { "tesc", VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT    },
                                                                           { "tese", VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT },
                                                                           { "geom", VkPipelineStageFlagBits::VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT                },
                                                                           { "frag", VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT                },
                                                                           { "comp", VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT                 },
                                                                   #ifdef NV_EXTENSIONS
                                                                           { "rgen",  EShaderStage::NVRayGen                },
                                                                           { "rint",  EShaderStage::NVIntersect             },
                                                                           { "rahit", EShaderStage::NVAnyHit                },
                                                                           { "rchit", EShaderStage::NVClosestHit            },
                                                                           { "rmiss", EShaderStage::NVMiss                  },
                                                                           { "rcall", EShaderStage::NVCallable              },
                                                                           { "mesh",  EShaderStage::NVMesh                  },
                                                                           { "task",  EShaderStage::NVTask                  },
                                                                   #endif
                                                                                                                  });

        if(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM == stage)
        {
            CLog::Error(logTag(), CString::format("Invalid file extension '%0'. Cannot map to stage.", stageName));
            return { EShadingLanguage::Unknown, stage };
        }

        return { language, stage };
    }

    /**
     * Determine the output name of the compiled shader based on language, stage and file base name.
     *
     * @param aFileBaseName
     * @param aLanguage
     * @param aStage
     * @return              See brief.
     */
    std::string const getOutputFilename(std::string             const &aFileBaseName,
                                        EShadingLanguage        const &aLanguage,
                                        VkPipelineStageFlagBits const &aStage) const
    {
        SHIRABE_UNUSED(aStage);

        std::string extension{};

    //     static std::unordered_map<EShaderStage, std::string> glslStageAssignment =
    //     {
    //         { EShaderStage::Vertex                  , "vert.glsl.spv" },
    //         { EShaderStage::TesselationControlPoint , "tesc.glsl.spv" },
    //         { EShaderStage::TesselationEvaluation   , "tese.glsl.spv" },
    //         { EShaderStage::Geometry                , "geom.glsl.spv" },
    //         { EShaderStage::Fragment                , "frag.glsl.spv" },
    //         { EShaderStage::Compute                 , "comp.glsl.spv" },
    // #ifdef NV_EXTENSIONS
    //         { EShaderStage::NVRayGen                , "rgen.spv"      },
    //         { EShaderStage::NVIntersect             , "rint.spv"      },
    //         { EShaderStage::NVAnyHit                , "rahit.spv"     },
    //         { EShaderStage::NVClosestHit            , "rchit.spv"     },
    //         { EShaderStage::NVMiss                  , "rmiss.spv"     },
    //         { EShaderStage::NVCallable              , "rcall.spv"     },
    //         { EShaderStage::NVMesh                  , "mesh.spv"      },
    //         { EShaderStage::NVTask                  , "task.spv"      },
    // #endif
    //         { EShaderStage::NotApplicable           , "unknown"       },
    //     };
    //
    //     static std::unordered_map<EShaderStage, std::string> hlslStageAssignment =
    //     {
    //         { EShaderStage::Vertex                  , "vs.hlsl.spv" },
    //         { EShaderStage::TesselationControlPoint , "hs.hlsl.spv" },
    //         { EShaderStage::TesselationEvaluation   , "ds.hlsl.spv" },
    //         { EShaderStage::Geometry                , "gs.hlsl.spv" },
    //         { EShaderStage::Fragment                , "ps.hlsl.spv" },
    //         { EShaderStage::Compute                 , "cs.hlsl.spv" },
    //         { EShaderStage::NotApplicable           , "unknown"     },
    //     };
    //
    //     std::string const hlslStage = mapValue<EShaderStage, std::string>(aStage, std::move(hlslStageAssignment));
    //     std::string const glslStage = mapValue<EShaderStage, std::string>(aStage, std::move(glslStageAssignment));

        std::unordered_map<EShadingLanguage, std::string> languageAssignment  =
        {
            { EShadingLanguage::CGLanguage, "cg.spv"   },
            { EShadingLanguage::XShade,     "xs.spv"   },
            { EShadingLanguage::GLSL,       "glsl.spv" },
            { EShadingLanguage::HLSL,       "hlsl.spv" },
            { EShadingLanguage::Unknown,    "unknown"  }
        };

        extension = mapValue<EShadingLanguage, std::string>(aLanguage, std::move(languageAssignment));

        return CString::format("%0.%1", aFileBaseName, extension);
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

    /**
     * Compile a single shader file.
     *
     * @param aFilename
     * @param aOptions
     * @return
     */
    CResult<SShaderCompilationUnit> generateCompilationUnit(  std::vector<std::filesystem::path> const &aFilenames
                                                            , std::filesystem::path              const &aModuleOutputPath
                                                            , SMaterialMasterIndex                     &aInOutIndex)
    {
        EShaderCompiler  compiler = EShaderCompiler::Unknown;
        EShadingLanguage language = EShadingLanguage::Unknown;

        auto const deriveElement = [&, this] (std::string const &aFilename) -> SShaderCompilationElement
        {
            std::string shaderString = readFile(aFilename);
            if(shaderString.empty())
            {
                CLog::Error(logTag(), CString::format("Shader file %0 is empty.", aFilename));
                return {};
            }

            // move to length-based strings, rather than null-terminated strings
            uint64_t const length = shaderString.size();
            SHIRABE_UNUSED(length);

            // Determine compiler
            auto const [language, stage] = compileTargetFromShaderFilename(aFilename);

            EShaderCompiler fileCompiler = mapValue<EShadingLanguage, EShaderCompiler>(language, {{ EShadingLanguage::CGLanguage, EShaderCompiler::CGLanguage               },
                                                                                                  { EShadingLanguage::GLSL,       EShaderCompiler::GlslangReferenceCompiler },
                                                                                                  { EShadingLanguage::HLSL,       EShaderCompiler::DxHlslCompiler           },
                                                                                                  { EShadingLanguage::XShade,     EShaderCompiler::XShadeCompiler           }});

            if(EShaderCompiler::Unknown != compiler && fileCompiler != compiler )
            {
                // Different compilers detected. This implies different languages...
                CLog::Error(logTag(), CString::format("Different compilers detected for a pair of files. C1: %0, C2: %1", EnumValueOf(compiler), EnumValueOf(fileCompiler)));
                return {};
            }

            std::string           const outputName = getOutputFilename(std::filesystem::path(aFilename).stem(), language, stage);
            std::filesystem::path const outputPath = (aModuleOutputPath / outputName);

            SShaderCompilationElement element {};
            element.fileName           = aFilename;
            element.contents           = shaderString;
            element.stage              = stage;
            element.outputPathAbsolute = outputPath;
            element.outputPathRelative = std::filesystem::relative(outputPath, (std::filesystem::current_path() / mConfig.outputPath));

            std::string const path = element.outputPathRelative;
            aInOutIndex.stages[stage].spvModuleAssetId = asset::assetIdFromUri(path);

            return element;
        };

        std::vector<SShaderCompilationElement> elements = {};
        std::transform(aFilenames.begin(), aFilenames.end(), std::back_inserter(elements), deriveElement);

        SShaderCompilationUnit unit{};
        unit.compiler = compiler;
        unit.language = language;
        unit.elements = std::move(elements);

        return { unit };
    }

    /**
     * Format a valid glslangValidator command line and invoke the command to create a .spv module reading its stdout/stderr output.
     *
     * @param aUnit Source data information for the glslangValidator command.
     * @return      EResult::Success           if successful.
     * @return      EResult::CompilationFailed on error.
     */
    CResult<EResult> runGlslang(SConfiguration const &aConfiguration, SShaderCompilationUnit &aUnit, bool const aCompileStagesIndividually = false)
    {
        std::string const application = CString::format("%0/tools/glslang/bin/glslangValidator", std::filesystem::current_path().string());
        std::string       options     = "-v -d -g -Od -V --target-env vulkan1.1";

        auto const appendIncludes = [&options] (std::string const &aInclude) -> void
        {
            options.append(" -I" + aInclude);
        };
        std::for_each(aConfiguration.includePaths.begin(), aConfiguration.includePaths.end(), appendIncludes);

        std::underlying_type_t<EResult> result = 0;

        auto const once = [&] (std::string const &aInputFilenames, std::string const &aOutputFilename) -> void
        {
            std::string                const command       = CString::format("%0 -o %2 %1 %3", application, options, aOutputFilename, aInputFilenames);
            CEngineResult<std::string> const commandResult = executeCmd(command);

            bool const compilationError = (std::string::npos != commandResult.data().find("compilation error")
                                           or std::string::npos != commandResult.data().find("not found"));
            if(compilationError || not commandResult.successful())
            {
                CLog::Error(logTag(), commandResult.data());
                result |= EnumValueOf(EResult::CompilationFailed);
            }
            else
            {
                CLog::Debug(logTag(), commandResult.data());
                result |= EnumValueOf(EResult::Success);
            }

            CLog::Debug(logTag(), command);
        };

        if(aCompileStagesIndividually)
        {
            auto const compile = [&] (SShaderCompilationElement const &aElement) -> void
            {
                std::string const outputFile = aElement.outputPathAbsolute;
                std::string const inputFile  = aElement.fileName;

                once(inputFile, outputFile);

                aUnit.outputFiles.push_back(outputFile);
            };
            std::for_each(aUnit.elements.begin(), aUnit.elements.end(), compile);
        }
        else
        {
            std::string const outputFiles = aUnit.elements.at(0).outputPathAbsolute;
            std::string       inputFiles  = "";

            auto const append = [&inputFiles] (SShaderCompilationElement const &aElement)
            {
                inputFiles.append(" " + aElement.fileName);
            };
            std::for_each(aUnit.elements.begin(), aUnit.elements.end(), append);

            once(inputFiles, outputFiles);
            aUnit.outputFiles.push_back(aUnit.elements.at(0).outputPathAbsolute);
        }

        return static_cast<EResult>(result);
    }

    /**
     * Format a valid spirv-dis command line and invoke the command to create a disassembled spirv module reading its stdout/stderr output.
     *
     * @param aInputFilenames Source data information for the glslangValidator command.
     * @return                EResult::Success      if successful.
     * @return                EResult::InputInvalid on error.
     */
    CResult<EResult> runSpirVDisassembler(std::vector<std::string> const &aInputFilenames)
    {
        std::string const application = CString::format("%0/tools/spirv-tools/bin/spirv-dis", std::filesystem::current_path());
        std::string const options     = "";

        std::underlying_type_t<EResult> result = 0;

        auto const disassemble = [&] (std::string const &aFilename) -> void
        {
            std::string const inputFile  = aFilename;
            std::string const outputFile = aFilename + ".dis";

            std::string                const command       = CString::format("%0 %1 -o %2 %3", application, options, outputFile, inputFile);
            CEngineResult<std::string> const commandResult = executeCmd(command);
            if(not commandResult.successful())
            {
                CLog::Error(logTag(), commandResult.data());
                result |= EnumValueOf(EResult::InputInvalid);
            }
            else
            {
                CLog::Debug(logTag(), commandResult.data());
                result |= EnumValueOf(EResult::Success);
            }
        };
        std::for_each(aInputFilenames.begin(), aInputFilenames.end(), disassemble);

        return static_cast<EResult>(result);
    }

    /**
     * Accept a SMaterial instance and serialize it to a JSON string.
     *
     * @param aMaterial
     * @param aOutSerializedData
     * @return
     */
    CResult<EResult> serializeMaterialIndex(SMaterialMasterIndex const &aMaterialIndex, std::string &aOutSerializedData)
    {
        using namespace resource_compiler::serialization;

        Unique<IJSONSerializer<SMaterialMasterIndex>> serializer = makeUnique<CJSONSerializer<SMaterialMasterIndex>>();
        bool const initialized = serializer->initialize();
        if(false == initialized)
        {
            return EResult::SerializationFailed;
        }
        CResult<Shared<serialization::ISerializer<SMaterialMasterIndex>::IResult>> const serialization = serializer->serialize(aMaterialIndex);
        if(not serialization.successful())
        {
            return EResult::SerializationFailed;
        }

        CResult<std::string> data = serialization.data()->asString();
        aOutSerializedData = data.data();

        bool const deinitialized = serializer->deinitialize();
        if(false == deinitialized)
        {
            return EResult::SerializationFailed;
        }

        serializer = nullptr;

        CLog::Debug(logTag(), aOutSerializedData);

        return EResult::Success;
    }

    /**
     * Accept a SMaterial instance and serialize it to a JSON string.
     *
     * @param aMaterial
     * @param aOutSerializedData
     * @return
     */
    CResult<EResult> serializeMaterialSignature(SMaterialSignature const &aMaterial, std::string &aOutSerializedData)
    {
        using namespace resource_compiler::serialization;

        Unique<IJSONSerializer<SMaterialSignature>> serializer = makeUnique<CJSONSerializer<SMaterialSignature>>();
        bool const initialized = serializer->initialize();
        if(false == initialized)
        {
            return EResult::SerializationFailed;
        }
        CResult<Shared<serialization::ISerializer<SMaterialSignature>::IResult>> const serialization = serializer->serialize(aMaterial);
        if(not serialization.successful())
        {
            return EResult::SerializationFailed;
        }

        CResult<std::string> data = serialization.data()->asString();
        aOutSerializedData = data.data();

        bool const deinitialized = serializer->deinitialize();
        if(false == deinitialized)
        {
            return EResult::SerializationFailed;
        }

        serializer = nullptr;

        CLog::Debug(logTag(), aOutSerializedData);

        return EResult::Success;
    }

    /**
     * @brief serializeMaterialConfig
     * @param aMaterialConfig
     * @param aOutSerializedData
     * @return
     */
    CResult<EResult> serializeMaterialConfig(CMaterialConfig const &aMaterialConfig, std::string &aOutSerializedData)
    {
        using namespace resource_compiler::serialization;

        Unique<IJSONSerializer<CMaterialConfig>> serializer = makeUnique<CJSONSerializer<CMaterialConfig>>();
        bool const initialized = serializer->initialize();
        if(false == initialized)
        {
            return EResult::SerializationFailed;
        }
        CResult<Shared<serialization::ISerializer<CMaterialConfig>::IResult>> const serialization = serializer->serialize(aMaterialConfig);
        if(not serialization.successful())
        {
            return EResult::SerializationFailed;
        }

        CResult<std::string> data = serialization.data()->asString();
        aOutSerializedData = data.data();

        bool const deinitialized = serializer->deinitialize();
        if(false == deinitialized)
        {
            return EResult::SerializationFailed;
        }

        serializer = nullptr;

        CLog::Debug(logTag(), aOutSerializedData);

        return EResult::Success;
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
            CLog::Error(logTag(), "Failed to run precompiler. Error: %0", e.what());

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
