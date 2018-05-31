#ifndef __SHIRABE_ASSET_STORAGE_H__
#define __SHIRABE_ASSET_STORAGE_H__

#include "Core/EngineTypeHelper.h"
#include "Resources/Core/IResourceManager.h"
#include "Resources/Types/Texture.h"

#include "Asset/AssetError.h"
#include "Asset/AssetTypes.h"

namespace Engine {
  namespace Asset {
    using Engine::Resources::PublicResourceId_t;
    using Engine::Resources::IResourceManager;

    /**********************************************************************************************//**
     * \class AssetRegistry
     *
     * \brief An asset registry.
     *
     * \tparam  T Generic type parameter.
     **************************************************************************************************/
    template <typename T>
    class AssetRegistry {
      using Index = Map<AssetId_t, T>;
    public:
      AssetErrorCode addAsset(AssetId_t const&, T const&);
      Optional<T>    getAsset(AssetId_t const&);

    private:
      Index m_index;
    };

    /**********************************************************************************************//**
     * \fn  template <typename T> AssetErrorCode AssetRegistry<T>::addAsset(AssetId_t const&id, T const&data)
     *
     * \brief Adds an asset to 'data'
     *
     * \tparam  T Generic type parameter.
     * \param id    The identifier.
     * \param data  The data.
     *
     * \return  An AssetErrorCode.
     **************************************************************************************************/
    template <typename T>
    AssetErrorCode
      AssetRegistry<T>::addAsset(AssetId_t const&id, T const&data)
    {
      if(m_index.find(id) != m_index.end())
        return AssetErrorCode::AssetAlreadyAdded;

      m_index[id] = data;

      return AssetErrorCode::Ok;
    }

    /**********************************************************************************************//**
     * \fn  template <typename T> Optional<T> AssetRegistry<T>::getAsset(AssetId_t const&id)
     *
     * \brief Gets an asset
     *
     * \tparam  T Generic type parameter.
     * \param id  The identifier.
     *
     * \return  The asset.
     **************************************************************************************************/
    template <typename T>
    Optional<T>
      AssetRegistry<T>::getAsset(AssetId_t const&id)
    {
      Optional<T> result{ };

      if(m_index.find(id) != m_index.end())
        result = m_index.at(id);

      return result;
    }

    /**********************************************************************************************//**
     * \class AssetBinaryData
     *
     * \brief An asset binary data.
     **************************************************************************************************/
    class AssetBinaryData;

    class AssetBinaryData {
    public:
      static Ptr<AssetBinaryData> fromMemory(
        int8_t   const*const data,
        uint64_t const       size);

    private:
      AssetBinaryData(
        int8_t   const*const data,
        uint64_t const       size);

      int8_t   const* m_data;
      uint64_t const  m_size;
    };

    /**********************************************************************************************//**
     * \fn  DeclareInterface(IAssetStorage);
     *
     * \brief Constructor
     *
     * \param parameter1  The first parameter.
     **************************************************************************************************/
    DeclareInterface(IAssetStorage);
    virtual AssetId_t createTextureAsset(
      std::string            const&,
      Resources::TextureInfo const&) = 0;

    virtual Asset                loadAsset(AssetId_t const&)              = 0;
    virtual Ptr<AssetBinaryData> loadAssetData(AssetDataReference const&) = 0;
    DeclareInterfaceEnd(IAssetStorage);

    DeclareSharedPointerType(IAssetStorage);

    /**********************************************************************************************//**
     * \class AssetStorage
     *
     * \brief An asset storage.
     **************************************************************************************************/
    class AssetStorage
      : public IAssetStorage
    {
      using AssetIndex       = AssetRegistry<Asset>;
      using TextureAssetData = AssetRegistry<TextureAsset>;
      using BufferAssetData  = AssetRegistry<BufferAsset>;
    public:
      AssetStorage(Ptr<Resources::IResourceManager> const);

      AssetId_t createTextureAsset(
        std::string            const&,
        Resources::TextureInfo const&);

      Asset                loadAsset(AssetId_t const&);
      Ptr<AssetBinaryData> loadAssetData(AssetDataReference const&);

    private:
      Ptr<AssetBinaryData> loadBufferAsset(Asset const&);
      Ptr<AssetBinaryData> loadTextureAsset(Asset const&);

      AssetIndex       m_assetIndex;
      TextureAssetData m_textureAssets;
      BufferAssetData  m_bufferAssets;
    };
    DeclareSharedPointerType(AssetStorage);
  }
}

#endif