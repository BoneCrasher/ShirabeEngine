#include <filesystem>
#include <fstream>

#include <spirv_cross/spirv_cross.hpp>

#include "helpers.h"
#include "extraction.h"

namespace shader_precompiler
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    std::vector<uint32_t> const readSpirVFile(std::string const &aFileName)
    {
        bool const fileExists = std::filesystem::exists(aFileName);
        if(not fileExists)
        {
            return {};
        }

        std::ifstream inputFileStream(aFileName.c_str(), std::ios::in|std::ios::binary|std::ios::ate);

        std::streamoff filesize = inputFileStream.tellg();
        uint64_t       datasize = static_cast<uint64_t>(filesize) / 4;

        inputFileStream.seekg(0, std::ios::beg);

        bool const inputFileStreamOk = inputFileStream.operator bool();
        if(not inputFileStreamOk)
        {
            return {};
        }

        std::vector<uint32_t> inputData;
        inputData.resize(datasize);

        for(uint64_t k=0; k<datasize; ++k)
        {
            uint32_t *const data = (inputData.data() + k);
            char     *const ch   = reinterpret_cast<char *const>(data);
            inputFileStream.read(ch, 4);
        }

        return inputData;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    static std::string const determineStageName(EShaderStage const &aStage)
    {
        switch(aStage)
        {
        case EShaderStage::Vertex:                  return "vertex";
        case EShaderStage::TesselationControlPoint: return "tesselation_controlpoint";
        case EShaderStage::TesselationEvaluation:   return "tesselation_evaluation";
        case EShaderStage::Geometry:                return "geometry";
        case EShaderStage::Fragment:                return "fragment";
        case EShaderStage::Compute:                 return "compute";
        default:                                    return "unknown";
        }
    }

    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CResult<SMaterial> spirvCrossExtract(SShaderCompilationUnit const &aUnit)
    {
        std::underlying_type_t<EResult> result = EnumValueOf(EResult::Success);

        SMaterial materialExtracted {};

        auto const reflect = [&] (SShaderCompilationElement const &aElement) -> void
        {
            SMaterialStage stageExtracted {};
            stageExtracted.stage     = aElement.stage;
            stageExtracted.stageName = determineStageName(aElement.stage);
            stageExtracted.filename  = aElement.fileName;

            std::string           const inputFile   = aElement.outputPath;
            std::vector<uint32_t> const spirvSource = readSpirVFile(inputFile);

            spirv_cross::Compiler compiler(std::move(spirvSource));

            //
            // Handle entry points
            //
            std::vector<spirv_cross::EntryPoint> entryPoints = compiler.get_entry_points_and_stages();
            auto const reflectEntryPoints = [&] (spirv_cross::EntryPoint const &aEntryPoint) -> void
            {
                SHIRABE_UNUSED(aEntryPoint);

                // Should be 'main', due to glsl requirement and no renaming being used.
                // aEntryPoint.name;
                // Value of spv::ExecutionModel. Could be used to validate the input.
                // aEntryPoint.execution_model;
            };
            std::for_each(entryPoints.begin(), entryPoints.end(), reflectEntryPoints);

            //
            // Handle resources
            //
            spirv_cross::ShaderResources const resources = compiler.get_shader_resources();

            CLog::Debug(logTag(), "Reflecting %0", inputFile);

            //
            // Read Stage Inputs
            //
            for (spirv_cross::Resource const &stageInput : resources.stage_inputs)
            {
                uint32_t const location = compiler.get_decoration(stageInput.id, spv::DecorationLocation);

                spirv_cross::SPIRType const &type = compiler.get_type(stageInput.base_type_id);
                // TODO: Go on to extract the type for input layout matching!

                SStageInput stageInputExtracted{};
                stageInputExtracted.name     = stageInput.name;
                stageInputExtracted.location = location;
                stageExtracted.inputs.push_back(stageInputExtracted);

                CLog::Debug(logTag(),
                            "\nStageInput: "
                            "\n  ID:       %0"
                            "\n  Name:     %1"
                            "\n  Location: %2",
                            stageInput.id,
                            stageInput.name,
                            location);
            }

            //
            // Stage Outputs
            //
            for (spirv_cross::Resource const &stageOutput : resources.stage_outputs)
            {
                uint32_t const location = compiler.get_decoration(stageOutput.id, spv::DecorationLocation);

                SStageOutput stageOutputExtracted{};
                stageOutputExtracted.name     = stageOutput.name;
                stageOutputExtracted.location = location;
                stageExtracted.outputs.push_back(stageOutputExtracted);

                CLog::Debug(logTag(),
                            "\nStageOutput: "
                            "\n  ID:       %0"
                            "\n  Name:     %1"
                            "\n  Location: %2",
                            stageOutput.id,
                            stageOutput.name,
                            location);
            }

            //
            // Subpass Input; Should only contain values for the fragment shader.
            //
            for (spirv_cross::Resource const &subPassInput : resources.subpass_inputs)
            {
                uint32_t const attachmentIndex = compiler.get_decoration(subPassInput.id, spv::DecorationInputAttachmentIndex);
                uint32_t const set             = compiler.get_decoration(subPassInput.id, spv::DecorationDescriptorSet);
                uint32_t const binding         = compiler.get_decoration(subPassInput.id, spv::DecorationBinding);

                SSubpassInput stageSubpassInputExtracted{};
                stageSubpassInputExtracted.name            = subPassInput.name;
                stageSubpassInputExtracted.attachmentIndex = attachmentIndex;
                stageSubpassInputExtracted.set             = set;
                stageSubpassInputExtracted.binding         = binding;
                materialExtracted.subpassInputs.push_back(stageSubpassInputExtracted);

                CLog::Debug(logTag(),
                            "\nSubpassInput: "
                            "\n  ID:              %0"
                            "\n  Name:            %1"
                            "\n  AttachmentIndex: %2"
                            "\n  Set:             %3"
                            "\n  Binding:         %4",
                            subPassInput.id,
                            subPassInput.name,
                            attachmentIndex,
                            set,
                            binding);
            }

            //
            // Read UBOs
            //
            for (spirv_cross::Resource const &uniformBuffer : resources.uniform_buffers)
            {
                uint32_t const set      = compiler.get_decoration(uniformBuffer.id, spv::DecorationDescriptorSet);
                uint32_t const binding  = compiler.get_decoration(uniformBuffer.id, spv::DecorationBinding);

                spirv_cross::SPIRType const &type       = compiler.get_type(uniformBuffer.base_type_id);
                size_t                const  bufferSize = compiler.get_declared_struct_size(type);

                SUniformBuffer uniformBufferExtracted{};
                uniformBufferExtracted.name     = uniformBuffer.name;
                uniformBufferExtracted.location.offset  = 0;
                uniformBufferExtracted.location.length  = bufferSize;
                uniformBufferExtracted.location.padding = 0;

                CLog::Debug(logTag(),
                            "\nUniformBuffer: "
                            "\n  ID:              %0"
                            "\n  Name:            %1"
                            "\n  Set:             %2"
                            "\n  Binding:         %3",
                            "\n  Buf.-Size:       %4",
                            uniformBuffer.id,
                            uniformBuffer.name,
                            set,
                            binding,
                            bufferSize);

                uint64_t memberCount = type.member_types.size();
                for(uint32_t k=0; k<memberCount; ++k)
                {
                    spirv_cross::SPIRType const &memberType = compiler.get_type(type.member_types[k]);
                    SHIRABE_UNUSED(memberType);

                    // Fetch basic information
                    std::string const &name   = compiler.get_member_name(type.self, k);
                    uint64_t    const  offset = compiler.type_struct_member_offset(type, k);
                    uint64_t    const  size   = compiler.get_declared_struct_member_size(type, k);

                    SUniformBufferMember uniformBufferMemberExtracted{};
                    uniformBufferMemberExtracted.name             = name;
                    uniformBufferMemberExtracted.location.offset  = offset;
                    uniformBufferMemberExtracted.location.length  = size;
                    uniformBufferMemberExtracted.location.padding = 0;
                    uniformBufferExtracted.members[name]          = uniformBufferMemberExtracted;

                    CLog::Debug(logTag(),
                                "\n    UniformBuffer-Member : "
                                "\n      Name:            %0"
                                "\n      Offset:          %1"
                                "\n      Size:            %2",
                                name, offset, size);

                    // if (not memberType.array.empty())
                    // {
                    //     // Get array stride, e.g. float4 foo[]; Will have array stride of 16 bytes.
                    //     size_t arrayStride = compiler.type_struct_member_array_stride(type, k);
                    // }
                    //
                    // if (1 < memberType.columns)
                    // {
                    //     // Get bytes stride between columns (if column major), for float4x4 -> 16 bytes.
                    //     size_t matrixStride = compiler.type_struct_member_matrix_stride(type, k);
                    // }                    
                }                

                // separate_samplers
                // separate_images

                materialExtracted.uniformBuffers.push_back(uniformBufferExtracted);
            }

            // Read Textures
            for (spirv_cross::Resource const &sampledImage : resources.sampled_images)
            {
                uint32_t const set      = compiler.get_decoration(sampledImage.id, spv::DecorationDescriptorSet);
                uint32_t const binding  = compiler.get_decoration(sampledImage.id, spv::DecorationBinding);

                SSampledImage image{};
                image.name    = sampledImage.name;
                image.set     = set;
                image.binding = binding;
                materialExtracted.sampledImages.push_back(image);

                CLog::Debug(logTag(),
                            "\nSampledImage: "
                            "\n  ID:              %0"
                            "\n  Name:            %1"
                            "\n  Set:             %2"
                            "\n  Binding:         %3",
                            sampledImage.id,
                            sampledImage.name,
                            set,
                            binding);
            }

            materialExtracted.stages[aElement.stage] = stageExtracted;

            result |= EnumValueOf(EResult::Success);
        };

        std::for_each(aUnit.elements.begin(), aUnit.elements.end(), reflect);

        return { ( EResult::Success == static_cast<EResult>(result) ), materialExtracted };
    }
    //<-----------------------------------------------------------------------------
}
