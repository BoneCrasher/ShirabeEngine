#ifndef __SHIRABE_ASSET_INDEX_H__
#define __SHIRABE_ASSET_INDEX_H__

#include "Core/EngineTypeHelper.h"

#include "Asset/AssetTypes.h"

#include "Util/Documents/XML.h"

namespace Engine {
  namespace Asset {

    class  AssetIndex {
    public:
      SHIRABE_TEST_EXPORT static AssetRegistry<Asset> loadIndexById(std::string const&indexId);
      // static AssetRegistry<Asset> loadIndexFromServer(std::string const&filename);
    };

  }
}

#endif