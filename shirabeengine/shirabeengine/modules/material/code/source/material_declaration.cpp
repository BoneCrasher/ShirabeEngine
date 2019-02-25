#include "material/material_declaration.h"
#include "material/materialserialization.h"

namespace engine
{
    namespace material
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        bool SMaterial::acceptSerializer(serialization::IMaterialSerializer &aSerializer) const
        {
            bool result = true;

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
    }
}
