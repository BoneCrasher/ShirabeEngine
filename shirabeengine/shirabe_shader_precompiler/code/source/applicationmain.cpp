#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

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
#include <core/string.h>
#include <core/bitfield.h>
#include <core/enginetypehelper.h>
#include <core/result.h>

using namespace engine;

#if ENABLE_OPT
static constexpr bool const OPTIMIZATION_ENABLED = true;
#else
static constexpr bool const OPTIMIZATION_ENABLED = false;
#endif

namespace Main
{
    SHIRABE_DECLARE_LOG_TAG(ShirabeEngineShaderPrecompiler);
}

/**
 * Describes error codes of the tool.
 */
enum class EResult
{
    Success      =       0,
    WrongUsage   = -100000,
    NoInput,
    FileError,
    InputInvalid,
};

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------

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
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------

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
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------

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

//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------

/**
 * Read a file into a string.
 *
 * @param aFileName Filename of the file to read.
 * @return          See brief.
 */
static std::string readFile(std::string const &aFileName)
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

    CLog::Warning(Main::logTag(), usageMessage);

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

    /**
     * Flags describing the selected shading language
     */
    enum class EShadingLanguage
    {
        Unknown    = 0,
        CGLanguage,
        GLSL,
        HLSL,
        XShade
    };

    /**
     * Describes the stage of the shader to compile.
     */
    enum class EShaderStage
    {
        NotApplicable = 0,
        Vertex,
        TesselationControlPoint,
        TesselationEvaluation,
        Geometry,
        Fragment,
        Compute
    };

    /**
     * Identifies the respective compiler to use to compile a shader file.
     */
    enum class EShaderCompiler
    {
        Unknown                  = 0,
        CGLanguage,
        GlslangReferenceCompiler,
        DxHlslCompiler,
        XShadeCompiler,
    };

private_structs:

    /**
     * @brief The SConfiguration struct
     */
    struct SConfiguration
    {
        std::vector<std::string>                inputPaths;
        std::string                             outputPath;
        std::vector<std::string>                outputFilenames;
        core::CBitField<CPrecompiler::EOptions> options;
    };


    /**
     * @brief The SShaderCompilationElement struct
     */
    struct SShaderCompilationElement
    {
        std::string  fileName;
        std::string  contents;
        EShaderStage stage;

    public_constructors:
        /**
         * @brief SShaderCompilationElement
         */
        SShaderCompilationElement()
            : fileName({})
            , contents({})
            , stage(EShaderStage::NotApplicable)
        {}

        /**
         * @brief SShaderCompilationElement
         *
         * @param aFileName
         * @param aContents
         * @param aStage
         */
        SShaderCompilationElement(std::string  const &aFileName,
                                  std::string  const &aContents,
                                  EShaderStage const aStage)
            : fileName(aFileName)
            , contents(aContents)
            , stage(aStage)
        {}
    };

    /**
     * Simple bundling of what makes a compilation unit for ease in passing around,
     * and separation of handling file IO versus API (programmatic) compilation.
     */
    struct SShaderCompilationUnit
    {
        EShaderCompiler           compiler;
        EShadingLanguage          language;

        std::vector<SShaderCompilationElement> elements;

    public_constructors:

        /**
         * @brief SShaderCompilationUnit
         */
        SShaderCompilationUnit()
            : compiler(EShaderCompiler::Unknown)
            , language(EShadingLanguage::Unknown)
        {}

        /**
         * @brief SShaderCompilationUnit
         * @param aCompiler
         * @param aLanguage
         */
        SShaderCompilationUnit(EShaderCompiler  const aCompiler,
                               EShadingLanguage const aLanguage)
            : compiler(aCompiler)
            , language(aLanguage)
        { }

    public_methods:

        /**
         * @brief addElement
         * @param aFileName
         * @param aContents
         * @param aStage
         * @return
         */
        EResult addElement(std::string  const &aFileName,
                           std::string  const &aContents,
                           EShaderStage const  aStage)
        {
            if(aFileName.empty() || aContents.empty())
            {
                CLog::Error(logTag(), CString::format("Invalid input for file %0:\n%1", aFileName, aContents));
                return EResult::InputInvalid;
            }

            addElement({ aFileName, aContents, aStage });
        }

        /**
         * @brief addElement
         * @param aOther
         * @return
         */
        EResult addElement(SShaderCompilationElement const &aOther)
        {
            elements.push_back(aOther);
        }
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

        core::CBitField<EOptions> options    = {};
        std::string               outputPath = {};
        std::vector<std::string>  inputPaths = {};

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
                { "-o",               [&] () { outputPath = referencableValue;                             return true; }},
                { "-i",               [&] () { inputPaths = readInputPaths(referencableValue).data();      return true; }},
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
        // Derive filenames from the list of input paths.
        //
        std::vector<std::vector<std::string>> derivedFilenames{};

        auto const deriveInputFilenames = [&] (std::string const &aInputPath) -> std::vector<std::string>
        {
            std::vector<std::string> inputFilenames{};
            std::vector<std::string> outputFilenames{};

            //
            // Determine a list of filenames.
            // IF the input path itself is a filename, it is just used.
            // Otherwise, if it's a directory, scan it for input files.
            // A directory can be scanned recursively, if configured to do so.
            //
            auto const inputFilename = std::filesystem::path(aInputPath);

            bool const exists = std::filesystem::exists(aInputPath);
            if(not exists)
            {
                CLog::Debug(logTag(), CString::format("Path '%0' does not exist. Skipping.", aInputPath));
                return {};
            }

            bool const isDirectory = std::filesystem::is_directory(inputFilename);
            if(isDirectory)
            {
                if(options.check(CPrecompiler::EOptions::RecursiveScan))
                {
                    auto const fileIterator = std::filesystem::recursive_directory_iterator(inputFilename);
                    for(std::filesystem::directory_entry const &file : fileIterator)
                    {
                        if(not file.is_regular_file())
                        {
                            continue;
                        }

                        inputFilenames.push_back(file.path().string());
                    }
                }
                else
                {
                    auto const fileIterator = std::filesystem::directory_iterator(inputFilename);
                    for(std::filesystem::directory_entry const &file : fileIterator)
                    {
                        if(not file.is_regular_file())
                        {
                            continue;
                        }

                        inputFilenames.push_back(file.path().string());
                    }
                }
            }
            else
            {
                CLog::Debug(logTag(), CString::format("Output path '%0' does not exist. Creating.", outputPath));
                inputFilenames.push_back(aInputPath);
            }

            return inputFilenames;
        };
        std::transform(inputPaths.begin(), inputPaths.end(), std::back_inserter(derivedFilenames), deriveInputFilenames);

        // Make sure the output config is correct.
        bool const outputPathExists = std::filesystem::exists(outputPath);
        if(not outputPathExists)
        {
            std::filesystem::path const outputPathAbsolute = std::filesystem::current_path() / outputPath;
            std::filesystem::create_directory(outputPathAbsolute);
        }

        SConfiguration config {};
        config.options    = options;
        config.outputPath = outputPath;

        //
        //  Reduce the list of per-path derived filenames to a single filename list.
        //
        auto const reducer = [&config] (std::vector<std::string> const &fileNames)
        {
            config.inputPaths.insert(config.inputPaths.end(), fileNames.begin(), fileNames.end());
        };
        std::for_each(derivedFilenames.begin(), derivedFilenames.end(), reducer);

        mConfig = config;

        return EResult::Success;
    }

    EResult run()
    {
        // Determine compilation items and config.
        CResult<SShaderCompilationUnit> const unitGeneration = generateCompilationUnit(mConfig.inputPaths);
        if(not unitGeneration.successful())
        {
            CLog::Error(logTag(), "Failed to derive shader compilation units and configuration");
            return EResult::InputInvalid;
        }

        SShaderCompilationUnit const unit = unitGeneration.data();

        // Invoke compiler


        // Perform reflection and generate output file

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

        static std::unordered_map<EShadingLanguage, std::string> languageAssignment  =
        {
            { EShadingLanguage::CGLanguage, "cg.spv"  },
            { EShadingLanguage::XShade,     "xs.spv"  },
            { EShadingLanguage::GLSL,       mapValue<EShaderStage, std::string>(aStage, std::move(glslStageAssignment)) },
            { EShadingLanguage::HLSL,       mapValue<EShaderStage, std::string>(aStage, std::move(hlslStageAssignment)) },
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

            // TODO store output filename in element.
            SShaderCompilationElement element {};
            element.fileName = aFilename;
            element.contents = shaderString;
            element.stage    = stage;

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
