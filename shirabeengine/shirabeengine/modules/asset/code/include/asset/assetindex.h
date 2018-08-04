#ifndef __SHIRABE_ASSET_INDEX_H__
#define __SHIRABE_ASSET_INDEX_H__

#include "core/enginetypehelper.h"

#include "Asset/AssetTypes.h"

#include "Util/Documents/XML.h"

namespace engine {
  namespace Asset {

    class  AssetIndex {
    public:
      SHIRABE_TEST_EXPORT static AssetRegistry<Asset> loadIndexById(std::string const&indexId);
      // static AssetRegistry<Asset> loadIndexFromServer(std::string const&filename);
    };

  }
}

#endif