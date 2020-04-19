#ifndef __SHIRABE_BUFFER_LOADER_H__
#define __SHIRABE_BUFFER_LOADER_H__

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

    namespace buffers
    {
        class CBufferInstance;
        class SBufferAsset;

        /**
         * @brief The CMaterialLoader class
         */
        class SHIRABE_LIBRARY_EXPORT CBufferLoader
        {
            SHIRABE_DECLARE_LOG_TAG(CBufferLoader);

        public_constructors:

            /**
             * @brief CBufferLoader
             * @param aAssetStorage
             */
            explicit CBufferLoader();

        public_methods:

            CEngineResult <Shared<CBufferInstance>> createInstance(asset::AssetID_t const &aAssetId);

            CEngineResult <Shared<CBufferInstance>> loadInstance( Shared<asset::IAssetStorage> const &aAssetStorage
                                                                , asset::AssetID_t             const &aAssetId);
            /**
             * @brief loadMaterial
             * @param aMaterialId
             * @return
             */
            CEngineResult <Shared<CBufferInstance>> loadInstance( std::string                  const &aInstanceId
                                                                , Shared<asset::IAssetStorage> const &aAssetStorage
                                                                , asset::AssetID_t             const &aAssetId);

            /**
             * @brief destroyMaterialInstance
             * @param aMaterialInstanceAssetId
             * @return
             */
            CEngineResult<> destroyInstance(asset::AssetID_t const &aAssetId);

        private_methods:

        private_members:
            Map <asset::AssetID_t, Shared<CBufferInstance>> mInstances;
        };

    }
}

#endif // MATERIAL_LOADER_H
