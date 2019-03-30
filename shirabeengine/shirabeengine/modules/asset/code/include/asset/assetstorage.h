#ifndef __SHIRABE_ASSET_STORAGE_H__
#define __SHIRABE_ASSET_STORAGE_H__

#include <core/enginetypehelper.h>
#include <core/databuffer.h>
#include <graphicsapi/resources/types/texture.h>

#include "asset/iassetdatasource.h"
#include "asset/asseterror.h"
#include "asset/assetregistry.h"

namespace engine
{
    namespace asset
    {
        /**
         * The IAssetStorage class describes the basic means of interaction with an asset storage.
         */
        class IAssetStorage
        {
            SHIRABE_DECLARE_INTERFACE(IAssetStorage);

        public_api:

            /**
             * Determine an asset by Uid.
             *
             * @param aUri
             * @return
             */
            virtual CEngineResult<SAsset> assetFromUri(std::filesystem::path const &aUri) = 0;

            /**
             * Register an asset in the index and write the provided byte buffer as asset
             * data to the harddisk (if not empty).
             *
             * @param aAssetType
             * @param aAssetDirectory
             * @return
             */
            virtual AssetId_t createAsset(EAssetType  const &aAssetType,
                                          std::string const &aAssetDirectory,
                                          std::string const &aAssetFilename,
                                          ByteBuffer  const &aInitialData = {}) = 0;


            /**
             * Load an asset from the respective asset source.
             *
             * @param aAssetUID The UID of the asset to load.
             * @return          A valid asset if successful. Empty otherwise.
             */
            virtual CEngineResult<SAsset > loadAsset(AssetId_t const &aAssetUID) = 0;

            /**
             * Load the byte data for a provided asset descriptor.
             *
             * @param aAsset The asset descriptor for which byte data should be loaded.
             * @return       A filled byte buffer if successful. False otherwise.
             */
            virtual CEngineResult<ByteBuffer> loadAssetData(AssetId_t const &aAsset) = 0;

            /**
             * Unload this asset and remove it's data from the index.
             * Note: This won't delete the data from the hard disk.
             *
             * @param aAssetUID
             * @return
             */
            virtual CEngineResult<> removeAsset(AssetId_t const &aAssetUID) = 0;
        };

        /**
         * The AssetStorage class provides a default asset storage implementation.
         */
        class SHIRABE_TEST_EXPORT CAssetStorage
                : public IAssetStorage
        {
        public_typedefs:
            using AssetRegistry_t = CAssetRegistry<SAsset>;

        public_constructors:
            CAssetStorage(CStdUniquePtr_t<IAssetDataSource> &&aAssetDataSource);

        public_methods:
            /**
             * Read an asset index into this storage.
             *
             * @param aIndex
             */
            void readIndex(AssetRegistry_t const &aIndex);

            /**
             * Determine an asset by Uid.
             *
             * @param aUri
             * @return
             */
            CEngineResult<SAsset> assetFromUri(std::filesystem::path const &aUri);

            /**
             * @brief createAsset
             *
             * @param aAssetType
             * @param aAssetDirectory
             * @return
             */
            AssetId_t createAsset(EAssetType  const &aAssetType,
                                  std::string const &aAssetDirectory,
                                  std::string const &aAssetFilename,
                                  ByteBuffer  const &aInitialData);

            /**
             * Load an asset from the respective asset source.
             *
             * @param aAssetUID The UID of the asset to load.
             * @return          A valid asset if successful. Empty otherwise.
             */
            CEngineResult<SAsset > loadAsset(AssetId_t const &aAssetUID);

            /**
             * Load the byte data for a provided asset descriptor.
             *
             * @param aAsset The asset descriptor for which byte data should be loaded.
             * @return       A filled byte buffer if successful. False otherwise.
             */
            CEngineResult<ByteBuffer> loadAssetData(AssetId_t const &aAsset);

            /**
             * Unload this asset and remove it's data from the index.
             * Note: This won't delete the data from the hard disk.
             *
             * @param aAssetUID
             * @return
             */
            CEngineResult<> removeAsset(AssetId_t const &aAssetUID);

        private_members:
            AssetRegistry_t                   mAssetIndex;
            CStdUniquePtr_t<IAssetDataSource> mAssetDataSource;
        };        

    }
}

#endif
