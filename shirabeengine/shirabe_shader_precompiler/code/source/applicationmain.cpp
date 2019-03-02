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

#include <log/log.h>
#include <base/string.h>
#include <core/bitfield.h>
#include <core/enginetypehelper.h>
#include <core/result.h>
#include <material/materialserialization.h>

#include "definition.h"
#include "helpers.h"
#include "extraction.h"

using namespace engine;
using namespace shader_precompiler;

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
            "Usage:                                                                           \n"
            "  ./shirabe_shader_precompiler <options>                                         \n"
            "                                                                                 \n"
            "Options:                                                                         \n"
            "  --verbose                                                                      \n"
            "      Values: true or false                                                      \n"
            "      Effect: Enables verbose output while doing the job.                        \n"
            "  --debug                                                                        \n"
            "      Effect: Enable debug output.                                               \n"
            "  --optimize                                                                     \n"
            "      Effect: Optimize the shader.                                               \n"
            "  --recursive_scan                                                               \n"
            "      Effect: If any of the paths in the -i option is a directory, include       \n"
            "              all subdirectories in the input file search.                       \n"
            "  -o=<path>                                                                      \n"
            "      Effect: Specifies the directory to save the compilation result to.         \n"
            "  -i=<comma-separated list of file- and directory-paths>                         \n"
            "      Effect: Specifies the list of files and directories to source from.        \n"
            "                                                                                 \n"
            " Example:                                                                        \n"
            "   ./shirabe_shader_precompiler --verbose --debug -o=./spv_output -i=./spv_input \n"
            "                                                                                 \n";

    CLog::Warning(shader_precompiler::logTag(), usageMessage);

    ::exit(EnumValueOf(EResult::WrongUsage));
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------

/**
 * @brief The CPrecompiler class
 */
class CPrecompiler
{
    SHIRABE_DECLARE_LOG_TAG(CPrecompiler);

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
        std::vector<std::string>                includePaths;
        std::vector<std::string>                inputPaths;
        std::string                             outputPath;
        std::filesystem::path                   outputFile;
        core::CBitField<CPrecompiler::EOptions> options;
    };

public_methods:


    EResult initialize()
    {
        return EResult::Success;
    }


    /**
     * @brief ProcessArguments
     * @param aArgC
     * @param aArgV
     * @return
     */
    EResult processArguments(uint32_t const aArgC, char **aArgV)
    {
        std::vector<std::string> usableArguments(aArgV + 1, aArgV + aArgC);

        core::CBitField<EOptions> options      = {};
        std::string               dataFile     = {};
        std::vector<std::string>  includePaths = {};
        std::string               indexFile    = {};
        std::string               outputPath   = {};

        //
        // Process all options provided to the application.
        //
        auto const processor = [&, this] (std::string const &aArgument) -> bool
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
                { "--verbose",        [&] () { options.set(CPrecompiler::EOptions::VerboseOutput);         return true; }},
                { "--debug",          [&] () { options.set(CPrecompiler::EOptions::DebugMode);             return true; }},
                { "--optimize",       [&] () { options.set(CPrecompiler::EOptions::OptimizationEnabled);   return true; }},
                { "--recursive_scan", [&] () { options.set(CPrecompiler::EOptions::RecursiveScan);         return true; }},
                { "-I",               [&] () { includePaths.push_back(referencableValue);                  return true; }},
                { "-o",               [&] () { outputPath = referencableValue;                             return true; }},
                { "-i",               [&] () { indexFile  = referencableValue;                             return true; }},
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

        // Make sure the output config is correct.
        std::filesystem::path const outputPathAbsolute = std::filesystem::current_path() / outputPath;

        bool const outputPathExists = std::filesystem::exists(outputPath);
        if(not outputPathExists)
        {
            std::filesystem::create_directory(outputPathAbsolute);
        }

        std::filesystem::path const indexFilePath(indexFile);
        std::filesystem::path const indexFileParentPath = indexFilePath.parent_path();
        std::filesystem::path const indexFileBaseName   = indexFilePath.stem();

        std::string const indexFileContents = readFile(indexFilePath);

        CStdSharedPtr_t<serialization::IDeserializer<SMaterialIndex>::IResult> indexDeserializationResult = nullptr;
        CStdSharedPtr_t<serialization::IJSONDeserializer<SMaterialIndex>> indexDeserializer =
                makeCStdSharedPtr<serialization::CJSONDeserializer<SMaterialIndex>>();
        indexDeserializer->initialize();
        indexDeserializer->deserialize(indexFileContents, indexDeserializationResult);
        indexDeserializer->deinitialize();

        std::vector<std::string> inputFiles;

        CStdSharedPtr_t<SMaterialIndex> indexData = nullptr;
        indexDeserializationResult->asT(indexData);
        for(auto const &[stage, path] : indexData->stages)
        {
            if(not path.empty())
            {
                inputFiles.push_back(std::filesystem::current_path()/indexFileParentPath/path);
            }
        }

        std::filesystem::path const outputFileName = outputPathAbsolute/indexFileBaseName/".data";

        SConfiguration config {};
        config.options      = options;
        config.inputPaths   = inputFiles;
        config.includePaths = includePaths;
        config.outputPath   = outputPath;
        config.outputFile   = outputFileName;

        mConfig = config;

        return EResult::Success;
    }

    /**
     * Format a valid glslangValidator command line and invoke the command to create a .spv module reading its stdout/stderr output.
     *
     * @param aUnit Source data information for the glslangValidator command.
     * @return      EResult::Success           if successful.
     * @return      EResult::CompilationFailed on error.
     */
    EResult runGlslang(SConfiguration const &aConfiguration, SShaderCompilationUnit &aUnit, bool const aCompileStagesIndividually = false)
    {
        std::string const application = CString::format("%0/tools/glslang/bin/glslangValidator", std::filesystem::current_path());
        std::string       options     = "-v -d -g -Od -V --target-env vulkan1.1";

        auto const appendIncludes = [&options] (std::string const &aInclude) -> void
        {
            options.append(" -I" + aInclude);
        };
        std::for_each(aConfiguration.includePaths.begin(), aConfiguration.includePaths.end(), appendIncludes);

        std::underlying_type_t<EResult> result = 0;

        auto const once = [&] (std::string const &aInputFilenames, std::string const &aOutputFilename) -> void
        {
            std::string          const command       = CString::format("%0 -o %2 %1 %3", application, options, aOutputFilename, aInputFilenames);
            CResult<std::string> const commandResult = executeCmd(command);

            bool const compilationError = (std::string::npos != commandResult.data().find("compilation error"));
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
                std::string const outputFile = aElement.outputPath;
                std::string const inputFile  = aElement.fileName;

                once(inputFile, outputFile);

                aUnit.outputFiles.push_back(outputFile);
            };
            std::for_each(aUnit.elements.begin(), aUnit.elements.end(), compile);
        }
        else
        {
            std::string const outputFiles = aUnit.elements.at(0).outputPath;
            std::string       inputFiles  = "";

            auto const append = [&inputFiles] (SShaderCompilationElement const &aElement)
            {
                inputFiles.append(" " + aElement.fileName);
            };
            std::for_each(aUnit.elements.begin(), aUnit.elements.end(), append);

            once(inputFiles, outputFiles);
            aUnit.outputFiles.push_back(aUnit.elements.at(0).outputPath);
        }

        return static_cast<EResult>(result);
    }

    /**
     * Format a valid spirv-dis command line and invoke the command to create a disassembled spirv module reading its stdout/stderr output.
     *
     * @param aUnit Source data information for the glslangValidator command.
     * @return      EResult::Success      if successful.
     * @return      EResult::InputInvalid on error.
     */
    EResult runSpirVDisassembler(std::vector<std::string> const &aInputFilenames)
    {
        std::string const application = CString::format("%0/tools/spirv-tools/bin/spirv-dis", std::filesystem::current_path());
        std::string const options     = "";

        std::underlying_type_t<EResult> result = 0;

        auto const disassemble = [&] (std::string const &aElement) -> void
        {
            std::string const inputFile  = aElement;
            std::string const outputFile = aElement + ".dis";

            std::string          const command       = CString::format("%0 %1 -o %2 %3", application, options, outputFile, inputFile);
            CResult<std::string> const commandResult = executeCmd(command);
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
     * Run the shader precompiler on the identified input items.
     *
     * @return EResult::Success      if successful.
     * @return EResult::InputInvalid on error.
     */
    EResult run()
    {
        // Determine compilation items and config.
        CResult<SShaderCompilationUnit> const unitGeneration = generateCompilationUnit(mConfig.inputPaths);
        if(not unitGeneration.successful())
        {
            CLog::Error(logTag(), "Failed to derive shader compilation units and configuration");
            return EResult::InputInvalid;
        }

        SShaderCompilationUnit unit = unitGeneration.data();

        EResult const glslangResult = runGlslang(mConfig, unit, true);
        if(EResult::Success != glslangResult)
        {
            CLog::Error(logTag(), "Failed to run glslang.");
            return glslangResult;
        }

        CResult<SMaterial> const extractionResult = spirvCrossExtract(unit);
        if(not extractionResult.successful())
        {
            CLog::Error(logTag(), "Failed to extract data from Spir-V file(s).");
            return EResult::InputInvalid;
        }

        using namespace shader_precompiler::serialization;

        CStdSharedPtr_t<IJSONSerializer<SMaterial>::IResult> result = nullptr;

        CStdUniquePtr_t<IJSONSerializer<SMaterial>> serializer = makeCStdUniquePtr<CJSONSerializer<SMaterial>>();
        bool const initialized   = serializer->initialize();
        bool const serialized    = serializer->serialize(extractionResult.data(), result);

        std::string serializedData {};
        bool const fetched = result->asString(serializedData);

        CLog::Debug(logTag(), serializedData);

        writeFile(mConfig.outputFile, serializedData);

        bool const deinitialized= serializer->deinitialize();
        serializer = nullptr;

        // CStdSharedPtr_t<CMaterialDeserializer::IResult> result1 = nullptr;
        //
        // CStdUniquePtr_t<IMaterialDeserializer> deserializer = makeCStdUniquePtr<CMaterialDeserializer>();
        // bool const initialized1   = deserializer->initialize();
        // bool const deserialized    = deserializer->deserialize(str, result1);
        //
        // CStdSharedPtr_t<SMaterial> mat{};
        // bool const fetched1 = result1->asT(mat);
        //
        // bool const deinitialized1 = deserializer->deinitialize();

        return EResult::Success;
    }

    EResult deinitialize()
    {
        return EResult::Success;
    }

private_methods:

    /**
     * Derive the shading language used and the stage to compile from the full shader file name.
     *
     * @param aFileName See brief.
     * @return          A tuple containing the language and stage information.
     */
    std::tuple<EShadingLanguage, EShaderStage> const compileTargetFromShaderFilename(std::string const &aFileName)
    {
        EShaderStage     stage    = {};
        EShadingLanguage language = EShadingLanguage::Unknown;

        // Possible filename variants:
        // 1. <basename>.<stage>.<language_unified_ext> ; e.g. awesomeShader.vert.glsl
        // 2. <basename>.<language_unified_ext>         ; e.g. awesomeShader.hlsl, awesomeShader.cg
        // 3. <basename>.<stage>                        ; e.g. awesomeShader.vert, awesomeShader.vs

        std::string primaryExtension   = std::string();
        std::string secondaryExtension = std::string();

        std::string const filename = std::filesystem::path(aFileName).filename();

        size_t const primaryExtensionPosition   = filename.find_last_of(".");
        size_t const secondaryExtensionPosition = filename.find_last_of(".", primaryExtensionPosition - 1);

        bool const hasPrimaryExtension   = (std::string::npos != primaryExtensionPosition);
        bool const hasSecondaryExtension = (std::string::npos != secondaryExtensionPosition);

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
            return { EShadingLanguage::Unknown, EShaderStage::NotApplicable };
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
            return { EShadingLanguage::Unknown, EShaderStage::NotApplicable };
        }

        // Determine stage
        stage = mapValue<std::string, EShaderStage>(stageName, {{ "vert", EShaderStage::Vertex                  },
                                                                { "tesc", EShaderStage::TesselationControlPoint },
                                                                { "tese", EShaderStage::TesselationEvaluation   },
                                                                { "geom", EShaderStage::Geometry                },
                                                                { "frag", EShaderStage::Fragment                },
                                                                { "comp", EShaderStage::Compute                 },
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

        if(EShaderStage::NotApplicable == stage)
        {
            CLog::Error(logTag(), CString::format("Invalid file extension '%0'. Cannot map to stage.", stageName));
            return { EShadingLanguage::Unknown, EShaderStage::NotApplicable };
        }

        return { language, stage };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------

    /**
     * Determine the output name of the compiled shader based on language, stage and file base name.
     *
     * @param aFileBaseName
     * @param aLanguage
     * @param aStage
     * @return              See brief.
     */
    std::string const getOutputFilename(std::string      const &aFileBaseName,
                                        EShadingLanguage const &aLanguage,
                                        EShaderStage     const &aStage) const
    {
        std::string extension{};

        static std::unordered_map<EShaderStage, std::string> glslStageAssignment =
        {
            { EShaderStage::Vertex                  , "vert.glsl.spv" },
            { EShaderStage::TesselationControlPoint , "tesc.glsl.spv" },
            { EShaderStage::TesselationEvaluation   , "tese.glsl.spv" },
            { EShaderStage::Geometry                , "geom.glsl.spv" },
            { EShaderStage::Fragment                , "frag.glsl.spv" },
            { EShaderStage::Compute                 , "comp.glsl.spv" },
    #ifdef NV_EXTENSIONS
            { EShaderStage::NVRayGen                , "rgen.spv"      },
            { EShaderStage::NVIntersect             , "rint.spv"      },
            { EShaderStage::NVAnyHit                , "rahit.spv"     },
            { EShaderStage::NVClosestHit            , "rchit.spv"     },
            { EShaderStage::NVMiss                  , "rmiss.spv"     },
            { EShaderStage::NVCallable              , "rcall.spv"     },
            { EShaderStage::NVMesh                  , "mesh.spv"      },
            { EShaderStage::NVTask                  , "task.spv"      },
    #endif
            { EShaderStage::NotApplicable           , "unknown"       },
        };

        static std::unordered_map<EShaderStage, std::string> hlslStageAssignment =
        {
            { EShaderStage::Vertex                  , "vs.hlsl.spv" },
            { EShaderStage::TesselationControlPoint , "hs.hlsl.spv" },
            { EShaderStage::TesselationEvaluation   , "ds.hlsl.spv" },
            { EShaderStage::Geometry                , "gs.hlsl.spv" },
            { EShaderStage::Fragment                , "ps.hlsl.spv" },
            { EShaderStage::Compute                 , "cs.hlsl.spv" },
            { EShaderStage::NotApplicable           , "unknown"     },
        };

        std::string const hlslStage = mapValue<EShaderStage, std::string>(aStage, std::move(hlslStageAssignment));
        std::string const glslStage = mapValue<EShaderStage, std::string>(aStage, std::move(glslStageAssignment));

        std::unordered_map<EShadingLanguage, std::string> languageAssignment  =
        {
            { EShadingLanguage::CGLanguage, "cg.spv"  },
            { EShadingLanguage::XShade,     "xs.spv"  },
            { EShadingLanguage::GLSL,       glslStage },
            { EShadingLanguage::HLSL,       hlslStage },
            { EShadingLanguage::Unknown,    "unknown" }
        };

        extension = mapValue<EShadingLanguage, std::string>(aLanguage, std::move(languageAssignment));

        return CString::format("%0.%1", aFileBaseName, extension);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------

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
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------

    /**
     * Compile a single shader file.
     *
     * @param aFilename
     * @param aOptions
     * @return
     */
    CResult<SShaderCompilationUnit> generateCompilationUnit(std::vector<std::string> const &aFilenames)
    {
        EShaderCompiler  compiler = EShaderCompiler::Unknown;
        EShadingLanguage language = EShadingLanguage::Unknown;

        auto const deriveElement = [&compiler, this] (std::string const &aFilename) -> SShaderCompilationElement
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

            std::string const outputName = getOutputFilename(std::filesystem::path(aFilename).stem(), language, stage);
            std::string const outputPath = std::filesystem::path(mConfig.outputPath) / outputName;

            SShaderCompilationElement element {};
            element.fileName   = aFilename;
            element.contents   = shaderString;
            element.stage      = stage;
            element.outputPath = outputPath;

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
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------

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
        std::shared_ptr<CPrecompiler> precompiler = std::make_shared<CPrecompiler>();

        // Read all shader-files and convert them to spirv.
        // Then go for SPIRV-cross, perform reflection and generate headers
        // for all shaders.
        precompiler->initialize();
        precompiler->processArguments(static_cast<uint32_t>(aArgC), aArgV);
        precompiler->run();
        precompiler->deinitialize();
        precompiler.reset();
    }
    catch (...)
    {
    }

    #ifdef SHIRABE_DEBUG
    CConsole::DeinitializeConsole();
    #endif

    return 0;
}
