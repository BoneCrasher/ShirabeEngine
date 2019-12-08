#ifndef __SHIARBE_MESH_LOADER_H__
#define __SHIARBE_MESH_LOADER_H__

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

    namespace mesh
    {
        struct SMaterialMeta;
        class CMeshInstance;

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

            CEngineResult <Shared<CMeshInstance>> createMeshInstance(asset::AssetID_t const &aMeshAssetId);

            CEngineResult <SMeshMeta> loadMeshMeta(Shared<asset::IAssetStorage> const &aAssetStorage, asset::AssetID_t const &aMeshAssetId);

            CEngineResult <Shared<CMeshInstance>> loadMeshInstance( Shared<asset::IAssetStorage> const &aAssetStorage
                                                                  , asset::AssetID_t             const &aMeshAssetId);
            /**
             * @brief loadMaterial
             * @param aMaterialId
             * @return
             */
            CEngineResult <Shared<CMeshInstance>> loadMeshInstance( std::string                  const &aMeshInstanceId
                                                                      , Shared<asset::IAssetStorage> const &aAssetStorage
                                                                      , asset::AssetID_t             const &aMeshAssetId);

            /**
             * @brief destroyMaterialInstance
             * @param aMaterialInstanceAssetId
             * @return
             */
            CEngineResult<> destroyMeshInstance(asset::AssetID_t const &aMeshAssetId);

        private_methods:

        private_members:
            Map <asset::AssetID_t, Shared<CMeshInstance>> mInstantiatedMeshes;
        };

    }
}

#endif // MATERIAL_LOADER_H
