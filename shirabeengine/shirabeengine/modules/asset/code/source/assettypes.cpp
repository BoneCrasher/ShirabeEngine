#include "asset/assettypes.h"
#include <util/crc32.h>
#include <graphicsapi/definitions.h>

namespace engine
{
    namespace asset
    {
        using namespace graphicsapi;

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        AssetId_t assetIdFromUri(std::filesystem::path const &aUri)
        {
            std::string const source = aUri.string();
            uint32_t    const hash   = util::crc32FromString(source);

            return hash;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        STextureInfo::STextureInfo()
                : width(0)
                  , height(1)
                  , depth(1)
                  , format(EFormat::Undefined)
                  , arraySize(1)
                  , mipLevels(1)
                  , multisampling()
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void STextureInfo::assignTextureInfoParameters(STextureInfo const &aOther)
        {
            width         = aOther.width;
            height        = aOther.height;
            depth         = aOther.depth;
            format        = aOther.format;
            arraySize     = aOther.arraySize;
            mipLevels     = aOther.mipLevels;
            multisampling = aOther.multisampling;
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

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <>
    std::string convert_to_string<asset::EAssetType>(asset::EAssetType const &aInput)
    {
        using namespace asset;

        switch(aInput)
        {
            case EAssetType::Mesh:     return "Mesh";
            case EAssetType::Material: return "Material";
            case EAssetType::Texture:  return "Texture";
            case EAssetType::Buffer:   return "Buffer";
            default:                   return "Invalid";
        }
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <>
    std::string convert_to_string<asset::EAssetSubtype>(asset::EAssetSubtype const &aInput)
    {
        using namespace asset;

        switch(aInput)
        {
            case EAssetSubtype::Master:    return "Master";
            case EAssetSubtype::Signature: return "Signature";
            case EAssetSubtype::Config:    return "Config";
            case EAssetSubtype::SPVModule: return "SPVModule";
            case EAssetSubtype::Instance:  return "Instance";
            default:                       return "Invalid";
        }
    }
    //<-----------------------------------------------------------------------------
}
