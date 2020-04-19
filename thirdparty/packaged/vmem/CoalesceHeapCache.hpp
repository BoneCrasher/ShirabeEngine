/*
	Copyright 2010 - 2017 PureDev Software Ltd. All Rights Reserved.

	This file is part of VMem.

	VMem is dual licensed. For use in open source software VMem can
	be used under the GNU license. For use in commercial applications a
	license can be purchased from PureDev Software.

	If used under the GNU license VMem is free software: you can redistribute
	it and/or modify it under the terms of the GNU General Public License as
	published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.

	Under the GNU Public License VMem is distributed in the hope that it will
	be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with VMem. If not, see <http://www.gnu.org/licenses/>.

	VMem can only be used in commercial products if a commercial license has been purchased
	from PureDev Software. Please see http://www.puredevsoftware.com/vmem/License.htm
*/
//------------------------------------------------------------------------
#ifndef VMEM_COALESCEHEAPCACHE_H_INCLUDED
#define VMEM_COALESCEHEAPCACHE_H_INCLUDED

//------------------------------------------------------------------------
#include "VMemCore.hpp"
#include "InternalHeap.hpp"
#include "RelaxedAtomic.hpp"
#include "List.hpp"

#ifdef VMEM_COALESCE_HEAP_CACHE
	#include <atomic>
#endif

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
#ifdef VMEM_COALESCE_HEAP_CACHE
	#define VMEM_COALESCE_CACHE_ALLOC_MARKER 0xccaaccaa

	//------------------------------------------------------------------------
	class CoalesceHeapCache
	{
		//------------------------------------------------------------------------
		struct CachedAlloc
		{
			CachedAlloc* mp_Prev;
			CachedAlloc* mp_Next;
			int m_Size;
			int m_Frame;
			int m_Marker;
		};

		//------------------------------------------------------------------------
		struct AllocList
		{
			RelaxedAtomic32<int> m_Empty;
			List<CachedAlloc> m_Allocs;
			CriticalSection m_CriticalSection;
			AllocList* mp_Prev;
			AllocList* mp_Next;
		};

	public:
		//------------------------------------------------------------------------
		CoalesceHeapCache(InternalHeap& internal_heap)
		:	m_MinSize(0),
			m_MaxSize(0),
			mp_SizeAllocLists(NULL),
			m_Size(0),
			m_InternalHeap(internal_heap),
			m_SizesCount(0),
			m_Frame(0)
		{
		}

		//------------------------------------------------------------------------
		~CoalesceHeapCache()
		{
			VMEM_ASSERT(m_Size == 0, "allocs still in cache");

			if(mp_SizeAllocLists)
			{
				for(int i=0; i<m_SizesCount; ++i)
					mp_SizeAllocLists[i].~AllocList();

				m_InternalHeap.Free(mp_SizeAllocLists);
			}
		}

		//------------------------------------------------------------------------
		bool Initialise(int min_size, int max_size)
		{
			VMEM_ASSERT(min_size >= sizeof(CachedAlloc), "invalid min size");
			VMEM_ASSERT(max_size <= VMEM_COALESCE_HEAP_CACHE_MAX_BUCKET_SIZE, "invalid max size");

			m_MinSize = AlignDownPow2(min_size, m_Alignment);
			m_MaxSize = AlignUpPow2(max_size, m_Alignment);
			
			m_SizesCount = (m_MaxSize - m_MinSize) / VMEM_COALESCE_HEAP_CACHE_BUCKET_STEP + 1;
			
			int sizes_size = m_SizesCount * sizeof(AllocList);
			mp_SizeAllocLists = (AllocList*)m_InternalHeap.Alloc(sizes_size);
			if(!mp_SizeAllocLists)
			{
				m_SizesCount = 0;
				return false;
			}

			memset(mp_SizeAllocLists, 0, sizes_size);
			for(int i=0; i<m_SizesCount; ++i)
			{
				new (mp_SizeAllocLists+i)AllocList();
				mp_SizeAllocLists[i].m_Empty = true;
			}

			return true;
		}

		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE bool Add(void* p, int size)
		{
			VMEM_ASSERT(size >= m_MinSize && size <= m_MaxSize, "invalid size");

			int index = (size - m_MinSize) / VMEM_COALESCE_HEAP_CACHE_BUCKET_STEP;
			AllocList& alloc_list = mp_SizeAllocLists[index];

			VMEM_ASSERT(size >= sizeof(CachedAlloc), "invalid size");
			CachedAlloc* p_cached_alloc = (CachedAlloc*)p;
			VMEM_ASSERT_CODE(p_cached_alloc->mp_Prev = p_cached_alloc->mp_Next = NULL);
			p_cached_alloc->m_Size = size;
			p_cached_alloc->m_Frame = m_Frame;
			p_cached_alloc->m_Marker = VMEM_COALESCE_CACHE_ALLOC_MARKER;

			bool was_empty;
			{
				CriticalSectionScope lock(alloc_list.m_CriticalSection);

				was_empty = alloc_list.m_Allocs.Empty();

				alloc_list.m_Empty = false;
				alloc_list.m_Allocs.AddHead(p_cached_alloc);
			}

			m_Size += size;

			if(was_empty)
				AddToUpdateList(&alloc_list);

			return true;
		}

		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE void* Remove(int size)
		{
			VMEM_ASSERT(size >= m_MinSize && size <= m_MaxSize, "invalid size");

			#ifdef VMEM_X64
				VMEM_ASSERT((size % m_Alignment) == 0, "invalid size");
				VMEM_ASSERT(m_Alignment == VMEM_NATURAL_ALIGNMENT, "m_Alignment has been corrupted");
			#else
				size = AlignSizeUpPow2(size, m_Alignment);
			#endif

			int index = (size - m_MinSize + VMEM_COALESCE_HEAP_CACHE_BUCKET_STEP - 1) / VMEM_COALESCE_HEAP_CACHE_BUCKET_STEP;
			AllocList& alloc_list = mp_SizeAllocLists[index];

			if(alloc_list.m_Empty)
				return NULL;

			CachedAlloc* p = NULL;
			{
				CriticalSectionScope lock(alloc_list.m_CriticalSection);

				if(alloc_list.m_Allocs.Empty())
					alloc_list.m_Empty = true;
				else
					p = alloc_list.m_Allocs.RemoveHead();
			}

			if (p)
			{
				VMEM_ASSERT(p->m_Marker == VMEM_COALESCE_CACHE_ALLOC_MARKER, "CoalesceHeap Cache alloc has been corrupted");
				int actual_size = p->m_Size;
				m_Size -= actual_size;
				VMEM_MEMSET(p, VMEM_ALLOCATED_MEM, actual_size);
			}

			return p;
		}

		//------------------------------------------------------------------------
		void Trim(size_t max_size, void*& p_allocs_to_free)
		{
			CriticalSectionScope lock(m_CriticalSection);

			AllocList* p_alloc_list = m_UpdateList.GetTail();
			AllocList* p_alloc_list_end = m_UpdateList.GetIterEnd();
			
			while(p_alloc_list != p_alloc_list_end)
			{
				CriticalSectionScope list_lock(p_alloc_list->m_CriticalSection);

				AllocList* p_prev_alloc_list = p_alloc_list->mp_Prev;

				List<CachedAlloc>& allocs = p_alloc_list->m_Allocs;

				CachedAlloc* p_iter_end = allocs.GetIterEnd();
				CachedAlloc* p_alloc = allocs.GetTail();

				while(p_alloc != p_iter_end)
				{
					CachedAlloc* p_prev_alloc = p_alloc->mp_Prev;

					if(m_Size > max_size || p_alloc->m_Frame + VMEM_COALESCE_HEAP_CACHE_MAX_AGE < m_Frame)
					{
						size_t size = p_alloc->m_Size;
						m_Size -= size;

						CachedAlloc* p_removed_alloc = allocs.RemoveTail();
						VMEM_ASSERT(p_removed_alloc == p_alloc, "fail");
						VMEM_UNREFERENCED_PARAM(p_removed_alloc);

						VMEM_ASSERT(p_alloc->m_Marker == VMEM_COALESCE_CACHE_ALLOC_MARKER, "CoalesceHeap Cache alloc has been corrupted");

						void* p = p_alloc;
						VMEM_MEMSET(p, VMEM_UNUSED_PAGE, size);

						*(void**)p = p_allocs_to_free;
						p_allocs_to_free = p;
					}
					else
					{
						break;
					}

					p_alloc = p_prev_alloc;
				}

				if(allocs.Empty())
				{
					p_alloc_list->m_Empty = true;
					RemoveFromUpdateList_NoLock(p_alloc_list);
				}

				p_alloc_list = p_prev_alloc_list;
			}
		}

		//------------------------------------------------------------------------
		size_t GetSize() const
		{
			return m_Size;
		}

		//------------------------------------------------------------------------
		void Update()
		{
			++m_Frame;
		}

		//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
		size_t GetMemoryUsage() const
		{
			return m_SizesCount * sizeof(AllocList);
		}
#endif

	private:
		//------------------------------------------------------------------------
		void AddToUpdateList(AllocList* p_allocs_list)
		{
			CriticalSectionScope lock(m_CriticalSection);

			// need to check that someone else didn't add it to the list before we aquired the lock
			if(!p_allocs_list->mp_Prev)
				m_UpdateList.AddTail(p_allocs_list);
		}

		//------------------------------------------------------------------------
		void RemoveFromUpdateList_NoLock(AllocList* p_alloc_list)
		{
			m_UpdateList.Remove(p_alloc_list);
		}

		//------------------------------------------------------------------------
		// data
	private:
#if VMEM_NATURAL_ALIGNMENT > 16
		static const int m_Alignment = VMEM_NATURAL_ALIGNMENT;
#else
		static const int m_Alignment = 16;
#endif
		int m_MinSize;
		int m_MaxSize;

		AllocList* mp_SizeAllocLists;

		std::atomic<size_t> m_Size;

		List<AllocList> m_UpdateList;

		InternalHeap& m_InternalHeap;

		CriticalSection m_CriticalSection;

		int m_SizesCount;

		int m_Frame;
	};
#endif		// #ifdef VMEM_COALESCE_HEAP_CACHE
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_COALESCEHEAPCACHE_H_INCLUDED

