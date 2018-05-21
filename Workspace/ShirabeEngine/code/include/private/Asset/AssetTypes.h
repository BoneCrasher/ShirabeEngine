#ifndef __SHIRABE_ASSET_TYPES_H__
#define __SHIRABE_ASSET_TYPES_H__

#include <stdint.h>
#include <string>

#include "Resources/Types/Texture.h"

namespace Engine {
  namespace Asset {

    using AssetId_t = uint64_t;

    enum class AssetSource {
      Runtime = 1,
      Local   = 2,
      URL     = 4
    };

    enum class AssetType {
      Texture = 1,
      Buffer  = 2,
    };

    struct Asset {
      AssetId_t   id;
      AssetSource source;
      std::string path;
      AssetType   type;
    };

    struct TextureAsset {
      std::string            name;
      Resources::TextureInfo textureInfo;
    };

    struct BufferAsset {
      std::string name;
    };
  }
}

#endif