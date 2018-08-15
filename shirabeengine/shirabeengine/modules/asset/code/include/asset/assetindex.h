#ifndef __SHIRABE_ASSET_INDEX_H__
#define __SHIRABE_ASSET_INDEX_H__

#include <core/enginetypehelper.h>
#include <util/documents/xml.h>
#include "asset/assettypes.h"

namespace engine
{
    namespace asset
    {
        /**
         * The CAssetIndex class describes an indexed set of assets by their UID, which is loaded from an
         * index file provided for each workspace/deployment.
         */
        class  CAssetIndex
        {
        public:
            SHIRABE_TEST_EXPORT static CAssetRegistry<SAsset> loadIndexById(std::string const &aIndexId);
            // static AssetRegistry<Asset> loadIndexFromServer(std::string const&filename);
        };

    }
}

#endif
