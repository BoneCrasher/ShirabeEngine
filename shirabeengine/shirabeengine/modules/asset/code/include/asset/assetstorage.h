#ifndef __SHIRABE_ASSET_STORAGE_H__
#define __SHIRABE_ASSET_STORAGE_H__

#include <core/enginetypehelper.h>
#include <core/databuffer.h>
#include <graphicsapi/resources/types/texture.h>
#include "asset/asseterror.h"
#include "asset/assettypes.h"

namespace engine
{
    namespace asset
    {
        /**
         * The IAssetLoader interface defines requirements and API for asset loading
         * implementations, which are registered from external components.
         */
        class IAssetLoader
        {
            SHIRABE_DECLARE_INTERFACE(IAssetLoader);

        public_api:
            /**
             * Load the specified asset, using the individual loader implementation.
             *
             * @param aAsset
             * @return
             */
            virtual CEngineResult<ByteBuffer> loadAsset(SAsset const &aAsset) = 0;
        };

        /**
         * The IAssetStorage class describes the basic means of interaction with an asset storage.
         */
        class IAssetStorage
        {
            SHIRABE_DECLARE_INTERFACE(IAssetStorage);

        public_api:
            /**
             * Create a dynamic texture asset on disk during editing/runtime.
             *
             * @param aAssetName   Name of the texture asset to create.
             * @param aTextureInfo Texture information for resource creation.
             * @return             A valid AssetId (> 0) if successful. 0 otherwise.
             */
            virtual AssetId_t createDynamicTextureAsset(
                    std::string          const &aAssetName,
                    gfxapi::STextureInfo const &aTextureInfo) = 0;

            /**
             * Load an asset from the respective asset source.
             *
             * @param aAssetUID The UID of the asset to load.
             * @return          A valid asset if successful. Empty otherwise.
             */
            virtual CEngineResult<SAsset> loadAsset(AssetId_t const &aAssetUID) = 0;

            /**
             * Load the byte data for a provided asset descriptor.
             *
             * @param aAsset The asset descriptor for which byte data should be loaded.
             * @return       A filled byte buffer if successful. False otherwise.
             */
            virtual CEngineResult<ByteBuffer> loadAssetData(SAsset const &aAsset) = 0;
        };

        /**
         * The AssetStorage class provides a default asset storage implementation.
         */
        class SHIRABE_TEST_EXPORT CAssetStorage
                : public IAssetStorage
        {
        private_typedefs:
            using TextureAssetData = CAssetRegistry<STextureAsset>;
            using BufferAssetData  = CAssetRegistry<SBufferAsset>;

        public_typedefs:
            using AssetIndex_t = CAssetRegistry<SAsset>;

        public_constructors:
            CAssetStorage();

        public_methods:
            /**
             * Register an asset loader for a specific asset type.
             *
             * @param aAssetType The asset type to register for.
             * @param aLoader    The loader for the respective asset type.
             * @return           EEngineStatus::Ok, if successful.
             * @return           EEngineStatus error code on error.
             */
            CEngineResult<> registerAssetLoader(EAssetType const &aAssetType, CStdSharedPtr_t<IAssetLoader> const aLoader);

            /**
             * Read an asset index into this storage.
             *
             * @param aIndex
             */
            void readIndex(AssetIndex_t const &aIndex);

            /**
             * Create a dynamic texture asset on disk during editing/runtime.
             *
             * @param aAssetName   Name of the texture asset to create.
             * @param aTextureInfo Texture information for resource creation.
             * @return             A valid AssetId (> 0) if successful. 0 otherwise.
             */
            AssetId_t createDynamicTextureAsset(
                    std::string          const &aName,
                    gfxapi::STextureInfo const &aTextureInfo);

            /**
             * Load an asset from the respective asset source.
             *
             * @param aAssetUID The UID of the asset to load.
             * @return          A valid asset if successful. Empty otherwise.
             */
            CEngineResult<SAsset> loadAsset(AssetId_t const &aAssetUID);

            /**
             * Load the byte data for a provided asset descriptor.
             *
             * @param aAsset The asset descriptor for which byte data should be loaded.
             * @return       A filled byte buffer if successful. False otherwise.
             */
            CEngineResult<ByteBuffer> loadAssetData(SAsset const &aAsset);

        private_methods:
            /**
             * Implementation of loading a buffer asset from disc.
             *
             * @param aAsset Asset descriptor of the buffer resource.
             * @return       A filled bytebuffer if successful. An empty buffer othwerise.
             */
            CEngineResult<ByteBuffer> loadBufferAsset(SAsset const &aAsset);

            /**
             * Implementation of loading texture assets from disc.
             *
             * @param aAsset Asset descriptor of the buffer resource.
             * @return       A filled bytebuffer if successful. An empty buffer othwerise.
             */
            CEngineResult<ByteBuffer> loadTextureAsset(SAsset const &aAsset);

        private_members:
            AssetIndex_t     mAssetIndex;
            TextureAssetData mTextureAssets;
            BufferAssetData  mBufferAssets;

            Map<EAssetType, CStdSharedPtr_t<IAssetLoader>> mLoaders;
        };
    }
}

#endif
