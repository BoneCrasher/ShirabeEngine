#ifndef __SHIRABE_ASSET_IASSETDATASOURCE_H__
#define __SHIRABE_ASSET_IASSETDATASOURCE_H__

#include <core/enginetypehelper.h>
#include <core/enginestatus.h>

#include "assettypes.h"

namespace engine
{
    namespace asset
    {

        class IAssetDataSource
        {
            SHIRABE_DECLARE_INTERFACE(IAssetDataSource);

        public_api:
            /**
             * @brief readAsset
             * @param aPath
             * @return
             */
            virtual CEngineResult<ByteBuffer> readAsset(std::filesystem::path const &aPath) = 0;

            /**
             * @brief writeAsset
             * @param aPath
             * @param aBuffer
             * @return
             */
            virtual CEngineResult<> writeAsset(std::filesystem::path const &aPath, ByteBuffer const &aBuffer) = 0;

        };

    }
}

#endif // __SHIARBE_ASSET_IASSETDATASOURCE_H__
