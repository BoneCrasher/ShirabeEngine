#include "materials/extraction.h"
#include "materials/shadercompilationunit.h"

#include <filesystem>
#include <fstream>
#include <optional>
#include <unordered_map>
#include <regex>

#include <spirv_cross/spirv_cross.hpp>
#include <source/spirv_reflect.h>
#include <core/helpers.h>
#include <asset/material/asset.h>


namespace materials
{
    using namespace engine::material;
    using namespace engine::asset;

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
    //
    //<-----------------------------------------------------------------------------
    std::string const readSpirVFileAsText(std::string const &aFileName)
    {
        bool const fileExists = std::filesystem::exists(aFileName);
        if(not fileExists)
        {
            return {};
        }

        std::ifstream inputFileStream(aFileName.c_str(), std::ios::in|std::ios::ate);
        std::string str((std::istreambuf_iterator<char>(inputFileStream)),
                         std::istreambuf_iterator<char>());

        return str;
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
            equalName          = ( aBuffer.name == aCompareBuffer.name );
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

                // SBufferMember const &otherMember = aBuffer.members.at(name);
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
    }//<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CResult<bool> spirvCrossExtract(materials::SShaderCompilationUnit const &aUnit, SMaterialAsset &aInOutAsset)
    {
        std::underlying_type_t<EResult> result = EnumValueOf(EResult::Success);

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

        struct SAnnotation
        {
            std::string                                  id;
            std::unordered_map<std::string, std::string> values;
        };

        auto const parseAnnotations = [] (SShaderCompilationElement const &aElement) -> std::vector<SAnnotation>
            {
                static constexpr std::size_t const IDX_KEY = 1;
                static constexpr std::size_t const IDX_VAL = 2;

                std::vector<SAnnotation> annotations;

                // SPV is always generated as human readable and since it will copy in all annotations, just use that file...
                std::string const data = readSpirVFileAsText(aElement.outputPathAbsolute);

                std::regex  RE_Annotation(R"(\/\/ @nnotation:(SHIRABE_[A-Z_]+)\(((?:[\d\w]+=\"[\d\w -]+\",?)+)\))");
                std::smatch annotation_match;
                std::string source = data;

                while(std::regex_search(source, annotation_match, RE_Annotation))
                {
                    std::unordered_map<std::string, std::string> values;

                    std::string const annotationKey    = annotation_match.str(IDX_KEY);
                    std::string const annotationValues = annotation_match.str(IDX_VAL);

                    std::regex  RE_Values(R"(?:([\d\w]+)=\"([\d\w]+)\",?)");
                    std::smatch value_match;
                    std::string value_source = annotationValues;

                    while(std::regex_search(value_source, value_match, RE_Values))
                    {
                        std::string const key   = value_match.str(IDX_KEY);
                        std::string const value = value_match.str(IDX_VAL);

                        values.insert({key, value});

                        value_source = value_match.suffix().str();
                    }

                    annotations.push_back({annotationKey, values});

                    source = annotation_match.suffix().str();
                }
            };

        auto const applyAnnotations = [] (std::vector<SAnnotation> const &aAnnotations, SMaterialAsset &aInOutAsset)
            {
                for(auto const &annotation : aAnnotations)
                {
                    if("SHIRABE_SHARING_MODE" == annotation.id)
                    {
                        std::string const bufferId = annotation.values.at("buffer_id");
                        std::string const mode     = annotation.values.at("mode");

                        for(auto &buffer : aInOutAsset.uniformBuffers)
                        {
                            if(buffer.name == bufferId)
                            {
                                buffer.sharingMode = sharingModeFromString(mode);
                                return;
                            }
                        }

                        for(auto &buffer : aInOutAsset.storageBuffers)
                        {
                            if(buffer.name == bufferId)
                            {
                                buffer.sharingMode = sharingModeFromString(mode);
                                return;
                            }
                        }
                    }
                }
            };

        std::function<Shared<SMaterialType const>(spirv_cross::Compiler const&, spirv_cross::SPIRType const&)> reflectType = nullptr;
        reflectType = [&] (spirv_cross::Compiler const &aCompiler, spirv_cross::SPIRType const &aType) -> Shared<SMaterialType const>
        {
            std::string const  typeName       = determineSPIRVTypeName(aType);
            uint32_t    const  typeByteWidth  = (aType.width /* bit */ / 8);
            uint32_t    const  typeVectorSize = aType.vecsize;

            uint32_t byteSize        = typeByteWidth;
            uint32_t matrixRows      = typeVectorSize;
            uint32_t matrixColumns   = aType.columns;
            uint32_t matrixStride    = (typeVectorSize * typeByteWidth);
            uint32_t arraySize       = 1; // Set to one, as a scalar of type T is equivalent to T[1];
            if (not aType.array.empty())
            {
                arraySize = aType.array[0]; // Multidimensional arrays not yet supported.
            }
            uint32_t arrayStride = 1;

            Shared<SMaterialType> type = makeShared<SMaterialType>();
            type->name               = determineSPIRVTypeName(aType);
            type->byteSize           = byteSize;
            type->vectorSize         = typeVectorSize;
            type->arraySize          = arraySize;
            type->arrayStride        = arrayStride;
            type->matrixRows         = matrixRows;
            type->matrixColumns      = matrixColumns;
            type->matrixColumnStride = matrixStride;

            if(not aType.member_types.empty())
            {
                for(auto const &member : aType.member_types)
                {
                    spirv_cross::SPIRType const &memberType          = aCompiler.get_type(member);
                    Shared<SMaterialType const>  processedMemberType = reflectType(aCompiler, memberType);
                    type->members[processedMemberType->name] = std::move(processedMemberType);
                }
            }

            CLog::Debug(logTag(),
                        "\n     Type:                       "
                        "\n         Type-Name:            {}"
                        "\n         Element-Size:         {}"
                        "\n         Vector-Size:          {}"
                        "\n         Array-Size:           {}"
                        "\n         Array-Stride:         {}"
                        "\n         Matrix-Rows:          {}"
                        "\n         Matrix-Columns:       {}"
                        "\n         Matrix-Column-Stride: {}",
                        type->name,
                        type->byteSize,
                        type->vectorSize,
                        type->arraySize,
                        type->arrayStride,
                        type->matrixRows,
                        type->matrixColumns,
                        type->matrixColumnStride);

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

            spirv_cross::Compiler compiler(spirvSource);

            //
            // Handle entry points
            //
            spirv_cross::SmallVector<spirv_cross::EntryPoint> entryPoints = compiler.get_entry_points_and_stages();
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

            CLog::Debug(logTag(), "Reflecting {}", inputFile);

            //
            // Read Stage Inputs
            //
            for (spirv_cross::Resource const &stageInput : resources.stage_inputs)
            {
                uint32_t const location = compiler.get_decoration(stageInput.id, spv::DecorationLocation);

                spirv_cross::SPIRType const &type          = compiler.get_type(stageInput.type_id);
                Shared<SMaterialType  const> typeExtracted = reflectType(compiler, type);

                SStageInput stageInputExtracted{};
                stageInputExtracted.name     = stageInput.name;
                stageInputExtracted.location = location;
                stageInputExtracted.type     = std::move(typeExtracted);
                stageExtracted.inputs.push_back(stageInputExtracted);

                CLog::Debug(logTag(),
                            "\nStageInput:   "
                            "\n  ID:       {}"
                            "\n  Name:     {}"
                            "\n  Location: {}",
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
                Shared<SMaterialType  const> typeExtracted = reflectType(compiler, type);

                SStageOutput stageOutputExtracted{};
                stageOutputExtracted.name     = stageOutput.name;
                stageOutputExtracted.location = location;
                stageOutputExtracted.type     = std::move(typeExtracted);
                stageExtracted.outputs.push_back(stageOutputExtracted);

                CLog::Debug(logTag(),
                            "\nStageOutput: "
                            "\n  ID:       {}"
                            "\n  Name:     {}"
                            "\n  Location: {}",
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
                aInOutAsset.subpassInputs.push_back(stageSubpassInputExtracted);

                CLog::Debug(logTag(),
                            "\nSubpassInput: "
                            "\n  ID:              {}"
                            "\n  Name:            {}"
                            "\n  AttachmentIndex: {}"
                            "\n  Set:             {}"
                            "\n  Binding:         {}",
                            subPassInput.id,
                            subPassInput.name,
                            attachmentIndex,
                            set,
                            binding);
            }

            //
            // Read StorageBuffers
            //
            for (spirv_cross::Resource const &storageBuffer : resources.storage_buffers)
            {
                uint32_t const location = compiler.get_decoration(storageBuffer.id, spv::DecorationLocation);
                uint32_t const set      = compiler.get_decoration(storageBuffer.id, spv::DecorationDescriptorSet);
                uint32_t const binding  = compiler.get_decoration(storageBuffer.id, spv::DecorationBinding);

                updateLayoutInfoFn(set, binding);

                spirv_cross::SPIRType const &baseType   = compiler.get_type(storageBuffer.base_type_id);
                spirv_cross::SPIRType const &type       = compiler.get_type(storageBuffer.type_id);
                size_t                const  bufferSize = compiler.get_declared_struct_size(baseType);

                uint64_t arrayLayers = 1;
                uint64_t arrayStride = 1;
                if(not type.array.empty())
                {
                    if(type.array_size_literal[0])
                    {
                        arrayLayers = type.array[0];
                    }
                    else
                    {
                        spirv_cross::SPIRConstant const &constant = compiler.get_constant(type.array[0]);
                        arrayLayers = constant.scalar_u64();
                    }
                }
                arrayStride = (bufferSize / arrayLayers);

                SUniformBuffer bufferExtracted{};
                bufferExtracted.name             = storageBuffer.name;
                bufferExtracted.set              = set;
                bufferExtracted.binding          = binding;
                bufferExtracted.array.layers     = arrayLayers;
                bufferExtracted.array.stride     = arrayStride;
                bufferExtracted.location.offset  = 0;
                bufferExtracted.location.length  = bufferSize * bufferExtracted.array.layers;
                bufferExtracted.location.padding = 0;
                bufferExtracted.stageBinding.set(stageExtracted.stage);

                CLog::Debug(logTag(),
                            "\nStorageBuffer: "
                            "\n  ID:              {}"
                            "\n  Name:            {}"
                            "\n  Set:             {}"
                            "\n  Binding:         {}"
                            "\n  Buf.-Size:       {}",
                            storageBuffer.id,
                            storageBuffer.name,
                            set,
                            binding,
                            bufferSize);

                std::function<BufferMemberMap_t(spirv_cross::Compiler const&, spirv_cross::SPIRType const &)> deriveBufferMembers = nullptr;
                deriveBufferMembers = [&] (spirv_cross::Compiler const&aCompiler, spirv_cross::SPIRType const &aParent) -> BufferMemberMap_t
                {
                    BufferMemberMap_t map {};

                    uint64_t memberCount = aParent.member_types.size();
                    for(uint64_t k=0; k<memberCount; ++k)
                    {
                        spirv_cross::SPIRType const &memberType = aCompiler.get_type(aParent.member_types[k]);
                        SHIRABE_UNUSED(memberType);

                        // Fetch basic information
                        std::string const &name   = aCompiler.get_member_name(aParent.self, k);
                        uint64_t    const  offset = aCompiler.type_struct_member_offset(aParent, k);
                        uint64_t    const  size   = aCompiler.get_declared_struct_member_size(aParent, k);
                        // spirv_cross::SPIRType const  &memberBaseType = memberType.basetype;
                        // Shared<SMaterialType  const>  localTypeExtracted = reflectType(compiler, type);

                        uint64_t arrayLayers = 1;
                        uint64_t arrayStride = 1;
                        if(not memberType.array.empty())
                        {
                            if(memberType.array_size_literal[0])
                            {
                                arrayLayers = memberType.array[0];
                            }
                            else
                            {
                                spirv_cross::SPIRConstant const &constant = compiler.get_constant(memberType.array[0]);
                                arrayLayers = constant.scalar_u64();
                            }
                        }
                        arrayStride = (size / arrayLayers);

                        Shared<SBufferMember> bufferMemberExtracted = makeShared<SBufferMember>();
                        bufferMemberExtracted->name             = name;
                        bufferMemberExtracted->location.offset  = offset;
                        bufferMemberExtracted->location.length  = size;
                        bufferMemberExtracted->location.padding = 0;
                        bufferMemberExtracted->array.layers     = arrayLayers;
                        bufferMemberExtracted->array.stride     = arrayStride;

                        if(not memberType.member_types.empty())
                        {
                            bufferMemberExtracted->members = deriveBufferMembers(aCompiler, memberType);
                        }

                        map.insert({ name, std::move(bufferMemberExtracted) });

                        CLog::Debug(logTag(),
                                    "\n    StorageBuffer-Member : "
                                    "\n      Name:            {}"
                                    "\n      Offset:          {}"
                                    "\n      Size:            {}",
                                    name, offset, size);
                    }

                    return map;
                };

                bufferExtracted.members = deriveBufferMembers(compiler, type);

                // separate_samplers
                // separate_images

                auto possiblyDuplicateBufferOrEmpty = checkForDuplicateBoundResource(aInOutAsset.storageBuffers, bufferExtracted);
                if(possiblyDuplicateBufferOrEmpty.has_value())
                {
                    possiblyDuplicateBufferOrEmpty->get().stageBinding.set(bufferExtracted.stageBinding); // Append the additional stage binding.
                    CLog::Warning(logTag(), "Storage buffer '{}' already added. Adjusting...", bufferExtracted.name);
                }
                else
                {
                    aInOutAsset.storageBuffers.push_back(bufferExtracted);
                }
            }

            //
            // Read UBOs
            //
            for (spirv_cross::Resource const &uniformBuffer : resources.uniform_buffers)
            {
                uint32_t const location = compiler.get_decoration(uniformBuffer.id, spv::DecorationLocation);
                uint32_t const set      = compiler.get_decoration(uniformBuffer.id, spv::DecorationDescriptorSet);
                uint32_t const binding  = compiler.get_decoration(uniformBuffer.id, spv::DecorationBinding);

                updateLayoutInfoFn(set, binding);

                spirv_cross::SPIRType const &baseType   = compiler.get_type(uniformBuffer.base_type_id);
                spirv_cross::SPIRType const &type       = compiler.get_type(uniformBuffer.type_id);
                size_t                const  bufferSize = compiler.get_declared_struct_size(baseType);

                uint64_t arrayLayers = 1;
                uint64_t arrayStride = 1;
                if(not type.array.empty())
                {
                    if(type.array_size_literal[0])
                    {
                        arrayLayers = type.array[0];
                    }
                    else
                    {
                        spirv_cross::SPIRConstant const &constant = compiler.get_constant(type.array[0]);
                        arrayLayers = constant.scalar_u64();
                    }
                }
                arrayStride = (bufferSize / arrayLayers);

                SUniformBuffer uniformBufferExtracted{};
                uniformBufferExtracted.name             = uniformBuffer.name;
                uniformBufferExtracted.set              = set;
                uniformBufferExtracted.binding          = binding;
                uniformBufferExtracted.array.layers     = arrayLayers;
                uniformBufferExtracted.array.stride     = arrayStride;
                uniformBufferExtracted.location.offset  = 0;
                uniformBufferExtracted.location.length  = bufferSize * uniformBufferExtracted.array.layers;
                uniformBufferExtracted.location.padding = 0;
                uniformBufferExtracted.stageBinding.set(stageExtracted.stage);

                CLog::Debug(logTag(),
                    "\nUniformBuffer: "
                    "\n  ID:              {}"
                    "\n  Name:            {}"
                    "\n  Set:             {}"
                    "\n  Binding:         {}"
                    "\n  Buf.-Size:       {}",
                    uniformBuffer.id,
                    uniformBuffer.name,
                    set,
                    binding,
                    bufferSize);

                std::function<BufferMemberMap_t(spirv_cross::Compiler const&, spirv_cross::SPIRType const &)> deriveUniformBufferMembers = nullptr;
                deriveUniformBufferMembers = [&] (spirv_cross::Compiler const&aCompiler, spirv_cross::SPIRType const &aParent) -> BufferMemberMap_t
                    {
                        BufferMemberMap_t map {};

                        uint64_t memberCount = aParent.member_types.size();
                        for(uint64_t k=0; k<memberCount; ++k)
                        {
                            spirv_cross::SPIRType const &memberType = aCompiler.get_type(aParent.member_types[k]);
                            SHIRABE_UNUSED(memberType);

                            // Fetch basic information
                            std::string const &name   = aCompiler.get_member_name(aParent.self, k);
                            uint64_t    const  offset = aCompiler.type_struct_member_offset(aParent, k);
                            uint64_t    const  size   = aCompiler.get_declared_struct_member_size(aParent, k);
                            // spirv_cross::SPIRType const  &memberBaseType = memberType.basetype;
                            // Shared<SMaterialType  const>  localTypeExtracted = reflectType(compiler, type);

                            uint64_t arrayLayers = 1;
                            uint64_t arrayStride = 1;
                            if(not memberType.array.empty())
                            {
                                if(memberType.array_size_literal[0])
                                {
                                    arrayLayers = memberType.array[0];
                                }
                                else
                                {
                                    spirv_cross::SPIRConstant const &constant = compiler.get_constant(memberType.array[0]);
                                    arrayLayers = constant.scalar_u64();
                                }
                            }
                            arrayStride = (size / arrayLayers);

                            Shared<SBufferMember> uniformBufferMemberExtracted = makeShared<SBufferMember>();
                            uniformBufferMemberExtracted->name             = name;
                            uniformBufferMemberExtracted->location.offset  = offset;
                            uniformBufferMemberExtracted->location.length  = size;
                            uniformBufferMemberExtracted->location.padding = 0;
                            uniformBufferMemberExtracted->array.layers     = arrayLayers;
                            uniformBufferMemberExtracted->array.stride     = arrayStride;

                            if(not memberType.member_types.empty())
                            {
                                uniformBufferMemberExtracted->members = deriveUniformBufferMembers(aCompiler, memberType);
                            }

                            map.insert({ name, std::move(uniformBufferMemberExtracted) });

                            CLog::Debug(logTag(),
                                "\n    UniformBuffer-Member : "
                                "\n      Name:            {}"
                                "\n      Offset:          {}"
                                "\n      Size:            {}",
                                name, offset, size);
                        }

                        return map;
                    };

                uniformBufferExtracted.members = deriveUniformBufferMembers(compiler, type);

                // separate_samplers
                // separate_images

                auto possiblyDuplicateBufferOrEmpty = checkForDuplicateBoundResource(aInOutAsset.uniformBuffers, uniformBufferExtracted);
                if(possiblyDuplicateBufferOrEmpty.has_value())
                {
                    possiblyDuplicateBufferOrEmpty->get().stageBinding.set(uniformBufferExtracted.stageBinding); // Append the additional stage binding.
                    CLog::Warning(logTag(), "Uniform buffer '{}' already added. Adjusting...", uniformBufferExtracted.name);
                }
                else
                {
                    aInOutAsset.uniformBuffers.push_back(uniformBufferExtracted);
                }
            }

            // Read push constants
            for(spirv_cross::Resource const &pushConstant : resources.push_constant_buffers)
            {
                spirv_cross::SPIRType const &baseType   = compiler.get_type(pushConstant.base_type_id);
                spirv_cross::SPIRType const &type       = compiler.get_type(pushConstant.type_id);
                size_t                const  bufferSize = compiler.get_declared_struct_size(baseType);

                uint64_t arrayLayers = 1;
                uint64_t arrayStride = 1;
                if(not type.array.empty())
                {
                    if(type.array_size_literal[0])
                    {
                        arrayLayers = type.array[0];
                    }
                    else
                    {
                        spirv_cross::SPIRConstant const &constant = compiler.get_constant(type.array[0]);
                        arrayLayers = constant.scalar_u64();
                    }
                }
                arrayStride = (bufferSize / arrayLayers);
                SUniformBuffer uniformBufferExtracted{};
                uniformBufferExtracted.name             = pushConstant.name;
                uniformBufferExtracted.set              = std::numeric_limits<uint32_t>::max();
                uniformBufferExtracted.binding          = std::numeric_limits<uint32_t>::max();;
                uniformBufferExtracted.array.layers     = arrayLayers;
                uniformBufferExtracted.array.stride     = arrayStride;
                uniformBufferExtracted.location.offset  = 0;
                uniformBufferExtracted.location.length  = bufferSize * uniformBufferExtracted.array.layers;
                uniformBufferExtracted.location.padding = 0;
                uniformBufferExtracted.stageBinding.set(stageExtracted.stage);

                CLog::Debug(logTag(),
                    "\nUniformBuffer: "
                    "\n  ID:              {}"
                    "\n  Name:            {}"
                    "\n  Set:             {}"
                    "\n  Binding:         {}"
                    "\n  Buf.-Size:       {}",
                    pushConstant.id,
                    pushConstant.name,
                    uniformBufferExtracted.set,
                    uniformBufferExtracted.binding,
                    bufferSize);

                std::function<BufferMemberMap_t(spirv_cross::Compiler const&, spirv_cross::SPIRType const &)> deriveUniformBufferMembers = nullptr;
                deriveUniformBufferMembers = [&] (spirv_cross::Compiler const&aCompiler, spirv_cross::SPIRType const &aParent) -> BufferMemberMap_t
                    {
                        BufferMemberMap_t map {};

                        uint64_t memberCount = aParent.member_types.size();
                        for(uint64_t k=0; k<memberCount; ++k)
                        {
                            spirv_cross::SPIRType const &memberType = aCompiler.get_type(aParent.member_types[k]);
                            SHIRABE_UNUSED(memberType);

                            // Fetch basic information
                            std::string const &name   = aCompiler.get_member_name(aParent.self, k);
                            uint64_t    const  offset = aCompiler.type_struct_member_offset(aParent, k);
                            uint64_t    const  size   = aCompiler.get_declared_struct_member_size(aParent, k);
                            // spirv_cross::SPIRType const  &memberBaseType = memberType.basetype;
                            // Shared<SMaterialType  const>  localTypeExtracted = reflectType(compiler, type);

                            uint64_t arrayLayers = 1;
                            uint64_t arrayStride = 1;
                            if(not memberType.array.empty())
                            {
                                if(memberType.array_size_literal[0])
                                {
                                    arrayLayers = memberType.array[0];
                                }
                                else
                                {
                                    spirv_cross::SPIRConstant const &constant = compiler.get_constant(memberType.array[0]);
                                    arrayLayers = constant.scalar_u64();
                                }
                            }
                            arrayStride = (size / arrayLayers);

                            Shared<SBufferMember> uniformBufferMemberExtracted = makeShared<SBufferMember>();
                            uniformBufferMemberExtracted->name             = name;
                            uniformBufferMemberExtracted->location.offset  = offset;
                            uniformBufferMemberExtracted->location.length  = size;
                            uniformBufferMemberExtracted->location.padding = 0;
                            uniformBufferMemberExtracted->array.layers     = arrayLayers;
                            uniformBufferMemberExtracted->array.stride     = arrayStride;

                            if(not memberType.member_types.empty())
                            {
                                uniformBufferMemberExtracted->members = deriveUniformBufferMembers(aCompiler, memberType);
                            }

                            map.insert({ name, std::move(uniformBufferMemberExtracted) });

                            CLog::Debug(logTag(),
                                "\n    PushConstant-Member : "
                                "\n      Name:            {}"
                                "\n      Offset:          {}"
                                "\n      Size:            {}",
                                name, offset, size);
                        }

                        return map;
                    };

                uniformBufferExtracted.members = deriveUniformBufferMembers(compiler, type);

                // separate_samplers
                // separate_images

                auto possiblyDuplicateBufferOrEmpty = checkForDuplicateBoundResource(aInOutAsset.pushConstantRanges, uniformBufferExtracted);
                if(possiblyDuplicateBufferOrEmpty.has_value())
                {
                    possiblyDuplicateBufferOrEmpty->get().stageBinding.set(uniformBufferExtracted.stageBinding); // Append the additional stage binding.
                    CLog::Warning(logTag(), "Uniform buffer '{}' already added. Adjusting...", uniformBufferExtracted.name);
                }
                else
                {
                    aInOutAsset.pushConstantRanges.push_back(uniformBufferExtracted);
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

                auto possiblyDuplicateImageOrEmpty = checkForDuplicateBoundResource(aInOutAsset.sampledImages, image);
                if(possiblyDuplicateImageOrEmpty.has_value())
                {
                    possiblyDuplicateImageOrEmpty->get().stageBinding.set(image.stageBinding); // Append the additional stage binding.
                    CLog::Warning(logTag(), "Sampled image '{}' already added. Adjusting...", image.name);
                }
                else
                {
                    aInOutAsset.sampledImages.push_back(image);
                }

                CLog::Debug(logTag(),
                    "\nSampledImage: "
                    "\n  ID:              {}"
                    "\n  Name:            {}"
                    "\n  Set:             {}"
                    "\n  Binding:         {}",
                    sampledImage.id,
                    sampledImage.name,
                    set,
                    binding);
            }

            std::vector<SAnnotation> const annotations = parseAnnotations(aElement);
            applyAnnotations(annotations, aInOutAsset);

            aInOutAsset.stages[aElement.stage] = stageExtracted;

            result |= EnumValueOf(EResult::Success);
        };

        std::for_each(aUnit.elements.begin(), aUnit.elements.end(), reflect);

        aInOutAsset.layoutInfo = layoutInfo;

        return { ( EResult::Success == static_cast<EResult>(result) ) };
    }
    //<-----------------------------------------------------------------------------
}
