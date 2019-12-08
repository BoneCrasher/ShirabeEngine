#include "material/declaration.h"
#include "material/serialization.h"
#include <util/documents/json.h>

namespace engine::material
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    std::unordered_map<VkPipelineStageFlagBits, SMaterialIndexStage> const SMaterialMasterIndex::sEmptyMasterMap =
    {
        { VkPipelineStageFlagBits::VK_PIPELINE_STAGE_VERTEX_SHADER_BIT                  , {} },
        { VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT    , {} },
        { VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT , {} },
        { VkPipelineStageFlagBits::VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT                , {} },
        { VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT                , {} },
        { VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT                 , {} }
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------

    std::unordered_map<VkPipelineStageFlagBits, SMaterialMetaStage> const SMaterialMeta::sEmptyMetaMap =
    {
            { VkPipelineStageFlagBits::VK_PIPELINE_STAGE_VERTEX_SHADER_BIT                  , {} },
            { VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT    , {} },
            { VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT , {} },
            { VkPipelineStageFlagBits::VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT                , {} },
            { VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT                , {} },
            { VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT                 , {} }
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    bool SMaterialMasterIndex::acceptSerializer(documents::IJSONSerializer<SMaterialMasterIndex> &aSerializer) const
    {
        aSerializer.beginObject(name);

        aSerializer.writeValue("uid",                       uid);
        aSerializer.writeValue("name",                      name);
        aSerializer.beginObject("stages");

        for(auto const &[stage, stageFileReferences] : stages)
        {
            std::string const stageName = serialization::stageToString(stage);
            aSerializer.beginObject(stageName);
            aSerializer.writeValue("glslSourceFilename", stageFileReferences.glslSourceFilename);
            aSerializer.endObject();
        }

        aSerializer.endObject();

        aSerializer.endObject();

        return true;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    bool SMaterialMasterIndex::acceptDeserializer(documents::IJSONDeserializer<SMaterialMasterIndex> &aDeserializer)
    {
        aDeserializer.readValue("uid", uid);
        aDeserializer.readValue("name",name);

        aDeserializer.beginObject("stages");

        std::vector<VkPipelineStageFlagBits> stageList = { VkPipelineStageFlagBits::VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
                                                           , VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT
                                                           , VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT
                                                           , VkPipelineStageFlagBits::VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT
                                                           , VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
                                                           , VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
                                                         };

        for(auto const stage : stageList)
        {
            std::string const stageName         = serialization::stageToString(stage);
            std::string       glslFilename      = std::string();
            asset::AssetId_t  spvModuleAssetId  = 0;

            bool const couldBeginObject = aDeserializer.beginObject(stageName);
            if(not couldBeginObject)
            {
                stages[stage] = SMaterialIndexStage { "" };
                continue;
            }

            aDeserializer.readValue("glslSourceFilename", glslFilename);
            aDeserializer.endObject();

            stages[stage] = SMaterialIndexStage { glslFilename };
        }

        aDeserializer.endObject();

        return true;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    bool SMaterialMeta::acceptSerializer(documents::IJSONSerializer<SMaterialMeta> &aSerializer) const
    {
        aSerializer.beginObject(name);

        aSerializer.writeValue("uid",                       uid);
        aSerializer.writeValue("name",                      name);
        aSerializer.writeValue("signatureFilename",         signatureAssetUid);
        aSerializer.writeValue("baseConfigurationFilename", configurationAssetUid);
        aSerializer.beginObject("stages");

        for(auto const &[stage, stageFileReferences] : stages)
        {
            std::string const stageName = serialization::stageToString(stage);
            aSerializer.beginObject(stageName);
            aSerializer.writeValue("spvModuleFilename",  stageFileReferences.spvModuleAssetId);
            aSerializer.endObject();
        }

        aSerializer.endObject();

        aSerializer.endObject();

        return true;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    bool SMaterialMeta::acceptDeserializer(documents::IJSONDeserializer<SMaterialMeta> &aDeserializer)
    {
        aDeserializer.readValue("uid",                       uid);
        aDeserializer.readValue("name",                      name);
        aDeserializer.readValue("signatureFilename",         signatureAssetUid);
        aDeserializer.readValue("baseConfigurationFilename", configurationAssetUid);

        aDeserializer.beginObject("stages");

        std::vector<VkPipelineStageFlagBits> stageList = { VkPipelineStageFlagBits::VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
                                                           , VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT
                                                           , VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT
                                                           , VkPipelineStageFlagBits::VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT
                                                           , VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
                                                           , VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
        };

        for(auto const stage : stageList)
        {
            std::string const stageName         = serialization::stageToString(stage);
            std::string       glslFilename      = std::string();
            asset::AssetId_t  spvModuleAssetId  = 0;

            bool const couldBeginObject = aDeserializer.beginObject(stageName);
            if(not couldBeginObject)
            {
                stages[stage] = SMaterialMetaStage { 0 };
                continue;
            }

            aDeserializer.readValue("spvModuleFilename",  spvModuleAssetId);
            aDeserializer.endObject();

            stages[stage] = SMaterialMetaStage { spvModuleAssetId };
        }

        aDeserializer.endObject();

        return true;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    bool SMaterialInstanceIndex::acceptSerializer(documents::IJSONSerializer<SMaterialInstanceIndex> &aSerializer) const
    {
        aSerializer.beginObject(name);

        aSerializer.writeValue("uid",                   uid);
        aSerializer.writeValue("name",                  name);
        aSerializer.writeValue("masterIndexFilename",   masterIndexAssetId);
        aSerializer.writeValue("configurationFilename", configurationAssetId);

        aSerializer.endObject();

        return true;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    bool SMaterialInstanceIndex::acceptDeserializer(documents::IJSONDeserializer<SMaterialInstanceIndex> &aDeserializer)
    {
        aDeserializer.readValue("uid",  uid);
        aDeserializer.readValue("name", name);

        aDeserializer.readValue("masterIndexFilename",   masterIndexAssetId);
        aDeserializer.readValue("configurationFilename", configurationAssetId);

        return true;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    bool SMaterialSignature::acceptSerializer(documents::IJSONSerializer<SMaterialSignature> &aSerializer) const
    {
        std::function<void(Shared<SMaterialType const> const &, bool)> writeType = nullptr;
        writeType = [&] (Shared<SMaterialType const> const &aType, bool aRecursive) -> void
        {
            std::function<void(Shared<SMaterialType const> const &)> writeMemberTree = nullptr;
            writeMemberTree = [&] (Shared<SMaterialType const> const &aMemberType) -> void
            {
                aSerializer.beginArray("members");
                for(auto const &[name, type] : aMemberType->members)
                {
                    writeType(type, aRecursive);
                }
                aSerializer.endArray();
            };

            aSerializer.beginObject("type");
            aSerializer.writeValue("name",               aType->name);
            aSerializer.writeValue("byteSize",           aType->byteSize);
            aSerializer.writeValue("vectorSize",         aType->vectorSize);
            aSerializer.writeValue("matrixRows",         aType->matrixRows);
            aSerializer.writeValue("matrixColumns",      aType->matrixColumns);
            aSerializer.writeValue("matrixColumnStride", aType->matrixColumnStride);
            aSerializer.writeValue("arraySize",          aType->arraySize);
            aSerializer.writeValue("arrayStride",        aType->arrayStride);

            if(aRecursive)
            {
                writeMemberTree(aType);
            }
            aSerializer.endObject();
        };

        aSerializer.beginObject(name);

        //
        // Write layout info
        //
        aSerializer.beginObject("layoutInfo");
        aSerializer.writeValue ("setCount", layoutInfo.setCount);
        aSerializer.beginObject("setBindingCounts");
        for(uint64_t k=0; k<layoutInfo.setBindingCount.size(); ++k)
        {
            aSerializer.writeValue(CString::format("{}", k), layoutInfo.setBindingCount[k]);
        }
        aSerializer.endObject(); // setBindingCounts
        aSerializer.endObject(); // layoutInfo

        //
        // Serialize subpass input
        //
        aSerializer.beginArray("subpassInputs");
        auto const iterateSubpassInputs = [&] (SSubpassInput const &aSubpassInput) -> void
        {
            aSerializer.beginObject(aSubpassInput.name);
            aSerializer.writeValue("name",            aSubpassInput.name);
            aSerializer.writeValue("attachmentIndex", aSubpassInput.attachmentIndex);
            aSerializer.writeValue("set",             aSubpassInput.set);
            aSerializer.writeValue("binding",         aSubpassInput.binding);
            aSerializer.endObject(); // aSubpassInput.name
        };
        std::for_each(this->subpassInputs.begin(), this->subpassInputs.end(), iterateSubpassInputs);
        aSerializer.endArray(); // subpassInputs

        //
        // Serialize uniform buffers
        //
        aSerializer.beginArray("uniformBuffers");
        auto const iterateUniformBuffers = [&] (SUniformBuffer const &aBuffer) -> void
        {
            aSerializer.beginObject(aBuffer.name);
            aSerializer.writeValue("name",        aBuffer.name);
            aSerializer.writeValue("offset",      aBuffer.location.offset);
            aSerializer.writeValue("size",        aBuffer.location.length);
            aSerializer.writeValue("padding",     aBuffer.location.padding);
            aSerializer.writeValue("set",         aBuffer.set);
            aSerializer.writeValue("binding",     aBuffer.binding);

            aSerializer.beginObject("array");
            aSerializer.writeValue("layers", aBuffer.array.layers);
            aSerializer.writeValue("stride", aBuffer.array.stride);
            aSerializer.endObject();

            std::function<void(BufferMemberMap_t const &)> iterateUniformBufferMembers = nullptr;
            iterateUniformBufferMembers = [&] (BufferMemberMap_t const &aMembers) -> void
            {
                aSerializer.beginArray("members");
                auto const iterate = [&] (BufferMemberMap_t::value_type const &aMember)
                {
                    std::string                 const  aKey   = aMember.first;
                    Shared<SBufferMember const> const &aValue = aMember.second;

                    aSerializer.beginObject(aKey);
                    aSerializer.writeValue("name",        aKey);
                    aSerializer.writeValue("offset",      aValue->location.offset);
                    aSerializer.writeValue("size",        aValue->location.length);
                    aSerializer.writeValue("padding",     aValue->location.padding);

                    aSerializer.beginObject("array");
                    aSerializer.writeValue("layers", aValue->array.layers);
                    aSerializer.writeValue("stride", aValue->array.stride);
                    aSerializer.endObject();

                    if(not aValue->members.empty())
                    {
                        iterateUniformBufferMembers(aValue->members);
                    }

                    aSerializer.endObject(); // aKey
                };
                std::for_each(aMembers.begin(), aMembers.end(), iterate);
                aSerializer.endArray(); // members
            };

            //if(not aBuffer.members.empty())
            //{
                iterateUniformBufferMembers(aBuffer.members);
            //}

            aSerializer.endObject(); // aBuffer.Name
        };
        std::for_each(this->uniformBuffers.begin(), this->uniformBuffers.end(), iterateUniformBuffers);
        aSerializer.endArray(); // uniformBuffers

        //
        // Serialize sampled images
        //
        aSerializer.beginArray("sampledImages");
        auto const iterateSampledImages = [&] (SSampledImage const &aSampledImage) -> void
        {
            aSerializer.beginObject(aSampledImage.name);
            aSerializer.writeValue("name",    aSampledImage.name);
            aSerializer.writeValue("set",     aSampledImage.set);
            aSerializer.writeValue("binding", aSampledImage.binding);
            aSerializer.endObject(); // aSampledImage.name
        };
        std::for_each(this->sampledImages.begin(), this->sampledImages.end(), iterateSampledImages);
        aSerializer.endArray(); // sampledImages

        //
        // Loop through stages
        //
        aSerializer.beginArray("stages");
        std::function<void(std::pair<VkPipelineStageFlagBits const, SMaterialStage> const &)> iterateStages = nullptr;
        iterateStages = [&] (std::pair<VkPipelineStageFlagBits const, SMaterialStage> const &aStage) -> void
        {


            VkPipelineStageFlagBits const  key   = aStage.first;
            SMaterialStage          const &stage = aStage.second;

            SHIRABE_UNUSED(key);

            aSerializer.beginObject(stage.stageName);
            aSerializer.writeValue("name",     stage.stageName);
            aSerializer.writeValue("filename", stage.filename);

            aSerializer.beginArray("inputs");
            auto const iterateInputs = [&] (SStageInput const &aInput) -> void
            {
               aSerializer.beginObject(aInput.name);
               aSerializer.writeValue("name",     aInput.name);
               aSerializer.writeValue("location", aInput.location);

               writeType(aInput.type, true);

               aSerializer.endObject();
            };
            std::for_each(stage.inputs.begin(), stage.inputs.end(), iterateInputs);
            aSerializer.endArray();

            aSerializer.beginArray("outputs");
            auto const iterateOutputs = [&] (SStageOutput const &aOutput) -> void
            {
                aSerializer.beginObject(aOutput.name);
                aSerializer.writeValue("name",     aOutput.name);
                aSerializer.writeValue("location", aOutput.location);

                writeType(aOutput.type, true);

                aSerializer.endObject();
            };
            std::for_each(stage.outputs.begin(), stage.outputs.end(), iterateOutputs);
            aSerializer.endArray();
            aSerializer.endObject();
        };
        std::for_each(this->stages.cbegin(), this->stages.cend(), iterateStages);

        aSerializer.endArray();
        aSerializer.endObject();

        return true;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------

    template <template <typename> typename TContainer, typename T>
    void convertToConstSmartPtrMap(
              std::unordered_map<std::string, TContainer<T>>       &aInput
            , std::unordered_map<std::string, TContainer<T const>> &aOutput)
    {
        auto const predicate = [] (typename std::unordered_map<std::string, TContainer<T>>::value_type &aMember)
                -> typename std::unordered_map<std::string, TContainer<T const>>::value_type
        {
            return { aMember.first, std::const_pointer_cast<T const>(aMember.second) };
        };
        std::transform(aInput.begin(), aInput.end(), getMapEmplacer(aOutput), predicate);
    }

    /**
     * @brief acceptDeserializer
     * @param aSerializer
     * @return
     */
    bool SMaterialSignature::acceptDeserializer(documents::IJSONDeserializer<SMaterialSignature> &aDeserializer)
    {
        std::function<void(Shared<SMaterialType> &, bool)> readType = nullptr;
        readType = [&] (Shared<SMaterialType> &aType, bool aRecursive) -> void
        {
            std::function<void(MutableMaterialTypeMap_t &)> readMembers = nullptr;
            readMembers = [&] (MutableMaterialTypeMap_t &aMembers) -> void
            {
                MutableMaterialTypeMap_t map {};

                uint32_t count = 0;
                aDeserializer.beginArray("members", count);
                for(uint32_t k=0; k<count; ++k)
                {
                    Shared<SMaterialType> type = makeShared<SMaterialType>();
                    aDeserializer.beginObject(k);
                    readType(type, aRecursive);
                    aDeserializer.endObject();
                    map.insert({ type->name, std::move(type) });
                }
                aDeserializer.endArray();

                aMembers = map;
            };

            aDeserializer.readValue("name",               aType->name);
            aDeserializer.readValue("byteSize",           aType->byteSize);
            aDeserializer.readValue("vectorSize",         aType->vectorSize);
            aDeserializer.readValue("matrixRows",         aType->matrixRows);
            aDeserializer.readValue("matrixColumns",      aType->matrixColumns);
            aDeserializer.readValue("matrixColumnStride", aType->matrixColumnStride);
            aDeserializer.readValue("arraySize",          aType->arraySize);
            aDeserializer.readValue("arrayStride",        aType->arrayStride);

            if(aRecursive)
            {
                MutableMaterialTypeMap_t mutableMembers{};
                readMembers(mutableMembers);
                convertToConstSmartPtrMap<Shared, SMaterialType>(mutableMembers, aType->members);
            }
        };

        //
        // Read layout info
        //
        aDeserializer.beginObject("layoutInfo");
        aDeserializer.readValue  ("setCount", layoutInfo.setCount);
        aDeserializer.beginObject("setBindingCounts");

        layoutInfo.setBindingCount.resize(layoutInfo.setCount);
        for(uint32_t k=0; k<layoutInfo.setCount; ++k)
        {
            aDeserializer.readValue(CString::format("{}", k), *(layoutInfo.setBindingCount.data() + k));
        }

        aDeserializer.endObject();
        aDeserializer.endObject();

        //
        // Serialize subpass input
        //
        uint32_t subpassInputCount = 0;
        aDeserializer.beginArray("subpassInputs", subpassInputCount);
        subpassInputs.resize(subpassInputCount);

        auto const iterateSubpassInputs = [&] (SSubpassInput &aSubpassInput, uint32_t const &aIndex) -> void
        {
            aDeserializer.beginObject(aIndex);
            aDeserializer.readValue("name",            aSubpassInput.name);
            aDeserializer.readValue("attachmentIndex", aSubpassInput.attachmentIndex);
            aDeserializer.readValue("set",             aSubpassInput.set);
            aDeserializer.readValue("binding",         aSubpassInput.binding);
            aDeserializer.endObject();
        };

        for(uint32_t k=0; k<subpassInputCount; ++k)
        {
            iterateSubpassInputs(*(subpassInputs.data() + k), k);
        }

        aDeserializer.endArray();

        //
        // Serialize uniform buffers
        //
        uint32_t uniformBufferCount = 0;
        aDeserializer.beginArray("uniformBuffers", uniformBufferCount);
        uniformBuffers.resize(uniformBufferCount);

        auto const iterateUniformBuffers = [&] (SUniformBuffer &aBuffer, uint32_t const &aIndex) -> void
        {
            aDeserializer.beginObject(aIndex);
            aDeserializer.readValue("name",        aBuffer.name);
            aDeserializer.readValue("offset",      aBuffer.location.offset);
            aDeserializer.readValue("size",        aBuffer.location.length);
            aDeserializer.readValue("padding",     aBuffer.location.padding);
            aDeserializer.readValue("set",         aBuffer.set);
            aDeserializer.readValue("binding",     aBuffer.binding);

            aDeserializer.beginObject("array");
            aDeserializer.readValue("layers", aBuffer.array.layers);
            aDeserializer.readValue("stride", aBuffer.array.stride);
            aDeserializer.endObject();

            std::function<void(MutableBufferMemberMap_t &)> readMembers = nullptr;
            readMembers = [&] (MutableBufferMemberMap_t &aOutMembers) -> void {
                auto const iterate = [&](Shared<SBufferMember> const &aMember,
                                         uint32_t              const &aIndex) -> void
                {
                    aDeserializer.beginObject(aIndex);
                    aDeserializer.readValue("name",        aMember->name);
                    aDeserializer.readValue("offset",      aMember->location.offset);
                    aDeserializer.readValue("size",        aMember->location.length);
                    aDeserializer.readValue("padding",     aMember->location.padding);

                    aDeserializer.beginObject("array");
                    aDeserializer.readValue("layers", aMember->array.layers);
                    aDeserializer.readValue("stride", aMember->array.stride);
                    aDeserializer.endObject();

                    aDeserializer.endObject();
                };

                MutableBufferMemberMap_t resultMap {};
                uint32_t size = 0;
                aDeserializer.beginArray("members", size);
                for (uint32_t k = 0; k < size; ++k) {
                    Shared<SBufferMember> member = makeShared<SBufferMember>();
                    iterate(member, k);
                    resultMap.insert({ member->name, std::move(member) });
                }
                aDeserializer.endArray();

                aOutMembers = resultMap;
            };

            MutableBufferMemberMap_t mutableMembers {};
            readMembers(mutableMembers);
            aBuffer.members = mutableMembers;
            //convertToConstSmartPtrMap<Shared, SBufferMember>(mutableMembers, aBuffer.members);

            //mutableMembers.clear();

            aDeserializer.endObject();
        };

        for(uint32_t k=0; k<uniformBufferCount; ++k)
        {
            iterateUniformBuffers(*(uniformBuffers.data() + k), k);
        }

        aDeserializer.endArray();

        //
        // Serialize sampled images
        //
        uint32_t sampledImageCount = 0;
        aDeserializer.beginArray("sampledImages", sampledImageCount);
        sampledImages.resize(sampledImageCount);

        auto const iterateSampledImages = [&] (SSampledImage &aSampledImage, uint32_t const &aIndex) -> void
        {
            aDeserializer.beginObject(aIndex);
            aDeserializer.readValue("name",    aSampledImage.name);
            aDeserializer.readValue("set",     aSampledImage.set);
            aDeserializer.readValue("binding", aSampledImage.binding);
            aDeserializer.endObject();
        };

        for(uint32_t k=0; k<sampledImageCount; ++k)
        {
            iterateSampledImages(*(sampledImages.data() + k), k);
        }

        aDeserializer.endArray();

        //
        // Loop through stages
        //
        uint32_t stageCount = 0;
        aDeserializer.beginArray("stages", stageCount);

        auto const iterateStages = [&] (SMaterialStage &aStage, uint32_t const &aIndex) -> void
        {


            aDeserializer.beginObject(aIndex);
            aDeserializer.readValue("name",     aStage.stageName);
            aDeserializer.readValue("filename", aStage.filename);
            aStage.stage = serialization::stageFromString(aStage.stageName);

            uint32_t inputCount = 0;
            aDeserializer.beginArray("inputs", inputCount);
            aStage.inputs.resize(inputCount);

            auto const iterateInputs = [&] (SStageInput &aInput, uint32_t const &aIndex) -> void
            {
               aDeserializer.beginObject(aIndex);
               aDeserializer.readValue("name",     aInput.name);
               aDeserializer.readValue("location", aInput.location);

               Shared<SMaterialType> processed = makeShared<SMaterialType>();
               aDeserializer.beginObject("type");
               readType(processed, true);
               aDeserializer.endObject();

               aInput.type = std::const_pointer_cast<SMaterialType const>(processed);

               aDeserializer.endObject();
            };

            for(uint32_t k=0; k<inputCount; ++k)
            {
                iterateInputs(*(aStage.inputs.data() + k), k);
            }

            aDeserializer.endArray();

            uint32_t outputCount = 0;
            aDeserializer.beginArray("outputs", outputCount);
            aStage.outputs.resize(outputCount);

            auto const iterateOutputs = [&] (SStageOutput &aOutput, uint32_t const &aIndex) -> void
            {
                aDeserializer.beginObject(aIndex);
                aDeserializer.readValue("name",     aOutput.name);
                aDeserializer.readValue("location", aOutput.location);

                Shared<SMaterialType> processed = makeShared<SMaterialType>();
                aDeserializer.beginObject("type");
                readType(processed, true);
                aDeserializer.endObject();

                aOutput.type = std::const_pointer_cast<SMaterialType const>(processed);

                aDeserializer.endObject();
            };

            for(uint32_t k=0; k<outputCount; ++k)
            {
                iterateOutputs(*(aStage.outputs.data() + k), k);
            }

            aDeserializer.endArray();
            aDeserializer.endObject();
        };

        for(uint32_t k=0; k<stageCount; ++k)
        {
            SMaterialStage stage{};

            iterateStages(stage, k);
            stages[stage.stage] = std::move(stage);
        }

        aDeserializer.endArray();
        aDeserializer.endObject();

        return true;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    EEngineStatus CMaterialInstance::createConfiguration(bool aIncludeSystemBuffers)
    {
        if(mConfiguration.has_value())
        {
            return EEngineStatus::Ok;
        }

        Shared<CMaterialMaster>   m         = master();
        SMaterialSignature const &signature = m->signature();

        CMaterialConfig config = CMaterialConfig::fromMaterialDesc(signature, aIncludeSystemBuffers);

        mConfiguration = std::move(config);

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CMaterialConfig CMaterialConfig::fromMaterialDesc(SMaterialSignature const &aMaterial, bool aIncludeSystemBuffers)
    {
        uint32_t const minUBOOffsetAlignment = 256; // Hardcoded for the platform... make accessible in any other way...
        std::size_t alignment = minUBOOffsetAlignment; // 16 * sizeof(float);
        // if (0 < minUBOOffsetAlignment) {
        //     alignment = (alignment + minUBOOffsetAlignment - 1) & ~(minUBOOffsetAlignment - 1);
        // }

        auto const nextMultiple = [] (uint32_t const &aInput, uint32_t const &aAlignment) -> uint32_t
        {
            return (aInput + aAlignment - 1) & -aAlignment;
        };

        std::function<void(CMaterialConfig::BufferValueIndex_t &
                         , Shared<SBufferMember> &
                         , std::string const &
                         , uint64_t const &
                         , uint64_t const &)> iterate = nullptr;

        iterate = [&iterate, &nextMultiple, &alignment] (
                              CMaterialConfig::BufferValueIndex_t       &aIndex
                            , Shared<SBufferMember>                     &aMember
                            , std::string                         const &aParentPath
                            , uint64_t                            const &aOffsetBackshift
                            , uint64_t                            const &aCurrentBaseOffset)
        {
            std::string const path = fmt::format("{}.{}", aParentPath, aMember->name);
            aIndex.insert({path, aMember});

            uint64_t const arrayLayers = std::max(1lu, aMember->array.layers); // Ensure at least one iteration with std::max...
            for(std::size_t k=0; k<arrayLayers; ++k)
            {
                Shared<SBufferMember> member = makeShared<SBufferMember>(*aMember);

                member->location.offset  = (aCurrentBaseOffset + member->location.offset + (k * member->array.stride));
                member->location.length  = member->location.length; // nextMultiple(aMember->location.length, alignment);

                std::string prefixPath = path;
                if(1 < arrayLayers)
                {
                    prefixPath = fmt::format("{}[{}]", path, k);
                }
                aIndex.insert({prefixPath, member});

                for(auto &[n, v] : member->members)
                {
                    iterate(aIndex, v, prefixPath, aOffsetBackshift, member->location.offset);
                }
            }
        };

        CMaterialConfig config {};

        if(aMaterial.uniformBuffers.empty())
        {
            return {}; // Nothing to do...
        }

        std::vector<SUniformBuffer> sorted(aMaterial.uniformBuffers);

        //
        // Filter out all non-user-set indexed buffers, so that they won't have any influence on the buffer size calculation.
        //
        bool const processSystemUBOs = aIncludeSystemBuffers;
        if(not processSystemUBOs)
        {
            static constexpr uint8_t sFirstPermittedUserSetIndex = 2;

            auto const filter = [](SUniformBuffer const &aBuffer) -> bool
            {
                return (sFirstPermittedUserSetIndex > aBuffer.set);
            };

            sorted.erase(
                    std::remove_if(sorted.begin(), sorted.end(), filter)
                    , sorted.end());
        }

        //
        // Sort by set, then binding, so that we properly calculate the buffer sizes and offsets...
        //
        auto const sort = [] (SUniformBuffer const &aLhs, SUniformBuffer const &aRhs) -> bool
        {
            return (aLhs.set < aRhs.set)                                      // First by set
                    || (aLhs.set == aRhs.set && aLhs.binding < aRhs.binding); // Then by binding;
        };
        std::sort(sorted.begin(), sorted.end(), sort);

        //
        // Due to previous sorting and optional filtering by set and index, we can simply check the offset of the first buffer and subtract it from all the other offsets.
        //
        uint64_t const baseBackShift = sorted[0].location.offset;
        for(auto &buffer : sorted)
        {
            CMaterialConfig::BufferValueIndex_t bufferValueIndex {};

            Shared<SBufferMember> member = makeShared<SBufferMember>();
            member->name             = buffer.name;
            member->location.offset  = buffer.location.offset;
            member->location.length  = buffer.location.length; // nextMultiple(buffer.location.length, alignment);
            member->location.padding = buffer.location.padding;
            member->array            = buffer.array;
            member->members          = buffer.members;

            std::string const path = fmt::format("{}", buffer.name);
            bufferValueIndex.insert({path, member});

            uint64_t const arrayLayers = std::max(1lu, buffer.array.layers); // Ensure at least one iteration with std::max...
            for(std::size_t k=0; k<arrayLayers; ++k)
            {
                buffer.location.offset = (buffer.location.offset + (k * buffer.array.stride));

                Shared<SBufferMember> arrayMember = makeShared<SBufferMember>();
                arrayMember->name             = buffer.name;
                arrayMember->location.offset  = buffer.location.offset;
                arrayMember->location.length  = buffer.location.length; // nextMultiple(buffer.location.length, alignment);
                arrayMember->location.padding = buffer.location.padding;
                arrayMember->array.layers     = 1;
                arrayMember->array.stride     = buffer.array.stride;
                arrayMember->members          = buffer.members;

                std::string arrayPath = buffer.name;
                if(1 < arrayLayers)
                {
                    arrayPath = fmt::format("{}[{}]", buffer.name, k);
                }
                bufferValueIndex.insert({arrayPath, arrayMember});

                for(auto &[n, v] : buffer.members)
                {
                    iterate(bufferValueIndex, v, arrayPath, 0lu, buffer.location.offset);
                }
            }

            config.mBufferIndex.insert({ buffer.name, bufferValueIndex });

            int8_t *alignedData = (int8_t *)aligned_alloc(alignment, member->location.length);
            memset(alignedData, 0, member->location.length);
            config.mData.insert({ buffer.name, Shared<void>(alignedData) });
        }

        // config.mData.resize(totalSize);
        // config.mData.assign(totalSize, 0);

        return config;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    bool checkIsZeroArray(uint8_t const *aValue, SBufferLocation const &aLocation)
    {
        uint8_t chk = 0;
        for(uint64_t k=0; k<aLocation.length; ++k)
        {
            chk |= *(aValue + k);
        }

        return (0 == chk);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CEngineResult<void const *const> CMaterialConfig::getBuffer(std::string const &aBufferName) const
    {
        bool const has = hasBuffer(aBufferName);
        if(not has)
        {
            return CEngineResult<void const *const>(EEngineStatus::Error, nullptr);
        }

        Shared<void> const alignedData = mData.at(aBufferName);

        // SBufferData     const &buffer   = mBufferIndex.at(aBufferName);
        // SBufferLocation const &location = buffer.getLocation();
//
        // void const *const data = (mData.data() + location.offset);

        return { EEngineStatus::Ok, alignedData.get() };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CEngineResult<void *const> CMaterialConfig::getBuffer(std::string const &aBufferName)
    {
        bool const has = hasBuffer(aBufferName);
        if(not has)
        {
            return CEngineResult<void *const>(EEngineStatus::Error, nullptr);
        }

        Shared<void> alignedData = mData.at(aBufferName);

        // SBufferData     const &buffer   = mBufferIndex.at(aBufferName);
        // SBufferLocation const &location = buffer.getLocation();

        // void *const data = (mData.data() + location.offset);

        return { EEngineStatus::Ok, alignedData.get() };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    bool CMaterialConfig::acceptSerializer(documents::IJSONSerializer<CMaterialConfig> &aSerializer) const
    {
        aSerializer.beginObject("");

        // aSerializer.writeValue("uniformBufferData", mData);

        // aSerializer.beginArray("uniformBuffers");
        // auto const iterateUniformBuffers = [&] (BufferValueIndex_t::value_type const &aBuffer) -> void
        // {
        //     std::string           const &name = aBuffer.first;
        //     Shared<SBufferMember> const &data = aBuffer.second;
//
        //     SBufferLocation const &location = data->location;
        //     SBufferArray    const &array    = data->array;
//
        //     aSerializer.beginObject(name);
        //     aSerializer.writeValue("name",        name);
        //     aSerializer.writeValue("offset",      location.offset);
        //     aSerializer.writeValue("size",        location.length);
        //     aSerializer.writeValue("padding",     location.padding);
        //     aSerializer.writeValue("arraySize",   array.layers);
        //     aSerializer.writeValue("arrayStride", array.stride);
//
        //     aSerializer.beginArray("members");
        //     auto const iterate = [&] (BufferValueIndex_t::value_type const &aMember)
        //     {
        //         std::string           const  memberName = aMember.first;
        //         Shared<SBufferMember> const &memberData = aMember.second;
//
        //         SBufferLocation const &memberLocation = memberData->location;
        //         SBufferArray    const &memberArray    = memberData->array;
//
        //         aSerializer.beginObject(name);
        //         aSerializer.writeValue("name",        memberName);
        //         aSerializer.writeValue("offset",      memberLocation.offset);
        //         aSerializer.writeValue("size",        memberLocation.length);
        //         aSerializer.writeValue("padding",     memberLocation.padding);
        //         aSerializer.writeValue("arraySize",   memberArray.layers);
        //         aSerializer.writeValue("arrayStride", memberArray.stride);
        //         aSerializer.endObject();
        //     };
        //     std::for_each(data->members.begin(), data->members.end(), iterate);
        //     aSerializer.endArray();
//
        //     aSerializer.endObject();
        // };
        // std::for_each(mBufferIndex.begin(), mBufferIndex.end(), iterateUniformBuffers);
        // aSerializer.endArray();

        aSerializer.endObject();

        return true;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    bool CMaterialConfig::acceptDeserializer(documents::IJSONDeserializer<CMaterialConfig> &aDeserializer)
    {
        // aDeserializer.readValue("uniformBufferData", mData);

        uint32_t bufferCount = 0;
        aDeserializer.beginArray("uniformBuffers", bufferCount);
        //for(uint32_t k=0; k<bufferCount; ++k)
        //{
        //    SBufferData buffer = {};
//
        //    std::string     name     = {};
//
        //    aDeserializer.beginObject(k);
//
        //    aDeserializer.readValue("name",    name);
        //    aDeserializer.readValue("offset",  buffer.mLocation.offset);
        //    aDeserializer.readValue("size",    buffer.mLocation.length);
        //    aDeserializer.readValue("padding", buffer.mLocation.padding);
//
        //    uint32_t memberCount = 0;
        //    aDeserializer.beginArray("members", memberCount);
        //    for(uint32_t l=0; l<memberCount; ++l)
        //    {
        //        std::string     memberName ={};
        //        SBufferLocation location   ={};
//
        //        aDeserializer.beginObject(l);
//
        //        aDeserializer.readValue("name",    memberName);
        //        aDeserializer.readValue("offset",  location.offset);
        //        aDeserializer.readValue("size",    location.length);
        //        aDeserializer.readValue("padding", location.padding);
//
        //        aDeserializer.endObject();
//
        //        buffer.mValueIndex[memberName] = location;
        //    }
        //    aDeserializer.endArray();
//
        //    aDeserializer.endObject();
//
        //    mBufferIndex[name] = buffer;
        //}
        aDeserializer.endArray();

        return true;
    }
    //<-----------------------------------------------------------------------------
}
