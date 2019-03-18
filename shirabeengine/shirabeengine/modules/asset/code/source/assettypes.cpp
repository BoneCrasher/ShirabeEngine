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

        if(0 == aInput.compare("Mesh"))
            return EAssetType::Mesh;
        if(0 == aInput.compare("MaterialMaster"))
            return EAssetType::MaterialMaster;
        if(0 == aInput.compare("MaterialInstance"))
            return EAssetType::MaterialInstance;
        if(0 == aInput.compare("Texture"))
            return EAssetType::Texture;
        if(0 == aInput.compare("Buffer"))
            return EAssetType::Buffer;

        return EAssetType::Undefined;
    }
    //<-----------------------------------------------------------------------------
}
