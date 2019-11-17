#ifndef _SHIRABE_RESOURCECOMP_COMPILATIONUNIT_H_
#define _SHIRABE_RESOURCECOMP_COMPILATIONUNIT_H_

#include <core/serialization/serialization.h>
#include <vulkan/vulkan.h>
#include "definition.h"

namespace resource_compiler
{
    // using namespace engine::material;

    /**
     * @brief The SShaderCompilationElement struct
     */
    struct SShaderCompilationElement
    {
        std::string             fileName;
        std::string             outputPathAbsolute;
        std::string             outputPathRelative;
        std::string             contents;
        VkPipelineStageFlagBits stage;
        char                    padding[4]; // Explicit alignment padding.

    public_constructors:
        /**
         * @brief SShaderCompilationElement
         */
        SShaderCompilationElement();

        /**
         * @brief SShaderCompilationElement
         *
         * @param aFileName
         * @param aContents
         * @param aStage
         */
        SShaderCompilationElement(std::string             const &aFileName,
                                  std::string             const &aOutputPathAbsolute,
                                  std::string             const &aOutputPathRelative,
                                  std::string             const &aContents,
                                  VkPipelineStageFlagBits const aStage);

        /**
         * @brief SShaderCompilationElement
         * @param aOther
         */
        SShaderCompilationElement(SShaderCompilationElement const &aOther);

        /**
         * @brief SShaderCompilationElement
         * @param aOther
         */
        SShaderCompilationElement(SShaderCompilationElement &&aOther);

    public_destructors:
        ~SShaderCompilationElement() = default;

    public_methods:        
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
        SShaderCompilationUnit();

        /**
         * @brief SShaderCompilationUnit
         * @param aCompiler
         * @param aLanguage
         */
        SShaderCompilationUnit(EShaderCompiler  aCompiler,
                               EShadingLanguage aLanguage);

        /**
         * @brief SShaderCompilationUnit
         * @param aOther
         */
        SShaderCompilationUnit(SShaderCompilationUnit const &aOther);

        /**
         * @brief SShaderCompilationUnit
         * @param aOther
         */
        SShaderCompilationUnit(SShaderCompilationUnit &&aOther) noexcept;

    public_destructors:
        virtual ~SShaderCompilationUnit() = default;

    public_methods:
        /**
         * @brief addElement
         * @param aFileName
         * @param aContents
         * @param aStage
         * @return
         */
        EResult addElement(std::string             const &aFileName,
                           std::string             const &aOutputPathAbsolute,
                           std::string             const &aOutputPathRelative,
                           std::string             const &aContents,
                           VkPipelineStageFlagBits const  aStage);

        /**
         * @brief addElement
         * @param aOther
         * @return
         */
        EResult addElement(SShaderCompilationElement const &aOther);
    };
}

#endif // _SHIRABE_SHADERPRECOMP_COMPILATIONUNIT_H_
