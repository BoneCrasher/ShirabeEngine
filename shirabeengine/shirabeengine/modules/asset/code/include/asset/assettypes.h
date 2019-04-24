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
            Material            ,
            Texture             ,
            Buffer              ,
        };

        /**
         * The EAssetType enum describes detailed asset categorization.
         */
        enum class EAssetSubtype
        {
            Undefined = 0,
            /** Material **/
            Master    = 10,
            Signature,
            Config,
            SPVModule,
            Instance
        };
        
        /**
         * The SAsset struct is the common descriptor for an engine asset.
         */
        struct SAsset
        {
        public_members:
            AssetId_t             id;
            AssetId_t             parent;
            EAssetType            type;
            EAssetSubtype         subtype;
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

        /**
         * Base class of all created assets to store the referenced
         * asset id.
         */
        class CAssetReference
        {
        public_constructors:
            SHIRABE_INLINE
            CAssetReference(AssetId_t const &aAssetId)
                : mAssetId(aAssetId)
            {}

            SHIRABE_INLINE
            AssetId_t const &getAssetId() const
            {
                return mAssetId;
            }

        private_members:
            AssetId_t mAssetId;
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

    /**
     * Extract an EAssetSubtype from string.
     *
     * @param aInput Input string.
     * @return       A EAssetType representation of the input string.
     *               If not matched, EAssetType::Undefined is returned.
     */
    template <> asset::EAssetSubtype from_string<asset::EAssetSubtype>(std::string const &aInput);
}

#endif
