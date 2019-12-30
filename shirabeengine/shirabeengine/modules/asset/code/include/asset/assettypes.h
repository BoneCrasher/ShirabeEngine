#ifndef __SHIRABE_ASSET_TYPES_H__
#define __SHIRABE_ASSET_TYPES_H__

#include <cstdint>
#include <string>
#include <filesystem>

#include <core/enginetypehelper.h>
#include <core/databuffer.h>
#include <graphicsapi/definitions.h>
#include "asset/asseterror.h"

namespace engine
{
    class CRange;

    namespace asset
    {
        using namespace graphicsapi;

        /**
         * UniqueId type to identify assets.
         */
        using AssetId_t = uint32_t;

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
            Meta,
            /** Material **/
            Master    = 10,
            Signature,
            Config,
            SPVModule,
            Instance,
            /** Meshes **/
            AttributeBuffer = 20,
            IndexBuffer,
            DataFile
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
         * Generate CRC32-Uid from the asset URI.
         *
         * @param aPath
         * @return
         */
        AssetId_t assetIdFromUri(std::filesystem::path const &aUri);


        /**
         * The SMultisapmling struct describes multisampling properties for various gfxapi
         * related components in the system.
         */
        struct SMultisapmling
        {
        public_members:
            uint8_t
                    size,
                    quality;
        };

        /**
         * The STextureInfo struct describes general texture attributes.
         */
        //struct SHIRABE_TEST_EXPORT STextureInfo
        //{
        //public_constructors:
        //    STextureInfo();
//
        //public_methods:
        //    void assignTextureInfoParameters(STextureInfo const&other);
//
        //public_members:
        //    uint32_t
        //            width,  // 0 - Undefined
        //            height, // At least 1
        //            depth,  // At least 1
        //            channels,
        //            bitsPerChannel;
        //    EFormat
        //            format;
        //    uint16_t
        //            arraySize; // At least 1 (basically everything is a vector...)
        //    uint16_t
        //            mipLevels;
        //    SMultisapmling
        //            multisampling;
        //};

        /**
         * The STextureAsset struct describes any kind of engine texture asset
         * including texture information.
         */
        struct STextureAsset
        {
        public_members:
            std::string  name;
            STextureInfo textureInfo;
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
            explicit CAssetReference(AssetId_t const &aAssetId)
                : mAssetId(aAssetId)
            {}

            [[nodiscard]]
            SHIRABE_INLINE
            AssetId_t const &getAssetId() const
            {
                return mAssetId;
            }

            SHIRABE_INLINE
            CAssetReference& operator=(AssetId_t const &aId)
            {
                mAssetId = aId;
                return (*this);
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

    /**
     * Extract an EAssetType from string.
     *
     * @param aInput Input string.
     * @return       A EAssetType string representation.
     *               If not matched, EAssetType::Undefined is returned.
     */
    template <> std::string convert_to_string<asset::EAssetType>(asset::EAssetType const &aInput);

    /**
     * Convert an EAssetSubtype to string.
     *
     * @param aInput Input string.
     * @return       A EAssetType string representation.
     *               If not matched, EAssetType::Undefined is returned.
     */
    template <> std::string convert_to_string<asset::EAssetSubtype>(asset::EAssetSubtype const &aInput);
}

#endif
