#include "materials/shadercompilationunit.h"
#include <asset/material/serialization.h>

namespace materials
{
    using documents::IJSONSerializer;

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    SShaderCompilationElement::SShaderCompilationElement()
        : fileName({})
        , outputPathAbsolute({})
        , outputPathRelative({})
        , contents({})
        , stage(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM)
    {}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    SShaderCompilationElement::SShaderCompilationElement(std::string            const &aFileName,
                                                         std::string            const &aOutputPathAbsolute,
                                                         std::string            const &aOutputPathRelative,
                                                         std::string            const &aContents,
                                                         VkPipelineStageFlagBits const aStage)
        : fileName(aFileName)
        , outputPathAbsolute(aOutputPathAbsolute)
        , outputPathRelative(aOutputPathRelative)
        , contents(aContents)
        , stage(aStage)
    {}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    SShaderCompilationElement::SShaderCompilationElement(SShaderCompilationElement const &aOther)
        : fileName(aOther.fileName)
        , outputPathAbsolute(aOther.outputPathAbsolute)
        , outputPathRelative(aOther.outputPathRelative)
        , contents(aOther.contents)
        , stage(aOther.stage)
    {}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    SShaderCompilationElement::SShaderCompilationElement(SShaderCompilationElement &&aOther)
        : fileName(std::move(aOther.fileName))
        , outputPathAbsolute(std::move(aOther.outputPathAbsolute))
        , outputPathRelative(std::move(aOther.outputPathRelative))
        , contents(std::move(aOther.contents))
        , stage(aOther.stage)
    {}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    SShaderCompilationUnit::SShaderCompilationUnit()
        : compiler(EShaderCompiler::Unknown)
        , language(EShadingLanguage::Unknown)
        , elements({})
    {}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    SShaderCompilationUnit::SShaderCompilationUnit(EShaderCompiler  const aCompiler,
                                                   EShadingLanguage const aLanguage)
        : compiler(aCompiler)
        , language(aLanguage)
        , elements({})
    { }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    SShaderCompilationUnit::SShaderCompilationUnit(SShaderCompilationUnit const &aOther)
        : compiler(aOther.compiler)
        , language(aOther.language)
        , elements(aOther.elements)
    {}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    SShaderCompilationUnit::SShaderCompilationUnit(SShaderCompilationUnit &&aOther) noexcept
        : compiler(aOther.compiler           )
        , language(aOther.language           )
        , elements(std::move(aOther.elements))
    {}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    EResult SShaderCompilationUnit::addElement(std::string             const &aFileName,
                                               std::string             const &aOutputPathAbsolute,
                                               std::string             const &aOutputPathRelative,
                                               std::string             const &aContents,
                                               VkPipelineStageFlagBits const  aStage)
    {
        if(aFileName.empty() || aOutputPathAbsolute.empty() || aOutputPathRelative.empty() || aContents.empty())
        {
            CLog::Error(logTag(), StaticStringHelpers::format("Invalid input for file {}:\n{}", aFileName, aContents));
            return EResult::InputInvalid;
        }

        addElement({ aFileName, aOutputPathAbsolute, aOutputPathRelative, aContents, aStage });

        return EResult::Success;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    EResult SShaderCompilationUnit::addElement(SShaderCompilationElement const &aOther)
    {
        elements.push_back(aOther);

        return EResult::Success;
    }
    //<-----------------------------------------------------------------------------
}
