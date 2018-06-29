#ifndef __SHIRABE__MEASUREMENT_DATA_STORE_H__
#define __SHIRABE__MEASUREMENT_DATA_STORE_H__

#include <cmath>
#include <math.h>
#include <vector>
#include <algorithm>

#include "Core/Memory/TypeBasedAllocatorBase/Allocator.h"

#include "Core/EngineStatus.h"

#include "Core/Benchmarking/MeasurementChunk.h"
#include "Core/Benchmarking/MeasurementChunkTraits.h"

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
        MeasurementDataStore(const size_type max_capacity = DEFAULT_CAPACITY)
          : m_max_chunks(max_capacity)
        {}

				virtual ~MeasurementDataStore() {
					this->clear();
				}

				virtual void clear() {
					if (m_chunks.size() <= 0)
						return;

					for (size_type chunkIdx = 0; chunkIdx < m_chunks.size(); ++chunkIdx)
						this->m_allocator.deallocate(m_chunks.at(chunkIdx), 0);

					m_chunks.clear();
				}

				template <typename ParamT, typename ValT>
				void push_chunk(const ParamT param, const ValT val) {
					/* Make sure that the chunklist is not null. */
					this->__validateChunkList();

					if (m_chunks.size() == m_max_chunks)
						// first in, first out: pop first item in list.
						m_chunks.erase(m_chunks.begin());

					// insert back,
					m_chunks.push_back(this->create<ParamT, ValT>(param, val));
				}

				template <typename ParamT, typename ValT>
				pointer create(const ParamT param, const ValT val) {
					pointer p = this->m_allocator.allocate(1);
					this->m_allocator.construct(p, param, val);

					return p;
				}

				const typename chunk_type *get_chunk_at(size_type index, EEngineStatus *statusFlag = NULL) const {
					chunk_type *pChunk = NULL;

					this->__validateChunkList();

					if (m_chunks.size() > index)
					{
						pChunk = m_chunks.at(index);
					} else
						if (statusFlag) statusFlag = RESULT::INVALID_ARGUMENT;

					return pChunk;
				}

				const size_type max_size() const { return m_max_chunks; }
				const size_type size()     const { return m_chunks.size(); }

				const typename chunk_type::param_type average(
					difference_type off = 0,
					size_type       n   = 0) const
				{
					typename chunk_type::param_type av = 0;
					
					/* At least two chunks required to calc average */
					if (m_chunks.size() >= 2)
					{
						// Make sure off is in a valid boundary
						off = std::min<uint32_t>((m_chunks.size() - 1u), std::max<uint32_t>(0u, off));
						// Make sure to average m_n entries at max or less
						// if there are not enough chunks stored starting 
						// from m_off.
						n = std::min((m_chunks.size() - off - 1), n);

						typename chunk_type::param_type interval = 0;
						interval = m_chunks.at(n)->parameter() - m_chunks.at(off)->parameter();
						if (interval > 0)
						{
							for (size_type k = 0; k < m_chunks.size(); ++k)
								av += m_chunks.at(k)->value();

							av = (typename chunk_type::param_type) ((av / interval) + 0.5);
						}
					}

					return av;
				}

			private:
				void __validateChunkList() { /*if(m_chunks == NULL) m_chunks = new chunk_container_type();*/ }

			private:
				alloc_type            m_allocator;
				size_type             m_max_chunks;
				chunk_container_type  m_chunks;
		};
	}
}

#endif