#include "material/material_declaration.h"
#include "material/materialserialization.h"

namespace engine
{
    namespace material
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
        bool SMaterialMasterIndex::acceptSerializer(serialization::IJSONSerializer<SMaterialMasterIndex> &aSerializer) const
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
        bool SMaterialMasterIndex::acceptDeserializer(serialization::IJSONDeserializer<SMaterialMasterIndex> &aDeserializer)
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
        bool SMaterialMeta::acceptSerializer(serialization::IJSONSerializer<SMaterialMeta> &aSerializer) const
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
        bool SMaterialMeta::acceptDeserializer(serialization::IJSONDeserializer<SMaterialMeta> &aDeserializer)
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
        bool SMaterialInstanceIndex::acceptSerializer(serialization::IJSONSerializer<SMaterialInstanceIndex> &aSerializer) const
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
        bool SMaterialInstanceIndex::acceptDeserializer(serialization::IJSONDeserializer<SMaterialInstanceIndex> &aDeserializer)
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
        bool SMaterialSignature::acceptSerializer(serialization::IJSONSerializer<SMaterialSignature> &aSerializer) const
        {
            aSerializer.beginObject(name);

            //
            // Write layout info
            //
            aSerializer.beginObject("layoutInfo");
            aSerializer.writeValue ("setCount", layoutInfo.setCount);
            aSerializer.beginObject("setBindingCounts");
            for(uint32_t k=0; k<layoutInfo.setBindingCount.size(); ++k)
            {
                aSerializer.writeValue(CString::format("%0", k), layoutInfo.setBindingCount[k]);
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
                aSerializer.writeValue("name",    aBuffer.name);
                aSerializer.writeValue("offset",  aBuffer.location.offset);
                aSerializer.writeValue("size",    aBuffer.location.length);
                aSerializer.writeValue("padding", aBuffer.location.padding);
                aSerializer.writeValue("set",     aBuffer.set);
                aSerializer.writeValue("binding", aBuffer.binding);

                aSerializer.beginArray("members");
                auto const iterate = [&] (std::pair<std::string, SUniformBufferMember> const &aMember)
                {
                    std::string          const aKey   = aMember.first;
                    SUniformBufferMember const aValue = aMember.second;

                    aSerializer.beginObject(aKey);
                    aSerializer.writeValue("name",    aKey);
                    aSerializer.writeValue("offset",  aValue.location.offset);
                    aSerializer.writeValue("size",    aValue.location.length);
                    aSerializer.writeValue("padding", aValue.location.padding);
                    aSerializer.endObject(); // aKey
                };
                std::for_each(aBuffer.members.begin(), aBuffer.members.end(), iterate);
                aSerializer.endArray(); // members

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
            auto const iterateStages = [&] (std::pair<VkPipelineStageFlagBits, SMaterialStage> const &aStage) -> void
            {
                auto const writeType = [&] (SMaterialType const &aType) -> void
                {
                    aSerializer.beginObject("type");
                    aSerializer.writeValue("name",               aType.name);
                    aSerializer.writeValue("byteSize",           aType.byteSize);
                    aSerializer.writeValue("vectorSize",         aType.vectorSize);
                    aSerializer.writeValue("matrixRows",         aType.matrixRows);
                    aSerializer.writeValue("matrixColumns",      aType.matrixColumns);
                    aSerializer.writeValue("matrixColumnStride", aType.matrixColumnStride);
                    aSerializer.writeValue("arraySize",          aType.arraySize);
                    aSerializer.writeValue("arrayStride",        aType.arrayStride);
                    aSerializer.endObject();
                };

                VkPipelineStageFlagBits const key   = aStage.first;
                SMaterialStage          const stage = aStage.second;

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

                   writeType(aInput.type);

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
                    aSerializer.endObject();
                };
                std::for_each(stage.outputs.begin(), stage.outputs.end(), iterateOutputs);
                aSerializer.endArray();
                aSerializer.endObject();
            };
            std::for_each(this->stages.begin(), this->stages.end(), iterateStages);

            aSerializer.endArray();
            aSerializer.endObject();

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------

        /**
         * @brief acceptDeserializer
         * @param aSerializer
         * @return
         */
        bool SMaterialSignature::acceptDeserializer(serialization::IJSONDeserializer<SMaterialSignature> &aDeserializer)
        {
            //
            // Read layout info
            //
            aDeserializer.beginObject("layoutInfo");
            aDeserializer.readValue  ("setCount", layoutInfo.setCount);
            aDeserializer.beginObject("setBindingCounts");

            layoutInfo.setBindingCount.resize(layoutInfo.setCount);
            for(uint32_t k=0; k<layoutInfo.setCount; ++k)
            {
                aDeserializer.readValue(CString::format("%0", k), *(layoutInfo.setBindingCount.data() + k));
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
                aDeserializer.readValue("name",    aBuffer.name);
                aDeserializer.readValue("offset",  aBuffer.location.offset);
                aDeserializer.readValue("size",    aBuffer.location.length);
                aDeserializer.readValue("padding", aBuffer.location.padding);
                aDeserializer.readValue("set",     aBuffer.set);
                aDeserializer.readValue("binding", aBuffer.binding);

                uint32_t uniformBufferMemberCount = 0;
                aDeserializer.beginArray("members", uniformBufferMemberCount);

                auto const iterate = [&] (SUniformBufferMember &aMember, uint32_t const &aIndex) -> void
                {
                    aDeserializer.beginObject(aIndex);
                    aDeserializer.readValue("name",    aMember.name);
                    aDeserializer.readValue("offset",  aMember.location.offset);
                    aDeserializer.readValue("size",    aMember.location.length);
                    aDeserializer.readValue("padding", aMember.location.padding);
                    aDeserializer.endObject();
                };

                for(uint32_t k=0; k<uniformBufferMemberCount; ++k)
                {
                    SUniformBufferMember member{};
                    iterate(member, k);
                    aBuffer.members[member.name] = member;
                }

                aDeserializer.endArray();
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
                auto const readType = [&] (SMaterialType &aType) -> void
                {
                    aDeserializer.beginObject("type");
                    aDeserializer.readValue("name",               aType.name);
                    aDeserializer.readValue("byteSize",           aType.byteSize);
                    aDeserializer.readValue("vectorSize",         aType.vectorSize);
                    aDeserializer.readValue("matrixRows",         aType.matrixRows);
                    aDeserializer.readValue("matrixColumns",      aType.matrixColumns);
                    aDeserializer.readValue("matrixColumnStride", aType.matrixColumnStride);
                    aDeserializer.readValue("arraySize",          aType.arraySize);
                    aDeserializer.readValue("arrayStride",        aType.arrayStride);
                    aDeserializer.endObject();
                };

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

                   readType(aInput.type);

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
                stages[stage.stage] = stage;
            }

            aDeserializer.endArray();
            aDeserializer.endObject();

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CMaterialConfig CMaterialConfig::fromMaterialDesc(SMaterialSignature const &aMaterial)
        {
            CMaterialConfig config {};

            uint64_t previousSize = 0;
            uint64_t currentSize  = 0;
            uint64_t totalSize    = 0;

            for(auto const &buffer : aMaterial.uniformBuffers)
            {
                SBufferLocation adjustedBufferLocation {};
                adjustedBufferLocation.offset  = (totalSize + buffer.location.offset);
                adjustedBufferLocation.length  = buffer.location.length;
                adjustedBufferLocation.padding = buffer.location.padding;

                SBufferData data{};
                data.mLocation = adjustedBufferLocation;

                for(auto const &[name, member] : buffer.members)
                {
                    SBufferLocation adjustedMemberLocation {};
                    adjustedMemberLocation.offset  = (adjustedBufferLocation.offset + member.location.offset);
                    adjustedMemberLocation.length  = member.location.length;
                    adjustedMemberLocation.padding = member.location.padding;

                    data.mValueIndex[name] = member.location;
                }

                config.mBufferIndex[buffer.name] = data;

                previousSize = currentSize;
                currentSize  = (buffer.location.offset + buffer.location.length + buffer.location.padding);
                totalSize   += currentSize;
            }

            config.mData.resize(totalSize);
            config.mData.assign(totalSize, 0);

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
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMaterialConfig::override(CMaterialConfig const &aOther)
        {
            for(auto const &[bufferId, data] : mBufferIndex)
            {
                for(auto const &[valueId, location] : data.mValueIndex)
                {
                    CEngineResult<uint8_t const *> valueFetch = aOther.getBufferValue(location);
                    if(not valueFetch.successful())
                    {
                        // Ignore, as the desired value might not be part of the master material.
                        continue;
                    }

                    // If override value is not null-array. Override.
                    uint8_t const *const value = valueFetch.data();
                    if(not checkIsZeroArray(value, location))
                    {
                        CEngineResult<> result = setBufferValue(location, value);
                    }
                }
            }

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CMaterialConfig::acceptSerializer(serialization::IJSONSerializer<CMaterialConfig> &aSerializer) const
        {
            aSerializer.beginObject("");

            aSerializer.writeValue("uniformBufferData", mData);

            aSerializer.beginArray("uniformBuffers");
            auto const iterateUniformBuffers = [&] (std::pair<std::string, SBufferData> const &aBuffer) -> void
            {
                std::string const &name = aBuffer.first;
                SBufferData const &data = aBuffer.second;

                SBufferLocation const &location = data.getLocation();

                aSerializer.beginObject(name);
                aSerializer.writeValue("name",    name);
                aSerializer.writeValue("offset",  location.offset);
                aSerializer.writeValue("size",    location.length);
                aSerializer.writeValue("padding", location.padding);

                aSerializer.beginArray("members");
                auto const iterate = [&] (std::pair<std::string, SBufferLocation> const &aMember)
                {
                    std::string     const name     = aMember.first;
                    SBufferLocation const location = aMember.second;

                    aSerializer.beginObject(name);
                    aSerializer.writeValue("name",    name);
                    aSerializer.writeValue("offset",  location.offset);
                    aSerializer.writeValue("size",    location.length);
                    aSerializer.writeValue("padding", location.padding);
                    aSerializer.endObject();
                };
                std::for_each(data.mValueIndex.begin(), data.mValueIndex.end(), iterate);
                aSerializer.endArray();

                aSerializer.endObject();
            };
            std::for_each(mBufferIndex.begin(), mBufferIndex.end(), iterateUniformBuffers);
            aSerializer.endArray();

            aSerializer.endObject();

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CMaterialConfig::acceptDeserializer(serialization::IJSONDeserializer<CMaterialConfig> &aDeserializer)
        {
            aDeserializer.readValue("uniformBufferData", mData);

            uint32_t bufferCount = 0;
            aDeserializer.beginArray("uniformBuffers", bufferCount);
            for(uint32_t k=0; k<bufferCount; ++k)
            {
                SBufferData buffer = {};

                std::string     name     = {};

                aDeserializer.beginObject(k);

                aDeserializer.readValue("name",    name);
                aDeserializer.readValue("offset",  buffer.mLocation.offset);
                aDeserializer.readValue("size",    buffer.mLocation.length);
                aDeserializer.readValue("padding", buffer.mLocation.padding);

                uint32_t memberCount = 0;
                aDeserializer.beginArray("members", memberCount);
                for(uint32_t l=0; l<memberCount; ++l)
                {
                    std::string     name     ={};
                    SBufferLocation location ={};

                    aDeserializer.beginObject(l);

                    aDeserializer.readValue("name",    name);
                    aDeserializer.readValue("offset",  location.offset);
                    aDeserializer.readValue("size",    location.length);
                    aDeserializer.readValue("padding", location.padding);

                    aDeserializer.endObject();

                    buffer.mValueIndex[name] = location;
                }
                aDeserializer.endArray();

                aDeserializer.endObject();

                mBufferIndex[name] = buffer;
            }
            aDeserializer.endArray();

            return true;
        }
        //<-----------------------------------------------------------------------------
    }
}
