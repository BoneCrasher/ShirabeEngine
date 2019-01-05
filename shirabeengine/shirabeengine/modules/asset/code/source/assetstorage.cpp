#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <core/enginetypehelper.h>

#include "asset/assetstorage.h"

namespace engine
{
    namespace asset
    {
        using gfxapi::STextureInfo;

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CAssetStorage::CAssetStorage()
            : IAssetStorage()
            , mAssetIndex()
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void CAssetStorage::readIndex(AssetIndex_t const &aIndex)
        {
            for(AssetIndex_t::value_type const &assignment : aIndex)
            {
                // Any debug ops?
                mAssetIndex.addAsset(assignment.first, assignment.second);
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        AssetId_t CAssetStorage::createDynamicTextureAsset(
                std::string  const &aName,
                STextureInfo const &aTextureInfo)
        {
            AssetId_t aid   = 0;
            SAsset    asset = {};
            asset.id        = aid;
            asset.URI       = "";
            asset.type      = EAssetType::Texture;

            STextureAsset textureAsset = {};
            textureAsset.name        = aName;
            textureAsset.textureInfo = aTextureInfo;

            mAssetIndex.addAsset(aid, asset);
            mTextureAssets.addAsset(aid, textureAsset);

            return aid;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SAsset> CAssetStorage::loadAsset(AssetId_t const &aAssetUID)
        {
            return mAssetIndex.getAsset(aAssetUID);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<ByteBuffer> CAssetStorage::loadAssetData(SAsset const &aAsset)
        {
            CEngineResult<ByteBuffer> data = { EEngineStatus::Error };

            switch(aAsset.type)
            {
            case EAssetType::Texture:
                data = loadTextureAsset(aAsset);
                break;
            case EAssetType::Buffer:
                data = loadBufferAsset(aAsset);
                break;
            default:
                break;
            }

            return data;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<ByteBuffer> CAssetStorage::loadBufferAsset(SAsset const &aAsset)
        {
            ByteBuffer data;

            /*Optional<TextureAsset> textureAsset = getTextureAsset(asset.id);

                  Texture::Descriptor desc{};
                  desc.name       = textureAsset->name;
                  desc.textureInfo = textureAsset->textureInfo;

                  Texture::CreationRequest request(desc);

                  EEngineStatus status = m_resourceManager->createTexture(request, pid);
                  HandleEngineStatusError(status, "Failed to create asset resource instance in resource manager.");
            */
            return CEngineResult<ByteBuffer>(EEngineStatus::Ok, data);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SImage> loadImageFromFile(std::string const &aFilename)
        {
            SImage image{};

            int w = 0, h = 0, c = 0;
            unsigned char* stbuc = stbi_load(aFilename.c_str(), &w, &h, &c, 4);

            uint64_t const size = (w * h * 4 * sizeof(int8_t));
            image.data         = ByteBuffer::DataArrayFromSize(size);
            image.width        = w;
            image.height       = h;
            image.channels     = c;

            stbi_image_free(stbuc);

            return CEngineResult<SImage>(EEngineStatus::Ok, image);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<ByteBuffer> CAssetStorage::loadTextureAsset(SAsset const &aAsset)
        {
            // Default for now...
            // Will check cache and download from server if necessary
            CEngineResult<SImage> image = loadImageFromFile(aAsset.URI);

            return CEngineResult<ByteBuffer>(EEngineStatus::Ok, image.data().data);
        }
        //<-----------------------------------------------------------------------------

    }
}
