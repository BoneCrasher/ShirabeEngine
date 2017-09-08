#ifndef __SHIRABE__MEASUREMENT_DATA_STORE_H__
#define __SHIRABE__MEASUREMENT_DATA_STORE_H__

#include <cmath>
#include <math.h>
#include <vector>

#include "Memory/TypeBasedAllocatorBase/Allocator.h"

#include "Core/EngineStatus.h"

#include "Benchmarking/MeasurementChunk.h"
#include "Benchmarking/MeasurementChunkTraits.h"

namespace Engine {
	namespace Benchmarking {

#define DEFAULT_CAPACITY 12

		/**************************************************************************************************
		 * Class:	MeasurementDataStore
		 *
		 * Summary:	A measurement data store.
		 *
		 * Author:	Dotti Developer
		 *
		 * Date:	02/07/2017
		 *
		 * Typeparams:
		 * ChunkT -   	Type of the chunk t.
		 * ChunkT -   	Type of the chunk t.
		 **************************************************************************************************/

		template <typename ChunkT,
			typename AllocT  = Engine::Memory::Allocator<ChunkT>,
			typename TraitsT = MeasurementChunkTraits<ChunkT, std::vector<ChunkT>>>
			class MeasurementDataStore {
			public:
				typedef MeasurementDataStore<ChunkT, AllocT, TraitsT> class_type;
				typedef ChunkT  chunk_type;
				typedef AllocT  alloc_type;
				typedef TraitsT traits_type;
				typedef std::vector<chunk_type *> chunk_container_type;
				typedef typename chunk_container_type::size_type container_size_type;

				typedef typename AllocT::difference_type difference_type;
				typedef typename AllocT::pointer         pointer;
				typedef typename AllocT::const_pointer   const_pointer;
				typedef typename AllocT::reference       reference;
				typedef typename AllocT::const_reference const_reference;
				typedef typename AllocT::size_type       size_type;

			public:
				MeasurementDataStore(const size_type max_capacity = DEFAULT_CAPACITY) {
					_max_chunks = max_capacity;
				}

				virtual ~MeasurementDataStore() {
					this->clear();
				}

				virtual void clear() {
					if (_chunks.size() <= 0)
						return;

					for (size_type chunkIdx = 0; chunkIdx < _chunks.size(); ++chunkIdx)
						this->_allocator.deallocate(_chunks.at(chunkIdx), 0);

					_chunks.clear();
				}

				template <typename ParamT, typename ValT>
				void push_chunk(const ParamT param, const ValT val) {
					/* Make sure that the chunklist is not null. */
					this->__validateChunkList();

					if (_chunks.size() == _max_chunks)
						// first in, first out: pop first item in list.
						_chunks.erase(_chunks.begin());

					// insert back,
					_chunks.push_back(this->create<ParamT, ValT>(param, val));
				}

				template <typename ParamT, typename ValT>
				pointer create(const ParamT param, const ValT val) {
					pointer p = this->_allocator.allocate(1);
					this->_allocator.construct(p, param, val);

					return p;
				}

				const typename chunk_type *get_chunk_at(size_type index, EEngineStatus *statusFlag = NULL) const {
					chunk_type *pChunk = NULL;

					this->__validateChunkList();

					if (_chunks.size() > index)
					{
						pChunk = _chunks.at(index);
					} else
						if (statusFlag) statusFlag = RESULT::INVALID_ARGUMENT;

					return pChunk;
				}

				const size_type max_size() const { return _max_chunks; }
				const size_type size()     const { return _chunks.size(); }

				const typename chunk_type::param_type average(
					difference_type off = 0,
					size_type       n   = 0) const
				{
					typename chunk_type::param_type av = 0;
					
					/* At least two chunks required to calc average */
					if (_chunks.size() >= 2)
					{
						// Make sure off is in a valid boundary
						off = std::fminl((_chunks.size() - 1), std::fmaxl(0, off));
						// Make sure to average _n entries at max or less
						// if there are not enough chunks stored starting 
						// from _off.
						n = std::fminl((_chunks.size() - off - 1), n);

						typename chunk_type::param_type interval = 0;
						interval = _chunks.at(n - 1)->parameter() - _chunks.at(off)->parameter();
						if (interval > 0)
						{
							for (size_type k = 0; k < _chunks.size(); ++k)
								av += _chunks.at(k)->value();

							av = (typename chunk_type::param_type) ((av / interval) + 0.5);
						}
					}

					return av;
				}

			private:
				void __validateChunkList() { /*if(_chunks == NULL) _chunks = new chunk_container_type();*/ }

			private:
				alloc_type            _allocator;
				size_type             _max_chunks;
				chunk_container_type  _chunks;
		};
	}
}

#endif