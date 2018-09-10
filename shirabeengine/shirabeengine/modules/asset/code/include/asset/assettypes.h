#ifndef __SHIRABE_ASSET_TYPES_H__
#define __SHIRABE_ASSET_TYPES_H__

#include <stdint.h>
#include <string>

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

        /**
         * The EAssetType enum describes rough asset categorization.
         */
        enum class EAssetType
        {
            Undefined = 0,
            Mesh      = 1,
            Material  = 2,
            Texture   = 3,
            Buffer    = 4,
        };
        
        /**
         * The SAsset struct is the common descriptor for an engine asset.
         */
        struct SAsset
        {
        public_members:
            AssetId_t   id;
            EAssetType  type;
            std::string URI;
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
         * The asset registry is the engine wide container for a specific engine asset type.
         *
         * @tparam T Underlying asset resource type of the registry.
         */
        template <typename T>
        class CAssetRegistry
        {
        private_typedefs:
            using Index_t = Map<AssetId_t, T>;

        public_typedefs:
            typedef typename Index_t::value_type value_type;

        public_methods:
            /**
             * Dynamically add an asset by Id to the registry.
             *
             * @param aAssetId UID of the new asset.
             * @param aAsset   Asset to add.
             * @return         EAssetErrorCode::Ok, if successful. An error code otherwise.
             */
            EAssetErrorCode addAsset(
                    AssetId_t const &aAssetId,
                    T         const &aAsset);

            /**
             * Try to fetch a previously registered asset.
             *
             * @param aAssetId UID of the asset to fetch.
             * @return         A filled optional of requested asset type T, or an empty optional.
             */
            Optional_t<T> getAsset(AssetId_t const &aAssetId);

            // Iterator compatibility
            typename Index_t::iterator       begin()       { return mIndex.begin(); }
            typename Index_t::iterator       end()         { return mIndex.end();   }
            typename Index_t::const_iterator begin() const { return mIndex.begin(); }
            typename Index_t::const_iterator end()   const { return mIndex.end();   }

        private_members:
            Index_t mIndex;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        EAssetErrorCode CAssetRegistry<T>::addAsset(
                AssetId_t const &aAssetId,
                T         const &aAsset)
        {
            if(mIndex.find(aAssetId) != mIndex.end())
                return EAssetErrorCode::AssetAlreadyAdded;

            mIndex[aAssetId] = aAsset;

            return EAssetErrorCode::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        Optional_t<T> CAssetRegistry<T>::getAsset(AssetId_t const &aAssetId)
        {
            Optional_t<T> result{ };

            if(mIndex.find(aAssetId) != mIndex.end())
                result = mIndex.at(aAssetId);

            return result;
        }
        //<-----------------------------------------------------------------------------
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
