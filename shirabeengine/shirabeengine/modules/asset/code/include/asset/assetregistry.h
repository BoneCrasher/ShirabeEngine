#ifndef __SHIRABE_ASSET_REGISTRY_H__
#define __SHIRABE_ASSET_REGISTRY_H__

#include "assettypes.h"

namespace engine
{
    namespace asset
    {

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
            CEngineResult<T> getAsset(AssetId_t const &aAssetId);

            /**
             * Try to fetch a previously registered asset.
             *
             * @param aAssetId UID of the asset to fetch.
             * @return         A filled optional of requested asset type T, or an empty optional.
             */
            CEngineResult<T const> getAsset(AssetId_t const &aAssetId) const;


            /**
             * Remove an asset from the index, if exists.
             *
             * @param aAssetId
             * @return
             */
            EAssetErrorCode removeAsset(AssetId_t const &aAssetId);

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
            if(mIndex.end() != mIndex.find(aAssetId))
            {
                return EAssetErrorCode::AssetAlreadyAdded;
            }

            mIndex[aAssetId] = aAsset;

            return EAssetErrorCode::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CEngineResult<T> CAssetRegistry<T>::getAsset(AssetId_t const &aAssetId)
        {
            CEngineResult<T> result = CEngineResult<T>(EEngineStatus::Error);

            if(mIndex.end() != mIndex.find(aAssetId))
            {
                T const &asset = mIndex.at(aAssetId);

                result = CEngineResult<T>(EEngineStatus::Ok, asset);
            }

            return result;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CEngineResult<T const> CAssetRegistry<T>::getAsset(AssetId_t const &aAssetId) const
        {
            CEngineResult<T const> result = CEngineResult<T>(EEngineStatus::Error);

            if(mIndex.end() != mIndex.find(aAssetId))
            {
                T const &asset = mIndex.at(aAssetId);

                result = CEngineResult<T>(EEngineStatus::Ok, asset);
            }

            return result;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        EAssetErrorCode CAssetRegistry<T>::removeAsset(AssetId_t const &aAssetId)
        {
            if(mIndex.end() == mIndex.find(aAssetId))
            {
                return EAssetErrorCode::AssetNotFound;
            }

            mIndex.erase(aAssetId);

            return EAssetErrorCode::Ok;
        }
        //<-----------------------------------------------------------------------------

    }
}

#endif // __SHIRABE_ASSET_REGISTRY_H__
