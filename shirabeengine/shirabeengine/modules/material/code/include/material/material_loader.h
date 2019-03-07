#ifndef __SHIARBE_MATERIAL_LOADER_H__
#define __SHIARBE_MATERIAL_LOADER_H__

#include <log/log.h>
#include <core/enginestatus.h>
#include <asset/assetstorage.h>

namespace engine
{
    namespace material
    {

        class CMaterialLoader
                : public asset::IAssetLoader
        {
            SHIRABE_DECLARE_LOG_TAG(CMaterialLoader);

        public_methods:
            /**
             * See IAssetLoader::loadAsset.
             *
             * @param aAsset
             * @return
             */
            CEngineResult<ByteBuffer> loadAsset(asset::SAsset const &aAsset);

        private_methods:
        };

    }
}

#endif // MATERIAL_LOADER_H
