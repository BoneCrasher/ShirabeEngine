#include "asset/textures/declaration.h"
#include <util/documents/json.h>
#include <graphicsapi/definitions.h>

namespace engine::textures
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    std::string toFormatString(asset::EFormat const &aFormat)
    {
        switch(aFormat)
        {
            default:
            case asset::EFormat::Undefined:                return "Undefined";
            case asset::EFormat::Automatic:                return "Automatic";
            case asset::EFormat::Structured:               return "Structured";
            case asset::EFormat::R8_TYPELESS:              return "R8_TYPELESS";
            case asset::EFormat::R8_SINT:                  return "R8_SINT";
            case asset::EFormat::R8_UINT:                  return "R8_UINT";
            case asset::EFormat::R8_SNORM:                 return "R8_SNORM";
            case asset::EFormat::R8_UNORM:                 return "R8_UNORM";
            case asset::EFormat::R8G8B8A8_TYPELESS:        return "R8G8B8A8_TYPELESS";
            case asset::EFormat::R8G8B8A8_SINT:            return "R8G8B8A8_SINT";
            case asset::EFormat::R8G8B8A8_UINT:            return "R8G8B8A8_UINT";
            case asset::EFormat::R8G8B8A8_SNORM:           return "R8G8B8A8_SNORM";
            case asset::EFormat::R8G8B8A8_UNORM:           return "R8G8B8A8_UNORM";
            case asset::EFormat::R8G8B8A8_UNORM_SRGB:      return "R8G8B8A8_UNORM_SRGB";
            case asset::EFormat::R8G8B8A8_FLOAT:           return "R8G8B8A8_FLOAT";
            case asset::EFormat::B8G8R8A8_TYPELESS:        return "B8G8R8A8_TYPELESS";
            case asset::EFormat::B8G8R8A8_SINT:            return "B8G8R8A8_SINT";
            case asset::EFormat::B8G8R8A8_UINT:            return "B8G8R8A8_UINT";
            case asset::EFormat::B8G8R8A8_SNORM:           return "B8G8R8A8_SNORM";
            case asset::EFormat::B8G8R8A8_UNORM:           return "B8G8R8A8_UNORM";
            case asset::EFormat::B8G8R8A8_UNORM_SRGB:      return "B8G8R8A8_UNORM_SRGB";
            case asset::EFormat::B8G8R8A8_FLOAT:           return "B8G8R8A8_FLOAT";
            case asset::EFormat::R16_TYPELESS:             return "R16_TYPELESS";
            case asset::EFormat::R16_SINT:                 return "R16_SINT";
            case asset::EFormat::R16_UINT:                 return "R16_UINT";
            case asset::EFormat::R16_SNORM:                return "R16_SNORM";
            case asset::EFormat::R16_UNORM:                return "R16_UNORM";
            case asset::EFormat::R16_FLOAT:                return "R16_FLOAT";
            case asset::EFormat::R16G16B16A16_TYPELESS:    return "R16G16B16A16_TYPELESS";
            case asset::EFormat::R16G16B16A16_SINT:        return "R16G16B16A16_SINT";
            case asset::EFormat::R16G16B16A16_UINT:        return "R16G16B16A16_UINT";
            case asset::EFormat::R16G16B16A16_SNORM:       return "R16G16B16A16_SNORM";
            case asset::EFormat::R16G16B16A16_UNORM:       return "R16G16B16A16_UNORM";
            case asset::EFormat::R16G16B16A16_FLOAT:       return "R16G16B16A16_FLOAT";
            case asset::EFormat::R24_UNORM_X8_TYPELESS:    return "R24_UNORM_X8_TYPELESS";
            case asset::EFormat::R32_TYPELESS:             return "R32_TYPELESS";
            case asset::EFormat::R32_SINT:                 return "R32_SINT";
            case asset::EFormat::R32_UINT:                 return "R32_UINT";
            case asset::EFormat::R32_SNORM:                return "R32_SNORM";
            case asset::EFormat::R32_UNORM:                return "R32_UNORM";
            case asset::EFormat::R32_FLOAT:                return "R32_FLOAT";
            case asset::EFormat::R32_FLOAT_S8X24_TYPELESS: return "R32_FLOAT_S8X24_TYPELESS";
            case asset::EFormat::R32G32B32A32_TYPELESS:    return "R32G32B32A32_TYPELESS";
            case asset::EFormat::R32G32B32A32_SINT:        return "R32G32B32A32_SINT";
            case asset::EFormat::R32G32B32A32_UINT:        return "R32G32B32A32_UINT";
            case asset::EFormat::D24_UNORM_S8_UINT:        return "D24_UNORM_S8_UINT";
            case asset::EFormat::D32_FLOAT:                return "D32_FLOAT";
            case asset::EFormat::D32_FLOAT_S8X24_UINT:     return "D32_FLOAT_S8X24_UINT";
        }
    }
    //<-----------------------------------------------------------------------------
    
    //<-----------------------------------------------------------------------------
    // 
    //<-----------------------------------------------------------------------------
    asset::EFormat fromFormatString(std::string const &aFormat)
    {
        
            if     ("Undefined"                == aFormat) return asset::EFormat::Undefined               ;
            else if("Automatic"                == aFormat) return asset::EFormat::Automatic               ;
            else if("Structured"               == aFormat) return asset::EFormat::Structured              ;
            else if("R8_TYPELESS"              == aFormat) return asset::EFormat::R8_TYPELESS             ;
            else if("R8_SINT"                  == aFormat) return asset::EFormat::R8_SINT                 ;
            else if("R8_UINT"                  == aFormat) return asset::EFormat::R8_UINT                 ;
            else if("R8_SNORM"                 == aFormat) return asset::EFormat::R8_SNORM                ;
            else if("R8_UNORM"                 == aFormat) return asset::EFormat::R8_UNORM                ;
            else if("R8G8B8A8_TYPELESS"        == aFormat) return asset::EFormat::R8G8B8A8_TYPELESS       ;
            else if("R8G8B8A8_SINT"            == aFormat) return asset::EFormat::R8G8B8A8_SINT           ;
            else if("R8G8B8A8_UINT"            == aFormat) return asset::EFormat::R8G8B8A8_UINT           ;
            else if("R8G8B8A8_SNORM"           == aFormat) return asset::EFormat::R8G8B8A8_SNORM          ;
            else if("R8G8B8A8_UNORM"           == aFormat) return asset::EFormat::R8G8B8A8_UNORM          ;
            else if("R8G8B8A8_UNORM_SRGB"      == aFormat) return asset::EFormat::R8G8B8A8_UNORM_SRGB     ;
            else if("R8G8B8A8_FLOAT"           == aFormat) return asset::EFormat::R8G8B8A8_FLOAT          ;
            else if("B8G8R8A8_TYPELESS"        == aFormat) return asset::EFormat::B8G8R8A8_TYPELESS       ;
            else if("B8G8R8A8_SINT"            == aFormat) return asset::EFormat::B8G8R8A8_SINT           ;
            else if("B8G8R8A8_UINT"            == aFormat) return asset::EFormat::B8G8R8A8_UINT           ;
            else if("B8G8R8A8_SNORM"           == aFormat) return asset::EFormat::B8G8R8A8_SNORM          ;
            else if("B8G8R8A8_UNORM"           == aFormat) return asset::EFormat::B8G8R8A8_UNORM          ;
            else if("B8G8R8A8_UNORM_SRGB"      == aFormat) return asset::EFormat::B8G8R8A8_UNORM_SRGB     ;
            else if("B8G8R8A8_FLOAT"           == aFormat) return asset::EFormat::B8G8R8A8_FLOAT          ;
            else if("R16_TYPELESS"             == aFormat) return asset::EFormat::R16_TYPELESS            ;
            else if("R16_SINT"                 == aFormat) return asset::EFormat::R16_SINT                ;
            else if("R16_UINT"                 == aFormat) return asset::EFormat::R16_UINT                ;
            else if("R16_SNORM"                == aFormat) return asset::EFormat::R16_SNORM               ;
            else if("R16_UNORM"                == aFormat) return asset::EFormat::R16_UNORM               ;
            else if("R16_FLOAT"                == aFormat) return asset::EFormat::R16_FLOAT               ;
            else if("R16G16B16A16_TYPELESS"    == aFormat) return asset::EFormat::R16G16B16A16_TYPELESS   ;
            else if("R16G16B16A16_SINT"        == aFormat) return asset::EFormat::R16G16B16A16_SINT       ;
            else if("R16G16B16A16_UINT"        == aFormat) return asset::EFormat::R16G16B16A16_UINT       ;
            else if("R16G16B16A16_SNORM"       == aFormat) return asset::EFormat::R16G16B16A16_SNORM      ;
            else if("R16G16B16A16_UNORM"       == aFormat) return asset::EFormat::R16G16B16A16_UNORM      ;
            else if("R16G16B16A16_FLOAT"       == aFormat) return asset::EFormat::R16G16B16A16_FLOAT      ;
            else if("R24_UNORM_X8_TYPELESS"    == aFormat) return asset::EFormat::R24_UNORM_X8_TYPELESS   ;
            else if("R32_TYPELESS"             == aFormat) return asset::EFormat::R32_TYPELESS            ;
            else if("R32_SINT"                 == aFormat) return asset::EFormat::R32_SINT                ;
            else if("R32_UINT"                 == aFormat) return asset::EFormat::R32_UINT                ;
            else if("R32_SNORM"                == aFormat) return asset::EFormat::R32_SNORM               ;
            else if("R32_UNORM"                == aFormat) return asset::EFormat::R32_UNORM               ;
            else if("R32_FLOAT"                == aFormat) return asset::EFormat::R32_FLOAT               ;
            else if("R32_FLOAT_S8X24_TYPELESS" == aFormat) return asset::EFormat::R32_FLOAT_S8X24_TYPELESS;
            else if("R32G32B32A32_TYPELESS"    == aFormat) return asset::EFormat::R32G32B32A32_TYPELESS   ;
            else if("R32G32B32A32_SINT"        == aFormat) return asset::EFormat::R32G32B32A32_SINT       ;
            else if("R32G32B32A32_UINT"        == aFormat) return asset::EFormat::R32G32B32A32_UINT       ;
            else if("D24_UNORM_S8_UINT"        == aFormat) return asset::EFormat::D24_UNORM_S8_UINT       ;
            else if("D32_FLOAT"                == aFormat) return asset::EFormat::D32_FLOAT               ;
            else if("D32_FLOAT_S8X24_UINT"     == aFormat) return asset::EFormat::D32_FLOAT_S8X24_UINT    ;
        
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    bool STextureFile::acceptSerializer(documents::IJSONSerializer<STextureFile> &aSerializer) const
    {
        aSerializer.beginObject(name);

        aSerializer.writeValue("uid",  uid);
        aSerializer.writeValue("name", name);

        aSerializer.beginArray("textureSourceFilenames");
        for(auto const &assetUID : textureSourceFilenames)
        {
            aSerializer.beginObject("");
            aSerializer.writeValue("filename", assetUID);
            aSerializer.endObject();
        }
        aSerializer.endArray();

        aSerializer.endObject();

        return true;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    bool STextureFile::acceptDeserializer(documents::IJSONDeserializer<STextureFile> &aDeserializer)
    {
        aDeserializer.readValue("uid",                       uid);
        aDeserializer.readValue("name",                      name);

        Vector<std::filesystem::path> textureFilenames {};
        uint32_t            filenameCount    = 0;
        aDeserializer.beginArray("textureSourceFilenames", filenameCount);
        for( uint32_t k=0; k < filenameCount; ++k)
        {
            aDeserializer.beginObject(k);

            std::string filename {};
            aDeserializer.readValue("filename", filename);
            std::filesystem::path p = ( std::filesystem::path(".") / filename).lexically_normal();
            textureFilenames.push_back(p);

            aDeserializer.endObject();
        }
        aDeserializer.endArray();
        textureSourceFilenames = textureFilenames;

        aDeserializer.endObject();

        return true;
    }

    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    bool STextureAsset::acceptSerializer(documents::IJSONSerializer<STextureAsset> &aSerializer) const
    {
        aSerializer.beginObject(name);

        aSerializer.writeValue("uid",  uid);
        aSerializer.writeValue("name", name);

        std::string formatString = toFormatString(textureInfo.format);

        aSerializer.beginObject("textureInfo");
        aSerializer.writeValue("width",                 textureInfo.width);
        aSerializer.writeValue("height",                textureInfo.height);
        aSerializer.writeValue("depth",                 textureInfo.depth);
        aSerializer.writeValue("channels",              textureInfo.channels);
        aSerializer.writeValue("bitsPerChannel",        textureInfo.bitsPerChannel);
        aSerializer.writeValue("format",                formatString);
        aSerializer.writeValue("arraySize",             textureInfo.arraySize);
        aSerializer.writeValue("mipLevels",             textureInfo.mipLevels);
        aSerializer.writeValue("multisampling_quality", textureInfo.multisampling.quality);
        aSerializer.writeValue("multisampling_size",    textureInfo.multisampling.size);
        aSerializer.endObject();

        aSerializer.writeValue("binaryFilenameAssetUID", imageLayersBinaryUid);

        aSerializer.endObject();

        return true;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    bool STextureAsset::acceptDeserializer(documents::IJSONDeserializer<STextureAsset> &aDeserializer)
    {
        aDeserializer.readValue("uid",                       uid);
        aDeserializer.readValue("name",                      name);

        std::string formatString = "";

        aDeserializer.beginObject("textureInfo");
        aDeserializer.readValue("width",                 textureInfo.width);
        aDeserializer.readValue("height",                textureInfo.height);
        aDeserializer.readValue("depth",                 textureInfo.depth);
        aDeserializer.readValue("channels",              textureInfo.channels);
        aDeserializer.readValue("bitsPerChannel",        textureInfo.bitsPerChannel);
        aDeserializer.readValue("format",                formatString);
        aDeserializer.readValue("arraySize",             textureInfo.arraySize);
        aDeserializer.readValue("mipLevels",             textureInfo.mipLevels);
        aDeserializer.readValue("multisampling_quality", textureInfo.multisampling.quality);
        aDeserializer.readValue("multisampling_size",    textureInfo.multisampling.size);
        aDeserializer.endObject();

        textureInfo.format = fromFormatString(formatString);

        aDeserializer.readValue("binaryFilenameAssetUID", imageLayersBinaryUid);

        aDeserializer.endObject();

        return true;
    }
    //<-----------------------------------------------------------------------------
}
