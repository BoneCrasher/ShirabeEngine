#ifndef __SHIRABE__MEASUREMENT_CHUNK_TRAITS_H__
#define __SHIRABE__MEASUREMENT_CHUNK_TRAITS_H__

#include <stddef.h>
#include <vector>

namespace Engine {
	namespace Benchmarking {


		template <typename ChunkT, typename StorageT = std::vector<ChunkT>>
		class MeasurementChunkTraits {
		public:
			typedef ChunkT                          chunk_type;
			typedef StorageT                        storage_type;
			typedef typename chunk_type::param_type param_type;
			typedef typename chunk_type::value_type value_type;
			typedef std::size_t                     size_type;

		public:
			static size_type average(const storage_type& chunks,
									 const size_type     off,
									 size_type           n)
			{

				return 0;
			}
		};
	}
}

#endif