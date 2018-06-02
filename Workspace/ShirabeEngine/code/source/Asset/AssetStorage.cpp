#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "Asset/AssetStorage.h"

#include "Core/EngineTypeHelper.h"
#include "Resources/Core/IResourceManager.h"

namespace Engine {
  namespace Asset {
    using namespace Resources;

    /**********************************************************************************************//**
     * \fn  AssetStorage::AssetStorage(Ptr<IResourceManager> const manager)
     *
     * \brief Constructor
     *
     * \param manager The manager.
     **************************************************************************************************/
    AssetStorage::AssetStorage(Ptr<IResourceManager> const manager)
    {}

    /**********************************************************************************************//**
     * \fn  AssetId_t AssetStorage::createTextureAsset( std::string const&name, TextureInfo const&texture)
     *
     * \brief Creates texture asset
     *
     * \param name    The name.
     * \param texture The texture.
     *
     * \return  The new texture asset.
     **************************************************************************************************/
    AssetId_t
      AssetStorage::createTextureAsset(
        std::string const&name,
        TextureInfo const&texture)
    {
      AssetId_t aid = 0;
      Asset asset{};
      asset.id = aid;

      AssetDataReference assetData{};
      assetData.URI    = "";
      assetData.source = AssetSource::Runtime;
      assetData.type   = AssetType::Texture;

      TextureAsset textureAsset{ };
      textureAsset.name        = name;
      textureAsset.textureInfo = texture;

      m_assetIndex.addAsset(aid, asset);
      m_textureAssets.addAsset(aid, textureAsset);

      return aid;
    }

    /**********************************************************************************************//**
     * \fn  Ptr<AssetBinaryData> AssetStorage::loadAsset(AssetId_t const&id)
     *
     * \brief Loads an asset
     *
     * \param id  The identifier.
     *
     * \return  The asset.
     **************************************************************************************************/
    Asset
      AssetStorage::loadAsset(AssetId_t const&id)
    {
      Optional<Asset> asset = m_assetIndex.getAsset(id);
      if(!asset.has_value())
        throw std::runtime_error("Asset not found.");

      return *asset;
    }

    AssetBinaryData
      AssetStorage::loadAssetData(AssetDataReference const&ref)
    {
      AssetBinaryData data;

      switch(ref.type) {
      case AssetType::Texture: data = loadTextureAsset(ref); break;
      case AssetType::Buffer:  data = loadBufferAsset(ref);  break;
      default:                                               break;
      }

      return std::move(data);
    }

    /**********************************************************************************************//**
     * \fn  Ptr<AssetBinaryData> AssetStorage::loadBufferAsset(Asset const&asset)
     *
     * \brief Loads buffer asset
     *
     * \param asset The asset.
     *
     * \return  The buffer asset.
     **************************************************************************************************/
    AssetBinaryData
      AssetStorage::loadBufferAsset(AssetDataReference const&asset)
    {
      AssetBinaryData data;

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

    /**********************************************************************************************//**
     * \fn  void loadImage( std::string const&filename, Image &image)
     *
     * \brief Loads an image
     *
     * \param           filename  Filename of the file.
     * \param [in,out]  image     The image.
     **************************************************************************************************/
    void loadImageFromFile(
      std::string const&filename,
      Image            &image)
    {
      int w = 0, h = 0, c = 0;
      unsigned char* stbuc = stbi_load(filename.c_str(), &w, &h, &c, 4);

      uint64_t size = (w * h * 4 * sizeof(int8_t));
      image.data     = std::move(AssetBinaryData::DataArrayFromSize(size));
      image.width    = w;
      image.height   = h;
      image.channels = c;

      memcpy(image.data.mutableData(), stbuc, image.data.size());

      stbi_image_free(stbuc);
    }

    /**********************************************************************************************//**
     * \fn  Ptr<AssetBinaryData> AssetStorage::loadTextureAsset(Asset const&asset)
     *
     * \brief Loads texture asset
     *
     * \param asset The asset.
     *
     * \return  The texture asset.
     **************************************************************************************************/
    AssetBinaryData
      AssetStorage::loadTextureAsset(AssetDataReference const&asset)
    {
      Image image{};

      switch(asset.source) {
      case AssetSource::Local:
        loadImageFromFile(asset.URI, image);
        break;
      case AssetSource::Runtime:
        // loadImageFromCache(asset.URI, image);
        // break;
      case AssetSource::URL:
        // downloadImage(asset.URI, ...);
        //
        // TODO !!! 
        // Consider:
        //   Should the knowledge of "URL" be hidden entirely, i.e. it is commonly 
        //   assumed that the file is loaded from disk/cache and will  be looked up 
        //   online by default if missing?
        break;
      }
      
      return std::move(image.data);
    }

  }
}