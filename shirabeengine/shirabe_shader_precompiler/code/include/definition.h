#ifndef   _SHIRABE_SHADERPRECOMP_DEFINITION_H_
#define   _SHIRABE_SHADERPRECOMP_DEFINITION_H_

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

namespace shader_precompiler
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
        InputInvalid,
        CompilationFailed,
        LinkFailed
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

    /**
     * @brief The SShaderCompilationElement struct
     */
    struct SShaderCompilationElement
    {
        std::string  fileName;
        std::string  outputPath;
        std::string  contents;
        EShaderStage stage;
        char         padding[4]; // Explicit alignment padding.

    public_constructors:
        /**
         * @brief SShaderCompilationElement
         */
        SHIRABE_INLINE
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
        SHIRABE_INLINE
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

        std::vector<std::string> outputFiles;

    public_constructors:

        /**
         * @brief SShaderCompilationUnit
         */
        SHIRABE_INLINE
        SShaderCompilationUnit()
            : compiler(EShaderCompiler::Unknown)
            , language(EShadingLanguage::Unknown)
        {}

        /**
         * @brief SShaderCompilationUnit
         * @param aCompiler
         * @param aLanguage
         */
        SHIRABE_INLINE
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
        SHIRABE_INLINE
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
        SHIRABE_INLINE
        EResult addElement(SShaderCompilationElement const &aOther)
        {
            elements.push_back(aOther);
        }
    };
}

#endif // _SHIRABE_SHADERPRECOMP_DEFINITION_H_
