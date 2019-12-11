#ifndef __SHIRABE_CORE_DATABUFFER_H__
#define __SHIRABE_CORE_DATABUFFER_H__

#include <vector>
#include <cstdint>
#include <base/declaration.h>

namespace engine {

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
        CDataBuffer(CDataBuffer const &aOther);
        CDataBuffer(CDataBuffer      &&aOther);

        /**
         * @brief CDataBuffer
         * @param data
         * @param size
         */
        CDataBuffer( std::vector<T> &&aData, uint64_t const aSize);
        CDataBuffer( T const *const aData, uint64_t const aSize);

    public_destructors:
        ~CDataBuffer()
        {
            if(mUseRawData || nullptr != mRawData)
            {
                //free(const_cast<T*>(mRawData));
            }
        };

    public_methods:
        /**
         * @brief data
         * @return
         */
        inline T const*const data() const
        {
            return mUseRawData
                   ? mRawData
                   : mVectorData.data();
        }

        /**
         * @brief mutableData
         * @return
         */
        inline T *const mutableData()
        {
            return mUseRawData
                   ? mRawData
                   : mVectorData.data();
        }

        inline std::vector<T> const &dataVector() const
        {
            return mVectorData;
        }

        inline std::vector<T> &mutableDataVector()
        {
            return mVectorData;
        }

        /**
         * @brief size
         * @return
         */
        inline uint64_t const size() const
        {
            return mSize;
        }

        CDataBuffer<T> const createView(uint64_t const &aOffset, uint64_t const &aSize) const
        {
            if(mSize <= (aOffset + aSize))
            {
                return {}; // Empty view...
            }

            return CDataBuffer(mRawData + aOffset, aSize);
        }

    private_members:
        std::vector<T> mVectorData;
        T       const *mRawData;
        uint64_t       mSize;
        bool           mUseRawData;
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
        : mVectorData()
        , mRawData(nullptr)
        , mSize(0)
        , mUseRawData(false)
    {}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <typename T>
    CDataBuffer<T>::CDataBuffer(CDataBuffer<T> const &aOther)
        : mVectorData(aOther.mVectorData)
        , mRawData(aOther.mRawData)
        , mSize(aOther.mSize)
        , mUseRawData(aOther.mUseRawData)
    { }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <typename T>
    CDataBuffer<T>::CDataBuffer(CDataBuffer<T> &&aOther)
        : mVectorData(std::move(aOther.mVectorData))
        , mRawData(std::move(aOther.mRawData))
        , mSize(std::move(aOther.mSize))
        , mUseRawData(aOther.mUseRawData)
    { }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <typename T>
    CDataBuffer<T>::CDataBuffer(
            std::vector<T>     &&aData,
            uint64_t       const aSize)
        : mVectorData(std::move(aData))
        , mRawData(nullptr)
        , mSize(aSize)
        , mUseRawData(false)
    {}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <typename T>
    CDataBuffer<T>::CDataBuffer(
            T const * const aData,
            uint64_t  const aSize)
            : mVectorData(0)
            , mRawData(std::move(aData))
            , mSize(aSize)
            , mUseRawData(true)
    {}
    //<-----------------------------------------------------------------------------

    /**
     *
     */
    using ByteBuffer = CDataBuffer<int8_t>;

    using DataSourceAccessor_t = std::function<ByteBuffer()>;
}

#endif
