#ifndef __SHIRABE__MEASUREMENT_DATA_STORE_H__
#define __SHIRABE__MEASUREMENT_DATA_STORE_H__

#include <cmath>
#include <math.h>
#include <vector>
#include <algorithm>

#include "core/enginestatus.h"
#include "core/benchmarking/measurementchunk.h"
#include "core/benchmarking/measurementchunktraits.h"

namespace engine
{
    namespace benchmarking
    {

#define DEFAULT_CAPACITY 12

        /**
         * A measurement data store can create and store an ordered list of chunks.
         * This implementation permits providing a custom allocator for creating chunks.
         *
         * @tparam TChunk The underlying chunk type to store.
         * @tparam TAlloc The allocator to use.
         * @tparam TTraits The chunk type specific trait implementation.
         */
        template <
                typename TChunk,
                typename TTraits = CMeasurementChunkTraits<TChunk, std::vector<TChunk>>
                >
        class CMeasurementDataStore
        {
        public_typedefs:
            using ClassType_t          = CMeasurementDataStore<TChunk, TTraits>;
            using ChunkType_t          = TChunk                                        ;
            using ChunkParameterType_t = typename TChunk::ParameterType_t              ;
            using ChunkValueType_t     = typename TChunk::ValueType_t                  ;
            using TraitsType_t         = TTraits                                       ;
            using StorageType_t        = std::vector<ChunkType_t>                      ;
            using StorageSizeType_t    = typename StorageType_t::size_type             ;

        public_constructors:
            /**
             * Construct an empty data store and preset it's capacity.
             *
             * @param aMaxCapacity
             */
            CMeasurementDataStore(StorageSizeType_t const &aMaxCapacity = DEFAULT_CAPACITY);

        public_destructors:
            /**
             * Destroy and run...
             */
            virtual ~CMeasurementDataStore();

        public_methods:
            /**
             * Remove all storage items.
             */
            virtual void clear();

            /**
             * Push a new chunk parameter and value into the storage.
             *
             * @param aParameter The parameter value of the chunk.
             * @param aValue     The value of the chunk.
             */
            void push(
                    ChunkParameterType_t const &aParameter,
                    ChunkValueType_t     const &aValue);

            /**
             * Return the chunk stored at index 'aIndex'.
             *
             * @param aIndex
             * @param aOutChunk
             * @return
             */
            bool at(
                    StorageSizeType_t  const &aIndex,
                    ChunkType_t              &aOutChunk) const;

            /**
             * @brief maxChunks
             * @return
             */
            SHIRABE_INLINE StorageSizeType_t const maxChunks() const { return mMaxChunks; }

            /**
             * @brief size
             * @return
             */
            SHIRABE_INLINE StorageSizeType_t const size() const { return mChunks.size(); }

            /**
             * @brief average
             * @param aOffset
             * @param aLength
             * @return
             */
            ChunkParameterType_t average(
                    StorageSizeType_t const &aOffset = 0,
                    StorageSizeType_t const &aLength = 0) const;

        private_methods:
            void validateChunkList() {}

        private_members:
            StorageSizeType_t mMaxChunks;
            StorageType_t     mChunks;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <
                typename TChunk,
                typename TTraits
                >
        CMeasurementDataStore<TChunk, TTraits>::CMeasurementDataStore(StorageSizeType_t const &aMaxCapacity)
            : mMaxChunks(aMaxCapacity)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <
                typename TChunk,
                typename TTraits
                >
        CMeasurementDataStore<TChunk, TTraits>::~CMeasurementDataStore()
        {
            clear();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <
                typename TChunk,
                typename TTraits
                >
        void CMeasurementDataStore<TChunk, TTraits>::clear()
        {
            mChunks.clear();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <
                typename TChunk,
                typename TTraits
                >
        void CMeasurementDataStore<TChunk, TTraits>::push(
                ChunkParameterType_t const &aParameter,
                ChunkValueType_t     const &aValue)
        {
            validateChunkList();

            if (mChunks.size() == mMaxChunks)
            {
                mChunks.erase(mChunks.begin());
            }

            mChunks.push_back(ChunkType_t(aParameter, aValue));
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <
                typename TChunk,
                typename TTraits
                >
        bool CMeasurementDataStore<TChunk, TTraits>::at(
                StorageSizeType_t const &aIndex,
                ChunkType_t             &aOutChunk) const
        {
            validateChunkList();

            if (mChunks.size() > aIndex)
            {
                aOutChunk = mChunks.at(aIndex);
                return true;
            }
            else
                return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <
                typename TChunk,
                typename TTraits
                >
        typename CMeasurementDataStore<TChunk, TTraits>::ChunkParameterType_t
            CMeasurementDataStore<TChunk, TTraits>::average(
                StorageSizeType_t const &aOffset,
                StorageSizeType_t const &aLength) const
        {
            ChunkParameterType_t average = 0;

            StorageSizeType_t off = aOffset;
            StorageSizeType_t len = aLength;

            /* At least two chunks required to calc averageerage */
            if (mChunks.size() >= 2)
            {
                off = std::max(0ul,                   off);
                off = std::min((mChunks.size() - 1u), off);
                len = std::min((mChunks.size() - off - 1), len);

                ChunkType_t const &chunk0 = mChunks.at(off);
                ChunkType_t const &chunk1 = mChunks.at(len);

                ChunkParameterType_t const interval = chunk1.parameter() - chunk0.parameter();
                if (interval > 0)
                {
                    for(StorageSizeType_t k = 0; k < mChunks.size(); ++k)
                    {
                        average += mChunks.at(k).value();
                    }

                    average = static_cast<ChunkParameterType_t>((average / interval) + 0.5);
                }
            }

            return average;
        }
        //<-----------------------------------------------------------------------------
    }
}

#endif
