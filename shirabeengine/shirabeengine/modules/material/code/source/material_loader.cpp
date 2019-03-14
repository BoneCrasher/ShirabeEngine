#include <core/enginetypehelper.h>
#include <core/helpers.h>
#include <asset/assetstorage.h>

#include "material/material_declaration.h"
#include "material/materialserialization.h"
#include "material/material_loader.h"

namespace engine
{
    namespace material
    {
        using namespace asset;

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CMaterialLoader::CMaterialLoader(CStdSharedPtr_t<asset::IAssetStorage> &aAssetStorage)
            : mStorage(aAssetStorage)
        {
            assert(nullptr != aAssetStorage);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CResult<SMaterialIndex> readMaterialIndexFile(std::string const &aLogTag, asset::IAssetStorage *aAssetStorage, asset::SAsset const &aAsset)
        {
            using namespace serialization;

            CEngineResult<ByteBuffer> const dataFetch = aAssetStorage->loadAssetData(aAsset.id);
            if(not dataFetch.successful())
            {
                return { false };
            }

            std::string rawInput(reinterpret_cast<char const *>(dataFetch.data().data()),
                                 dataFetch.data().size());

            CJSONDeserializer<SMaterialIndex> deserializer {};
            deserializer.initialize();

            CResult<CStdSharedPtr_t<IDeserializer<SMaterialIndex>::IResult>> deserialization = deserializer.deserialize(rawInput);
            if(not deserialization.successful())
            {
                CLog::Error(aLogTag, "Could not load material '%0'", aAsset.id);
            }

            CResult<SMaterialIndex> const index = deserialization.data()->asT();

            deserializer.deinitialize();

            return index;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<CStdSharedPtr_t<CMaterial>> CMaterialLoader::loadMaterial(std::string const &aMaterialId)
        {
            CEngineResult<ByteBuffer> data = {};

            // CResult<SMaterialIndex> const index = readMaterialIndexFile(logTag(), aAsset);

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

    }
}
