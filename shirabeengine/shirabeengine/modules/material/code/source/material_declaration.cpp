#include "material/material_declaration.h"
#include "material/materialserialization.h"

namespace engine
{
    namespace material
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        bool SMaterialIndex::acceptSerializer(serialization::IJSONSerializer<SMaterialIndex> &aSerializer) const
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool SMaterialIndex::acceptDeserializer(serialization::IJSONDeserializer<SMaterialIndex> &aDeserializer)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool SMaterial::acceptSerializer(serialization::IJSONSerializer<SMaterial> &aSerializer) const
        {
            aSerializer.beginObject(name);

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
                aSerializer.endObject();
            };
            std::for_each(this->subpassInputs.begin(), this->subpassInputs.end(), iterateSubpassInputs);
            aSerializer.endArray();

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
                    aSerializer.endObject();
                };
                std::for_each(aBuffer.members.begin(), aBuffer.members.end(), iterate);
                aSerializer.endArray();

                aSerializer.endObject();
            };
            std::for_each(this->uniformBuffers.begin(), this->uniformBuffers.end(), iterateUniformBuffers);
            aSerializer.endArray();

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
                aSerializer.endObject();
            };
            std::for_each(this->sampledImages.begin(), this->sampledImages.end(), iterateSampledImages);
            aSerializer.endArray();

            //
            // Loop through stages
            //
            aSerializer.beginArray("stages");
            auto const iterateStages = [&] (std::pair<EShaderStage, SMaterialStage> const &aStage) -> void
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

                EShaderStage   const key   = aStage.first;
                SMaterialStage const stage = aStage.second;

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
        bool SMaterial::acceptDeserializer(serialization::IJSONDeserializer<SMaterial> &aDeserializer)
        {
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
                // TODO: Determine stage from stage-key or sth.

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
    }
}
