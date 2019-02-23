#include "shadercompilationunit.h"

namespace shader_precompiler
{
    using serialization::IMaterialSerializer;

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    SShaderCompilationElement::SShaderCompilationElement()
        : fileName({})
        , outputPath({})
        , contents({})
        , stage(EShaderStage::NotApplicable)
    {}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    SShaderCompilationElement::SShaderCompilationElement(std::string  const &aFileName,
                                                         std::string  const &aOutputPath,
                                                         std::string  const &aContents,
                                                         EShaderStage const aStage)
        : fileName(aFileName)
        , outputPath(aOutputPath)
        , contents(aContents)
        , stage(aStage)
    {}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    SShaderCompilationElement::SShaderCompilationElement(SShaderCompilationElement const &aOther)
        : fileName(aOther.fileName)
        , outputPath(aOther.outputPath)
        , contents(aOther.contents)
        , stage(aOther.stage)
    {}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    SShaderCompilationElement::SShaderCompilationElement(SShaderCompilationElement &&aOther)
        : fileName(std::move(aOther.fileName))
        , outputPath(std::move(aOther.outputPath))
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
    SShaderCompilationUnit::SShaderCompilationUnit(SShaderCompilationUnit &&aOther)
        : compiler(std::move(aOther.compiler))
        , language(std::move(aOther.language))
        , elements(std::move(aOther.elements))
    {}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    EResult SShaderCompilationUnit::addElement(std::string  const &aFileName,
                                               std::string  const &aOutputPath,
                                               std::string  const &aContents,
                                               EShaderStage const  aStage)
    {
        if(aFileName.empty() || aOutputPath.empty() || aContents.empty())
        {
            CLog::Error(logTag(), CString::format("Invalid input for file %0:\n%1", aFileName, aContents));
            return EResult::InputInvalid;
        }

        addElement({ aFileName, aOutputPath, aContents, aStage });
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    EResult SShaderCompilationUnit::addElement(SShaderCompilationElement const &aOther)
    {
        elements.push_back(aOther);
    }
    //<-----------------------------------------------------------------------------
}
