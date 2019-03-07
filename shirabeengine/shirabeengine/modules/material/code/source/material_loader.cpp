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
        CResult<SMaterialIndex> readMaterialIndexFile(std::string const &aLogTag, asset::SAsset const &aAsset)
        {
            using namespace serialization;

            std::string inputFile = aAsset.URI;

            if(not std::filesystem::exists(inputFile))
            {
                CLog::Error(aLogTag, "Could not find material index file '%0'", inputFile);
                return { false };
            }

            std::string rawInput = readFile(inputFile);

            CJSONDeserializer<SMaterialIndex> deserializer {};
            deserializer.initialize();

            CResult<CStdSharedPtr_t<IDeserializer<SMaterialIndex>::IResult>> deserialization = deserializer.deserialize(rawInput);
            if(not deserialization.successful())
            {
                CLog::Error(aLogTag, "Could not load material '%0'", aAsset.id);
            }

            CResult<SMaterialIndex> const index = deserialization.data()->asT();

            deserializer.deinitialize();
        }

        CEngineResult<ByteBuffer> CMaterialLoader::loadAsset(asset::SAsset const &aAsset)
        {
            CEngineResult<ByteBuffer> data = {};

            CResult<SMaterialIndex> const index = readMaterialIndexFile(logTag(), aAsset);



            return data;
        }
        //<-----------------------------------------------------------------------------

    }
}
