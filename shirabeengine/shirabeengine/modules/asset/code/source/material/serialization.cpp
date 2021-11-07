#include "asset/material/source.h"
#include "asset/material/asset.h"
#include "asset/material/serialization.h"

namespace engine
{
    namespace material
    {
        using namespace engine::documents;
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        VkPipelineStageFlagBits stageFromString(std::string const &aString)
        {
            if("vertex"             == aString) return VkPipelineStageFlagBits::VK_PIPELINE_STAGE_VERTEX_SHADER_BIT                 ;
            if("tess_control_point" == aString) return VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT   ;
            if("tess_evaluation"    == aString) return VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
            if("geometry"           == aString) return VkPipelineStageFlagBits::VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT               ;
            if("fragment"           == aString) return VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT               ;
            if("compute"            == aString) return VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT                ;

            return VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        std::string const stageToString(VkPipelineStageFlagBits const &aStage)
        {
            switch(aStage)
            {
            case VkPipelineStageFlagBits::VK_PIPELINE_STAGE_VERTEX_SHADER_BIT                 : return "vertex";
            case VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT   : return "tess_control_point";
            case VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT: return "tess_evaluation";
            case VkPipelineStageFlagBits::VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT               : return "geometry";
            case VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT               : return "fragment";
            case VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT                : return "compute";
            default:                                                                            return "unknown";
            }
        }
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
                std::string const stageName = stageToString(stage);
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
                std::string const stageName         = stageToString(stage);
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
        bool SMaterialAsset::acceptSerializer(documents::IJSONSerializer<SMaterialAsset> &aSerializer) const
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
                {
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
                }
                aSerializer.endObject();
            };

            aSerializer.beginObject(name);
            {
                aSerializer.writeValue("uid",                       uid);
                aSerializer.writeValue("name",                      name);
                aSerializer.writeValue("signatureFilename",         signatureAssetUid);
                aSerializer.writeValue("baseConfigurationFilename", configurationAssetUid);

                aSerializer.beginObject("layoutInfo");
                {
                    aSerializer.writeValue ("setCount", layoutInfo.setCount);

                    aSerializer.beginObject("setBindingCounts");
                    for(uint64_t k=0; k<layoutInfo.setBindingCount.size(); ++k)
                    {
                        aSerializer.writeValue(CString::format("{}", k), layoutInfo.setBindingCount[k]);
                    }
                    aSerializer.endObject(); // setBindingCounts
                }
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
                // Serialize buffers
                //
                auto const iterateBuffers = [&](SUniformBuffer const &aBuffer) -> void
                    {
                        aSerializer.beginObject(aBuffer.name);
                        {
                            aSerializer.writeValue("name",         aBuffer.name);
                            aSerializer.writeValue("offset",       aBuffer.location.offset);
                            aSerializer.writeValue("size",         aBuffer.location.length);
                            aSerializer.writeValue("padding",      aBuffer.location.padding);
                            aSerializer.writeValue("set",          aBuffer.set);
                            aSerializer.writeValue("binding",      aBuffer.binding);
                            aSerializer.writeValue("sharing_mode", sharingModeToString(aBuffer.sharingMode));

                            aSerializer.beginObject("array");
                            aSerializer.writeValue("layers", aBuffer.array.layers);
                            aSerializer.writeValue("stride", aBuffer.array.stride);
                            aSerializer.endObject();

                            std::function<void(BufferMemberMap_t const &)> iterateBufferMembers = nullptr;
                            iterateBufferMembers = [&](BufferMemberMap_t const &aMembers) -> void
                                {
                                    aSerializer.beginArray("members");
                                    auto const iterate = [&] (BufferMemberMap_t::value_type const &aMember)
                                        {
                                            std::string                 const  aKey   = aMember.first;
                                            Shared<SBufferMember const> const &aValue = aMember.second;

                                            aSerializer.beginObject(aKey);
                                            aSerializer.writeValue("name",    aKey);
                                            aSerializer.writeValue("offset",  aValue->location.offset);
                                            aSerializer.writeValue("size",    aValue->location.length);
                                            aSerializer.writeValue("padding", aValue->location.padding);

                                            aSerializer.beginObject("array");
                                            aSerializer.writeValue("layers", aValue->array.layers);
                                            aSerializer.writeValue("stride", aValue->array.stride);
                                            aSerializer.endObject();

                                            if(not aValue->members.empty())
                                            {
                                                iterateBufferMembers(aValue->members);
                                            }

                                            aSerializer.endObject(); // aKey
                                        };
                                    std::for_each(aMembers.begin(), aMembers.end(), iterate);
                                    aSerializer.endArray(); // members
                                };

                            //if(not aBuffer.members.empty())
                            //{
                            iterateBufferMembers(aBuffer.members);
                            //}
                        }
                        aSerializer.endObject(); // aBuffer.Name
                    };

                //
                // Serialize uniform buffers
                //
                aSerializer.beginArray("uniformBuffers");
                std::for_each(this->uniformBuffers.begin(), this->uniformBuffers.end(), iterateBuffers);
                aSerializer.endArray(); // uniformBuffers

                //
                // Serialize storage buffers
                //
                aSerializer.beginArray("storageBuffers");
                std::for_each(this->storageBuffers.begin(), this->storageBuffers.end(), iterateBuffers);
                aSerializer.endArray(); // storageBuffers

                //
                // Serialize storage buffers
                //
                aSerializer.beginArray("pushConstantRanges");
                std::for_each(this->pushConstantRanges.begin(), this->pushConstantRanges.end(), iterateBuffers);
                aSerializer.endArray(); // pushConstantRanges

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
                    {
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
                    }
                    aSerializer.endObject();
                };
                std::for_each(this->stages.cbegin(), this->stages.cend(), iterateStages);
                aSerializer.endArray();
            }
            aSerializer.endObject();

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
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
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool SMaterialAsset::acceptDeserializer(documents::IJSONDeserializer<SMaterialAsset> &aDeserializer)
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

            aDeserializer.readValue("uid",                       uid);
            aDeserializer.readValue("name",                      name);
            aDeserializer.readValue("signatureFilename",         signatureAssetUid);
            aDeserializer.readValue("baseConfigurationFilename", configurationAssetUid);

            aDeserializer.beginObject("layoutInfo");
            {
                aDeserializer.readValue("setCount", layoutInfo.setCount);

                aDeserializer.beginObject("setBindingCounts");
                layoutInfo.setBindingCount.resize(layoutInfo.setCount);
                for(uint32_t k=0; k<layoutInfo.setCount; ++k)
                {
                    aDeserializer.readValue(CString::format("{}", k), *(layoutInfo.setBindingCount.data() + k));
                }
                aDeserializer.endObject();
            }
            aDeserializer.endObject();

            //
            // Deserialize subpass input
            //
            uint32_t subpassInputCount = 0;
            aDeserializer.beginArray("subpassInputs", subpassInputCount);
            {
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
            }
            aDeserializer.endArray();

            //
            // Buffer deserialization
            //
            auto const iterateBuffers = [&](SUniformBuffer &aBuffer, uint32_t const &aIndex) -> void
                {
                    aDeserializer.beginObject(aIndex);
                    {
                        aDeserializer.readValue("name",        aBuffer.name);
                        aDeserializer.readValue("offset",      aBuffer.location.offset);
                        aDeserializer.readValue("size",        aBuffer.location.length);
                        aDeserializer.readValue("padding",     aBuffer.location.padding);
                        aDeserializer.readValue("set",         aBuffer.set);
                        aDeserializer.readValue("binding",     aBuffer.binding);

                        std::string sharingMode;
                        aDeserializer.readValue("sharing_mode", sharingMode);
                        {
                            aBuffer.sharingMode = sharingModeFromString(sharingMode);
                        }

                        aDeserializer.beginObject("array");
                        aDeserializer.readValue("layers", aBuffer.array.layers);
                        aDeserializer.readValue("stride", aBuffer.array.stride);
                        aDeserializer.endObject();

                        std::function<void(MutableBufferMemberMap_t &)> readMembers = nullptr;
                        readMembers = [&] (MutableBufferMemberMap_t &aOutMembers) -> void
                            {
                                std::function<void(Shared<SBufferMember> const &, uint32_t const &)> iterate = nullptr;
                                iterate = [&](Shared<SBufferMember> const &aMember,
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

                                        MutableBufferMemberMap_t mutableMembers {};
                                        readMembers(mutableMembers);
                                        aMember->members = mutableMembers;

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
                    }
                    aDeserializer.endObject();
                };

            //
            // Deserialize uniform buffers
            //
            uint32_t uniformBufferCount = 0;
            aDeserializer.beginArray("uniformBuffers", uniformBufferCount);
            {
                uniformBuffers.resize(uniformBufferCount);
                for(uint32_t k=0; k<uniformBufferCount; ++k)
                {
                    iterateBuffers(*(uniformBuffers.data() + k), k);
                }
            }
            aDeserializer.endArray();

            //
            // Deserialize storage buffers
            //
            uint32_t storageBufferCount = 0;
            aDeserializer.beginArray("storageBuffers", storageBufferCount);
            {
                storageBuffers.resize(storageBufferCount);
                for(uint32_t k=0; k<storageBufferCount; ++k)
                {
                    iterateBuffers(*(storageBuffers.data() + k), k);
                }
            }
            aDeserializer.endArray();

            //
            // Deserialize push constant ranges
            //
            uint32_t pushConstantRangeCount = 0;
            aDeserializer.beginArray("pushConstantRanges", pushConstantRangeCount);
            {
                pushConstantRanges.resize(pushConstantRangeCount);
                for(uint32_t k=0; k<pushConstantRangeCount; ++k)
                {
                    iterateBuffers(*(pushConstantRanges.data() + k), k);
                }
            }
            aDeserializer.endArray();

            //
            // Serialize sampled images
            //
            uint32_t sampledImageCount = 0;
            aDeserializer.beginArray("sampledImages", sampledImageCount);
            {
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
            }
            aDeserializer.endArray();

            //
            // Loop through stages
            //
            uint32_t stageCount = 0;
            aDeserializer.beginArray("stages", stageCount);
            {
                auto const iterateStages = [&] (SMaterialStage &aStage, uint32_t const &aIndex) -> void
                {
                    aDeserializer.beginObject(aIndex);
                    {
                        aDeserializer.readValue("name",     aStage.stageName);
                        aDeserializer.readValue("filename", aStage.filename);
                        aStage.stage = stageFromString(aStage.stageName);

                        uint32_t inputCount = 0;
                        aDeserializer.beginArray("inputs", inputCount);
                        {
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
                        }
                        aDeserializer.endArray();

                        uint32_t outputCount = 0;
                        aDeserializer.beginArray("outputs", outputCount);
                        {
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
                        }
                        aDeserializer.endArray();
                    }
                    aDeserializer.endObject();
                };

                for(uint32_t k=0; k<stageCount; ++k)
                {
                    SMaterialStage stage{};

                    iterateStages(stage, k);
                    stages[stage.stage] = std::move(stage);
                }
            }
            aDeserializer.endArray();

            // aDeserializer.endObject();

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<SMaterialMasterIndex> readMaterialMasterIndexFile(std::string const &aLogTag, Shared<asset::IAssetStorage> const &aAssetStorage, asset::AssetId_t const &aAssetUID)
        {
            return serialization::readMaterialFile<SMaterialMasterIndex>(aLogTag, aAssetStorage, aAssetUID);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
    }
}
