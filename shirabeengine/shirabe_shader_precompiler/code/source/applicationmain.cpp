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

#include "resourcelimits.h"
#include "worklist.h"
#include "directorystackfileincluder.h"

using namespace engine;
using namespace glslang_wrapper;

#if ENABLE_OPT
static constexpr bool const OPTIMIZATION_ENABLED = true;
#else
static constexpr bool const OPTIMIZATION_ENABLED = false;
#endif

namespace Main
{
    SHIRABE_DECLARE_LOG_TAG(ShirabeEngineShaderPrecompiler);
}

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------

// Replacement implementation of safe fopen on non-MSVC or non-secure MINGW
// enviroments
#if not defined _MSC_VER && not defined MINGW_HAS_SECURE_API

#include <errno.h>

/**
 * Safely open a file named 'aFilename' and return the file handle to it.
 *
 * @param aFilename
 * @param aMode
 * @param aOutFile
 * @return
 */
int fopen_s(
   char const *aFilename,
   char const *aMode,
   FILE      **aOutFile)
{
   if (nullptr == aOutFile || nullptr == aFilename || nullptr == aMode)
   {
      return EINVAL;
   }

   FILE *f = fopen(aFilename, aMode);
   if (nullptr == f)
   {
      if (0 != errno)
      {
         return errno;
      }
      else
      {
         return ENOENT;
      }
   }

   *aOutFile = f;

   return 0;
}

#endif // not defined _MSC_VER && not defined MINGW_HAS_SECURE_API

//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
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
 * Enum class describing suitable precompiler error states.
 */
enum class EFailCode
        : uint8_t
{
    Success = 0,
    FailUsage,
    FailCompile,
    FailLink,
    FailCompilerCreate,
    FailThreadCreate,
    FailLinkerCreate
};

//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------

/**
 * Print a formatted error message.
 *
 * @param aMessage The message to print.
 */
[[noreturn]]
void Error(std::string const &aTag, std::string const &aMessage)
{
    CLog::Error(aTag, CString::format("%0 (use -h for usage)\n", aMessage));

    exit(EnumValueOf(EFailCode::FailUsage));
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------

/**
 * Outputs the given string, but only if it is non-null and non-empty.
 * This prevents erroneous newlines from appearing.
 *
 * @param str
 */
void putsIfNonEmpty(const char* str)
{
    if (nullptr != str && str[0])
    {
        puts(str);
    }
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------

/**
 * Outputs the given string to stderr, but only if it is non-null and non-empty.
 * This prevents erroneous newlines from appearing.
 *
 * @param str
 */
void stderrIfNonEmpty(const char* str)
{
    if (nullptr != str && str[0])
    {
        fprintf(stderr, "%s\n", str);
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
static std::string ReadFileData(std::string const &aFileName)
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

    printf("Usage: glslangValidator [option]... [file]...\n"
           "\n"
           "'file' can end in .<stage> for auto-stage classification, where <stage> is:\n"
           "    .conf   to provide a config file that replaces the default configuration\n"
           "            (see -c option below for generating a template)\n"
           "    .vert   for a vertex shader\n"
           "    .tesc   for a tessellation control shader\n"
           "    .tese   for a tessellation evaluation shader\n"
           "    .geom   for a geometry shader\n"
           "    .frag   for a fragment shader\n"
           "    .comp   for a compute shader\n"
#ifdef NV_EXTENSIONS
           "    .mesh   for a mesh shader\n"
           "    .task   for a task shader\n"
           "    .rgen    for a ray generation shader\n"
           "    .rint    for a ray intersection shader\n"
           "    .rahit   for a ray any hit shader\n"
           "    .rchit   for a ray closest hit shader\n"
           "    .rmiss   for a ray miss shader\n"
           "    .rcall   for a ray callable shader\n"
#endif
           "    .glsl   for .vert.glsl, .tesc.glsl, ..., .comp.glsl compound suffixes\n"
           "    .hlsl   for .vert.hlsl, .tesc.hlsl, ..., .comp.hlsl compound suffixes\n"
           "\n"
           "Options:\n"
           "  -C          cascading errors; risk crash from accumulation of error recoveries\n"
           "  -D          input is HLSL (this is the default when any suffix is .hlsl)\n"
           "  -D<macro=def>\n"
           "  -D<macro>   define a pre-processor macro\n"
           "  -E          print pre-processed GLSL; cannot be used with -l;\n"
           "              errors will appear on stderr\n"
           "  -G[ver]     create SPIR-V binary, under OpenGL semantics; turns on -l;\n"
           "              default file name is <stage>.spv (-o overrides this);\n"
           "              'ver', when present, is the version of the input semantics,\n"
           "              which will appear in #define GL_SPIRV ver;\n"
           "              '--client opengl100' is the same as -G100;\n"
           "              a '--target-env' for OpenGL will also imply '-G'\n"
           "  -H          print human readable form of SPIR-V; turns on -V\n"
           "  -I<dir>     add dir to the include search path; includer's directory\n"
           "              is searched first, followed by left-to-right order of -I\n"
           "  -Od         disables optimization; may cause illegal SPIR-V for HLSL\n"
           "  -Os         optimizes SPIR-V to minimize size\n"
           "  -S <stage>  uses specified stage rather than parsing the file extension\n"
           "              choices for <stage> are vert, tesc, tese, geom, frag, or comp\n"
           "  -U<macro>   undefine a pre-processor macro\n"
           "  -V[ver]     create SPIR-V binary, under Vulkan semantics; turns on -l;\n"
           "              default file name is <stage>.spv (-o overrides this)\n"
           "              'ver', when present, is the version of the input semantics,\n"
           "              which will appear in #define VULKAN ver\n"
           "              '--client vulkan100' is the same as -V100\n"
           "              a '--target-env' for Vulkan will also imply '-V'\n"
           "  -c          configuration dump;\n"
           "              creates the default configuration file (redirect to a .conf file)\n"
           "  -d          default to desktop (#version 110) when there is no shader #version\n"
           "              (default is ES version 100)\n"
           "  -e <name> | --entry-point <name>\n"
           "              specify <name> as the entry-point function name\n"
           "  -f{hlsl_functionality1}\n"
           "              'hlsl_functionality1' enables use of the\n"
           "              SPV_GOOGLE_hlsl_functionality1 extension\n"
           "  -g          generate debug information\n"
           "  -h          print this usage message\n"
           "  -i          intermediate tree (glslang AST) is printed out\n"
           "  -l          link all input files together to form a single module\n"
           "  -m          memory leak mode\n"
           "  -o <file>   save binary to <file>, requires a binary option (e.g., -V)\n"
           "  -q          dump reflection query database\n"
           "  -r | --relaxed-errors"
           "              relaxed GLSL semantic error-checking mode\n"
           "  -s          silence syntax and semantic error reporting\n"
           "  -t          multi-threaded mode\n"
           "  -v | --version\n"
           "              print version strings\n"
           "  -w | --suppress-warnings\n"
           "              suppress GLSL warnings, except as required by \"#extension : warn\"\n"
           "  -x          save binary output as text-based 32-bit hexadecimal numbers\n"
           "  -u<name>:<loc> specify a uniform location override for --aml\n"
           "  --uniform-base <base> set a base to use for generated uniform locations\n"
           "  --auto-map-bindings | --amb       automatically bind uniform variables\n"
           "                                    without explicit bindings\n"
           "  --auto-map-locations | --aml      automatically locate input/output lacking\n"
           "                                    'location' (fragile, not cross stage)\n"
           "  --client {vulkan<ver>|opengl<ver>} see -V and -G\n"
           "  -dumpfullversion | -dumpversion   print bare major.minor.patchlevel\n"
           "  --flatten-uniform-arrays | --fua  flatten uniform texture/sampler arrays to\n"
           "                                    scalars\n"
           "  --hlsl-offsets                    allow block offsets to follow HLSL rules\n"
           "                                    works independently of source language\n"
           "  --hlsl-iomap                      perform IO mapping in HLSL register space\n"
           "  --hlsl-enable-16bit-types         allow 16-bit types in SPIR-V for HLSL\n"
           "  --hlsl-dx9-compatible             interprets sampler declarations as a texture/sampler combo like DirectX9 would."
           "  --invert-y | --iy                 invert position.Y output in vertex shader\n"
           "  --keep-uncalled | --ku            don't eliminate uncalled functions\n"
           "  --no-storage-format | --nsf       use Unknown image format\n"
           "  --resource-set-binding [stage] name set binding\n"
           "                                    set descriptor set and binding for\n"
           "                                    individual resources\n"
           "  --resource-set-binding [stage] set\n"
           "                                    set descriptor set for all resources\n"
           "  --rsb                             synonym for --resource-set-binding\n"
           "  --shift-image-binding [stage] num\n"
           "                                    base binding number for images (uav)\n"
           "  --shift-image-binding [stage] [num set]...\n"
           "                                    per-descriptor-set shift values\n"
           "  --sib                             synonym for --shift-image-binding\n"
           "  --shift-sampler-binding [stage] num\n"
           "                                    base binding number for samplers\n"
           "  --shift-sampler-binding [stage] [num set]...\n"
           "                                    per-descriptor-set shift values\n"
           "  --ssb                             synonym for --shift-sampler-binding\n"
           "  --shift-ssbo-binding [stage] num  base binding number for SSBOs\n"
           "  --shift-ssbo-binding [stage] [num set]...\n"
           "                                    per-descriptor-set shift values\n"
           "  --sbb                             synonym for --shift-ssbo-binding\n"
           "  --shift-texture-binding [stage] num\n"
           "                                    base binding number for textures\n"
           "  --shift-texture-binding [stage] [num set]...\n"
           "                                    per-descriptor-set shift values\n"
           "  --stb                             synonym for --shift-texture-binding\n"
           "  --shift-uav-binding [stage] num   base binding number for UAVs\n"
           "  --shift-uav-binding [stage] [num set]...\n"
           "                                    per-descriptor-set shift values\n"
           "  --suavb                           synonym for --shift-uav-binding\n"
           "  --shift-UBO-binding [stage] num   base binding number for UBOs\n"
           "  --shift-UBO-binding [stage] [num set]...\n"
           "                                    per-descriptor-set shift values\n"
           "  --sub                             synonym for --shift-UBO-binding\n"
           "  --shift-cbuffer-binding | --scb   synonyms for --shift-UBO-binding\n"
           "  --spirv-dis                       output standard-form disassembly; works only\n"
           "                                    when a SPIR-V generation option is also used\n"
           "  --spirv-val                       execute the SPIRV-Tools validator\n"
           "  --source-entrypoint <name>        the given shader source function is\n"
           "                                    renamed to be the <name> given in -e\n"
           "  --sep                             synonym for --source-entrypoint\n"
           "  --stdin                           read from stdin instead of from a file;\n"
           "                                    requires providing the shader stage using -S\n"
           "  --target-env {vulkan1.0 | vulkan1.1 | opengl | \n"
           "                spirv1.0 | spirv1.1 | spirv1.2 | spirv1.3}\n"
           "                                    set execution environment that emitted code\n"
           "                                    will execute in (versus source language\n"
           "                                    semantics selected by --client) defaults:\n"
           "                                     * 'vulkan1.0' under '--client vulkan<ver>'\n"
           "                                     * 'opengl' under '--client opengl<ver>'\n"
           "                                     * 'spirv1.0' under --target-env vulkan1.0\n"
           "                                     * 'spirv1.3' under --target-env vulkan1.1\n"
           "                                    multiple --targen-env can be specified.\n"
           "  --variable-name <name>\n"
           "  --vn <name>                       creates a C header file that contains a\n"
           "                                    uint32_t array named <name>\n"
           "                                    initialized with the shader binary code\n"
           );

    ::exit(EnumValueOf(EFailCode::FailUsage));
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
        Intermediate         = (1u <<  0),
        SuppressInfolog      = (1u <<  1),
        MemoryLeakMode       = (1u <<  2),
        RelaxedErrors        = (1u <<  3),
        GiveWarnings         = (1u <<  4),
        LinkProgram          = (1u <<  5),
        MultiThreaded        = (1u <<  6),
        DumpConfig           = (1u <<  7),
        DumpReflection       = (1u <<  8),
        SuppressWarnings     = (1u <<  9),
        DumpVersions         = (1u << 10),
        Spv                  = (1u << 11),
        HumanReadableSpv     = (1u << 12),
        VulkanRules          = (1u << 13),
        DefaultDesktop       = (1u << 14),
        OutputPreprocessed   = (1u << 15),
        OutputHexadecimal    = (1u << 16),
        ReadHlsl             = (1u << 17),
        CascadingErrors      = (1u << 18),
        AutoMapBindings      = (1u << 19),
        FlattenUniformArrays = (1u << 20),
        NoStorageFormat      = (1u << 21),
        KeepUncalled         = (1u << 22),
        HlslOffsets          = (1u << 23),
        HlslIoMapping        = (1u << 24),
        AutoMapLocations     = (1u << 25),
        Debug                = (1u << 26),
        Stdin                = (1u << 27),
        OptimizeDisable      = (1u << 28),
        OptimizeSize         = (1u << 29),
        InvertY              = (1u << 30),
        DumpBareVersion      = (1u << 31),
    };

private_structs:

    /**
     * Simple bundling of what makes a compilation unit for ease in passing around,
     * and separation of handling file IO versus API (programmatic) compilation.
     */
    struct SShaderCompilationUnit
    {
        EShLanguage               stage;
        uint64_t                  count;        // live number of strings/names
        std::vector<char const *> text;         // memory owned/managed externally
        std::vector<std::string>  fileName;     // hold's the memory, but...
        std::vector<char const *> fileNameList; // downstream interface wants pointers

        /**
         * @brief SShaderCompilationUnit
         * @param stage
         */
        SShaderCompilationUnit(EShLanguage stage)
            : stage(stage)
            , count(0)
        { }

        /**
         * @brief SShaderCompilationUnit
         * @param aOther
         */
        SShaderCompilationUnit(SShaderCompilationUnit const &aOther)
            : stage       (aOther.stage)
            , count       (aOther.count)
            , text        (aOther.text )
            , fileName    (aOther.fileName)
            , fileNameList(aOther.fileNameList)
        { }

        /**
         * @brief addString
         * @param ifileName
         * @param itext
         */
        [[noreturn]]
        void addString(std::string const &aFileName, std::string const &aText)
        {
            static constexpr uint64_t const MAX_COUNT = 1;
            assert(1 > MAX_COUNT);

            text        .push_back(aText.c_str());
            fileName    .push_back(aFileName);
            fileNameList.push_back(aFileName.c_str());

            ++count;
        }
    };

private_classes:

    /**
     * Add things like "#define ..." to a preamble to use in the beginning of the shader.
     */
    class CPreamble
    {
    public_constructors:
        /**
         * Default constructor
         */
        CPreamble()
        { }

    public_methods:
        /**
         * Check, if there's a preamble text currently set.
         *
         * @return True, if set.
         */
        bool isSet() const
        {
            return (not mText.empty());
        }

        /**
         * Return the current preamble text.
         *
         * @return See brief
         */
        std::string get() const
        {
            return mText.c_str();
        }

        /**
         * Add a definition to the preamble.
         *
         * @param aDefinition The #define to add.
         */
        void addDefinition(std::string aDefinition)
        {
            mText.append("#define ");

            fixLine(aDefinition);

            // The first "=" needs to turn into a space
            size_t const equalCharacterPos = aDefinition.find_first_of("=");

            if (aDefinition.npos != equalCharacterPos)
            {
                aDefinition[equalCharacterPos] = ' ';
            }

            mText.append(aDefinition);
            mText.append("\n");
        }

        /**
         * Add an "undefinition" to the preamble
         *
         * @param aUndefinition The #undef to add.
         */
        void addUndefinition(std::string aUndefinition)
        {
            mText.append("#undef ");

            fixLine(aUndefinition);

            mText.append(aUndefinition);
            mText.append("\n");
        }

    protected_methods:
        /**
         * Remove \n from the un/definition string.
         *
         * @param aInOutLine The line to fix.
         */
        void fixLine(std::string &aInOutLine)
        {
            // Can't go past a newline in the line
            size_t const endOfLinePos = aInOutLine.find_first_of("\n");
            if(aInOutLine.npos != endOfLinePos)
            {
                aInOutLine = aInOutLine.substr(0, endOfLinePos);
            }
        }

    private_members:
        std::string mText;  // Contents of preamble
    };

public_methods:


    EFailCode initialize()
    {
        return EFailCode::Success;
    }

    EFailCode run()
    {
        CWorklist workList{};

        auto const handler = [&workList] (std::unique_ptr<CWorkItem> &item)
        {
            assert(item);
            workList.add(item.get());
        };

        std::for_each(mWorkItems.begin(), mWorkItems.end(), handler);

        if (mOptions.check(EOptions::DumpConfig))
        {
            printf("%s", GetDefaultBuiltInResourceString().c_str());
            if (workList.empty())
            {
                return EFailCode::Success;
            }
        }

        if (mOptions.check(EOptions::DumpBareVersion))
        {
            printf("%d.%d.%d\n", glslang::GetSpirvGeneratorVersion(), GLSLANG_MINOR_VERSION, GLSLANG_PATCH_LEVEL);

            if (workList.empty())
            {
                return EFailCode::Success;
            }
        }
        else if (mOptions.check(EOptions::DumpVersions))
        {
            printf("Glslang Version: %d.%d.%d\n", glslang::GetSpirvGeneratorVersion(), GLSLANG_MINOR_VERSION, GLSLANG_PATCH_LEVEL);
            printf("ESSL Version: %s\n",          glslang::GetEsslVersionString());
            printf("GLSL Version: %s\n",          glslang::GetGlslVersionString());

            std::string spirvVersion;
            glslang::GetSpirvVersion(spirvVersion);

            printf("SPIR-V Version %s\n",                    spirvVersion.c_str());
            printf("GLSL.std.450 Version %d, Revision %d\n", GLSLstd450Version, GLSLstd450Revision);
            printf("Khronos Tool ID %d\n",                   glslang::GetKhronosToolId());
            printf("SPIR-V Generator Version %d\n",          glslang::GetSpirvGeneratorVersion());
            printf("GL_KHR_vulkan_glsl version %d\n",        100);
            printf("ARB_GL_gl_spirv version %d\n",           100);

            if (workList.empty())
            {
                return EFailCode::Success;
            }
        }

        if (workList.empty() && not mOptions.check(EOptions::Stdin))
        {
            usage();
        }

        if (mOptions.check(EOptions::Stdin))
        {
            mWorkItems.push_back(std::unique_ptr<CWorkItem> {new CWorkItem("stdin")} );
            workList.add(mWorkItems.back().get());
        }

        processConfigFile();

        if(mOptions.check(EOptions::ReadHlsl) && not (mOptions.check(EOptions::OutputPreprocessed) || mOptions.check(EOptions::Spv)))
        {
            Error(logTag(), "ERROR: HLSL requires SPIR-V code generation (or preprocessing only)");
        }

        //
        // Two modes:
        // 1) linking all arguments together, single-threaded, new C++ interface
        // 2) independent arguments, can be tackled by multiple asynchronous threads, for testing thread safety, using the old handle interface
        //
        if (mOptions.check(core::CBitField<EOptions>(EOptions::LinkProgram) | EOptions::OutputPreprocessed))
        {
            glslang::InitializeProcess();
            glslang::InitializeProcess();  // also test reference counting of users
            glslang::InitializeProcess();  // also test reference counting of users
            glslang::FinalizeProcess();    // also test reference counting of users
            glslang::FinalizeProcess();    // also test reference counting of users

            compileAndLinkShaderFiles(workList);

            glslang::FinalizeProcess();
        }
        else
        {
            ShInitialize();
            ShInitialize();  // also test reference counting of users
            ShFinalize();    // also test reference counting of users

            bool printShaderNames = (1 < workList.size());

            if (mOptions.check(EOptions::MultiThreaded))
            {
                std::array<std::thread, 16> threads;

                for (unsigned int k = 0; k < threads.size(); ++k)
                {
                    threads[k] = std::thread(&CPrecompiler::compileShaders, this, std::ref(workList));

                    if (std::thread::id() == threads[k].get_id())
                    {
                        fprintf(stderr, "Failed to create thread\n");
                        return EFailCode::FailThreadCreate;
                    }
                }

                auto const joiner = [] (std::thread &t)
                {
                    if(t.joinable())
                    {
                        t.join();
                    }
                };
                std::for_each(threads.begin(), threads.end(), joiner);
            }
            else
            {
                compileShaders(workList);
            }

            // Print out all the resulting infologs
            for (size_t w = 0; w < mWorkItems.size(); ++w)
            {
                if(mWorkItems[w])
                {
                    if (printShaderNames || 0 < mWorkItems[w]->results().size())
                    {
                        putsIfNonEmpty(mWorkItems[w]->name().c_str());
                    }

                    putsIfNonEmpty(mWorkItems[w]->results().c_str());
                }
            }

            ShFinalize();
        }

        if (mCompileFailed)
        {
            return EFailCode::FailCompile;
        }

        if (mLinkFailed)
        {
            return EFailCode::FailLink;
        }

        return EFailCode::Success;
    }

    void deinitialize()
    {

    }

private_methods:

    //
    //   Deduce the language from the filename.  Files must end in one of the
    //   following extensions:
    //
    //   .vert = vertex
    //   .tesc = tessellation control
    //   .tese = tessellation evaluation
    //   .geom = geometry
    //   .frag = fragment
    //   .comp = compute
    //   .rgen = ray generation
    //   .rint = ray intersection
    //   .rahit = ray any hit
    //   .rchit = ray closest hit
    //   .rmiss = ray miss
    //   .rcall = ray callable
    //   .mesh  = mesh
    //   .task  = task
    //   Additionally, the file names may end in .<stage>.glsl and .<stage>.hlsl
    //   where <stage> is one of the stages listed above.
    //
    EShLanguage determineTargetLanguage(std::string const &aFilename, bool aParseStageName, core::CBitField<EOptions> &aInOutOptions)
    {
        std::string stageName {};

        static std::string shaderStageName = "";

        if (not shaderStageName.empty())
        {
            stageName = shaderStageName;
        }
        else if (aParseStageName)
        {
            // Note: "first" extension means "first from the end", i.e.
            // if the file is named foo.vert.glsl, then "glsl" is first,
            // "vert" is second.
            size_t      firstExtStart  = aFilename.find_last_of(".");
            bool        hasFirstExt    = (std::string::npos != firstExtStart);
            size_t      secondExtStart = hasFirstExt
                                            ? aFilename.find_last_of(".", firstExtStart - 1)
                                            : std::string::npos;
            bool        hasSecondExt   = (std::string::npos != secondExtStart);

            std::string firstExt       = aFilename.substr(firstExtStart + 1, std::string::npos);
            bool        usesUnifiedExt = hasFirstExt && ("glsl" == firstExt || "hlsl" == firstExt);

            if (usesUnifiedExt && "hlsl" == firstExt)
            {
                // We need to enable HLSL processing
                aInOutOptions.set(EOptions::ReadHlsl);
            }

            if (hasFirstExt && not usesUnifiedExt)
            {
                // No .glsl or .hlsl in the end so the first extension will
                // provide the proper stage name file extension.
                stageName = firstExt;
            }
            else if (usesUnifiedExt && hasSecondExt)
            {
                // Extract the proper stage name file extension, i.e. the second extension
                stageName = aFilename.substr(secondExtStart + 1, firstExtStart - secondExtStart - 1);
            }
            else
            {
                // Undefined, assume vertex shader.
                usage();
                return EShLangVertex;
            }
        }
        else
        {
            // Just set the filename...
            stageName = aFilename;
        }

        if ("vert" == stageName)
        {
            return EShLangVertex;
        }
        else if ("tesc" == stageName)
        {
            return EShLangTessControl;
        }
        else if ("tese" == stageName)
        {
            return EShLangTessEvaluation;
        }
        else if ("geom" == stageName)
        {
            return EShLangGeometry;
        }
        else if ("frag" == stageName)
        {
            return EShLangFragment;
        }
        else if ("comp" == stageName)
        {
            return EShLangCompute;
        }
    #ifdef NV_EXTENSIONS
        else if (stageName == "rgen")
            return EShLangRayGenNV;
        else if (stageName == "rint")
            return EShLangIntersectNV;
        else if (stageName == "rahit")
            return EShLangAnyHitNV;
        else if (stageName == "rchit")
            return EShLangClosestHitNV;
        else if (stageName == "rmiss")
            return EShLangMissNV;
        else if (stageName == "rcall")
            return EShLangCallableNV;
        else if (stageName == "mesh")
            return EShLangMeshNV;
        else if (stageName == "task")
            return EShLangTaskNV;
    #endif

        usage();

        return EShLangVertex;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------

    /**
     * Create the default name for saving a binary if -o is not provided.
     *
     * @param aStage The stage to determine a binary name for.
     * @return See brief
     */
    std::string const GetBinaryName(EShLanguage const aStage) const
    {
        std::string name{};

        if (mBinaryFileName.empty())
        {
            switch (aStage)
            {
            case EShLangVertex:         name = "vert.spv";  break;
            case EShLangTessControl:    name = "tesc.spv";  break;
            case EShLangTessEvaluation: name = "tese.spv";  break;
            case EShLangGeometry:       name = "geom.spv";  break;
            case EShLangFragment:       name = "frag.spv";  break;
            case EShLangCompute:        name = "comp.spv";  break;
    #ifdef NV_EXTENSIONS
            case EShLangRayGenNV:       name = "rgen.spv";  break;
            case EShLangIntersectNV:    name = "rint.spv";  break;
            case EShLangAnyHitNV:       name = "rahit.spv"; break;
            case EShLangClosestHitNV:   name = "rchit.spv"; break;
            case EShLangMissNV:         name = "rmiss.spv"; break;
            case EShLangCallableNV:     name = "rcall.spv"; break;
            case EShLangMeshNV:         name = "mesh.spv";  break;
            case EShLangTaskNV:         name = "task.spv";  break;
    #endif
            default:                     name = "unknown";  break;
            }
        }
        else
        {
            name = mBinaryFileName;
        }

        return name;
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
                 ShHandle                          const &aCompiler,
                 engine::core::CBitField<EOptions> const &aOptions)
    {
        int result = 0;

        std::string shaderString = ReadFileData(aFilename);
        if(shaderString.empty())
        {
            Error(logTag(), "Shader file is empty.");
            return;
        }

        // move to length-based strings, rather than null-terminated strings
        uint64_t const length = shaderString.size();
        SHIRABE_UNUSED(length);

        EShMessages messages = EShMsgDefault;
        messages = static_cast<EShMessages>(messages | EShMsgDebugInfo);
        messages = static_cast<EShMessages>(messages | EShMsgAST);

        // SetMessageOptions(messages);

        //if (UserPreamble.isSet())
        //    Error("-D and -U options require -l (linking)\n");

        int32_t const limit = aOptions.check(EOptions::MemoryLeakMode) ? 100 : 1;

        for (int32_t i = 0; i < limit; ++i)
        {
            for (int32_t j = 0; j < limit; ++j)
            {
                char const *data = shaderString.data();
                result = ShCompile(/* ShHandle          = */ aCompiler,
                                   /* shaderStrings     = */ &data,
                                   /* numStrings        = */ 1,
                                   /* lengths           = */ nullptr,
                                   /* optimizationLevel = */ EShOptNone,
                                   /* resources         = */ &mResources,
                                   /* debugOptions      = */ static_cast<int>(aOptions.value()),
                                   /* defaultVersion    = */ (aOptions.check(EOptions::DefaultDesktop)) ? 110 : 100,
                                   /* forwardCompatible = */ false,
                                   /* messages          = */ messages);
            }

            if (aOptions.check(EOptions::MemoryLeakMode))
            {
                // glslang::OS_DumpMemoryCounters();
            }
        }

        if(0 == result)
        {
            mCompileFailed = true;
        }
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
