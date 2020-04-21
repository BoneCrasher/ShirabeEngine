#ifndef __SHIARBE_MATERIAL_LOADER_H__
#define __SHIARBE_MATERIAL_LOADER_H__

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

    namespace material
    {
        struct SMaterialAsset;
        class  CMaterialMaster;
        class  CMaterialInstance;

        /**
         * @brief The CMaterialLoader class
         */
        class SHIRABE_LIBRARY_EXPORT CMaterialLoader
        {
            SHIRABE_DECLARE_LOG_TAG(CMaterialLoader);

        public_constructors:

            /**
             * @brief CMaterialLoader
             * @param aAssetStorage
             */
            explicit CMaterialLoader();

        public_methods:

            /**
             * During editing-time, request the creation of a new material instance based on a base material asset id.
             * This will register a new material instance in the asset manager and load the base material.
             *
             * @param aNewMaterialAssetId
             * @param aBaseMaterialAssetId
             * @return
             */
            CEngineResult <Shared<CMaterialInstance>> createInstance(asset::AssetID_t const &aAssetId);

            CEngineResult <Shared<CMaterialInstance>> loadInstance( Shared<asset::IAssetStorage> const &aAssetStorage
                                                                  , asset::AssetID_t             const &aMaterialInstanceAssetId
                                                                  , bool                                aAutoCreateConfiguration
                                                                  , bool                                aIncludeSystemBuffers = false);
            /**
             * @brief loadMaterial
             * @param aMaterialId
             * @return
             */
            CEngineResult <Shared<CMaterialInstance>> loadInstance( std::string                  const &aMaterialInstanceId
                                                                  , Shared<asset::IAssetStorage> const &aAssetStorage
                                                                  , asset::AssetID_t             const &aMaterialInstanceAssetId
                                                                  , bool                                aAutoCreateConfiguration);

            /**
             * @brief destroyMaterialInstance
             * @param aMaterialInstanceAssetId
             * @return
             */
            CEngineResult<> destroyInstance(asset::AssetID_t const &aMaterialInstanceAssetId);

        private_methods:

        private_members:
            Map <asset::AssetID_t, Shared<CMaterialMaster>>          mMasterInstances;
            Map <resources::ResourceId_t, Shared<CMaterialInstance>> mInstanceInstances;
        };

    }
}

#endif // MATERIAL_LOADER_H
