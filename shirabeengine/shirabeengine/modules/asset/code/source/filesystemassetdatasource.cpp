#include "asset/filesystemassetdatasource.h"
#include <core/helpers.h>

namespace engine
{
    namespace asset
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CFileSystemAssetDataSource::CFileSystemAssetDataSource(std::filesystem::path const &aAssetSourcePath)
            : mAssetSourcePath(aAssetSourcePath)
        { }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<ByteBuffer> CFileSystemAssetDataSource::readAsset(std::filesystem::path const &aPath)
        {
            std::vector<uint8_t> fileContents = readFileBytes(aPath);
            if(fileContents.empty())
            {
                return { EEngineStatus::Error };
            }

            ByteBuffer buffer(std::move(fileContents), fileContents.size());

            return { EEngineStatus::Ok, std::move(buffer) };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFileSystemAssetDataSource::writeAsset(std::filesystem::path const &aPath, ByteBuffer const &aBuffer)
        {
            return { EEngineStatus::Error };
        }
        //<-----------------------------------------------------------------------------

    }
}
