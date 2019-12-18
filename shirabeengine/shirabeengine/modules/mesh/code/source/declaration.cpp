#include "mesh/declaration.h"
#include "mesh/serialization.h"
#include <util/documents/json.h>

namespace engine::mesh
{
    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    bool SMeshMeta::acceptSerializer(documents::IJSONSerializer<SMeshMeta> &aSerializer) const
    {
        aSerializer.beginObject(name);

        aSerializer.writeValue("uid",        uid);
        aSerializer.writeValue("name",       name);
        aSerializer.writeValue("dataFileId", dataFileId);

        aSerializer.endObject();

        return true;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    bool SMeshMeta::acceptDeserializer(documents::IJSONDeserializer<SMeshMeta> &aDeserializer)
    {
        aDeserializer.beginObject(name);

        aDeserializer.readValue("uid",        uid);
        aDeserializer.readValue("name",       name);
        aDeserializer.readValue("dataFileId", dataFileId);

        aDeserializer.endObject();

        return true;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    bool SMeshDataFile::acceptSerializer(documents::IJSONSerializer<SMeshDataFile> &aSerializer) const
    {
        auto const writeAttributeDescription = [](documents::IJSONSerializer<SMeshDataFile> &aSerializer, SMeshAttributeDescription const &aDesc)
        {
            aSerializer.writeValue("name",           aDesc.name);
            aSerializer.writeValue("index",          aDesc.index);
            aSerializer.writeValue("offset",         aDesc.offset);
            aSerializer.writeValue("length",         aDesc.length);
            aSerializer.writeValue("bytesPerSample", aDesc.bytesPerSample);
        };

        aSerializer.beginObject(name);

        aSerializer.writeValue("uid",                  uid);
        aSerializer.writeValue("name",                 name);
        aSerializer.writeValue("indexBinaryFilename",  indexBinaryFilename);
        aSerializer.writeValue("dataBinaryFilename",   dataBinaryFilename);
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

        aSerializer.endObject();

        return true;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    bool SMeshDataFile::acceptDeserializer(documents::IJSONDeserializer<SMeshDataFile> &aDeserializer)
    {
        auto const readAttributeDescription = [](documents::IJSONDeserializer<SMeshDataFile> &aDeserializer, SMeshAttributeDescription &aDesc)
        {
            aDeserializer.readValue("name",           aDesc.name);
            aDeserializer.readValue("index",          aDesc.index);
            aDeserializer.readValue("offset",         aDesc.offset);
            aDeserializer.readValue("length",         aDesc.length);
            aDeserializer.readValue("bytesPerSample", aDesc.bytesPerSample);
        };

        aDeserializer.beginObject(name);

        aDeserializer.readValue("uid",                 uid);
        aDeserializer.readValue("name",                name);

        std::string indexBinaryFilenameString {};
        std::string dataBinaryFilenameString  {};
        aDeserializer.readValue("indexBinaryFilename", indexBinaryFilenameString);
        aDeserializer.readValue("dataBinaryFilename",  dataBinaryFilenameString);
        dataBinaryFilename  = std::filesystem::path(dataBinaryFilenameString);
        indexBinaryFilename = std::filesystem::path(indexBinaryFilenameString);

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

        aDeserializer.endObject();

        return true;
    }
}
