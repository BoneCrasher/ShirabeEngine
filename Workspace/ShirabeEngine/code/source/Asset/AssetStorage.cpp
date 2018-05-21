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
      asset.id     = aid;
      asset.path   = "";
      asset.source = AssetSource::Runtime;
      asset.type   = AssetType::Texture;

      TextureAsset textureAsset{ };
      textureAsset.name        = name;
      textureAsset.textureInfo = texture;

      m_assetIndex.addAsset(aid, asset);
      m_textureAssets.addAsset(aid, textureAsset);

      return aid;
    }

    /**********************************************************************************************//**
     * \fn  Ptr<AssetBinaryData> AssetBinaryData::fromMemory(int8_t const*const data, uint64_t const size)
     *
     * \brief From memory
     *
     * \param data  The data.
     * \param size  The size.
     *
     * \return  A Ptr&lt;AssetBinaryData&gt;
     **************************************************************************************************/
    Ptr<AssetBinaryData>
      AssetBinaryData::fromMemory(int8_t const*const data, uint64_t const size)
    {
      Ptr<AssetBinaryData> result = nullptr;

      return result;
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
    Ptr<AssetBinaryData>
      AssetStorage::loadAsset(AssetId_t const&id)
    {
      Ptr<AssetBinaryData> data = 0;

      Optional<Asset> asset = m_assetIndex.getAsset(id);
      if(asset.has_value()) {
        switch(asset->type) {
        case AssetType::Texture: data = loadTextureAsset(*asset); break;
        case AssetType::Buffer:  data = loadBufferAsset(*asset);  break;
        default:                 data = nullptr;                  break;
        }
      }

      return data;
    }

    /**********************************************************************************************//**
     * \fn  void AssetStorage::unloadAsset(AssetId_t const&id)
     *
     * \brief Unload asset
     *
     * \param id  The identifier.
     **************************************************************************************************/
    void
      AssetStorage::unloadAsset(AssetId_t const&id)
    {
      return;
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
    Ptr<AssetBinaryData>
      AssetStorage::loadBufferAsset(Asset const&asset)
    {
      Ptr<AssetBinaryData> data = nullptr;

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
     * \fn  Ptr<AssetBinaryData> AssetStorage::loadTextureAsset(Asset const&asset)
     *
     * \brief Loads texture asset
     *
     * \param asset The asset.
     *
     * \return  The texture asset.
     **************************************************************************************************/
    Ptr<AssetBinaryData>
      AssetStorage::loadTextureAsset(Asset const&asset)
    {
      Ptr<AssetBinaryData> data = nullptr;

      Optional<TextureAsset> textureAsset = m_textureAssets.getAsset(asset.id);

      return data;
    }


  }
}