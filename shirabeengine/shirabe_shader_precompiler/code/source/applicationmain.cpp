#include <filesystem>

#include <glslang/Public/ShaderLang.h>
#include <log/log.h>

namespace Main
{
    SHIRABE_DECLARE_LOG_TAG(Shirabe_Shader_Precompiler)
}

static void usage()
{
    using namespace engine;

    CLog::Error(Main::logTag(), "Usage...");

    return;
}

enum EOptions
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
EShLanguage determineTargetLanguage(std::string const &aFilename, bool aParseStageName, int32_t &aInOutOptions)
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
            aInOutOptions |= EOptions::ReadHlsl;
        }

        if (hasFirstExt && not usesUnifiedExt)
        {
            stageName = firstExt;
        }
        else if (usesUnifiedExt && hasSecondExt)
        {
            stageName = aFilename.substr(secondExtStart + 1, firstExtStart - secondExtStart - 1);
        }
        else
        {
            usage();
            return EShLangVertex;
        }
    }
    else
    {
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

/**
 * @brief compile
 * @param aFilename
 * @param aCompiler
 */
void compile(std::string const &aFilename, ShHandle const &aCompiler, int32_t const &aOptions)
{
    int result = 0;

    char *shaderString = ReadFileData(aFilename);

    // move to length-based strings, rather than null-terminated strings
    int const length = static_cast<int32_t>(strlen(shaderString));

    EShMessages messages = EShMsgDefault;
    messages = static_cast<EShMessages>(messages | EShMsgDebugInfo);
    messages = static_cast<EShMessages>(messages | EShMsgAST);

    // SetMessageOptions(messages);

    //if (UserPreamble.isSet())
    //    Error("-D and -U options require -l (linking)\n");

    int32_t const limit = static_cast<uint32_t>(aOptions) & EOptions::MemoryLeakMode) ? 100 : 1;

    for (int32_t i = 0; i < limit; ++i)
    {
        for (int32_t j = 0; j < limit; ++j)
        {
            // ret = ShCompile(compiler, shaderStrings, NumShaderStrings, lengths, EShOptNone, &Resources, Options, (Options & EOptionDefaultDesktop) ? 110 : 100, false, messages);
            result = ShCompile(aCompiler, &shaderString, 1, nullptr, EShOptNone, &Resources, Options, (Options & EOptionDefaultDesktop) ? 110 : 100, false, messages);
            // const char* multi[12] = { "# ve", "rsion", " 300 e", "s", "\n#err",
            //                         "or should be l", "ine 1", "string 5\n", "float glo", "bal",
            //                         ";\n#error should be line 2\n void main() {", "global = 2.3;}" };
            // const char* multi[7] = { "/", "/", "\\", "\n", "\n", "#", "version 300 es" };
            // ret = ShCompile(compiler, multi, 7, nullptr, EShOptNone, &Resources, Options, (Options & EOptionDefaultDesktop) ? 110 : 100, false, messages);
        }

        if (Options & EOptionMemoryLeakMode)
            glslang::OS_DumpMemoryCounters();
    }

    delete [] lengths;
    FreeFileData(shaderString);

    if (ret == 0)
        CompileFailed = true;
}
//<-----------------------------------------------------------------------------

/**
 * @brief process
 * @param aInputFilenames
 */
static void process(std::vector<std::string> const &aInputFilenames)
{
    using namespace engine;

    ShInitialize();

    int32_t options = 0;

    for(std::string const &filename : aInputFilenames)
    {
        EShLanguage const lang = determineTargetLanguage(filename, true, options);

        ShHandle compiler = ShConstructCompiler(lang, options);
        if (nullptr == compiler)
        {
            return;
        }

        compile("stdin", compiler);

        if (false == (static_cast<uint32_t>(options) & EOptions::SuppressInfolog))
        {
            std::string const log = ShGetInfoLog(compiler);
            CLog::Error(Main::logTag(), log);
        }

        ShDestruct(compiler);
    }

    ShFinalize();
}

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

    // Read all shader-files and convert them to spirv.
    // Then go for SPIRV-cross, perform reflection and generate headers
    // for all shaders.


    #ifdef SHIRABE_DEBUG
    CConsole::DeinitializeConsole();
    #endif

    return 0;
}
