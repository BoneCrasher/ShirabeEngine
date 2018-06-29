#include "Asset/AssetTypes.h"

namespace Engine {
  namespace Asset {
    

  }
  
  template <>
  Asset::AssetType from_string<Asset::AssetType>(std::string const&input)
  {
    using namespace Asset;

    if(input.compare("Mesh") == 0)
      return AssetType::Mesh;
    if(input.compare("Material") == 0)
      return AssetType::Material;
    if(input.compare("Texture") == 0)
      return AssetType::Texture;
    if(input.compare("Buffer") == 0)
      return AssetType::Buffer;

    return AssetType::Undefined;
  }
}