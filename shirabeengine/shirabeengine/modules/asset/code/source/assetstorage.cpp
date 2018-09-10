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
        SAsset CAssetStorage::loadAsset(AssetId_t const &aAssetUID)
        {
            Optional_t<SAsset> asset = mAssetIndex.getAsset(aAssetUID);
            if(!asset.has_value())
            {
                throw std::runtime_error("Asset not found.");
            }

            return *asset;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        ByteBuffer CAssetStorage::loadAssetData(SAsset const &aAsset)
        {
            ByteBuffer data{};

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

            return std::move(data);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        ByteBuffer CAssetStorage::loadBufferAsset(SAsset const &aAsset)
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
            return data;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void loadImageFromFile(
                std::string const&aFilename,
                SImage           &aImage)
        {
            int w = 0, h = 0, c = 0;
            unsigned char* stbuc = stbi_load(aFilename.c_str(), &w, &h, &c, 4);

            uint64_t const size = (w * h * 4 * sizeof(int8_t));
            aImage.data         = std::move(ByteBuffer::DataArrayFromSize(size));
            aImage.width        = w;
            aImage.height       = h;
            aImage.channels     = c;

            memcpy(aImage.data.mutableData(), stbuc, aImage.data.size());

            stbi_image_free(stbuc);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        ByteBuffer CAssetStorage::loadTextureAsset(SAsset const &aAsset)
        {
            SImage image ={};

            // Default for now...
            // Will check cache and download from server if necessary
            loadImageFromFile(aAsset.URI, image);

            return std::move(image.data);
        }
        //<-----------------------------------------------------------------------------

    }
}
