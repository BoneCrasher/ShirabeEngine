#include "Resources/Types/Texture.h"

namespace Engine {
  namespace Resources {

    TextureInfo::TextureInfo()
      : width(0)
      , height(1)
      , depth(1)
      , format(Format::Undefined)
      , arraySize(1)
      , mipLevels(1)
      , multisampling()
    {}

    void TextureInfo::assignTextureInfoParameters(TextureInfo const&other) {
      width         = other.width;
      height        = other.height;
      depth         = other.depth;
      format        = other.format;
      arraySize     = other.arraySize;
      mipLevels     = other.mipLevels;
      multisampling = other.multisampling;
    }

  }
}