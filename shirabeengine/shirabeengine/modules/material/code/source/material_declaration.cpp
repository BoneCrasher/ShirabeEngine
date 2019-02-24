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
                aSerializer.writeValue       ("name",            aSubpassInput.name);
                aSerializer.writeNumericValue("attachmentIndex", aSubpassInput.attachmentIndex);
                aSerializer.writeNumericValue("set",             aSubpassInput.set);
                aSerializer.writeNumericValue("binding",         aSubpassInput.binding);
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
                aSerializer.writeValue       ("name",    aBuffer.name);
                aSerializer.writeNumericValue("offset",  aBuffer.location.offset);
                aSerializer.writeNumericValue("size",    aBuffer.location.length);
                aSerializer.writeNumericValue("padding", aBuffer.location.padding);

                aSerializer.beginArray("members");
                auto const iterate = [&] (std::pair<std::string, SUniformBufferMember> const &aMember)
                {
                    std::string          const aKey   = aMember.first;
                    SUniformBufferMember const aValue = aMember.second;

                    aSerializer.beginObject(aKey);
                    aSerializer.writeValue       ("name",    aKey);
                    aSerializer.writeNumericValue("offset",  aValue.location.offset);
                    aSerializer.writeNumericValue("size",    aValue.location.length);
                    aSerializer.writeNumericValue("padding", aValue.location.padding);
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
                aSerializer.writeValue       ("name",    aSampledImage.name);
                aSerializer.writeNumericValue("set",     aSampledImage.set);
                aSerializer.writeNumericValue("binding", aSampledImage.binding);
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
                   aSerializer.writeValue       ("name",     aInput.name);
                   aSerializer.writeNumericValue("location", aInput.location);
                   aSerializer.endObject();
                };
                std::for_each(stage.inputs.begin(), stage.inputs.end(), iterateInputs);
                aSerializer.endArray();

                aSerializer.beginArray("outputs");
                auto const iterateOutputs = [&] (SStageOutput const &aOutput) -> void
                {
                    aSerializer.beginObject(aOutput.name);
                    aSerializer.writeValue       ("name",     aOutput.name);
                    aSerializer.writeNumericValue("location", aOutput.location);
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
