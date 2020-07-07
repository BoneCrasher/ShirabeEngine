#ifndef   _SHIRABE_RESOURCECOMP_MATERIALS_DEFINITION_H_
#define   _SHIRABE_RESOURCECOMP_MATERIALS_DEFINITION_H_

#include <string>
#include <vector>

#include <base/declaration.h>
#include <base/string.h>
#include <log/log.h>

#include "common/definition.h"

namespace materials
{
    SHIRABE_DECLARE_LOG_TAG(MaterialProcessor)

    using namespace engine;

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
