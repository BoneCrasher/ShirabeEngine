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

    virtual Asset           loadAsset(AssetId_t const&)              = 0;
    virtual AssetBinaryData loadAssetData(AssetDataReference const&) = 0;
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

      Asset           loadAsset(AssetId_t const&);
      AssetBinaryData loadAssetData(AssetDataReference const&);

    private:
      AssetBinaryData loadBufferAsset(AssetDataReference const&);
      AssetBinaryData loadTextureAsset(AssetDataReference const&);

      AssetIndex       m_assetIndex;
      TextureAssetData m_textureAssets;
      BufferAssetData  m_bufferAssets;
    };
    DeclareSharedPointerType(AssetStorage);
  }
}

#endif