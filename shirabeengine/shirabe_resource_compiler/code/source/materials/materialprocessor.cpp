#include "materials/materialprocessor.h"
#include "materials/definition.h"
#include "materials/extraction.h"
#include "materials/shadercompilationunit.h"

#include "common/config.h"
#include "common/functions.h"

#include <core/helpers.h>
#include <util/documents/json.h>
#include <material/declaration.h>

namespace materials
{
    using namespace engine::core;
    using namespace engine::documents;
    using namespace engine::material;

    /**
     * Derive the shading language used and the stage to compile from the full shader file name.
     *
     * @param aFileName See brief.
     * @return          A tuple containing the language and stage information.
     */
    std::tuple<materials::EShadingLanguage, VkPipelineStageFlagBits> const compileTargetFromShaderFilename(std::string const &aFileName)
    {
        using namespace materials;

        VkPipelineStageFlagBits stage    = {};
        EShadingLanguage        language = EShadingLanguage::Unknown;

        // Possible filename variants:
        // 1. <basename>.<stage>.<language_unified_ext> ; e.g. awesomeShader.vert.glsl
        // 2. <basename>.<language_unified_ext>         ; e.g. awesomeShader.hlsl, awesomeShader.cg
        // 3. <basename>.<stage>                        ; e.g. awesomeShader.vert, awesomeShader.vs

        std::string const filename = std::filesystem::path(aFileName).filename();

        size_t const primaryExtensionPosition   = filename.find_last_of(".");
        size_t const secondaryExtensionPosition = filename.find_last_of(".", primaryExtensionPosition - 1);

        bool const hasPrimaryExtension   = (std::string::npos != primaryExtensionPosition);
        bool const hasSecondaryExtension = (std::string::npos != secondaryExtensionPosition);

        std::string primaryExtension   = std::string();
        std::string secondaryExtension = std::string();

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
            // usage();
            return { EShadingLanguage::Unknown, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM };
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
            CLog::Error(logTag(), CString::format("Invalid extension '{}'. Cannot adapt language.", stageName));
            return { EShadingLanguage::Unknown, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM };
        }

        // Determine stage
        stage = mapValue<std::string, VkPipelineStageFlagBits>(stageName, {{ "vert", VkPipelineStageFlagBits::VK_PIPELINE_STAGE_VERTEX_SHADER_BIT                  },
                { "tesc", VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT    },
                { "tese", VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT },
                { "geom", VkPipelineStageFlagBits::VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT                },
                { "frag", VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT                },
                { "comp", VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT                 },
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

        if(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM == stage)
        {
            CLog::Error(logTag(), CString::format("Invalid file extension '{}'. Cannot map to stage.", stageName));
            return { EShadingLanguage::Unknown, stage };
        }

        return { language, stage };
    }

    /**
     * Determine the output name of the compiled shader based on language, stage and file base name.
     *
     * @param aFileBaseName
     * @param aLanguage
     * @param aStage
     * @return              See brief.
     */
    [[nodiscard]]
    std::string const getOutputFilename(std::string                 const &aFileBaseName,
                                        materials::EShadingLanguage const &aLanguage,
                                        VkPipelineStageFlagBits     const &aStage)
    {
        using namespace materials;

        SHIRABE_UNUSED(aStage);

        std::string extension{};

        //     static std::unordered_map<EShaderStage, std::string> glslStageAssignment =
        //     {
        //         { EShaderStage::Vertex                  , "vert.glsl.spv" },
        //         { EShaderStage::TesselationControlPoint , "tesc.glsl.spv" },
        //         { EShaderStage::TesselationEvaluation   , "tese.glsl.spv" },
        //         { EShaderStage::Geometry                , "geom.glsl.spv" },
        //         { EShaderStage::Fragment                , "frag.glsl.spv" },
        //         { EShaderStage::Compute                 , "comp.glsl.spv" },
        // #ifdef NV_EXTENSIONS
        //         { EShaderStage::NVRayGen                , "rgen.spv"      },
        //         { EShaderStage::NVIntersect             , "rint.spv"      },
        //         { EShaderStage::NVAnyHit                , "rahit.spv"     },
        //         { EShaderStage::NVClosestHit            , "rchit.spv"     },
        //         { EShaderStage::NVMiss                  , "rmiss.spv"     },
        //         { EShaderStage::NVCallable              , "rcall.spv"     },
        //         { EShaderStage::NVMesh                  , "mesh.spv"      },
        //         { EShaderStage::NVTask                  , "task.spv"      },
        // #endif
        //         { EShaderStage::NotApplicable           , "unknown"       },
        //     };
        //
        //     static std::unordered_map<EShaderStage, std::string> hlslStageAssignment =
        //     {
        //         { EShaderStage::Vertex                  , "vs.hlsl.spv" },
        //         { EShaderStage::TesselationControlPoint , "hs.hlsl.spv" },
        //         { EShaderStage::TesselationEvaluation   , "ds.hlsl.spv" },
        //         { EShaderStage::Geometry                , "gs.hlsl.spv" },
        //         { EShaderStage::Fragment                , "ps.hlsl.spv" },
        //         { EShaderStage::Compute                 , "cs.hlsl.spv" },
        //         { EShaderStage::NotApplicable           , "unknown"     },
        //     };
        //
        //     std::string const hlslStage = mapValue<EShaderStage, std::string>(aStage, std::move(hlslStageAssignment));
        //     std::string const glslStage = mapValue<EShaderStage, std::string>(aStage, std::move(glslStageAssignment));

        std::unordered_map<EShadingLanguage, std::string> languageAssignment  =
                                                                  {
                                                                          { EShadingLanguage::CGLanguage, "cg.spv"   },
                                                                          { EShadingLanguage::XShade,     "xs.spv"   },
                                                                          { EShadingLanguage::GLSL,       "glsl.spv" },
                                                                          { EShadingLanguage::HLSL,       "hlsl.spv" },
                                                                          { EShadingLanguage::Unknown,    "unknown"  }
                                                                  };

        extension = mapValue<EShadingLanguage, std::string>(aLanguage, std::move(languageAssignment));

        return CString::format("{}.{}", aFileBaseName, extension);
    }

    /**
     * Compile a single shader file.
     *
     * @param aFilename
     * @param aOptions
     * @return
     */
    CResult<SShaderCompilationUnit> generateCompilationUnit(  SConfiguration                     const &aConfiguration
                                                            , std::vector<std::filesystem::path> const &aFilenames
                                                            , std::filesystem::path              const &aModuleOutputPath
                                                            , SMaterialAsset                            &aInOutMeta)
    {
        using namespace materials;

        EShaderCompiler  compiler = EShaderCompiler::Unknown;
        EShadingLanguage language = EShadingLanguage::Unknown;

        auto const deriveElement = [&] (std::string const &aFilename) -> SShaderCompilationElement
        {
            std::string shaderString = readFile(aFilename);
            if(shaderString.empty())
            {
                CLog::Error(logTag(), CString::format("Shader file {} is empty.", aFilename));
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
                CLog::Error(logTag(), CString::format("Different compilers detected for a pair of files. C1: {}, C2: {}", EnumValueOf(compiler), EnumValueOf(fileCompiler)));
                return {};
            }

            std::string           const outputName = getOutputFilename(std::filesystem::path(aFilename).stem(), language, stage);
            std::filesystem::path const outputPath = (aModuleOutputPath / outputName);

            SShaderCompilationElement element {};
            element.fileName           = aFilename;
            element.contents           = shaderString;
            element.stage              = stage;
            element.outputPathAbsolute = outputPath;
            element.outputPathRelative = std::filesystem::relative(outputPath, (std::filesystem::current_path() / aConfiguration.outputPath));

            std::string const path = element.outputPathRelative;
            aInOutMeta.stages[stage].filename = asset::assetIdFromUri(path);

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

    /**
     * Format a valid glslangValidator command line and invoke the command to create a .spv module reading its stdout/stderr output.
     *
     * @param aUnit Source data information for the glslangValidator command.
     * @return      EResult::Success           if successful.
     * @return      EResult::CompilationFailed on error.
     */
    static CResult<EResult> runGlslang(SConfiguration const &aConfiguration, SShaderCompilationUnit &aUnit, bool const aCompileStagesIndividually = false)
    {
        std::string const application = CString::format("{}/tools/glslang/bin/glslangValidator", std::filesystem::current_path().string());
        std::string       options     = "-v -d -g -Od -V --target-env vulkan1.1";

        auto const appendIncludes = [&options] (std::string const &aInclude) -> void
        {
            options.append(" -I" + aInclude);
        };
        std::for_each(aConfiguration.includePaths.begin(), aConfiguration.includePaths.end(), appendIncludes);

        std::underlying_type_t<EResult> result = 0;

        auto const once = [&] (std::string const &aInputFilenames, std::string const &aOutputFilename) -> void
        {
            std::string                const command       = CString::format("{} {} -o {} {}", application, options, aOutputFilename, aInputFilenames);
            CEngineResult<std::string> const commandResult = executeCmd(command);

            bool const compilationError = (std::string::npos != commandResult.data().find("compilation error")
                                           or std::string::npos != commandResult.data().find("not found"));
            if(compilationError || not commandResult.successful())
            {
                CLog::Error(logTag(), commandResult.data());
                result |= EnumValueOf(EResult::CompilationFailed);
            }
            else
            {
                CLog::Debug(logTag(), commandResult.data());
                result |= EnumValueOf(EResult::Success);
            }

            CLog::Debug(logTag(), command);
        };

        if(aCompileStagesIndividually)
        {
            auto const compile = [&] (SShaderCompilationElement const &aElement) -> void
            {
                std::string const outputFile = aElement.outputPathAbsolute;
                std::string const inputFile  = aElement.fileName;

                once(inputFile, outputFile);

                aUnit.outputFiles.push_back(outputFile);
            };
            std::for_each(aUnit.elements.begin(), aUnit.elements.end(), compile);
        }
        else
        {
            std::string const outputFiles = aUnit.elements.at(0).outputPathAbsolute;
            std::string       inputFiles  = "";

            auto const append = [&inputFiles] (SShaderCompilationElement const &aElement)
            {
                inputFiles.append(" " + aElement.fileName);
            };
            std::for_each(aUnit.elements.begin(), aUnit.elements.end(), append);

            once(inputFiles, outputFiles);
            aUnit.outputFiles.push_back(aUnit.elements.at(0).outputPathAbsolute);
        }

        return static_cast<EResult>(result);
    }

    /**
     * Format a valid spirv-dis command line and invoke the command to create a disassembled spirv module reading its stdout/stderr output.
     *
     * @param aInputFilenames Source data information for the glslangValidator command.
     * @return                EResult::Success      if successful.
     * @return                EResult::InputInvalid on error.
     */
    static CResult<EResult> runSpirVDisassembler(std::vector<std::string> const &aInputFilenames)
    {
        std::string const application = CString::format("{}/tools/spirv-tools/bin/spirv-dis", std::filesystem::current_path().string());
        std::string const options     = "";

        std::underlying_type_t<EResult> result = 0;

        auto const disassemble = [&] (std::string const &aFilename) -> void
        {
            std::string const inputFile  = aFilename;
            std::string const outputFile = aFilename + ".dis";

            std::string                const command       = CString::format("{} {} -o {} {}", application, options, outputFile, inputFile);
            CEngineResult<std::string> const commandResult = executeCmd(command);
            if(not commandResult.successful())
            {
                CLog::Error(logTag(), commandResult.data());
                result |= EnumValueOf(EResult::InputInvalid);
            }
            else
            {
                CLog::Debug(logTag(), commandResult.data());
                result |= EnumValueOf(EResult::Success);
            }
        };
        std::for_each(aInputFilenames.begin(), aInputFilenames.end(), disassemble);

        return static_cast<EResult>(result);
    }

    /**
     * Accept a SMaterial instance and serialize it to a JSON string.
     *
     * @param aMaterial
     * @param aOutSerializedData
     * @return
     */
    static CResult<EResult> serializeMaterialIndex(SMaterialMasterIndex const &aMaterialIndex, std::string &aOutSerializedData)
    {
        using namespace resource_compiler::serialization;

        Unique<IJSONSerializer<SMaterialMasterIndex>> serializer = makeUnique<CJSONSerializer<SMaterialMasterIndex>>();
        bool const initialized = serializer->initialize();
        if(false == initialized)
        {
            return EResult::SerializationFailed;
        }
        CResult<Shared<serialization::ISerializer<SMaterialMasterIndex>::IResult>> const serialization = serializer->serialize(aMaterialIndex);
        if(not serialization.successful())
        {
            return EResult::SerializationFailed;
        }

        CResult<std::string> data = serialization.data()->asString();
        aOutSerializedData = data.data();

        bool const deinitialized = serializer->deinitialize();
        if(false == deinitialized)
        {
            return EResult::SerializationFailed;
        }

        serializer = nullptr;

        CLog::Debug(logTag(), aOutSerializedData);

        return EResult::Success;
    }

    /**
     * Accept a SMaterial instance and serialize it to a JSON string.
     *
     * @param aMaterial
     * @param aOutSerializedData
     * @return
     */
    static CResult<EResult> serializeMaterialAsset(SMaterialAsset const &aMaterialMeta, std::string &aOutSerializedData)
    {
        using namespace resource_compiler::serialization;

        Unique<IJSONSerializer<SMaterialAsset>> serializer  = makeUnique<CJSONSerializer<SMaterialAsset>>();
        bool const                              initialized = serializer->initialize();
        if(false == initialized)
        {
            return EResult::SerializationFailed;
        }
        CResult<Shared<serialization::ISerializer<SMaterialAsset>::IResult>> const serialization = serializer->serialize(aMaterialMeta);
        if(not serialization.successful())
        {
            return EResult::SerializationFailed;
        }

        CResult<std::string> data = serialization.data()->asString();
        aOutSerializedData = data.data();

        bool const deinitialized = serializer->deinitialize();
        if(false == deinitialized)
        {
            return EResult::SerializationFailed;
        }

        serializer = nullptr;

        CLog::Debug(logTag(), aOutSerializedData);

        return EResult::Success;
    }

    /**
     * @brief serializeMaterialConfig
     * @param aMaterialConfig
     * @param aOutSerializedData
     * @return
     */
    static CResult<EResult> serializeMaterialConfig(CMaterialConfig const &aMaterialConfig, std::string &aOutSerializedData)
    {
        using namespace resource_compiler::serialization;

        Unique<IJSONSerializer<CMaterialConfig>> serializer = makeUnique<CJSONSerializer<CMaterialConfig>>();
        bool const initialized = serializer->initialize();
        if(false == initialized)
        {
            return EResult::SerializationFailed;
        }
        CResult<Shared<serialization::ISerializer<CMaterialConfig>::IResult>> const serialization = serializer->serialize(aMaterialConfig);
        if(not serialization.successful())
        {
            return EResult::SerializationFailed;
        }

        CResult<std::string> data = serialization.data()->asString();
        aOutSerializedData = data.data();

        bool const deinitialized = serializer->deinitialize();
        if(false == deinitialized)
        {
            return EResult::SerializationFailed;
        }

        serializer = nullptr;

        CLog::Debug(logTag(), aOutSerializedData);

        return EResult::Success;
    }

    CResult<EResult> processMaterial(std::filesystem::path const &aMaterialFile, SConfiguration const &aConfig)
    {
        std::filesystem::path const &materialPathAbs  = std::filesystem::current_path() / aMaterialFile;
        std::filesystem::path const &parentPath       = std::filesystem::relative(aMaterialFile, aConfig.inputPath).parent_path();
        std::filesystem::path const &materialID       = aMaterialFile.stem();

        // Make sure the output config is correct.
        std::filesystem::path const outputPath                      = ( parentPath)                                                              .lexically_normal();
        std::filesystem::path const outputModulePath                = ( parentPath / "modules")                                                  .lexically_normal();
        std::filesystem::path const outputIndexPath                 = ( parentPath / (std::filesystem::path(materialID.string() + ".material")))     .lexically_normal();
        std::filesystem::path const outputConfigurationPath         = ( parentPath / (std::filesystem::path(materialID.string() + ".config")))       .lexically_normal();
        std::filesystem::path const outputPathAbsolute              = (std::filesystem::current_path() / aConfig.outputPath / outputPath             ).lexically_normal();
        std::filesystem::path const outputModulePathAbsolute        = (std::filesystem::current_path() / aConfig.outputPath / outputModulePath       ).lexically_normal();
        std::filesystem::path const outputIndexPathAbsolute         = (std::filesystem::current_path() / aConfig.outputPath / outputIndexPath        ).lexically_normal();
        std::filesystem::path const outputConfigurationPathAbsolute = (std::filesystem::current_path() / aConfig.outputPath / outputConfigurationPath).lexically_normal();

        checkPathExists(outputPathAbsolute);
        checkPathExists(outputModulePathAbsolute);
        // checkPathExists(outputIndexPathAbsolute);
        // checkPathExists(outputMetaPathAbsolute);
        // checkPathExists(outputSignaturePathAbsolute);
        // checkPathExists(outputConfigurationPathAbsolute);

        std::string const indexFileContents = readFile(materialPathAbs);

        Shared<documents::IJSONDeserializer<SMaterialMasterIndex>> indexDeserializer = makeShared<documents::CJSONDeserializer<SMaterialMasterIndex>>();
        indexDeserializer->initialize();

        auto [success, index] = indexDeserializer->deserialize(indexFileContents);
        if(not success)
        {
            CLog::Error(logTag(), "Could not deserialize material index file.");
            return false;
        }

        indexDeserializer->deinitialize();

        std::vector<std::filesystem::path> inputFiles {};

        SMaterialMasterIndex const indexData = *static_cast<SMaterialMasterIndex const *>(&(index->asT().data()));

        SMaterialAsset assetData = {};
        assetData.uid                   = indexData.uid;
        assetData.name                  = indexData.name;
        assetData.configurationAssetUid = asset::assetIdFromUri(outputConfigurationPath);

        for(auto const &[stage, pathReferences] : indexData.stages)
        {
            if(not pathReferences.glslSourceFilename.empty())
            {
                inputFiles.push_back(aConfig.inputPath / parentPath / pathReferences.glslSourceFilename);
            }
        }

        // Determine compilation items and config.
        auto [generationSuccessful, unit] = generateCompilationUnit(aConfig, inputFiles, outputModulePathAbsolute, assetData);
        if(not generationSuccessful)
        {
            CLog::Error(logTag(), "Failed to adapt shader compilation units and configuration");
            return EResult::InputInvalid;
        }

        CResult<EResult> const glslangResult = runGlslang(aConfig, unit, true);
        if(not glslangResult.successful())
        {
            CLog::Error(logTag(), "Failed to run glslang.");
            return glslangResult;
        }

        CResult<bool> const extractionResult = spirvCrossExtract(unit, assetData);
        if(not extractionResult.successful())
        {
            CLog::Error(logTag(), "Failed to extract data from Spir-V file(s).");
            return EResult::ExtractionFailed;
        }

        std::string serializedData = {};

        // Write meta
        CResult<EResult> const assetSerializationResult = serializeMaterialAsset(assetData, serializedData);
        if(not assetSerializationResult.successful())
        {
            CLog::Error(logTag(), "Failed to serialize asset data.");
            return EResult::SerializationFailed;
        }

        writeFile(outputIndexPathAbsolute, serializedData);

        //CMaterialConfig        config                    = CMaterialConfig::fromMaterialDesc(extractionResult.data(), );
        //CResult<EResult> const configSerializationResult = serializeMaterialConfig(config, serializedData);
        //if(not configSerializationResult.successful())
        //{
        //    CLog::Error(logTag(), "Failed to serialize config data.");
        //    return EResult::SerializationFailed;
        //}
        //
        //writeFile(outputConfigurationPathAbsolute, serializedData);

        return EResult::Success;
    }
}
