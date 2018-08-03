#ifndef __SHIRABE__MEASUREMENT_CHUNK_TRAITS_H__
#define __SHIRABE__MEASUREMENT_CHUNK_TRAITS_H__

#include <stddef.h>
#include <vector>

#include <base/declaration.h>

namespace engine
{
    namespace benchmarking
    {
        /**
         * MeasurementChunkTraits implement a set of functions used in a measurement data store
         * specialized for TChunk and TStorage.
         *
         * @tparam TChunk   The specific chunk type used.
         * @tparam TStorage The storage type used for the chunks.
         */
        template <
                typename TChunk,
                typename TStorage = std::vector<TChunk>
                >
        class CMeasurementChunkTraits
        {
        public:
            using ChunkType_t     = TChunk;
            using StorageType_t   = TStorage;
            using ParameterType_t = typename ChunkType_t::ParameterType_t;
            using ValueType_t     = typename ChunkType_t::ValueType_t;
            using SizeType_t      = std::size_t;

        public:
            /**
             * Average a contigious subset of chunks from a chunk storage using
             * the arithmetic mean.
             *
             * @param aChunkStorage
             * @param aOffset
             * @param aCount
             * @return
             */
            static SizeType_t average(
                        StorageType_t const &aChunkStorage,
                        SizeType_t    const &aOffset,
                        SizeType_t    const &aCount)
            {
                SHIRABE_UNUSED(aChunkStorage);
                SHIRABE_UNUSED(aOffset);
                SHIRABE_UNUSED(aCount);

                return 0;
            }
        };
    }
}

#endif
