#ifndef __SHIARBE_MESH_LOADER_H__
#define __SHIARBE_MESH_LOADER_H__

#include <log/log.h>
#include <core/enginestatus.h>

namespace engine
{
    namespace asset
    {
        using AssetID_t = uint64_t;

        class IAssetStorage;
    }

    namespace mesh
    {
        class CMeshInstance;
        class SMeshAsset;

        /**
         * @brief The CMaterialLoader class
         */
        class SHIRABE_LIBRARY_EXPORT CMeshLoader
        {
            SHIRABE_DECLARE_LOG_TAG(CMeshLoader);

        public_constructors:

            /**
             * @brief CMaterialLoader
             * @param aAssetStorage
             */
            explicit CMeshLoader();

        public_methods:

            CEngineResult <Shared<CMeshInstance>> createInstance(asset::AssetID_t const &aAssetId);

            CEngineResult <Shared<CMeshInstance>> loadInstance( Shared<asset::IAssetStorage> const &aAssetStorage
                    , asset::AssetID_t             const &aAssetId);
            /**
             * @brief loadMaterial
             * @param aMaterialId
             * @return
             */
            CEngineResult <Shared<CMeshInstance>> loadInstance( std::string                  const &aMeshInstanceId
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
            Map <asset::AssetID_t, Shared<CMeshInstance>> mInstances;
        };

    }
}

#endif // MATERIAL_LOADER_H
