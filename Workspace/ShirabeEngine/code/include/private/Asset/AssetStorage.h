#ifndef __SHIRABE_ASSET_STORAGE_H__
#define __SHIRABE_ASSET_STORAGE_H__

#include "Core/EngineTypeHelper.h"
#include "Core/DataBuffer.h"

#include "Resources/Subsystems/GFXAPI/Types/Texture.h"

#include "Asset/AssetError.h"
#include "Asset/AssetTypes.h"

namespace Engine {
  namespace Asset {

    /**********************************************************************************************//**
     * \fn  DeclareInterface(IAssetStorage);
     *
     * \brief Constructor
     *
     * \param parameter1  The first parameter.
     **************************************************************************************************/
    DeclareInterface(IAssetStorage);
    virtual AssetId_t createDynamicTextureAsset(
      std::string         const&,
      GFXAPI::TextureInfo const&) = 0;

    virtual Asset      loadAsset(AssetId_t const&) = 0;
    virtual ByteBuffer loadAssetData(Asset const&) = 0;
    DeclareInterfaceEnd(IAssetStorage);

    DeclareSharedPointerType(IAssetStorage);

    /**********************************************************************************************//**
     * \class AssetStorage
     *
     * \brief An asset storage.
     **************************************************************************************************/
    class SHIRABE_TEST_EXPORT AssetStorage
      : public IAssetStorage
    {
      using TextureAssetData = AssetRegistry<TextureAsset>;
      using BufferAssetData  = AssetRegistry<BufferAsset>;
    public:
      using AssetIndex = AssetRegistry<Asset>;

      AssetStorage();

      void readIndex(AssetIndex const&);

      AssetId_t createDynamicTextureAsset(
        std::string         const&,
        GFXAPI::TextureInfo const&);

      Asset      loadAsset(AssetId_t const&);
      ByteBuffer loadAssetData(Asset const&);

    private:
      ByteBuffer loadBufferAsset(Asset const&);
      ByteBuffer loadTextureAsset(Asset const&);

      AssetIndex       m_assetIndex;
      TextureAssetData m_textureAssets;
      BufferAssetData  m_bufferAssets;
    };
    DeclareSharedPointerType(AssetStorage);
  }
}

#endif