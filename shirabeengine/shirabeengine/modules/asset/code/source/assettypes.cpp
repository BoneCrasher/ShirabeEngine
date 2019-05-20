#include "asset/assettypes.h"
#include <util/crc32.h>

namespace engine
{
    namespace asset
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        AssetId_t assetIdFromUri(std::filesystem::path const &aUri)
        {
            std::string const source = static_cast<std::string>(aUri);
            uint32_t    const hash   = util::crc32FromString(source);

            return hash;
        }
        //<-----------------------------------------------------------------------------
    }

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <>
    asset::EAssetType from_string<asset::EAssetType>(std::string const &aInput)
    {
        using namespace asset;

        if(0 == aInput.compare("Mesh"))
            return EAssetType::Mesh;
        if(0 == aInput.compare("Material"))
            return EAssetType::Material;
        if(0 == aInput.compare("Texture"))
            return EAssetType::Texture;
        if(0 == aInput.compare("Buffer"))
            return EAssetType::Buffer;

        return EAssetType::Undefined;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <>
    asset::EAssetSubtype from_string<asset::EAssetSubtype>(std::string const &aInput)
    {
        using namespace asset;

        if(0 == aInput.compare("Master"))
            return EAssetSubtype::Master;
        if(0 == aInput.compare("Signature"))
            return EAssetSubtype::Signature;
        if(0 == aInput.compare("Config"))
            return EAssetSubtype::Config;
        if(0 == aInput.compare("SPVModule"))
            return EAssetSubtype::SPVModule;
        if(0 == aInput.compare("Instance"))
            return EAssetSubtype::Instance;

        return EAssetSubtype::Undefined;
    }
    //<-----------------------------------------------------------------------------
}
