#include "asset/assettypes.h"

namespace engine
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <>
    asset::EAssetType from_string<asset::EAssetType>(std::string const &aInput)
    {
        using namespace asset;

        if(aInput.compare("Mesh") == 0)
            return EAssetType::Mesh;
        if(aInput.compare("Material") == 0)
            return EAssetType::Material;
        if(aInput.compare("Texture") == 0)
            return EAssetType::Texture;
        if(aInput.compare("Buffer") == 0)
            return EAssetType::Buffer;

        return EAssetType::Undefined;
    }
    //<-----------------------------------------------------------------------------
}
