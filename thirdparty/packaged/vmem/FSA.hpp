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
#ifndef VMEM_FSA_H_INCLUDED
#define VMEM_FSA_H_INCLUDED

//------------------------------------------------------------------------
#include "VMemCore.hpp"
#include "VMemStats.hpp"
#include "TrailGuard.hpp"
#include "TrailGuard.hpp"
#include "VMemCriticalSection.hpp"
#include "FSACache.hpp"
#include "PageHeap.hpp"
#include "List.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

#if defined(VMEM_CHECK_ALLOCATED_MARKERS) || defined(VMEM_ENABLE_STATS)
	#define VMEM_FSA_TRACK_FULL_PAGES
#endif

#if defined(VMEM_ASSERTS) || defined(VMEM_X64)
	#define VMEM_FSA_HEADER_MARKER
#endif

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	class FSA;

	//------------------------------------------------------------------------
	struct FSAPageHeader
	{
#ifdef VMEM_FSA_HEADER_MARKER
		unsigned int m_Marker;
#endif
		int m_UsedSlotCount;
		FSA* mp_FSA;
		void* mp_FreeSlots;
		FSAPageHeader* mp_Prev;
		FSAPageHeader* mp_Next;
	};

	//------------------------------------------------------------------------
	class FSAUpdateList
	{
	public:
		FSAUpdateList()
		:	mp_List(NULL)
		{
		}

		VMEM_FORCE_INLINE void Add(FSA* p_fsa);

		VMEM_FORCE_INLINE FSA* GetAndClearList();

	private:
		FSA* mp_List;
		CriticalSection m_CriticalSection;
	};

	//------------------------------------------------------------------------
#ifdef VMEM_FSA_GUARDS
	// p points to the start of the slot
	inline void SetGuards(void* p, int size, int slot_size)
	{
		SetGuards(p, VMEM_FSA_PRE_GUARD_SIZE);
		SetGuards((byte*)p + size, slot_size - size - VMEM_FSA_PRE_GUARD_SIZE);
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_FSA_GUARDS
	inline void CheckGuards(void* p, int size, int slot_size)
	{
		CheckMemory((byte*)p, VMEM_FSA_PRE_GUARD_SIZE, VMEM_GUARD_MEM);
		CheckMemory((byte*)p + VMEM_FSA_PRE_GUARD_SIZE + size, slot_size - size - VMEM_FSA_PRE_GUARD_SIZE, VMEM_GUARD_MEM);
	}
#endif

	//------------------------------------------------------------------------
	// FSA is a Fixed Size Allocator. It is setup with an allocation size, alignment
	// and a page heap. It allocates pages from the page heap and divides them up
	// into slots of the allocation size. The pages are put onto a free page list.
	// The slots are linked onto a free list. When allocating a slot of popped off
	// the free list, and added back onto the free list when freeing. When a page is
	// full it is removed from the free page list. When a page is empty it is given
	// back to the page heap. When a slot is freed from a full page the page is put
	// back onto the free page list. The freee page list is sorted by address for
	// biasing to reduce fragmentation.
	// This class is not thread safe.
	class FSA
	{
		friend FSAUpdateList;

	public:
		FSA(
			int size,
			int alignment,					// allocation size, alignment of each allocation and the page heap.
			PageHeap* p_page_heap,
			InternalHeap& internal_heap,
			FSAUpdateList& fsa_update_list,
			int page_size);

		~FSA();

		bool Initialise();

		VMEM_FORCE_INLINE void* Alloc();

		VMEM_FORCE_INLINE void Free(void* p);

		VMEM_FORCE_INLINE void Free(void* p, int page_size);

		VMEM_FORCE_INLINE static FSA* GetFSA(void* p, int page_size);		// get the FSA object for the address. p must be from an FSA.

		VMEM_FORCE_INLINE int GetSize() const;						// the allocation size

		void Flush();

#if defined(VMEM_FSA_PAGE_CACHE) || defined(VMEM_POPULATE_FSA_CACHE)
		void Update();
#endif
		void CheckIntegrity() const;

#ifdef VMEM_ENABLE_STATS
		inline const Stats& GetStats() const;
		inline const Stats& GetStatsNoLock() const;
		size_t WriteAllocs(int* p_allocs_written_count=NULL, int max_allocs_to_write=VMEM_WRITE_ALLOCS_MAX, bool write_header=true) const;
		void SendStatsToMemPro(MemProSendFn send_fn, void* p_context);
#endif

#ifdef VMEM_TRAIL_GUARDS
		inline void InitialiseTrailGuard(int size, int check_freq);
#endif

		void Lock() { m_CriticalSection.Enter(); }

		void Release() { m_CriticalSection.Leave(); }

		FSA* GetNextUpdateFSA() const { return mp_NextUpdateFSA; }

	protected:
#ifdef VMEM_FSA_CACHE
		void AddAllocToStats();
		void RemoveAllocFromStats();
		#ifdef VMEM_POPULATE_FSA_CACHE
			void PopulateCache();
		#endif
#endif

	private:
		VMEM_FORCE_INLINE void* Alloc_NoLock();

		VMEM_FORCE_INLINE void Free_NoLock(void* p, int page_size);

		void CheckIntegrity(FSAPageHeader* p_page) const;

		FSAPageHeader* AllocPage();

		VMEM_NO_INLINE FSAPageHeader* AllocPageAndInsert();

		void FreePage(FSAPageHeader* p_page);

		void InsertPageInFreeList(FSAPageHeader* p_page);

#ifdef VMEM_FSA_PAGE_CACHE
		VMEM_NO_INLINE void AddPageToEmptyList(FSAPageHeader* p_page);
		void ReleaseEmptyPages();
#endif

#ifdef VMEM_FSA_CACHE
		void ClearCache();
#endif
		// not implemented
		FSA(const FSA&);
		FSA& operator=(const FSA&);

		//------------------------------------------------------------------------
		// data
	private:
#ifdef VMEM_FSA_CACHE
		FSACache m_FSACache;
#endif
		int m_Size;				// the allocation size
		int m_SlotSize;			// the slot size can be bigger than the allocation size because of alignment and guards
		int m_SlotsPerPage;

#ifdef VMEM_X64
		static const int m_Alignment = VMEM_NATURAL_ALIGNMENT;
#else
		const int m_Alignment;
#endif
		int m_AlignedHeaderSize;

		PageHeap* mp_PageHeap;

		List<FSAPageHeader> m_FreePageList;		// only contains partially full pages.

		FSAPageHeader* mp_LastInsertedFreePage;

		bool m_InUpdateList;
		FSA* mp_NextUpdateFSA;
		FSAUpdateList& m_UpdateList;

#ifdef VMEM_FSA_PAGE_CACHE
		List<FSAPageHeader> m_EmptyPageList;
		int m_EmptyPageListCount;
#endif

#ifdef VMEM_ASSERTS
		unsigned int m_Marker;
#endif

#ifdef VMEM_FSA_TRACK_FULL_PAGES
		List<FSAPageHeader> m_FullPageList;
#endif

#ifdef VMEM_ENABLE_STATS
		Stats m_Stats;
#endif

#ifdef VMEM_TRAIL_GUARDS
		TrailGuard m_TrailGuard;
#endif
		mutable CriticalSection m_CriticalSection;
	};

	//------------------------------------------------------------------------
	FSA* FSA::GetFSA(void* p, int page_size)
	{
		size_t page_mask = ~(page_size - 1);
		FSAPageHeader* p_page = (FSAPageHeader*)((size_t)p & page_mask);
		#ifdef VMEM_FSA_HEADER_MARKER
			VMEM_MEM_CHECK(&p_page->m_Marker, VMEM_FSA_PAGE_HEADER_MARKER);
		#endif
		FSA* p_fsa = p_page->mp_FSA;
		VMEM_ASSERT_CODE(VMEM_MEM_CHECK(&p_fsa->m_Marker, VMEM_FSA_MARKER));
		return p_fsa;
	}

	//------------------------------------------------------------------------
	void* FSA::Alloc()
	{
#ifdef VMEM_FSA_CACHE
		void* p_cached = m_FSACache.Alloc();
		if(p_cached)
		{
			#ifdef VMEM_ENABLE_STATS
				CriticalSectionScope lock(m_CriticalSection);
				AddAllocToStats();
			#endif
			return p_cached;
		}
#endif
		CriticalSectionScope lock(m_CriticalSection);
		return Alloc_NoLock();
	}

	//------------------------------------------------------------------------
	void* FSA::Alloc_NoLock()
	{
		// get the first page with a free slot
		FSAPageHeader* VMEM_RESTRICT p_page = m_FreePageList.GetHead();		// if m_FreePageList is empty GetHead will return the end iterator which will have a NULL mp_FreeSlots

		void* VMEM_RESTRICT p = p_page->mp_FreeSlots;

		// allocate a page if we need to
		if(!p)
		{
			p_page = AllocPageAndInsert();
			if(!p_page)
				return NULL;		// out of memory
			p = p_page->mp_FreeSlots;
		}

		#ifdef VMEM_FSA_HEADER_MARKER
			VMEM_MEM_CHECK(&p_page->m_Marker, VMEM_FSA_PAGE_HEADER_MARKER);
		#endif

		// take a slot off the page
		VMEM_ASSERT_MEM(p >= p_page+1 && p < (byte*)p_page + mp_PageHeap->GetPageSize(), &p_page->mp_FreeSlots);

		void* VMEM_RESTRICT p_next = *(void**)p;
		p_page->mp_FreeSlots = p_next;
		VMEM_ASSERT_MEM(!p_page->mp_FreeSlots || (p_page->mp_FreeSlots >= p_page+1 && p_page->mp_FreeSlots < (byte*)p_page + mp_PageHeap->GetPageSize()), p);

		++p_page->m_UsedSlotCount;

		// if the page is full take it off the free list
		if(!p_next)
		{
			if(p_page == mp_LastInsertedFreePage)
				mp_LastInsertedFreePage = p_page->mp_Prev;

			m_FreePageList.Remove(p_page);

			#ifdef VMEM_FSA_TRACK_FULL_PAGES
				// put the page onto the full list
				m_FullPageList.AddTail(p_page);
			#endif
		}

		#ifdef VMEM_ENABLE_MEMSET
			if(m_Size > (int)sizeof(void*))
				CheckMemory((byte*)p + sizeof(void*), m_SlotSize - sizeof(void*), VMEM_FREED_MEM);
		#endif
		VMEM_MEMSET(p, VMEM_ALLOCATED_MEM, m_Size);

		#ifdef VMEM_FSA_GUARDS
			SetGuards(p, m_Size, m_SlotSize);
			p = (byte*)p + VMEM_FSA_PRE_GUARD_SIZE;
		#endif

		VMEM_STATS(m_Stats.m_Unused -= m_SlotSize);
		VMEM_STATS(m_Stats.m_Used += m_Size);
		VMEM_STATS(m_Stats.m_Overhead += m_SlotSize - m_Size);

		return p;
	}

	//------------------------------------------------------------------------
	void FSA::Free(void* VMEM_RESTRICT p)
	{
		Free(p, mp_PageHeap->GetPageSize());
	}

	//------------------------------------------------------------------------
	void FSA::Free(void* VMEM_RESTRICT p, int page_size)
	{
#ifdef VMEM_FSA_CACHE
		if(m_FSACache.Free(p))
		{
			#ifdef VMEM_ENABLE_STATS
				CriticalSectionScope lock(m_CriticalSection);
				RemoveAllocFromStats();
			#endif
			return;
		}
#endif
		CriticalSectionScope lock(m_CriticalSection);
		Free_NoLock(p, page_size);
	}

	//------------------------------------------------------------------------
	void FSA::Free_NoLock(void* VMEM_RESTRICT p, int page_size)
	{
		#ifdef VMEM_TRAIL_GUARDS
			if(m_Size >= sizeof(void*) + sizeof(unsigned int) && *((unsigned int*)((void**)p+1)) == VMEM_TRAIL_GUARD_MEM)
			{
				bool already_free = m_TrailGuard.Contains(p);
				VMEM_ASSERT2(!already_free, "Double free of pointer %s%p", VMEM_POINTER_PREFIX, p);
				if(already_free)
					return;
			}
		#endif

		#ifdef VMEM_FSA_GUARDS
			// check for double free
			VMEM_ASSERT(m_SlotSize - m_Size >= (int)sizeof(unsigned int), "VMEM_FSA_POST_GUARD_SIZE must be >= sizeof(unsigned int)");
			if(*(unsigned int*)((byte*)p + m_Size) == VMEM_FREED_MEM)
			{
				VMem::DebugWrite(_T("Possible double free of pointer 0x%p\n"), p);
				if(!VMem::Break(_T("VMem Assert\n")))
					VMEM_DEBUG_BREAK;
				return;
			}

			p = (byte*)p - VMEM_FSA_PRE_GUARD_SIZE;
			CheckGuards(p, m_Size, m_SlotSize);
		#endif

		#ifdef VMEM_TRAIL_GUARDS
			if(m_TrailGuard.GetSize())
			{
				p = m_TrailGuard.Add(p);
				VMEM_STATS(m_Stats.m_Used -= m_Size);		// move stats to overhead
				VMEM_STATS(m_Stats.m_Overhead += m_Size);
				if(!p) return;							// return here if TrailGuard ddin't return an alloc to free

				VMEM_STATS(m_Stats.m_Used += m_Size);		// move stats back to used
				VMEM_STATS(m_Stats.m_Overhead -= m_Size);
				#ifdef VMEM_FSA_GUARDS
					SetGuards(p, m_Size, m_SlotSize);	// re-apply our guards
				#endif
			}
		#endif

		// memset to VMEM_FREED_MEM
		#if defined(VMEM_ENABLE_MEMSET) || defined(VMEM_MEMSET_ONLY_SMALL)
			void* p_memset = p;
			if(m_SlotSize > (int)sizeof(void*))
				VMEM_MEMSET((byte*)p_memset + sizeof(void*), VMEM_FREED_MEM, m_SlotSize - sizeof(void*));
		#endif

		// get the page that owns this alloc
		FSAPageHeader* VMEM_RESTRICT p_page = (FSAPageHeader*)AlignDownPow2(p, page_size);
		#ifdef VMEM_ASSERTS
			VMEM_ASSERT((size_t)p >= (size_t)AlignUp((byte*)p_page + sizeof(FSAPageHeader), m_Alignment) && (size_t)p < (size_t)p_page + m_AlignedHeaderSize + m_SlotSize * m_SlotsPerPage, "Bad pointer passed to Free. Pointer not in free list range");
			VMEM_ASSERT((((size_t)p - (size_t)AlignUp((byte*)p_page + sizeof(FSAPageHeader), m_Alignment)) % m_SlotSize) == 0, "Bad pointer passed to Free. Pointer not aligned to slot");
		#endif

		// if the page was full put it back onto the free list
		void* p_free_slot = p_page->mp_FreeSlots;
		if(!p_free_slot)
		{
			#ifdef VMEM_FSA_TRACK_FULL_PAGES
				m_FullPageList.Remove(p_page);
			#endif
			InsertPageInFreeList(p_page);
		}

		// put the slot back onto the free list
		*(void**)p = p_free_slot;
		p_page->mp_FreeSlots = p;

		// if the page is empty free it
		if(!--p_page->m_UsedSlotCount)
		{
			if(p_page == mp_LastInsertedFreePage)
				mp_LastInsertedFreePage = p_page->mp_Prev;

			m_FreePageList.Remove(p_page);

			#ifdef VMEM_FSA_PAGE_CACHE
				AddPageToEmptyList(p_page);
			#else
				FreePage(p_page);
			#endif
		}

		VMEM_STATS(m_Stats.m_Unused += m_SlotSize);
		VMEM_STATS(m_Stats.m_Used -= m_Size);
		VMEM_STATS(m_Stats.m_Overhead -= m_SlotSize - m_Size);
	}

	//------------------------------------------------------------------------
	int FSA::GetSize() const
	{
		return m_Size;
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	const Stats& FSA::GetStats() const
	{
		CriticalSectionScope lock(m_CriticalSection);
		return m_Stats;
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	const Stats& FSA::GetStatsNoLock() const
	{
		return m_Stats;
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_TRAIL_GUARDS
	void FSA::InitialiseTrailGuard(int size, int check_freq)
	{
		CriticalSectionScope lock(m_CriticalSection);
		m_TrailGuard.Initialise(size, check_freq, m_SlotSize);
	}
#endif

	//------------------------------------------------------------------------
	void FSAUpdateList::Add(FSA* p_fsa)
	{
		CriticalSectionScope lock(m_CriticalSection);
		p_fsa->mp_NextUpdateFSA = mp_List;
		mp_List = p_fsa;
	}

	//------------------------------------------------------------------------
	FSA* FSAUpdateList::GetAndClearList()
	{
		CriticalSectionScope lock(m_CriticalSection);
		FSA* p_list = mp_List;
		mp_List = NULL;
		return p_list;
	}
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_FSA_H_INCLUDED

