#ifndef __SHIRABE_ASSET_FILESYSTEMASSETDATASOURCE_H__
#define __SHIRABE_ASSET_FILESYSTEMASSETDATASOURCE_H__

#include "iassetdatasource.h"

namespace engine
{
    namespace asset
    {

        class CFileSystemAssetDataSource
                : public IAssetDataSource
        {
        public_constructors:
            CFileSystemAssetDataSource(std::filesystem::path const &aAssetSourcePath);

        public_destructors:
            ~CFileSystemAssetDataSource() = default;

        public_methods:
            CEngineResult<ByteBuffer> readAsset(std::filesystem::path const &aPath);

            CEngineResult<> writeAsset(std::filesystem::path const &aPath, ByteBuffer const &aBuffer);

        private_members:
            std::filesystem::path mAssetSourcePath;
        };

    }
}

#endif // __SHIRABE_ASSET_FILESYSTEMASSETDATASOURCE_H__
