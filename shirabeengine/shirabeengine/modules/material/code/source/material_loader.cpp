#include <core/enginetypehelper.h>
#include <core/helpers.h>

#include "material/material_loader.h"
#include "material/materialserialization.h"

namespace engine
{
    namespace material
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<ByteBuffer> CMaterialLoader::loadAsset(asset::SAsset const &aAsset)
        {
            using namespace serialization;

            CEngineResult<ByteBuffer> data = {};

            std::string rawInput = readFile(aAsset.URI);

            CJSONDeserializer<SMaterialIndex> deserializer {};
            deserializer.initialize();

            CStdSharedPtr_t<IDeserializer<SMaterialIndex>::IResult> result = nullptr;

            CResult<CStdSharedPtr_t<IDeserializer<SMaterialIndex>::IResult>> deserialization = deserializer.deserialize(rawInput);
            if(not deserialization.successful())
            {
                CLog::Error(logTag(), "Could not load material '%0'", aAsset.id);
            }

            SMaterialIndex const &index = deserialization.data()->asT();

            deserializer.deinitialize();

            return data;
        }
        //<-----------------------------------------------------------------------------

    }
}
