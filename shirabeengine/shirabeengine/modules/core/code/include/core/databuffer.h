#ifndef __SHIRABE_CORE_CDataBuffer_H__
#define __SHIRABE_CORE_CDataBuffer_H__

#include <vector>
#include <stdint.h>
#include <base/declaration.h>

namespace Engine {

    /**
     *
     */
    template <typename T>
    class CDataBuffer
    {
    public_static_constants:
        static const constexpr std::size_t element_byte_size = sizeof(T);

    public_static_functions:
        static CDataBuffer<T> DataArrayFromSize(uint64_t const aSize);

    public_constructors:
        CDataBuffer();

    public_methods:
        /**
         * @brief data
         * @return
         */
        inline T const*const data() const
        {
            return mData.data();
        }

        /**
         * @brief mutableData
         * @return
         */
        inline T *const mutableData()
        {
            return mData.data();
        }

        /**
         * @brief size
         * @return
         */
        inline uint64_t const size() const
        {
            return mSize;
        }

    private_constructors:
        /**
         * @brief CDataBuffer
         * @param data
         * @param size
         */
        CDataBuffer( std::vector<T> &&aData, uint64_t const aSize);

    private_members:
        std::vector<T> mData;
        uint64_t       mSize;
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <typename T>
    CDataBuffer<T> CDataBuffer<T>::DataArrayFromSize(uint64_t const size)
    {
        std::vector<T> data{};
        data.resize(size);

        CDataBuffer<T> binaryData = CDataBuffer<T>(std::move(data), size);

        return binaryData;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <typename T>
    CDataBuffer<T>::CDataBuffer()
        : mData()
        , mSize(0)
    {}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <typename T>
    CDataBuffer<T>::CDataBuffer(
            std::vector<T>     &&aData,
            uint64_t       const aSize)
        : mData(std::move(aData))
        , mSize(aSize)
    {}
    //<-----------------------------------------------------------------------------

    /**
     *
     */
    using ByteBuffer = CDataBuffer<int8_t>;
}

#endif
