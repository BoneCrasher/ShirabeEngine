#ifndef __SHIRABE_ASSET_TYPES_H__
#define __SHIRABE_ASSET_TYPES_H__

#include <stdint.h>
#include <string>

#include "Core/EngineTypeHelper.h"

#include "Resources/Types/Texture.h"

namespace Engine {
  namespace Asset {

    using AssetId_t = uint64_t;

    enum class AssetSource {
      Runtime = 1,
      Local   = 2,
      URL     = 4
    };

    enum class AssetType {
      Texture = 1,
      Buffer  = 2,
    };

    struct AssetDataReference {
      AssetSource source;
      std::string URI;
      AssetType   type;
    };

    struct Asset {
      AssetId_t             
        id;
      Vector<AssetDataReference> 
        dataReferences;
    };

    struct TextureAsset {
      std::string            name;
      Resources::TextureInfo textureInfo;
    };

    struct BufferAsset {
      std::string name;
    };

    struct Image {
      AssetBinaryData
        data;
      uint32_t
        width,
        height,
        channels;
    };
        
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
      friend class AssetStorage;

    public:
      AssetBinaryData() = default;

      static AssetBinaryData DataArrayFromSize(
        uint64_t const size);

      inline
        int8_t const*const data() const { return m_data.data(); }
      inline
        int8_t *const mutableData()  { return m_data.data(); }
      inline
        uint64_t const size() const { return m_size; }

    private:
      AssetBinaryData(
        std::vector<int8_t>     &&data,
        uint64_t            const size);

      std::vector<int8_t> m_data;
      uint64_t           m_size;
    };

  }
}

#endif