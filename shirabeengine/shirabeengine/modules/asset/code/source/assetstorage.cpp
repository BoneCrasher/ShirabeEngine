#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "Asset/AssetStorage.h"

#include "core/enginetypehelper.h"

namespace engine {
  namespace Asset {
    using GFXAPI::TextureInfo;

    /**********************************************************************************************//**
     * \fn  AssetStorage::AssetStorage(CStdSharedPtr_t<IResourceManager> const manager)
     *
     * \brief Constructor
     *
     * \param manager The manager.
     **************************************************************************************************/
    AssetStorage::AssetStorage()
      : IAssetStorage()
      , m_assetIndex()
    {}

    void 
      AssetStorage::readIndex(AssetIndex const&index)
    {
      for(AssetIndex::value_type const&assignment : index) {
        // Any debug ops?
        m_assetIndex.addAsset(assignment.first, assignment.second);
      }
    }

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
      AssetStorage::createDynamicTextureAsset(
        std::string const&name,
        TextureInfo const&texture)
    {
      AssetId_t aid = 0;
      Asset asset{};
      asset.id     = aid;
      asset.URI    = "";
      asset.type   = AssetType::Texture;

      TextureAsset textureAsset{ };
      textureAsset.name        = name;
      textureAsset.textureInfo = texture;

      m_assetIndex.addAsset(aid, asset);
      m_textureAssets.addAsset(aid, textureAsset);

      return aid;
    }

    /**********************************************************************************************//**
     * \fn  CStdSharedPtr_t<ByteBuffer> AssetStorage::loadAsset(AssetId_t const&id)
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

    ByteBuffer
      AssetStorage::loadAssetData(Asset const&ref)
    {
      ByteBuffer data;

      switch(ref.type) {
      case AssetType::Texture: data = loadTextureAsset(ref); break;
      case AssetType::Buffer:  data = loadBufferAsset(ref);  break;
      default:                                               break;
      }

      return std::move(data);
    }

    /**********************************************************************************************//**
     * \fn  CStdSharedPtr_t<ByteBuffer> AssetStorage::loadBufferAsset(Asset const&asset)
     *
     * \brief Loads buffer asset
     *
     * \param asset The asset.
     *
     * \return  The buffer asset.
     **************************************************************************************************/
    ByteBuffer
      AssetStorage::loadBufferAsset(Asset const&asset)
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
      image.data     = std::move(ByteBuffer::DataArrayFromSize(size));
      image.width    = w;
      image.height   = h;
      image.channels = c;

      memcpy(image.data.mutableData(), stbuc, image.data.size());

      stbi_image_free(stbuc);
    }

    /**********************************************************************************************//**
     * \fn  CStdSharedPtr_t<ByteBuffer> AssetStorage::loadTextureAsset(Asset const&asset)
     *
     * \brief Loads texture asset
     *
     * \param asset The asset.
     *
     * \return  The texture asset.
     **************************************************************************************************/
    ByteBuffer
      AssetStorage::loadTextureAsset(Asset const&asset)
    {
      Image image{};

      // Default for now... 
      // Will check cache and download from server if necessary
      loadImageFromFile(asset.URI, image);

      return std::move(image.data);
    }

  }
}