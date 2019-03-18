#ifndef __SHIRABE_ASSET_TYPES_H__
#define __SHIRABE_ASSET_TYPES_H__

#include <stdint.h>
#include <string>
#include <filesystem>

#include <core/enginetypehelper.h>
#include <core/databuffer.h>
#include <graphicsapi/resources/types/texture.h>
#include "asset/asseterror.h"

namespace engine
{
    namespace asset
    {
        /**
         * UniqueId type to identify assets.
         */
        using AssetId_t = uint64_t;

        constexpr AssetId_t operator "" _uid( unsigned long long aUnit )
        {
          return static_cast<AssetId_t>(aUnit);
        }

        /**
         * The EAssetType enum describes rough asset categorization.
         */
        enum class EAssetType
        {
            Undefined        = 0,
            Mesh                ,
            MaterialMaster      ,
            MaterialInstance    ,
            Texture             ,
            Buffer              ,
        };
        
        /**
         * The SAsset struct is the common descriptor for an engine asset.
         */
        struct SAsset
        {
        public_members:
            AssetId_t             id;
            EAssetType            type;
            std::filesystem::path uri;
        };

        /**
         * The STextureAsset struct describes any kind of engine texture asset
         * including texture information.
         */
        struct STextureAsset
        {
        public_members:
            std::string          name;
            gfxapi::STextureInfo textureInfo;
        };

        /**
         * The BufferAsset struct describes any kind of engine buffer assets
         * TBDone: SBufferInfo struct.
         */
        struct SBufferAsset
        {
        public_members:
            std::string name;
        };

        /**
         * The SImage struct describes a raw image resource.
         */
        struct SImage
        {
        public_members:
            ByteBuffer data;
            uint32_t   width, height, channels;
        };
    }

    /**
     * Extract an EAssetType from string.
     *
     * @param aInput Input string.
     * @return       A EAssetType representation of the input string.
     *               If not matched, EAssetType::Undefined is returned.
     */
    template <> asset::EAssetType from_string<asset::EAssetType>(std::string const &aInput);
}

#endif
