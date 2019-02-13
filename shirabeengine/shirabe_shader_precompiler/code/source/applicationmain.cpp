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

#include <glslang/Include/ShHandle.h>
#include <glslang/Include/revision.h>
#include <glslang/Public/ShaderLang.h>

#include <SPIRV/GlslangToSpv.h>
#include <SPIRV/GLSL.std.450.h>
#include <SPIRV/doc.h>
#include <SPIRV/disassemble.h>

#include <log/log.h>
#include <core/string.h>
#include <core/bitfield.h>
#include <core/enginetypehelper.h>

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
        MultiThreaded        = (1u << 1),
        DumpConfig           = (1u << 2),
        DumpReflection       = (1u << 3),
        DumpBareVersion      = (1u << 4),
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

            elements.push_back({ aFileName, aContents, aStage });
        }
    };

public_methods:


    EResult initialize()
    {
        return EResult::Success;
    }

    EResult run()
    {
        return EResult::Success;
    }

    EResult deinitialize()
    {
        return EResult::Success;
    }

private_methods:

    /**
     * Accept an inline list of options and check if a specific value is contained in this list.
     *
     * @param aOptions
     * @param aCompare
     * @return
     */
    template <typename TValue>
    bool anyOf(std::vector<TValue> const &&aOptions, TValue const &aCompare)
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
    std::enable_if_t<std::is_default_constructible_v<TValue>, TValue> const mapValue(TKey const &aExtension, std::unordered_map<TKey, TValue> const &&aOptions)
    {
        bool const contained = (aOptions.end() != std::find(aOptions.begin(), aOptions.end(), aExtension));
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

        size_t const primaryExtensionPosition   = aFileName.find_last_of(".");
        size_t const secondaryExtensionPosition = aFileName.find_last_of(".", primaryExtensionPosition - 1);

        bool const hasPrimaryExtension   = (std::string::npos != primaryExtensionPosition);
        bool const hasSecondaryExtension = (std::string::npos != secondaryExtensionPosition);

        if(hasPrimaryExtension)
        {
            primaryExtension = aFileName.substr(primaryExtensionPosition + 1, std::string::npos);
        }

        if(hasSecondaryExtension)
        {
            secondaryExtension = aFileName.substr(secondaryExtensionPosition + 1, (primaryExtensionPosition - secondaryExtensionPosition - 1));
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
            usage();
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
            usage();
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

        if(EShadingLanguage::CGLanguage == aLanguage)
        {
            extension = ".cg.spv";
        }
        else if(EShadingLanguage::XShade == aLanguage)
        {
            extension = "spv";
        }
        else
        {
            switch (aStage)
            {
            case EShaderStage::Vertex:                  extension = (EShadingLanguage::HLSL == aLanguage) ? "vs.hlsl.spv" : "vert.glsl.spv";  break;
            case EShaderStage::TesselationControlPoint: extension = (EShadingLanguage::HLSL == aLanguage) ? "hs.hlsl.spv" : "tesc.glsl.spv";  break;
            case EShaderStage::TesselationEvaluation:   extension = (EShadingLanguage::HLSL == aLanguage) ? "ds.hlsl.spv" : "tese.glsl.spv";  break;
            case EShaderStage::Geometry:                extension = (EShadingLanguage::HLSL == aLanguage) ? "gs.hlsl.spv" : "geom.glsl.spv";  break;
            case EShaderStage::Fragment:                extension = (EShadingLanguage::HLSL == aLanguage) ? "ps.hlsl.spv" : "frag.glsl.spv";  break;
            case EShaderStage::Compute:                 extension = (EShadingLanguage::HLSL == aLanguage) ? "cs.hlsl.spv" : "comp.glsl.spv";  break;
    #ifdef NV_EXTENSIONS
            case EShaderStage::NVRayGen:                extension = "rgen.spv";  break;
            case EShaderStage::NVIntersect:             extension = "rint.spv";  break;
            case EShaderStage::NVAnyHit:                extension = "rahit.spv"; break;
            case EShaderStage::NVClosestHit:            extension = "rchit.spv"; break;
            case EShaderStage::NVMiss:                  extension = "rmiss.spv"; break;
            case EShaderStage::NVCallable:              extension = "rcall.spv"; break;
            case EShaderStage::NVMesh:                  extension = "mesh.spv";  break;
            case EShaderStage::NVTask:                  extension = "task.spv";  break;
    #endif
            default:                                    extension = "unknown";   break;
            }
        }

        return CString::format("%0.%1", aFileBaseName, aStage);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------

    /**
     * @brief compile
     * @param aFilename
     * @param aCompiler
     */
    void compile(std::string                       const &aFilename,
                 engine::core::CBitField<EOptions> const &aOptions)
    {
        int result = 0;

        std::string shaderString = readFile(aFilename);
        if(shaderString.empty())
        {
            CLog::Error(logTag(), CString::format("Shader file %0 is empty.", aFilename));
            return;
        }

        // move to length-based strings, rather than null-terminated strings
        uint64_t const length = shaderString.size();
        SHIRABE_UNUSED(length);

        // Invoke specific compiler.


    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------

    /**
     * Thread entry point, for non-linking asynchronous mode.
     *
     * @param worklist
     */
    void compileShaders(CWorklist &aWorklist)
    {
        if (mOptions.check(EOptions::Debug))
        {
            Error(logTag(), "cannot generate debug information unless linking to generate code");
        }

        auto const compileFn = [this] (CWorkItem *aWorkItem, std::string const &aFilename) -> bool
        {
            if(nullptr == aWorkItem)
            {
                return false;
            }

            EShLanguage const language = determineTargetLanguage(aFilename, false, mOptions);
            int32_t     const options  = static_cast<int32_t>(mOptions.value());

            ShHandle compiler = ShConstructCompiler(language, options);
            if (nullptr == compiler)
            {
                return false;
            }

            compile(aFilename, compiler, mOptions);

            if (not mOptions.check(EOptions::SuppressInfolog))
            {
                aWorkItem->results() = ShGetInfoLog(compiler);
            }

            ShDestruct(compiler);

            return true;
        };

        CWorkItem *workItem = nullptr;
        if (mOptions.check(EOptions::Stdin))
        {
            if (aWorklist.remove(workItem))
            {
                bool const success = compileFn(workItem, "stdin");
                SHIRABE_UNUSED(success);
            }
        }
        else
        {
            while (aWorklist.remove(workItem))
            {
                bool const success = compileFn(workItem, workItem->name());
                SHIRABE_UNUSED(success);
            }
        }
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------

    /**
     * Do file IO part of compile and link, handing off the pure
     * API/programmatic mode to CompileAndLinkShaderUnits(), which can
     * be put in a loop for testing memory footprint and performance.
     *
     * This is just for linking mode: meaning all the shaders will be put into the
     * the same program linked together.
     *
     * This means there are a limited number of work items (not multi-threading mode)
     * and that the point is testing at the linking level. Hence, to enable
     * performance and memory testing, the actual compile/link can be put in
     * a loop, independent of processing the work items and file IO.
     *
     * @param aWorklist List of CWorkItem's to process.
     */
    void compileAndLinkShaderFiles(CWorklist &aWorklist)
    {
        std::vector<SShaderCompilationUnit> compilationUnits;

        // If this is using stdin, we can't really detect multiple different file
        // units by input type. We need to assume that we're just being given one
        // file of a certain type.
        if(mOptions.check(EOptions::Stdin))
        {
            EShLanguage const language = determineTargetLanguage("stdin", false, mOptions);

            std::string const fileName = "stdin";
            std::string const cinInput((std::istreambuf_iterator<char>(std::cin)),
                                        std::istreambuf_iterator<char>());

            SShaderCompilationUnit compilationUnit(language);
            compilationUnit .addString(fileName, strdup(cinInput.c_str()));
            compilationUnits.push_back(compilationUnit);
        }
        else
        {
            // Transfer all the work items from to a simple list of
            // of compilation units.  (We don't care about the thread
            // work-item distribution properties in this path, which
            // is okay due to the limited number of shaders, know since
            // they are all getting linked together.)
            CWorkItem *workItem = nullptr;

            while (aWorklist.remove(workItem))
            {
                EShLanguage const language = determineTargetLanguage(workItem->name(), true, mOptions);

                std::string fileText = ReadFileData(workItem->name().c_str());
                if (fileText.empty())
                {
                    usage();
                }

                SShaderCompilationUnit compilationUnit(language);
                compilationUnit .addString(workItem->name(), fileText);
                compilationUnits.push_back(compilationUnit);
            }
        }

        // Actual call to programmatic processing of compile and link,
        // in a loop for testing memory and performance.  This part contains
        // all the perf/memory that a programmatic consumer will care about.
        uint64_t const limit = mOptions.check(EOptions::MemoryLeakMode) ? 100 : 1;

        for (uint64_t i = 0; i < limit; ++i)
        {
            for (uint64_t j = 0; j < limit; ++j)
            {
               compileAndLinkShaderUnits(compilationUnits);
            }

            if(mOptions.check(EOptions::MemoryLeakMode))
            {
                // glslang::OS_DumpMemoryCounters();
            }
        }
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------

    /**
     * For linking mode: Will independently parse each compilation unit, but then put them
     * in the same program and link them together, making at most one linked module per
     * pipeline stage.
     *
     * Uses the new C++ interface instead of the old handle-based interface.
     *
     * @param aCompilationUnits
     */
    void compileAndLinkShaderUnits(std::vector<SShaderCompilationUnit> aCompilationUnits)
    {
        // keep track of what to free
        std::list<glslang::TShader*> shaders;

        EShMessages messages = EShMsgDefault;
        SetMessageOptions(messages);

        //
        // Per-shader processing...
        //

        glslang::TProgram *program = new glslang::TProgram;

        for(SShaderCompilationUnit const &unit : aCompilationUnits)
        {
            glslang::TShader *shader = new glslang::TShader(unit.stage);
            shader->setStringsWithLengthsAndNames(unit.text.data(),
                                                  nullptr,
                                                  unit.fileNameList.data(),
                                                  static_cast<int32_t>(unit.count));
            //
            // ENTRY POINT
            //
            if (not mEntryPointName.empty())
            {
                shader->setEntryPoint(mEntryPointName.c_str());
            }

            if (not mSourceEntryPointName.empty())
            {
                if (mEntryPointName.empty())
                {
                    printf("Warning: Changing source entry point name without setting an entry-point name.\n"
                           "Use '-e <name>'.\n");
                }

                shader->setSourceEntryPoint(mSourceEntryPointName.c_str());
            }

            //
            // PREAMBLE
            //
            if (mUserPreamble.isSet())
            {
                shader->setPreamble(mUserPreamble.get().c_str());
            }

            shader->addProcesses(mProcesses);

            //
            // BINDING/IO-MAPPING
            //

            // Set IO mapper binding shift values
            for (int r = 0; r < glslang::EResCount; ++r)
            {
                const glslang::TResourceType resource = glslang::TResourceType(r);

                // Set base bindings
                shader->setShiftBinding(resource, mBaseBinding[resource][unit.stage]);

                // Set bindings for particular resource sets
                // TODO: use a range based for loop here, when available in all environments.
                for(auto const &[set, base] : mBaseBindingForSet[resource][unit.stage])
                {
                    shader->setShiftBindingForSet(resource, base, set);
                }

            }

            shader->setFlattenUniformArrays(mOptions.check(EOptions::FlattenUniformArrays));
            shader->setNoStorageFormat     (mOptions.check(EOptions::NoStorageFormat));
            shader->setResourceSetBinding  (mBaseResourceSetBinding[unit.stage]);

            if (mOptions.check(EOptions::HlslIoMapping))
            {
                shader->setHlslIoMapping(true);
            }

            if (mOptions.check(EOptions::AutoMapBindings))
            {
                shader->setAutoMapBindings(true);
            }

            if (mOptions.check(EOptions::AutoMapLocations))
            {
                shader->setAutoMapLocations(true);
            }

            if (mOptions.check(EOptions::InvertY))
            {
                shader->setInvertY(true);
            }

            for (auto const &[name, location] : mUniformLocationOverrides)
            {
                shader->addUniformLocationOverride(name.c_str(), location);
            }

            shader->setUniformLocationBase(static_cast<int32_t>(mUniformBase));

            // Set up the environment, some subsettings take precedence over earlier
            // ways of setting things.
            if (mOptions.check(EOptions::Spv))
            {
                shader->setEnvInput(mOptions.check(EOptions::ReadHlsl)
                                        ? glslang::EShSourceHlsl
                                        : glslang::EShSourceGlsl,
                                    unit.stage,
                                    mClient,
                                    mClientInputSemanticsVersion);

                shader->setEnvClient(mClient, mClientVersion);
                shader->setEnvTarget(mTargetLanguage, mTargetVersion);

                if (mTargetHlslFunctionality1)
                {
                    shader->setEnvTargetHlslFunctionality1();
                }
            }

            shaders.push_back(shader);

            int32_t const defaultVersion = mOptions.check(EOptions::DefaultDesktop) ? 110 : 100;

            CDirectoryStackFileIncluder includer {};

            auto const includeAction = [&includer] (std::string const &aDirectory)
            {
                includer.pushExternalLocalDirectory(aDirectory);
            };
            std::for_each(mIncludeDirectoryList.rbegin(), mIncludeDirectoryList.rend(), includeAction);

            if (mOptions.check(EOptions::OutputPreprocessed))
            {
                std::string outputString {};

                if (shader->preprocess(&mResources,
                                       defaultVersion,
                                       ENoProfile,
                                       false,
                                       false,
                                       messages,
                                       &outputString,
                                       includer))
                {
                    putsIfNonEmpty(outputString.c_str());
                }
                else
                {
                    mCompileFailed = true;
                }

                stderrIfNonEmpty(shader->getInfoLog());
                stderrIfNonEmpty(shader->getInfoDebugLog());

                continue;
            }

            bool const parseSuccessful = shader->parse(&mResources, defaultVersion, false, messages, includer);
            if(not parseSuccessful)
            {
                mCompileFailed = true;
            }

            program->addShader(shader);

            if (not mOptions.check(EOptions::SuppressInfolog) &&
                not mOptions.check(EOptions::MemoryLeakMode))
            {
                putsIfNonEmpty(unit.fileName[0].c_str());
                putsIfNonEmpty(shader->getInfoLog());
                putsIfNonEmpty(shader->getInfoDebugLog());
            }
        }

        //
        // Program-level processing...
        //

        // Link
        if (not mOptions.check(EOptions::OutputPreprocessed) && not program->link(messages))
        {
            mLinkFailed = true;
        }

        // Map IO
        if (mOptions.check(EOptions::Spv))
        {
            if (not program->mapIO())
            {
                mLinkFailed = true;
            }
        }

        // Report
        if (not mOptions.check(EOptions::SuppressInfolog) &&
            not mOptions.check(EOptions::MemoryLeakMode))
        {
            putsIfNonEmpty(program->getInfoLog());
            putsIfNonEmpty(program->getInfoDebugLog());
        }

        // Reflect
        if (mOptions.check(EOptions::DumpReflection))
        {
            program->buildReflection();
            program->dumpReflection();
        }

        // Dump SPIR-V
        if (mOptions.check(EOptions::Spv))
        {
            if (mCompileFailed || mLinkFailed)
            {
                printf("SPIR-V is not generated for failed compile or link\n");
            }
            else
            {
                for (int stage = 0; stage < EShLangCount; ++stage)
                {
                    EShLanguage const language = static_cast<EShLanguage>(stage);
                    if (program->getIntermediate(language))
                    {
                        std::vector<unsigned int> spirv;
                        std::string               warningsErrors;
                        spv::SpvBuildLogger       logger;
                        glslang::SpvOptions       spvOptions;

                        if (mOptions.check(EOptions::Debug))
                        {
                            spvOptions.generateDebugInfo = true;
                        }

                        spvOptions.disableOptimizer = mOptions.check(EOptions::OptimizeDisable);
                        spvOptions.optimizeSize     = mOptions.check(EOptions::OptimizeSize);
                        spvOptions.disassemble      = mSpvToolsDisassembler;
                        spvOptions.validate         = mSpvToolsValidate;

                        glslang::TIntermediate const *const intermediate = program->getIntermediate(language);

                        glslang::GlslangToSpv(*intermediate,
                                               spirv,
                                              &logger,
                                              &spvOptions);

                        // Dump the spv to a file or stdout, etc., but only if not doing
                        // memory/perf testing, as it's not internal to programmatic use.
                        if (not mOptions.check(EOptions::MemoryLeakMode))
                        {
                            printf("%s", logger.getAllMessages().c_str());

                            std::string const binaryName = GetBinaryName(static_cast<EShLanguage>(stage));

                            if (mOptions.check(EOptions::OutputHexadecimal))
                            {
                                glslang::OutputSpvHex(spirv, binaryName.c_str(), mVariableName.c_str());
                            }
                            else
                            {
                                glslang::OutputSpvBin(spirv, binaryName.c_str());
                            }

                            if (not mSpvToolsDisassembler && mOptions.check(EOptions::HumanReadableSpv))
                            {
                                spv::Disassemble(std::cout, spirv);
                            }
                        }
                    }
                }
            }
        }

        // Free everything up, program has to go before the shaders
        // because it might have merged stuff from the shaders, and
        // the stuff from the shaders has to have its destructors called
        // before the pools holding the memory in the shaders is freed.
        delete program;

        while (0 < shaders.size())
        {
            delete shaders.back();
            shaders.pop_back();
        }
    }    
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------

    /**
     * Parse either a .conf file provided by the user or the default from glslang::DefaultTBuiltInResource
     */
    void processConfigFile()
    {
        if(0 == mConfigFile.size())
        {
            mResources = glslang_wrapper::DefaultBuiltInResource();
        }
        else
        {
            std::string configString  = ReadFileData(mConfigFile);
            char       *configCString = configString.data();

            DecodeResourceLimits(&mResources, configCString);
        }
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------

    /**
     * Process a list of files.
     *
     * @param aInputFilenames See brief.
     */
    void process(std::vector<std::string> const &aInputFilenames)
    {
        using namespace engine;

        ShInitialize();

        for(std::string const &filename : aInputFilenames)
        {
            EShLanguage const lang = determineTargetLanguage(filename, true, mOptions);

            ShHandle compiler = ShConstructCompiler(lang, static_cast<int>(mOptions.value()));
            if (nullptr == compiler)
            {
                return;
            }

            compile("stdin", compiler, mOptions);

            if (false == (mOptions.check(EOptions::SuppressInfolog)))
            {
                std::string const log = ShGetInfoLog(compiler);
                CLog::Error(Main::logTag(), log);
            }

            ShDestruct(compiler);
        }

        ShFinalize();
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------

    /**
     * Translate the meaningful subset of command-line options to parser-behavior options.
     *
     * @param messages
     */
    void SetMessageOptions(EShMessages &aOutMessages)
    {
        if (mOptions.check(EOptions::RelaxedErrors))
        {
            aOutMessages = static_cast<EShMessages>(aOutMessages | EShMsgRelaxedErrors);
        }

        if (mOptions.check(EOptions::Intermediate))
        {
            aOutMessages = static_cast<EShMessages>(aOutMessages | EShMsgAST);
        }

        if (mOptions.check(EOptions::SuppressWarnings))
        {
            aOutMessages = static_cast<EShMessages>(aOutMessages | EShMsgSuppressWarnings);
        }

        if (mOptions.check(EOptions::Spv))
        {
            aOutMessages = static_cast<EShMessages>(aOutMessages | EShMsgSpvRules);
        }

        if (mOptions.check(EOptions::VulkanRules))
        {
            aOutMessages = static_cast<EShMessages>(aOutMessages | EShMsgVulkanRules);
        }

        if (mOptions.check(EOptions::OutputPreprocessed))
        {
            aOutMessages = static_cast<EShMessages>(aOutMessages | EShMsgOnlyPreprocessor);
        }

        if (mOptions.check(EOptions::ReadHlsl))
        {
            aOutMessages = static_cast<EShMessages>(aOutMessages | EShMsgReadHlsl);
        }

        if (mOptions.check(EOptions::CascadingErrors))
        {
            aOutMessages = static_cast<EShMessages>(aOutMessages | EShMsgCascadingErrors);
        }

        if (mOptions.check(EOptions::KeepUncalled))
        {
            aOutMessages = static_cast<EShMessages>(aOutMessages | EShMsgKeepUncalled);
        }

        if (mOptions.check(EOptions::HlslOffsets))
        {
            aOutMessages = static_cast<EShMessages>(aOutMessages | EShMsgHlslOffsets);
        }

        if (mOptions.check(EOptions::Debug))
        {
            aOutMessages = static_cast<EShMessages>(aOutMessages | EShMsgDebugInfo);
        }

        if (mHlslEnable16BitTypes)
        {
            aOutMessages = static_cast<EShMessages>(aOutMessages | EShMsgHlslEnable16BitTypes);
        }

        if (mOptions.check(EOptions::OptimizeDisable) || not OPTIMIZATION_ENABLED)
        {
            aOutMessages = static_cast<EShMessages>(aOutMessages | EShMsgHlslLegalization);
        }

        if (mHlslDX9compatible)
        {
            aOutMessages = static_cast<EShMessages>(aOutMessages | EShMsgHlslDX9Compatible);
        }
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------

    /**
     * Process an optional binding base of one the forms:
     *   --argname [stage] base            // base for stage (if given) or all stages (if not)
     *   --argname [stage] [base set]...   // set/base pairs: set the base for given binding set.
     *
     * Where stage is one of the forms accepted by FindLanguage, and base is an integer
     *
     *  @param aArgC
     * @param aArgV
     * @param aResource
     */
    void ProcessBindingBase(uint32_t &aArgC, char**& aArgV, glslang::TResourceType aResource)
    {
        if(2 > aArgC)
        {
            usage();
        }

        EShLanguage language = EShLangCount;

        uint32_t            singleBase = 0;
        PerSetBaseBinding_t perSetBase = {};

        uint32_t arg = 1;

        // Parse stage, if given
        if(not std::isdigit(aArgV[arg][0]))
        {
            if(3 > aArgC)
            {
                usage();
            }

            language = determineTargetLanguage(aArgV[arg++], false, mOptions);
        }

        if( 2 < (aArgC - arg) && std::isdigit(aArgV[arg + 0][0]) && std::isdigit(aArgV[arg + 1][0]) )
        {
            while(2 < (aArgC - arg) && std::isdigit(aArgV[arg + 0][0]) && std::isdigit(aArgV[arg + 1][0]) )
            {
                uint32_t const baseNumber = CString::fromString<uint32_t>(aArgV[arg++]);
                uint32_t const setNumber  = CString::fromString<uint32_t>(aArgV[arg++]);

                perSetBase[setNumber] = baseNumber;
            }
        }
        else
        {
            singleBase = CString::fromString<uint32_t>(aArgV[arg++]);
        }

        aArgC -= (arg - 1);
        aArgV += (arg - 1);

        // Set one or all languages
        uint32_t const languageMin = (EShLangCount > language) ? (language + 0) : 0;
        uint32_t const languageMax = (EShLangCount > language) ? (language + 1) : EShLangCount;

        for(uint32_t languageIndex = languageMin; languageIndex < languageMax; ++languageIndex)
        {
            if(not perSetBase.empty())
            {
                mBaseBindingForSet[aResource][languageIndex].insert(perSetBase.begin(), perSetBase.end());
            }
            else
            {
                mBaseBinding[aResource][languageIndex] = singleBase;
            }
        }
    }

    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------

    void ProcessResourceSetBindingBase(uint32_t &aArgC, char **&aArgV, std::array<std::vector<std::string>, EShLangCount> &aBase)
    {
        if(2 > aArgC)
        {
            usage();
        }

        if(not std::isdigit(aArgV[1][0]))
        {
            if(3 > aArgC)
            {
                usage();
            }

            // Parse form: --argname stage [regname set base...], or:
            //             --argname stage set
            EShLanguage const language = determineTargetLanguage(aArgV[1], false, mOptions);

            --aArgC;
            ++aArgV;

            while(1 < aArgC && nullptr != aArgV[1] && '-' != aArgV[1][0])
            {
                aBase[language].push_back(aArgV[1]);

                --aArgC;
                ++aArgV;
            }

            if(1 != aBase[language].size() && 0 != (aBase[language].size() % 3))
            {
                usage();
            }
        }
        else
        {
            for(uint32_t language = 0; language < EShLangCount; ++language)
            {
                aBase[language].push_back(aArgV[1]);
            }

            --aArgC;
            ++aArgV;
        }
    }

    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------

    /**
     * Do all command-line argument parsing.  This includes building up the work-items
     * to be processed later, and saving all the command-line options.
     *
     * Does not return (it exits) if command-line is fatally flawed.
     *
     * @param workItems
     * @param argc
     * @param argv
     */
    void ProcessArguments(uint32_t aArgC, char *aArgV[], std::vector<std::unique_ptr<CWorkItem>>& aOutWorkItems)
    {
        for (uint64_t k = 0; k < glslang::EResCount; ++k)
        {
            mBaseBinding[k].fill(0);
        }

        mExecutableName = aArgV[0];

        mWorkItems.reserve(static_cast<size_t>(aArgC));

        const auto bumpArg = [&]()
        {
            if (0 < aArgC)
            {
                --aArgC;
                ++aArgV;
            }
        };

        // read a string directly attached to a single-letter option
        const auto getStringOperand = [&] (char const *aDesc)
        {
            if (0 == aArgV[0][2])
            {
                printf("%s must immediately follow option (no spaces)\n", aDesc);
                std::exit(EnumValueOf(EFailCode::FailUsage));
            }

            return (aArgV[0] + 2);
        };

        // read a number attached to a single-letter option
        const auto getAttachedNumber = [&] (char const *aDesc)
        {
            int num = atoi(aArgV[0] + 2);
            if (0 == num)
            {
                printf("%s: expected attached non-0 number\n", aDesc);
                std::exit(EnumValueOf(EFailCode::FailUsage));
            }

            return num;
        };

        // minimum needed (without overriding something else) to target Vulkan SPIR-V
        const auto setVulkanSpv = [this] ()
        {
            if (glslang::EShClientNone == mClient)
            {
                mClientVersion = glslang::EShTargetVulkan_1_0;
            }

            mClient = glslang::EShClientVulkan;

            mOptions.set(EOptions::Spv);
            mOptions.set(EOptions::VulkanRules);
            mOptions.set(EOptions::LinkProgram);
        };

        // minimum needed (without overriding something else) to target OpenGL SPIR-V
        const auto setOpenGlSpv = [this] ()
        {
            if (glslang::EShClientNone == mClient)
            {
                mClientVersion = glslang::EShTargetOpenGL_450;
            }

            mClient = glslang::EShClientOpenGL;

            mOptions.set  (EOptions::Spv);
            mOptions.set  (EOptions::LinkProgram);
            mOptions.unset(EOptions::VulkanRules);
        };

        const auto getUniformOverride = [getStringOperand] () -> std::pair<std::string, uint64_t>
        {
            std::string            const arg           = getStringOperand("-u<name>:<location>");
            std::string::size_type const splitPosition = arg.find_first_of(':');

            if (std::string::npos == splitPosition)
            {
                printf("%s: missing location\n", arg.c_str());
                std::exit(EnumValueOf(EFailCode::FailUsage));
            }

            std::string const nameString     = arg.substr(0,                 splitPosition);
            std::string const locationString = arg.substr(splitPosition + 1, std::string::npos);
            uint64_t    const location       = CString::fromString<uint64_t>(locationString);

            auto const pair = std::make_pair(nameString, location);
            return pair;
        };

        for (bumpArg(); aArgC >= 1; bumpArg())
        {
            if ('-' == aArgV[0][0])
            {
                switch (aArgV[0][1])
                {
                case '-': // --<flag>
                    {
                        std::string flag(aArgV[0] + 2);

                        std::transform(flag.begin(), flag.end(), flag.begin(), ::tolower);

                        // handle --word style options
                        if ("auto-map-bindings" == flag ||  // synonyms
                            "auto-map-binding"  == flag ||
                            "amb"               == flag)
                        {
                            mOptions.set(EOptions::AutoMapBindings);
                        }
                        else if ("auto-map-locations" == flag || // synonyms
                                 "aml"                == flag)
                        {
                            mOptions.set(EOptions::AutoMapLocations);
                        }
                        else if ("uniform-base" == flag)
                        {
                            if (1 >= aArgC)
                            {
                                Error(logTag(), "no <base> provided for --uniform-base");
                            }

                            mUniformBase = CString::fromString<uint32_t>(std::string(aArgV[1]));

                            bumpArg();
                            break;
                        }
                        else if ("client" == flag)
                        {
                            if (1 < aArgC)
                            {
                                std::string clientString = aArgV[1];
                                if("vulkan100" == clientString)
                                {
                                    setVulkanSpv();
                                }
                                else if("opengl100" == clientString)
                                {
                                    setOpenGlSpv();
                                }
                                else
                                {
                                    Error(logTag(), "--client expects vulkan100 or opengl100");
                                }
                            }

                            bumpArg();
                        }
                        else if ("entry-point" == flag)
                        {
                            mEntryPointName = std::string(aArgV[1]);

                            if(1 >= aArgC)
                            {
                                Error(logTag(), "no <name> provided for --entry-point");
                            }

                            bumpArg();
                        }
                        else if ("flatten-uniform-arrays" == flag || // synonyms
                                 "flatten-uniform-array"  == flag ||
                                 "fua"                    == flag )
                        {
                            mOptions.set(EOptions::FlattenUniformArrays);
                        }
                        else if ("hlsl-offsets" == flag)
                        {
                            mOptions.set(EOptions::HlslOffsets);
                        }
                        else if ("hlsl-iomap"     == flag ||
                                 "hlsl-iomapper"  == flag ||
                                 "hlsl-iomapping" == flag )
                        {
                            mOptions.set(EOptions::HlslIoMapping);
                        }
                        else if ("hlsl-enable-16bit-types" == flag)
                        {
                            mHlslEnable16BitTypes = true;
                        }
                        else if ("hlsl-dx9-compatible" == flag)
                        {
                            mHlslDX9compatible = true;
                        }
                        else if ("invert-y" == flag ||  // synonyms
                                 "iy"       == flag )
                        {
                            mOptions.set(EOptions::InvertY);
                        }
                        else if ("keep-uncalled" == flag || // synonyms
                                 "ku"            == flag )
                        {
                            mOptions.set(EOptions::KeepUncalled);
                        }
                        else if ("no-storage-format" == flag || // synonyms
                                 "nsf"               == flag )
                        {
                            mOptions.set(EOptions::NoStorageFormat);
                        }
                        else if ("relaxed-errors" == flag)
                        {
                            mOptions.set(EOptions::RelaxedErrors);
                        }
                        else if ("resource-set-bindings" == flag ||  // synonyms
                                 "resource-set-binding"  == flag ||
                                 "rsb"                   == flag )
                        {
                            ProcessResourceSetBindingBase(aArgC, aArgV, mBaseResourceSetBinding);
                        }
                        else if ("shift-image-bindings" == flag ||  // synonyms
                                 "shift-image-binding"  == flag ||
                                 "sib"                  == flag )
                        {
                            ProcessBindingBase(aArgC, aArgV, glslang::EResImage);
                        }
                        else if ("shift-sampler-bindings" == flag || // synonyms
                                 "shift-sampler-binding"  == flag ||
                                 "ssb"                    == flag )
                        {
                            ProcessBindingBase(aArgC, aArgV, glslang::EResSampler);
                        }
                        else if ("shift-uav-bindings" == flag ||  // synonyms
                                 "shift-uav-binding"  == flag ||
                                 "suavb"              == flag )
                        {
                            ProcessBindingBase(aArgC, aArgV, glslang::EResUav);
                        }
                        else if ("shift-texture-bindings" == flag ||  // synonyms
                                 "shift-texture-binding"  == flag ||
                                 "stb"                    == flag )
                        {
                            ProcessBindingBase(aArgC, aArgV, glslang::EResTexture);
                        }
                        else if ("shift-ubo-bindings"     == flag ||  // synonyms
                                 "shift-ubo-binding"      == flag ||
                                 "shift-cbuffer-bindings" == flag ||
                                 "shift-cbuffer-binding"  == flag ||
                                 "sub"                    == flag ||
                                 "scb"                    == flag )
                        {
                            ProcessBindingBase(aArgC, aArgV, glslang::EResUbo);
                        }
                        else if ("shift-ssbo-bindings" == flag ||  // synonyms
                                 "shift-ssbo-binding"  == flag ||
                                 "sbb"                 == flag )
                        {
                            ProcessBindingBase(aArgC, aArgV, glslang::EResSsbo);
                        }
                        else if ("source-entrypoint" == flag || // synonyms
                                 "sep"               == flag )
                        {
                            if (1 >= aArgC)
                            {
                                Error(logTag(), "no <entry-point> provided for --source-entrypoint");
                            }

                            mSourceEntryPointName = std::string(aArgV[1]);

                            bumpArg();
                            break;
                        }
                        else if ("spirv-dis" == flag)
                        {
                            mSpvToolsDisassembler = true;
                        }
                        else if ("spirv-val" == flag)
                        {
                            mSpvToolsValidate = true;
                        }
                        else if ("stdin" == flag)
                        {
                            mOptions.set(EOptions::Stdin);
                            mShaderStageName = std::string(aArgV[1]);
                        }
                        else if ("suppress-warnings" == flag)
                        {
                            mOptions.set(EOptions::SuppressWarnings);
                        }
                        else if ("target-env" == flag)
                        {
                            if (1 < aArgC)
                            {
                                std::string targetEnvironmentString = std::string(aArgV[1]);

                                if ("vulkan1.0" == targetEnvironmentString)
                                {
                                    setVulkanSpv();
                                    mClientVersion = glslang::EShTargetVulkan_1_0;
                                }
                                else if ("vulkan1.1" == targetEnvironmentString)
                                {
                                    setVulkanSpv();
                                    mClientVersion = glslang::EShTargetVulkan_1_1;
                                }
                                else if ("opengl" == targetEnvironmentString)
                                {
                                    setOpenGlSpv();
                                    mClientVersion = glslang::EShTargetOpenGL_450;
                                }
                                else if ("spirv1.0" == targetEnvironmentString)
                                {
                                    mTargetLanguage = glslang::EShTargetSpv;
                                    mTargetVersion  = glslang::EShTargetSpv_1_0;
                                }
                                else if ("spirv1.1" == targetEnvironmentString)
                                {
                                    mTargetLanguage = glslang::EShTargetSpv;
                                    mTargetVersion  = glslang::EShTargetSpv_1_1;
                                }
                                else if ("spirv1.2" == targetEnvironmentString)
                                {
                                    mTargetLanguage = glslang::EShTargetSpv;
                                    mTargetVersion  = glslang::EShTargetSpv_1_2;
                                }
                                else if ("spirv1.3" == targetEnvironmentString)
                                {
                                    mTargetLanguage = glslang::EShTargetSpv;
                                    mTargetVersion  = glslang::EShTargetSpv_1_3;
                                }
                                else if ("spirv1.4" == targetEnvironmentString)
                                {
                                    mTargetLanguage = glslang::EShTargetSpv;
                                    mTargetVersion  = glslang::EShTargetSpv_1_4;
                                }
                                else
                                {
                                    Error(logTag(), "--target-env expected one of: vulkan1.0, vulkan1.1, opengl, spirv1.0, spirv1.1, spirv1.2, or spirv1.3");
                                }
                            }

                            bumpArg();
                        }
                        else if ("variable-name" == flag || // synonyms
                                 "vn"            == flag )
                        {
                            mOptions.set(EOptions::OutputHexadecimal);

                            if (1 >= aArgC)
                            {
                                Error(logTag(), "no <C-variable-name> provided for --variable-name");
                            }

                            mVariableName = std::string(aArgV[1]);

                            bumpArg();
                            break;
                        }
                        else if ("version" == flag)
                        {
                            mOptions.set(EOptions::DumpVersions);
                        }
                        else
                        {
                            usage();
                        }
                    }
                    break;
                case 'C':
                    mOptions.set(EOptions::CascadingErrors);
                    break;
                case 'D':
                    if (0 == aArgV[0][2])
                    {
                        mOptions.set(EOptions::ReadHlsl);
                    }
                    else
                    {
                        std::string const definition = getStringOperand("-D<macro> macro name");

                        mProcesses.push_back("D");
                        mProcesses.back().append(definition);

                        mUserPreamble.addDefinition(definition);
                    }
                    break;
                case 'u':
                    {
                        std::pair<std::string, uint64_t> uniformOverride = getUniformOverride();
                        mUniformLocationOverrides.push_back(uniformOverride);
                    }
                    break;
                case 'E':
                    mOptions.set(EOptions::OutputPreprocessed);
                    break;
                case 'G':
                    // OpenGL client
                    setOpenGlSpv();

                    if (0 != aArgV[0][2])
                    {
                        mClientInputSemanticsVersion = getAttachedNumber("-G<num> client input semantics");
                    }
                    break;
                case 'H':
                    mOptions.set(EOptions::HumanReadableSpv);
                    if (not mOptions.check(EOptions::Spv))
                    {
                        // default to Vulkan
                        setVulkanSpv();
                    }
                    break;
                case 'I':
                    {
                        std::string const operand = getStringOperand("-I<dir> include path");
                        mIncludeDirectoryList.push_back(operand);
                    }
                    break;
                case 'O':
                    {
                        if ('d' == aArgV[0][2])
                        {
                            mOptions.set(EOptions::OptimizeDisable);
                        }
                        else if ('s' == aArgV[0][2])
                        {
                            if(OPTIMIZATION_ENABLED)
                            {
                                mOptions.set(EOptions::OptimizeSize);
                            }
                            else
                            {
                                Error(logTag(), "-Os not available; optimizer not linked");
                            }
                        }
                        else
                        {
                            Error(logTag(), "unknown -O option");
                        }
                    }
                    break;
                case 'S':
                    if (1 >= aArgC)
                    {
                        Error(logTag(), "no <stage> specified for -S");
                    }

                    mShaderStageName = std::string(aArgV[1]);

                    bumpArg();
                    break;
                case 'U':
                    {
                        std::string const undefinition = getStringOperand("-U<macro>: macro name");

                        mProcesses.push_back("U");
                        mProcesses.back().append(undefinition);

                        mUserPreamble.addUndefinition(undefinition);
                    }
                    break;
                case 'V':
                    setVulkanSpv();
                    if (0 != aArgV[0][2])
                    {
                        mClientInputSemanticsVersion = getAttachedNumber("-V<num> client input semantics");
                    }
                    break;
                case 'c':
                    mOptions.set(EOptions::DumpConfig);
                    break;
                case 'd':
                    {
                        std::string const mode = std::string(aArgV[0]);
                        if("dumpversion"     == mode ||
                           "dumpfullversion" == mode )
                        {
                            mOptions.set(EOptions::DumpBareVersion);
                        }
                        else
                        {
                            mOptions.set(EOptions::DefaultDesktop);
                        }
                    }
                    break;
                case 'e':
                    mEntryPointName = std::string(aArgV[1]);

                    if (1 >= aArgC)
                    {
                        Error(logTag(), "no <name> provided for -e");
                    }

                    bumpArg();
                    break;
                case 'f':
                    {
                        std::string const functionality = std::string(&aArgV[0][2]);

                        if("hlsl_functionality1" == functionality)
                        {
                            mTargetHlslFunctionality1 = true;
                        }
                        else
                        {
                            Error(logTag(), "-f: expected hlsl_functionality1");
                        }
                    }
                    break;
                case 'g':
                    mOptions.set(EOptions::Debug);
                    break;
                case 'h':
                    usage();
                    break;
                case 'i':
                    mOptions.set(EOptions::Intermediate);
                    break;
                case 'l':
                    mOptions.set(EOptions::LinkProgram);
                    break;
                case 'm':
                    mOptions.set(EOptions::MemoryLeakMode);
                    break;
                case 'o':
                    if (1 >= aArgC)
                    {
                        Error(logTag(), "no <file> provided for -o");
                    }

                    mBinaryFileName = std::string(aArgV[1]);

                    bumpArg();
                    break;
                case 'q':
                    mOptions.set(EOptions::DumpReflection);
                    break;
                case 'r':
                    mOptions.set(EOptions::RelaxedErrors);
                    break;
                case 's':
                    mOptions.set(EOptions::SuppressInfolog);
                    break;
                case 't':
                    mOptions.set(EOptions::MultiThreaded);
                    break;
                case 'v':
                    mOptions.set(EOptions::DumpVersions);
                    break;
                case 'w':
                    mOptions.set(EOptions::SuppressWarnings);
                    break;
                case 'x':
                    mOptions.set(EOptions::OutputHexadecimal);
                    break;
                default:
                    usage();
                    break;
                }
            }
            else
            {
                std::string const name(aArgV[0]);

                if (not SetConfigFile(name))
                {
                    aOutWorkItems.push_back(std::unique_ptr<CWorkItem>(new CWorkItem(name)));
                }
            }
        }

        // Make sure that -S is always specified if --stdin is specified
        if (mOptions.check(EOptions::Stdin) && mShaderStageName.empty())
        {
            Error(logTag(), "must provide -S when --stdin is given");
        }

        // Make sure that -E is not specified alongside linking (which includes SPV generation)
        if (mOptions.check(EOptions::OutputPreprocessed) && mOptions.check(EOptions::LinkProgram))
        {
            Error(logTag(), "can't use -E when linking is selected");
        }

        // -o or -x makes no sense if there is no target binary
        if (not mBinaryFileName.empty() && not mOptions.check(EOptions::Spv))
        {
            Error(logTag(), "no binary generation requested (e.g., -V)");
        }

        if (    mOptions.check(EOptions::FlattenUniformArrays) &&
            not mOptions.check(EOptions::ReadHlsl))
        {
            Error(logTag(), "uniform array flattening only valid when compiling HLSL source.");
        }

        // rationalize client and target language
        if (glslang::EShTargetNone == mTargetLanguage)
        {
            switch (mClientVersion)
            {
            case glslang::EShTargetVulkan_1_0:
                mTargetLanguage = glslang::EShTargetSpv;
                mTargetVersion  = glslang::EShTargetSpv_1_0;
                break;
            case glslang::EShTargetVulkan_1_1:
                mTargetLanguage = glslang::EShTargetSpv;
                mTargetVersion  = glslang::EShTargetSpv_1_3;
                break;
            case glslang::EShTargetOpenGL_450:
                mTargetLanguage = glslang::EShTargetSpv;
                mTargetVersion  = glslang::EShTargetSpv_1_0;
                break;
            default:
                break;
            }
        }
        if (glslang::EShTargetNone != mTargetLanguage && glslang::EShClientNone == mClient)
        {
            Error(logTag(), "To generate SPIR-V, also specify client semantics. See -G and -V.");
        }
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------

    /**
     * *.conf => this is a config file that can set limits/resources
     *
     * @param aConfigFilename
     * @return
     */
    bool SetConfigFile(std::string const &aConfigFilename)
    {
        bool const hasValidSize = (5 > aConfigFilename.size());
        if(hasValidSize)
        {
            return false;
        }

        bool const hasValidSuffix = (0 == aConfigFilename.compare(aConfigFilename.size() - 5, 5, ".conf"));
        if (hasValidSuffix)
        {
            mConfigFile = aConfigFilename;
            return true;
        }

        return false;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------

private_members:
    // array of unique places to leave the shader names and infologs for the asynchronous compiles
    std::vector<std::unique_ptr<CWorkItem>> mWorkItems;

    std::string              mConfigFile;
    std::vector<std::string> mIncludeDirectoryList;
    TBuiltInResource         mResources;

    core::CBitField<EOptions> mOptions = EOptions::None;

    // SPIR-V
    bool mSpvToolsDisassembler     = false;
    bool mSpvToolsValidate         = false;

    // HLSL
    bool mTargetHlslFunctionality1 = false;
    bool mHlslEnable16BitTypes     = false;
    bool mHlslDX9compatible        = false;

    std::string mExecutableName       = nullptr;
    std::string mBinaryFileName       = nullptr;
    std::string mEntryPointName       = nullptr;
    std::string mSourceEntryPointName = nullptr;
    std::string mShaderStageName      = nullptr;
    std::string mVariableName         = nullptr;

    bool mCompileFailed = false;
    bool mLinkFailed    = false;


    // Source environment
    // (source 'Client' is currently the same as target 'Client')
    int mClientInputSemanticsVersion = 100;

    // Target environment
    glslang::EShClient                mClient         = glslang::EShClientNone;                               // will stay EShClientNone if only validating
    glslang::EShTargetClientVersion   mClientVersion  = glslang::EShTargetClientVersion::EShTargetVulkan_1_1; // not valid until Client is set
    glslang::EShTargetLanguage        mTargetLanguage = glslang::EShTargetNone;
    glslang::EShTargetLanguageVersion mTargetVersion  = glslang::EShTargetLanguageVersion::EShTargetSpv_1_4; // not valid until TargetLanguage is set

    std::vector<std::string> mProcesses; // what should be recorded by OpModuleProcessed, or equivalent

    // Per descriptor-set binding base data
    typedef std::map<unsigned int, unsigned int> PerSetBaseBinding_t;

    std::vector<std::pair<std::string, int32_t>> mUniformLocationOverrides;
    uint32_t                                     mUniformBase;

    std::array<std::array<unsigned int,        EShLangCount>, glslang::EResCount> mBaseBinding;
    std::array<std::array<PerSetBaseBinding_t, EShLangCount>, glslang::EResCount> mBaseBindingForSet;
    std::array<std::vector<std::string>,       EShLangCount>                      mBaseResourceSetBinding;

    CPreamble mUserPreamble;
};



#if defined SHIRABE_PLATFORM_WINDOWS
int WINAPI WinMain(
        HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        PSTR      szCmdLine,
        int       iCmdShow)
#elif defined SHIRABE_PLATFORM_LINUX
int main(int aArgc, char **aArgv)
#endif
{
    using namespace engine;

    #ifdef SHIRABE_DEBUG
    CConsole::InitializeConsole();
    #endif

    if(3 != aArgc)
    {
        usage();
        return -1;
    }

    std::string const inputPath  = aArgv[1];
    std::string const outputPath = aArgv[2];

    bool const inputPathExists  = std::filesystem::exists(inputPath);
    bool const outputPathExists = std::filesystem::exists(outputPath);

    if(not inputPathExists)
    {
        CLog::Error(Main::logTag(), "Input path not valid.");
        return -2;
    }

    if(not outputPathExists)
    {
        bool const successfullyCreated = std::filesystem::create_directories(outputPath);
        if(not successfullyCreated)
        {
            CLog::Error(Main::logTag(), "Cannot create output path.");
            return -3;
        }
    }

    try
    {
        std::shared_ptr<CPrecompiler> precompiler = std::make_shared<CPrecompiler>();

        // Read all shader-files and convert them to spirv.
        // Then go for SPIRV-cross, perform reflection and generate headers
        // for all shaders.
        precompiler->initialize();
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
