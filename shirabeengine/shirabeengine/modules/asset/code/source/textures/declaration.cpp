#include "asset/textures/declaration.h"

namespace engine::textures
{
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    Shared<CTextureInstance> CTextureInstance::fromAsset(STextureAsset const &aAsset)
    {
        auto texture = makeShared<CTextureInstance>();

        texture->mName                      = aAsset.name;
        texture->mTextureInfo               = aAsset.textureInfo;
        texture->mImageLayersBinaryAssetUid = aAsset.imageLayersBinaryUid;

        return texture;
    }
    //<-----------------------------------------------------------------------------
}
