#include <util/documents/json.h>
#include "asset/buffers/declaration.h"

namespace engine::buffers
{
    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    bool SBufferAsset::acceptSerializer(documents::IJSONSerializer<SBufferAsset> &aSerializer) const
    {
        aSerializer.beginObject(name);
        {
            aSerializer.writeValue("uid",               uid);
            aSerializer.writeValue("name",              name);
            aSerializer.writeValue("bufferSizeInBytes", bufferSize);
            aSerializer.writeValue("binFilename",       binaryFilename);
        }
        aSerializer.endObject();

        return true;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    bool SBufferAsset::acceptDeserializer(documents::IJSONDeserializer<SBufferAsset> &aDeserializer)
    {
        aDeserializer.beginObject(name);
        {
            aDeserializer.readValue("uid",               uid);
            aDeserializer.readValue("name",              name);
            aDeserializer.readValue("bufferSizeInBytes", bufferSize);

            std::string binaryFilenameString  {};
            aDeserializer.readValue("binaryFilename", binaryFilenameString);
            binaryFilename = std::filesystem::path(binaryFilenameString);
        }
        aDeserializer.endObject();

        return true;
    }
}
