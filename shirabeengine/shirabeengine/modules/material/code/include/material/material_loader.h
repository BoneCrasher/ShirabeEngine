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
             * @brief loadMaterial
             * @param aMaterialId
             * @return
             */
            CEngineResult<CStdSharedPtr_t<CMaterial>> loadMaterial(std::string const &aMaterialId);

        private_methods:
            CStdSharedPtr_t<asset::IAssetStorage> &mStorage;
            Map<std::string, asset::AssetID_t>     mMaterial2AssetMapping;
        };

    }
}

#endif // MATERIAL_LOADER_H
