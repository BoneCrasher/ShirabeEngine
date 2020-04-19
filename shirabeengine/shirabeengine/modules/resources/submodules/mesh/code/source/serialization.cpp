#include <util/documents/json.h>
#include "mesh/declaration.h"

namespace engine::mesh
{
    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    bool SMeshAsset::acceptSerializer(documents::IJSONSerializer<SMeshAsset> &aSerializer) const
    {
        aSerializer.beginObject(name);
        {
            aSerializer.writeValue("uid",        uid);
            aSerializer.writeValue("name",       name);

            auto const writeAttributeDescription = [](documents::IJSONSerializer<SMeshAsset> &aSerializer, SMeshAttributeDescription const &aDesc)
             {
                aSerializer.writeValue("name",           aDesc.name);
                aSerializer.writeValue("index",          aDesc.index);
                aSerializer.writeValue("offset",         aDesc.offset);
                aSerializer.writeValue("length",         aDesc.length);
                aSerializer.writeValue("bytesPerSample", aDesc.bytesPerSample);
             };

            aSerializer.beginObject("signature");
            {
                aSerializer.writeValue("binaryFilename",      binaryFilename);
                aSerializer.writeValue("attributeSampleCount", attributeSampleCount);
                aSerializer.writeValue("indexSampleCount",     indexSampleCount);

                aSerializer.beginArray("attributes");
                for(auto const &attributeDesc : attributes)
                {
                    aSerializer.beginObject("");
                    writeAttributeDescription(aSerializer, attributeDesc);
                    aSerializer.endObject();
                }
                aSerializer.endArray();

                aSerializer.beginObject("indices");
                writeAttributeDescription(aSerializer, indices);
                aSerializer.endObject();

            }
            aSerializer.endObject();
        }
        aSerializer.endObject();

        return true;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    bool SMeshAsset::acceptDeserializer(documents::IJSONDeserializer<SMeshAsset> &aDeserializer)
    {
        aDeserializer.beginObject(name);
        {
            aDeserializer.readValue("uid",        uid);
            aDeserializer.readValue("name",       name);

            aDeserializer.beginObject(name);
            {
                auto const readAttributeDescription = [](documents::IJSONDeserializer<SMeshAsset> &aDeserializer, SMeshAttributeDescription &aDesc)
                {
                    aDeserializer.readValue("name",           aDesc.name);
                    aDeserializer.readValue("index",          aDesc.index);
                    aDeserializer.readValue("offset",         aDesc.offset);
                    aDeserializer.readValue("length",         aDesc.length);
                    aDeserializer.readValue("bytesPerSample", aDesc.bytesPerSample);
                };

                std::string binaryFilenameString  {};
                aDeserializer.readValue("binaryFilename", binaryFilenameString);
                binaryFilename = std::filesystem::path(binaryFilenameString);

                aDeserializer.readValue("attributeSampleCount", attributeSampleCount);
                aDeserializer.readValue("indexSampleCount",     indexSampleCount);

                uint32_t attributeCount = 0;
                aDeserializer.beginArray("attributes", attributeCount);
                for(uint32_t k=0; k<attributeCount; ++k)
                {
                    SMeshAttributeDescription attributeDesc {};

                    aDeserializer.beginObject(k);
                    readAttributeDescription(aDeserializer, attributeDesc);
                    aDeserializer.endObject();
                    attributes.push_back(attributeDesc);
                }
                aDeserializer.endArray();

                SMeshAttributeDescription indicesDesc {};
                aDeserializer.beginObject("indices");
                readAttributeDescription(aDeserializer, indicesDesc);
                aDeserializer.endObject();
                indices = indicesDesc;
            }
            aDeserializer.endObject();
        }
        aDeserializer.endObject();

        return true;
    }
}
