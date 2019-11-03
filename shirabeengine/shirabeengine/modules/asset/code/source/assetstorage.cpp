// #define STB_IMAGE_STATIC
// #define STB_IMAGE_IMPLEMENTATION
// #include <stb/stb_image.h>

#include <filesystem>
#include <core/enginetypehelper.h>
#include <core/helpers.h>

#include "asset/assetstorage.h"

namespace engine
{
    namespace asset
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CAssetStorage::CAssetStorage(Unique<IAssetDataSource> &&aAssetDataSource)
            : IAssetStorage()
            , mAssetIndex()
            , mAssetDataSource(std::move(aAssetDataSource))
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void CAssetStorage::readIndex(AssetRegistry_t const &aIndex)
        {
            for(auto const &[id, asset] : aIndex)
            {
                // Any debug ops?
                mAssetIndex.addAsset(id, asset);
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SAsset> CAssetStorage::assetFromUri(std::filesystem::path const &aUri)
        {
            for(auto const &[uid, asset]: mAssetIndex)
            {
                if(0 == asset.uri.compare(aUri))
                {
                    return { EEngineStatus::Ok, asset };
                }
            }

            return { EEngineStatus::Error };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        // AssetId_t CAssetStorage::createDynamicTextureAsset(std::string  const &aName,
        //                                                    STextureInfo const &aTextureInfo)
        // {
        //     AssetId_t aid   = 0;
        //     SAsset    asset = {};
        //     asset.id        = aid;
        //     asset.URI       = "";
        //     asset.type      = EAssetType::Texture;
        //
        //     STextureAsset textureAsset = {};
        //     textureAsset.name        = aName;
        //     textureAsset.textureInfo = aTextureInfo;
        //
        //     mAssetIndex.addAsset(aid, asset);
        //     mTextureAssets.addAsset(aid, textureAsset);
        //
        //     return aid;
        // }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        AssetId_t CAssetStorage::createAsset(EAssetType  const &aAssetType,
                                             std::string const &aAssetDirectory,
                                             std::string const &aAssetFilename,
                                             ByteBuffer  const &aInitialData)
        {
            AssetId_t assetUID = 0_uid;

            SAsset asset {};
            asset.id   = assetUID;
            asset.type = aAssetType;
            asset.uri  = (std::filesystem::path(aAssetDirectory)/std::filesystem::path(aAssetFilename)).lexically_normal();

            if(0 < aInitialData.size())
            {
                mAssetDataSource->writeAsset(asset.uri, aInitialData);
            }

            mAssetIndex.addAsset(assetUID, asset);

            return 0_uid;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SAsset > CAssetStorage::loadAsset(AssetId_t const &aAssetUID)
        {
            return mAssetIndex.getAsset(aAssetUID);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<ByteBuffer> CAssetStorage::loadAssetMeta(AssetId_t const &aAssetUID)
        {
            CEngineResult<ByteBuffer> data = { EEngineStatus::Error };

            CEngineResult<SAsset> assetFetch = mAssetIndex.getAsset(aAssetUID);
            if(not assetFetch.successful())
            {
                return { EEngineStatus::Error };
            }

            SAsset const asset = assetFetch.data();

            // Append .meta to the file in order to load an associated meta file.
            return mAssetDataSource->readAsset(std::filesystem::path(asset.uri.string() + ".meta"));
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<ByteBuffer> CAssetStorage::loadAssetData(AssetId_t const &aAssetUID)
        {
            CEngineResult<ByteBuffer> data = { EEngineStatus::Error };

            CEngineResult<SAsset> assetFetch = mAssetIndex.getAsset(aAssetUID);
            if(not assetFetch.successful())
            {
                return { EEngineStatus::Error };
            }

            SAsset const asset = assetFetch.data();

            return mAssetDataSource->readAsset(asset.uri);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CAssetStorage::removeAsset(AssetId_t const &aAssetUID)
        {
            mAssetIndex.removeAsset(aAssetUID);

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        // CEngineResult<SImage> loadImageFromFile(std::string const &aFilename)
        // {
        //     SImage image{};
        //
        //     int w = 0, h = 0, c = 0;
        //     unsigned char* stbuc = stbi_load(aFilename.c_str(), &w, &h, &c, 4);
        //
        //     uint64_t const size = (static_cast<uint64_t>(w) *
        //                            static_cast<uint64_t>(h) *
        //                            4u * sizeof(int8_t));
        //
        //     image.data         = ByteBuffer::DataArrayFromSize(size);
        //     image.width        = static_cast<uint32_t>(w);
        //     image.height       = static_cast<uint32_t>(h);
        //     image.channels     = static_cast<uint32_t>(c);
        //
        //     stbi_image_free(stbuc);
        //
        //     return CEngineResult<SImage>(EEngineStatus::Ok, image);
        // }
        //<-----------------------------------------------------------------------------

    }
}
