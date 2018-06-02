#include "Asset/AssetTypes.h"

namespace Engine {
  namespace Asset {
    
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
    AssetBinaryData
      AssetBinaryData::DataArrayFromSize(
        uint64_t const size)
    {      
      std::vector<int8_t> data{};
      data.resize(size);

      AssetBinaryData binaryData = AssetBinaryData(std::move(data), size);

      return binaryData;
    }
    
    AssetBinaryData::AssetBinaryData(
      std::vector<int8_t>     &&data,
      uint64_t            const size)
      : m_data(std::move(data))
      , m_size(size)
    {}


  }
}