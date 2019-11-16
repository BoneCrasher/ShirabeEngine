#ifndef   _SHIRABE_RESOURCECOMP_DEFINITION_H_
#define   _SHIRABE_RESOURCECOMP_DEFINITION_H_

#include <string>
#include <vector>

#include <base/declaration.h>
#include <base/string.h>
#include <log/log.h>

#if ENABLE_OPT
static constexpr bool const OPTIMIZATION_ENABLED = true;
#else
static constexpr bool const OPTIMIZATION_ENABLED = false;
#endif

namespace resource_compiler
{
    SHIRABE_DECLARE_LOG_TAG(ShirabeEngineShaderPrecompiler);

    using namespace engine;

    /**
     * Describes error codes of the tool.
     */
    enum class EResult
    {
        Success      =       0,
        WrongUsage   = -100000,
        NoInput,
        FileError,
        ReadFailed,
        WriteFailed,
        InputInvalid,
        CompilationFailed,
        LinkFailed,
        ExtractionFailed,
        SerializationFailed
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
}

#endif // _SHIRABE_SHADERPRECOMP_DEFINITION_H_
