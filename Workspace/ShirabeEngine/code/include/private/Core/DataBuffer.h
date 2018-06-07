#ifndef __SHIRABE_CORE_DATABUFFER_H__
#define __SHIRABE_CORE_DATABUFFER_H__

#include <vector>
#include <stdint.h>

namespace Engine {

  /**
   * \class DataBuffer
   *
   * \brief Buffer for data.
   *
   * \tparam  T Generic type parameter.
   **************************************************************************************************/
  template <typename T>
  class DataBuffer {
  public:
    DataBuffer();

    static DataBuffer<T> DataArrayFromSize(
      uint64_t const size);

    inline
      T const*const data() const { return m_data.data(); }
    inline
      T *const mutableData() { return m_data.data(); }
    inline
      uint64_t const size() const { return m_size; }

  private:
    DataBuffer(
      std::vector<T>     &&data,
      uint64_t            const size);

    std::vector<T> m_data;
    uint64_t       m_size;
  };

  /**
   * \fn  AssetBinaryData AssetBinaryData::DataArrayFromSize( uint64_t const size)
   *
   * \brief From memory
   *
   * \param size  The size.
   *
   * \return  A Ptr&lt;AssetBinaryData&gt;
   *
   * ### param  data  The data.
   **************************************************************************************************/
  template <typename T>
  DataBuffer<T>
    DataBuffer<T>::DataArrayFromSize(
      uint64_t const size)
  {
    std::vector<T> data{};
    data.resize(size);

    DataBuffer<T> binaryData = DataBuffer<T>(std::move(data), size);

    return binaryData;
  }

  /**
   * \fn  template <typename T> DataBuffer<T>::DataBuffer()
   *
   * \brief Default constructor
   *
   * \tparam  T Generic type parameter.
   **************************************************************************************************/
  template <typename T>
  DataBuffer<T>::DataBuffer()
    : m_data()
    , m_size(0)
  {}

  /**
   * \fn  template <typename T> DataBuffer<T>::DataBuffer( std::vector<int8_t> &&data, uint64_t const size) : m_data(std::move(data)) , m_size(size)
   *
   * \brief Constructor
   *
   * \tparam  T Generic type parameter.
   * \param [in,out]  data  The data.
   * \param           size  The size.
   **************************************************************************************************/
  template <typename T>
  DataBuffer<T>::DataBuffer(
    std::vector<T>          &&data,
    uint64_t            const size)
    : m_data(std::move(data))
    , m_size(size)
  {}


  /** \brief Buffer for byte data */
  using ByteBuffer = DataBuffer<int8_t>;
}

#endif