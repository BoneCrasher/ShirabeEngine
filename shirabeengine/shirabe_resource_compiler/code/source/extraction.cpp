#include <filesystem>
#include <fstream>
#include <optional>

#include <spirv_cross/spirv_cross.hpp>
#include <core/helpers.h>
#include "extraction.h"

namespace resource_compiler
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
    static std::string const determineStageName(VkPipelineStageFlagBits const &aStage)
    {
        switch(aStage)
        {
        case VkPipelineStageFlagBits::VK_PIPELINE_STAGE_VERTEX_SHADER_BIT:                  return "vertex";
        case VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT:    return "tesselation_controlpoint";
        case VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT: return "tesselation_evaluation";
        case VkPipelineStageFlagBits::VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT:                return "geometry";
        case VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT:                return "fragment";
        case VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT:                 return "compute";
        default:                                                                            return "unknown";
        }
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    static std::string const determineSPIRVTypeName(spirv_cross::SPIRType const &aType)
    {
        switch(aType.basetype)
        {
        case spirv_cross::SPIRType::BaseType::Unknown          : return "Unknown";
        case spirv_cross::SPIRType::BaseType::Void             : return "Void";
        case spirv_cross::SPIRType::BaseType::Boolean          : return "Boolean";
        case spirv_cross::SPIRType::BaseType::Char             : return "Char";
        case spirv_cross::SPIRType::BaseType::SByte            : return "SByte";
        case spirv_cross::SPIRType::BaseType::UByte            : return "UByte";
        case spirv_cross::SPIRType::BaseType::Short            : return "Short";
        case spirv_cross::SPIRType::BaseType::UShort           : return "UShort";
        case spirv_cross::SPIRType::BaseType::Int              : return "Int";
        case spirv_cross::SPIRType::BaseType::UInt             : return "UInt";
        case spirv_cross::SPIRType::BaseType::Int64            : return "Int64";
        case spirv_cross::SPIRType::BaseType::UInt64           : return "UInt64";
        case spirv_cross::SPIRType::BaseType::AtomicCounter    : return "AtomicCounter";
        case spirv_cross::SPIRType::BaseType::Half             : return "Half";
        case spirv_cross::SPIRType::BaseType::Float            : return "Float";
        case spirv_cross::SPIRType::BaseType::Double           : return "Double";
        case spirv_cross::SPIRType::BaseType::Struct           : return "Struct";
        case spirv_cross::SPIRType::BaseType::Image            : return "Image";
        case spirv_cross::SPIRType::BaseType::SampledImage     : return "SampledImage";
        case spirv_cross::SPIRType::BaseType::Sampler          : return "Sampler";
        case spirv_cross::SPIRType::BaseType::ControlPointArray: return "ControlPointArray";
        default:                                                 return "Unknown";
        }
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <typename T>
    auto checkForDuplicateBoundResource (std::vector<T> &aSource, T const &aBuffer)
        -> std::optional<
               std::reference_wrapper<
                   std::enable_if_t<std::is_base_of_v<SBoundResource, T>, T>
               >
           >
    {
        if(aSource.empty())
        {
            return {};
        }

        for(T &aCompareBuffer : aSource)
        {
            bool equalName          = false;
            bool equalSetAndBinding = false;
            // Make sure that globally, there are no duplicate buffer names.
            equalName          = ( 0 == aCompareBuffer.name.compare(aBuffer.name) );
            // Make sure that globally, no buffers have equal sets and bindings.
            equalSetAndBinding = ( aCompareBuffer.set == aBuffer.set and aCompareBuffer.binding == aBuffer.binding );

            // TODO: Binding overlap check

            // if ( aCompareBuffer.location.equals(aBuffer.location)
            //      or aCompareBuffer.location.overlapsWith(aBuffer.location) )
            // {
            //     // Locations may not overlap.
            //     goto invalid;
            // }
            //
            // for( auto const &[name, member] : aCompareBuffer.members )
            // {
                //
                // We don't have to check the members itself, since duplicate names inside a
                // single GLSL file are caught by the glslangValidator and cross-stage duplicate
                // names are no problem.
                //

                // bool const containsMember = (aBuffer.members.end() != aBuffer.members.find(name));
                // if(containsMember)
                // {
                //     // Duplicate name in other buffer. Not allowed.
                //     goto invalid;
                // }

                // SUniformBufferMember const &otherMember = aBuffer.members.at(name);
                // if ( member.location.equals(otherMember.location)
                //      or member.location.overlapsWith(otherMember.location) )
                // {
                //     // Locations may not overlap.
                //     goto invalid;
                // }
            // }

            if(equalName and equalSetAndBinding)
            {
                return { aCompareBuffer }; // Return the identified duplicate buffer...
            }
        }

        return {};
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CResult<SMaterialSignature> spirvCrossExtract(SShaderCompilationUnit const &aUnit)
    {
        std::underlying_type_t<EResult> result = EnumValueOf(EResult::Success);

        SMaterialSignature materialExtracted {};

        //
        // As we process elements below, which affect sets and bindings, store some information
        // on them.
        //
        SMaterialLayoutInfo layoutInfo {};
        layoutInfo.setCount        = 0;
        layoutInfo.setBindingCount = {};

        auto const updateLayoutInfoFn = [&layoutInfo] (uint32_t const &setIndex, uint32_t const &bindingIndex) -> void
        {
            if(layoutInfo.setCount <= setIndex)
            {
                layoutInfo.setCount = (setIndex + 1);
                layoutInfo.setBindingCount.resize(layoutInfo.setCount);
            }

            if(layoutInfo.setBindingCount[setIndex] <= bindingIndex)
            {
                layoutInfo.setBindingCount[setIndex] = (bindingIndex + 1);
            }
        };

        auto const reflectType = [] (spirv_cross::SPIRType const &aType) -> SMaterialType
        {
            std::string           const  typeName       = determineSPIRVTypeName(aType);
            uint32_t              const  typeByteWidth  = (aType.width /* bit */ / 8);
            uint32_t              const  typeVectorSize = aType.vecsize;

            uint32_t arraySize     = 1;
            uint32_t arrayStride   = (typeVectorSize * typeByteWidth);
            uint32_t matrixRows    = typeVectorSize;
            uint32_t matrixColumns = aType.columns;
            uint32_t matrixStride  = (typeVectorSize * typeByteWidth);
            if (not aType.array.empty())
            {
                arraySize = aType.array[0]; // Multidimensional arrays not yet supported.
            }

            SMaterialType type {};
            type.name               = determineSPIRVTypeName(aType);
            type.vectorSize         = typeVectorSize;
            type.arraySize          = arraySize;
            type.arrayStride        = arrayStride;
            type.matrixRows         = matrixRows;
            type.matrixColumns      = matrixColumns;
            type.matrixColumnStride = matrixStride;

            CLog::Debug(logTag(),
                        "\n     Type:                       "
                        "\n         Type-Name:            %0"
                        "\n         Vector-Size:          %1"
                        "\n         Array-Size:           %2"
                        "\n         Array-Stride:         %3"
                        "\n         Matrix-Rows:          %4"
                        "\n         Matrix-Columns:       %5"
                        "\n         Matrix-Column-Stride: %6",
                        type.name,
                        type.vectorSize,
                        type.arraySize,
                        type.arrayStride,
                        type.matrixRows,
                        type.matrixColumns,
                        type.matrixColumnStride);

            return type;
        };

        auto const reflect = [&] (SShaderCompilationElement const &aElement) -> void
        {
            SMaterialStage stageExtracted {};
            stageExtracted.stage     = aElement.stage;
            stageExtracted.stageName = determineStageName(aElement.stage);
            stageExtracted.filename  = aElement.outputPathRelative;

            std::string           const inputFile   = aElement.outputPathAbsolute;
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

                spirv_cross::SPIRType const &type          = compiler.get_type(stageInput.type_id);
                SMaterialType         const  typeExtracted = reflectType(type);

                SStageInput stageInputExtracted{};
                stageInputExtracted.name     = stageInput.name;
                stageInputExtracted.location = location;
                stageInputExtracted.type     = typeExtracted;
                stageExtracted.inputs.push_back(stageInputExtracted);

                CLog::Debug(logTag(),
                            "\nStageInput:   "
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

                spirv_cross::SPIRType const &type          = compiler.get_type(stageOutput.type_id);
                SMaterialType         const  typeExtracted = reflectType(type);

                SStageOutput stageOutputExtracted{};
                stageOutputExtracted.name     = stageOutput.name;
                stageOutputExtracted.location = location;
                stageOutputExtracted.type     = typeExtracted;
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

                updateLayoutInfoFn(set, binding);

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

                updateLayoutInfoFn(set, binding);

                spirv_cross::SPIRType const &type       = compiler.get_type(uniformBuffer.base_type_id);
                size_t                const  bufferSize = compiler.get_declared_struct_size(type);

                SUniformBuffer uniformBufferExtracted{};
                uniformBufferExtracted.name             = uniformBuffer.name;
                uniformBufferExtracted.set              = set;
                uniformBufferExtracted.binding          = binding;
                uniformBufferExtracted.location.offset  = 0;
                uniformBufferExtracted.location.length  = bufferSize;
                uniformBufferExtracted.location.padding = 0;
                uniformBufferExtracted.stageBinding.set(stageExtracted.stage);

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
                }                

                // separate_samplers
                // separate_images

                auto possiblyDuplicateBufferOrEmpty = checkForDuplicateBoundResource(materialExtracted.uniformBuffers, uniformBufferExtracted);
                if(possiblyDuplicateBufferOrEmpty.has_value())
                {
                    possiblyDuplicateBufferOrEmpty->get().stageBinding.set(uniformBufferExtracted.stageBinding); // Append the additional stage binding.
                    CLog::Warning(logTag(), "Uniform buffer '%0' already added. Adjusting...", uniformBufferExtracted.name);
                }
                else
                {
                    materialExtracted.uniformBuffers.push_back(uniformBufferExtracted);
                }
            }

            // Read Textures
            for (spirv_cross::Resource const &sampledImage : resources.sampled_images)
            {
                uint32_t const set      = compiler.get_decoration(sampledImage.id, spv::DecorationDescriptorSet);
                uint32_t const binding  = compiler.get_decoration(sampledImage.id, spv::DecorationBinding);

                updateLayoutInfoFn(set, binding);

                SSampledImage image{};
                image.name    = sampledImage.name;
                image.set     = set;
                image.binding = binding;
                image.stageBinding.set(stageExtracted.stage);

                auto possiblyDuplicateImageOrEmpty = checkForDuplicateBoundResource(materialExtracted.sampledImages, image);
                if(possiblyDuplicateImageOrEmpty.has_value())
                {
                    possiblyDuplicateImageOrEmpty->get().stageBinding.set(image.stageBinding); // Append the additional stage binding.
                    CLog::Warning(logTag(), "Sampled image '%0' already added. Adjusting...", image.name);
                }
                else
                {
                    materialExtracted.sampledImages.push_back(image);
                }

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

        materialExtracted.layoutInfo = layoutInfo;

        return { ( EResult::Success == static_cast<EResult>(result) ), materialExtracted };
    }
    //<-----------------------------------------------------------------------------
}
