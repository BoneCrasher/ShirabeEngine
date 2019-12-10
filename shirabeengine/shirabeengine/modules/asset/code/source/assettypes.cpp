#include "asset/assettypes.h"
#include <util/crc32.h>
#include <graphicsapi/definitions.h>

#include <core/basictypes.h>

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

        if("Mesh"     == aInput) return EAssetType::Mesh;
        if("Material" == aInput) return EAssetType::Material;
        if("Texture"  == aInput) return EAssetType::Texture;
        if("Buffer"   == aInput) return EAssetType::Buffer;

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

        if("Meta"            == aInput) return EAssetSubtype::Meta;
        if("Master"          == aInput) return EAssetSubtype::Master;
        if("Signature"       == aInput) return EAssetSubtype::Signature;
        if("Config"          == aInput) return EAssetSubtype::Config;
        if("SPVModule"       == aInput) return EAssetSubtype::SPVModule;
        if("Instance"        == aInput) return EAssetSubtype::Instance;
        if("AttributeBuffer" == aInput) return EAssetSubtype::AttributeBuffer;
        if("IndexBuffer"     == aInput) return EAssetSubtype::IndexBuffer;
        if("DataFile"        == aInput) return EAssetSubtype::DataFile;

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
            case EAssetSubtype::Meta:            return "Meta";
            case EAssetSubtype::Master:          return "Master";
            case EAssetSubtype::Signature:       return "Signature";
            case EAssetSubtype::Config:          return "Config";
            case EAssetSubtype::SPVModule:       return "SPVModule";
            case EAssetSubtype::Instance:        return "Instance";
            case EAssetSubtype::AttributeBuffer: return "AttributeBuffer";
            case EAssetSubtype::IndexBuffer:     return "IndexBuffer";
            case EAssetSubtype::DataFile:        return "DataFile";
            default:                             return "Invalid";
        }
    }
    //<-----------------------------------------------------------------------------
}
