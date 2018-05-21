#include "Resources/Types/Texture.h"

namespace Engine {
  namespace Resources {

    TextureInfo::TextureInfo()
      : width(0)
      , height(1)
      , depth(1)
      , format(Format::Undefined)
      , arraySize(1)
    {}
  }
}