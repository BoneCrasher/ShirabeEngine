#ifndef __SHIARBE_TEXTURE_LOADER_H__
#define __SHIARBE_TEXTURE_LOADER_H__

#include <log/log.h>
#include <core/enginestatus.h>
#include <resources/resourcedescriptions.h>

namespace engine
{
    namespace asset
    {
        using AssetID_t = uint64_t;

        class IAssetStorage;
    }

    namespace textures
    {
        struct STextureMeta;
        class  CTextureInstance;

        /**
         * @brief The CMaterialLoader class
         */
        class SHIRABE_LIBRARY_EXPORT CTextureLoader
        {
            SHIRABE_DECLARE_LOG_TAG(CTextureLoader);

        public_constructors:

            /**
             * @brief CTextureLoader
             * @param aAssetStorage
             */
            explicit CTextureLoader();

        public_methods:

            CEngineResult <Shared<CTextureInstance>> createInstance(asset::AssetID_t const &aAssetId);

            CEngineResult <STextureMeta> loadMeta(Shared<asset::IAssetStorage> const &aAssetStorage, asset::AssetID_t const &aAssetId);

            CEngineResult <Shared<CTextureInstance>> loadInstance( Shared<asset::IAssetStorage> const &aAssetStorage
                                                                 , asset::AssetID_t             const &aAssetId);

            CEngineResult<> destroyInstance(asset::AssetID_t const &aAssetId);

        private_members:
            Map <asset::AssetID_t, Shared<CTextureInstance>> mInstantiatedInstances;
        };

    }
}

#endif // MATERIAL_LOADER_H
