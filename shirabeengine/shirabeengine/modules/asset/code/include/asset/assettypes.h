#ifndef __SHIRABE_ASSET_TYPES_H__
#define __SHIRABE_ASSET_TYPES_H__

#include <stdint.h>
#include <string>

#include "Core/EngineTypeHelper.h"
#include "Core/DataBuffer.h"

#include "GraphicsAPI/Resources/Types/Texture.h"

#include "Asset/AssetError.h"

namespace engine {
  namespace Asset {

    using AssetId_t = uint64_t;
  
    enum class AssetType {
      Undefined = 0,
      Mesh      = 1,
      Material  = 2,
      Texture   = 3,
      Buffer    = 4,
    };
        
    struct Asset {
      AssetId_t   id;
      AssetType   type;
      std::string URI;
    };

    struct TextureAsset {
      std::string         name;
      GFXAPI::TextureInfo textureInfo;
    };

    struct BufferAsset {
      std::string name;
    };   
    
    struct Image {
      ByteBuffer
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
      typedef typename Index::value_type value_type;

      AssetErrorCode addAsset(AssetId_t const&, T const&);
      Optional<T>    getAsset(AssetId_t const&);

      typename Index::iterator       begin()       { return m_index.begin(); }
      typename Index::iterator       end()         { return m_index.end();   }
      typename Index::const_iterator begin() const { return m_index.begin(); }
      typename Index::const_iterator end()   const { return m_index.end();   }

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

    
  }

  template <>
  Asset::AssetType from_string<Asset::AssetType>(std::string const&input);
}

#endif