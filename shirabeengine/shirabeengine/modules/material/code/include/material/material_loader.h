#ifndef __SHIARBE_MATERIAL_LOADER_H__
#define __SHIARBE_MATERIAL_LOADER_H__

#include <log/log.h>
#include <core/enginestatus.h>

namespace engine
{
    namespace asset
    {
        using AssetID_t = uint64_t;

        class IAssetStorage;
    }

    namespace material
    {
        class CMaterial;
        class CMaterialInstance;

        /**
         * @brief The CMaterialLoader class
         */
        class CMaterialLoader
        {
            SHIRABE_DECLARE_LOG_TAG(CMaterialLoader);

        public_constructors:
            /**
             * @brief CMaterialLoader
             * @param aAssetStorage
             */
            CMaterialLoader(CStdSharedPtr_t<asset::IAssetStorage> &aAssetStorage);

        public_methods:

            /**
             * During editing-time, request the creation of a new material instance based on a base material asset id.
             * This will register a new material instance in the asset manager and load the base material.
             *
             * @param aNewMaterialAssetId
             * @param aBaseMaterialAssetId
             * @return
             */
            CEngineResult<CStdSharedPtr_t<CMaterialInstance>> createMaterialInstance(asset::AssetID_t const &aMasterMaterialAssetId);

            /**
             * @brief loadMaterial
             * @param aMaterialId
             * @return
             */
            CEngineResult<CStdSharedPtr_t<CMaterialInstance>> loadMaterialInstance(asset::AssetID_t const &aMaterialInstanceAssetId);

            /**
             * @brief destroyMaterialInstance
             * @param aMaterialInstanceAssetId
             * @return
             */
            CEngineResult<> destroyMaterialInstance(asset::AssetID_t const &aMaterialInstanceAssetId);

        private_methods:
            CStdSharedPtr_t<asset::IAssetStorage>                    &mStorage;
            Map<asset::AssetID_t, CStdSharedPtr_t<CMaterial>>         mInstantiatedMaterialMasters;
            Map<asset::AssetID_t, CStdSharedPtr_t<CMaterialInstance>> mInstantiatedMaterialInstances;
        };

    }
}

#endif // MATERIAL_LOADER_H
