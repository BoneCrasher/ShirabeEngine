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
#include "VMem_PCH.hpp"
#include "FSA.hpp"
#include "PageHeap.hpp"
#include "VMemMemProStats.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
#ifdef VMEM_FSA_GUARDS
	VMEM_STATIC_ASSERT((VMEM_FSA_PRE_GUARD_SIZE & 3) == 0, "pre-guard size not aligned");
	VMEM_STATIC_ASSERT((VMEM_FSA_POST_GUARD_SIZE & 3) == 0, "post guard size not aligned");
#endif

	//------------------------------------------------------------------------
	FSA::FSA(
		int size,
		int alignment,
		PageHeap* p_page_heap,
		InternalHeap& internal_heap,
		FSAUpdateList& fsa_update_list,
		int page_size)
	:
#ifdef VMEM_FSA_CACHE
		m_FSACache(AlignUpToNextPow2(VMEM_FSA_CACHE_SIZE / size), internal_heap),
#endif
		m_Size(size),
		m_SlotSize(size),
#ifndef VMEM_X64
		m_Alignment(alignment),
#endif
		mp_PageHeap(p_page_heap),
		mp_LastInsertedFreePage(NULL),
		m_InUpdateList(false),
		mp_NextUpdateFSA(NULL),
		m_UpdateList(fsa_update_list)
#ifdef VMEM_FSA_PAGE_CACHE
		,m_EmptyPageListCount(0)
#endif
	{
#ifndef VMEM_FSA_CACHE
		VMEM_UNREFERENCED_PARAM(internal_heap);
#endif

#ifdef VMEM_ASSERTS
		m_Marker = VMEM_FSA_MARKER;
#endif

		if((page_size % alignment) == 0)
			m_AlignedHeaderSize = AlignUp(ToInt(sizeof(FSAPageHeader)), alignment);
		else
			m_AlignedHeaderSize = sizeof(FSAPageHeader) + alignment;

#ifdef VMEM_X64
		VMEM_ASSERT(alignment == m_Alignment, "Non-natural alignment not supported in 64bit");
#endif

#ifdef VMEM_FSA_GUARDS
		m_SlotSize += VMEM_FSA_PRE_GUARD_SIZE + VMEM_FSA_POST_GUARD_SIZE;
#endif
		m_SlotSize = AlignUp(m_SlotSize, alignment);

		m_SlotsPerPage = (p_page_heap->GetPageSize() - m_AlignedHeaderSize) / m_SlotSize;

#ifdef VMEM_FSA_CACHE
		VMEM_STATS(m_Stats.m_Overhead += m_FSACache.GetMemoryOverhead());
#endif

		// it's important to setup these values because they are returned by GetHead if the list is empty
		FSAPageHeader* p_iter_end = m_FreePageList.GetIterEnd();
		#ifdef VMEM_FSA_HEADER_MARKER
			p_iter_end->m_Marker = VMEM_FSA_PAGE_HEADER_MARKER;
		#endif
		p_iter_end->mp_FSA = NULL;
		p_iter_end->mp_FreeSlots = NULL;
		p_iter_end->m_UsedSlotCount = 0;

		mp_LastInsertedFreePage = m_FreePageList.GetIterEnd();

#ifdef VMEM_FSA_TRACK_FULL_PAGES
		FSAPageHeader* p_full_iter_end = m_FullPageList.GetIterEnd();
		#ifdef VMEM_FSA_HEADER_MARKER
			p_full_iter_end->m_Marker = VMEM_FSA_PAGE_HEADER_MARKER;
		#endif
		p_full_iter_end->mp_FSA = NULL;
		p_full_iter_end->mp_FreeSlots = NULL;
		p_full_iter_end->m_UsedSlotCount = 0;
#endif
	}

	//------------------------------------------------------------------------
	FSA::~FSA()
	{
#ifdef VMEM_FSA_CACHE
		int cache_count = m_FSACache.GetCount();
		for(int i=0; i<cache_count; ++i)
		{
			void* p = m_FSACache[i];
			if(p)
				Free(p);
		}
#endif

#ifdef VMEM_TRAIL_GUARDS
		void* p_trail_alloc = m_TrailGuard.Shutdown();
		while(p_trail_alloc)
		{
			void* p_next = *(void**)p_trail_alloc;
#ifdef VMEM_FSA_GUARDS
			SetGuards(p_trail_alloc, m_Size, m_SlotSize);	// re-apply our guards
#endif
			Free(p_trail_alloc);
			p_trail_alloc = p_next;
		}
#endif
	}

	//------------------------------------------------------------------------
	bool FSA::Initialise()
	{
#ifdef VMEM_FSA_CACHE
		return m_FSACache.Initialise();
#else
		return true;
#endif
	}

	//------------------------------------------------------------------------
	FSAPageHeader* FSA::AllocPageAndInsert()
	{
		FSAPageHeader* p_page = AllocPage();

		VMEM_ASSERT(m_FreePageList.Empty(), "m_FreePageList shouold be empty at this point");
		if(p_page)
			m_FreePageList.AddHead(p_page);

		return p_page;
	}

	//------------------------------------------------------------------------
	FSAPageHeader* FSA::AllocPage()
	{
#ifdef VMEM_FSA_PAGE_CACHE
		VMEM_ASSERT(m_EmptyPageListCount == m_EmptyPageList.GetCount(), "Empty page list has been corrupted");
		if(m_EmptyPageListCount)
		{
			--m_EmptyPageListCount;
			VMEM_ASSERT(m_EmptyPageListCount >= 0, "m_EmptyPageListCount has been corrupted");

			return m_EmptyPageList.RemoveHead();
		}
#endif

		// allocate the page
		FSAPageHeader* p_page = (FSAPageHeader*)mp_PageHeap->Alloc();
		if(!p_page)
			return NULL;	// out of memory

		// put all slots onto the free list
		// note that slots are always aligned to slot size
		byte* p_free_slots = (byte*)AlignUp((byte*)p_page + sizeof(FSAPageHeader), m_Alignment);
		byte* p = p_free_slots;
		byte* p_end = p_free_slots + (m_SlotsPerPage-1) * m_SlotSize;
		#if defined(VMEM_ENABLE_MEMSET) || defined(VMEM_MEMSET_ONLY_SMALL)
			bool do_memset = m_Size > (int)sizeof(void*);
		#endif
		while(p != p_end)
		{
			byte* p_next = p + m_SlotSize;
			*(byte**)p = p_next;
			#if defined(VMEM_ENABLE_MEMSET) || defined(VMEM_MEMSET_ONLY_SMALL)
				if(do_memset) VMEM_MEMSET((byte*)p + sizeof(void*), VMEM_FREED_MEM, m_SlotSize - sizeof(void*));
			#endif
			p = p_next;
		}
		#if defined(VMEM_ENABLE_MEMSET) || defined(VMEM_MEMSET_ONLY_SMALL)
			if(do_memset) VMEM_MEMSET((byte*)p + sizeof(void*), VMEM_FREED_MEM, m_SlotSize - sizeof(void*));
		#endif
		*(byte**)p = NULL;

		// setup the page
		p_page->mp_FSA = this;
		p_page->mp_FreeSlots = p_free_slots;
		p_page->m_UsedSlotCount = 0;
		p_page->mp_Prev = NULL;
		p_page->mp_Next = NULL;

#ifdef VMEM_ASSERTS
		int page_size = mp_PageHeap->GetPageSize();
		int used_size = m_AlignedHeaderSize + m_SlotsPerPage * m_SlotSize;
		int unused_size = page_size - used_size;
		if (unused_size)
			memset((byte*)p_page + used_size, VMEM_GUARD_MEM, unused_size);
#endif

#ifdef VMEM_FSA_HEADER_MARKER
		p_page->m_Marker = VMEM_FSA_PAGE_HEADER_MARKER;
#endif
		VMEM_STATS(m_Stats.m_Unused += page_size - m_AlignedHeaderSize);
		VMEM_STATS(m_Stats.m_Overhead += m_AlignedHeaderSize);

		return p_page;
	}

	//------------------------------------------------------------------------
	void FSA::FreePage(FSAPageHeader* p_page)
	{
		#ifdef VMEM_FSA_HEADER_MARKER
			VMEM_MEM_CHECK(&p_page->m_Marker, VMEM_FSA_PAGE_HEADER_MARKER);
		#endif

		VMEM_ASSERT_CODE(CheckIntegrity(p_page));

		mp_PageHeap->Free(p_page);

		VMEM_STATS(m_Stats.m_Unused -= mp_PageHeap->GetPageSize() - m_AlignedHeaderSize);
		VMEM_STATS(m_Stats.m_Overhead -= m_AlignedHeaderSize);
	}

	//------------------------------------------------------------------------
	// keeps the free page list sorted by address for the biasing
	void FSA::InsertPageInFreeList(FSAPageHeader* p_page)
	{
		#ifdef VMEM_FSA_HEADER_MARKER
			VMEM_MEM_CHECK(&p_page->m_Marker, VMEM_FSA_PAGE_HEADER_MARKER);
		#endif

#ifdef VMEM_DISABLE_BIASING
		FSAPageHeader* p_srch_page = m_FreePageList.GetHead();
#else
		// find the previous page < p_page
		FSAPageHeader* p_end_page = m_FreePageList.GetIterEnd();
		FSAPageHeader* p_srch_page = mp_LastInsertedFreePage != p_end_page ? mp_LastInsertedFreePage : m_FreePageList.GetHead();

		if(p_srch_page != p_end_page && p_srch_page > p_page)
		{
			do
			{
				p_srch_page = p_srch_page->mp_Prev;
			} while(p_srch_page != p_end_page && p_srch_page > p_page);
		}
		else
		{
			while(p_srch_page != p_end_page && p_srch_page < p_page)
				p_srch_page = p_srch_page->mp_Next;

			p_srch_page = p_srch_page->mp_Prev;
		}

		mp_LastInsertedFreePage = p_page;
#endif
		m_FreePageList.Insert(p_srch_page, p_page);
	}

	//------------------------------------------------------------------------
	void FSA::Flush()
	{
#if defined(VMEM_FSA_CACHE) || defined(VMEM_FSA_PAGE_CACHE)
		CriticalSectionScope lock(m_CriticalSection);
#endif

#ifdef VMEM_FSA_CACHE
		ClearCache();
#endif

#ifdef VMEM_FSA_PAGE_CACHE
		ReleaseEmptyPages();
#endif
	}

	//------------------------------------------------------------------------
#ifdef VMEM_FSA_CACHE
	void FSA::ClearCache()
	{
		int page_size = mp_PageHeap->GetPageSize();

		for(int i=0; i<m_FSACache.GetCount(); ++i)
		{
			void* p = m_FSACache[i];
			if(p)
			{
				#ifdef VMEM_ENABLE_STATS
					CriticalSectionScope lock(m_CriticalSection);
					AddAllocToStats();
				#endif

				Free_NoLock(p, page_size);

				if(m_FSACache.Set(p, i))
					RemoveAllocFromStats();
			}
		}
	}
#endif

	//------------------------------------------------------------------------
#if defined(VMEM_FSA_PAGE_CACHE) || defined(VMEM_POPULATE_FSA_CACHE)
	void FSA::Update()
	{
		CriticalSectionScope lock(m_CriticalSection);

		#ifdef VMEM_FSA_PAGE_CACHE
			VMEM_ASSERT(m_EmptyPageListCount == m_EmptyPageList.GetCount(), "Empty page list has been corrupted");
			int remove_count = m_EmptyPageListCount - VMEM_FSA_PAGE_CACHE_COUNT_MIN;
			if(remove_count > 0)
			{
				for(int i=0; i<remove_count; ++i)
				{
					FSAPageHeader* p_page = m_EmptyPageList.RemoveTail();
					FreePage(p_page);
				}
				m_EmptyPageListCount = VMEM_FSA_PAGE_CACHE_COUNT_MIN;
			}

			mp_NextUpdateFSA = NULL;
		#endif

		m_InUpdateList = false;

#if defined(VMEM_FSA_CACHE) && defined(VMEM_POPULATE_FSA_CACHE)
		PopulateCache();
#endif
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_FSA_PAGE_CACHE
	void FSA::ReleaseEmptyPages()
	{
		while(!m_EmptyPageList.Empty())
		{
			FSAPageHeader* p_page = m_EmptyPageList.RemoveTail();
			FreePage(p_page);
		}

		m_EmptyPageListCount = 0;
	}
#endif

	//------------------------------------------------------------------------
#if defined(VMEM_FSA_CACHE) && defined(VMEM_POPULATE_FSA_CACHE)
	void FSA::PopulateCache()
	{
		void* p = NULL;

		for(int i=0; i<m_FSACache.GetCount(); ++i)
		{
			if(!m_FSACache[i])
			{
				if(!p)
					p = Alloc_NoLock();

				if(m_FSACache.Set(p, i))
					RemoveAllocFromStats();
			}
		}

		if(p)
			Free_NoLock(p, mp_PageHeap->GetPageSize());
	}
#endif

	//------------------------------------------------------------------------
	void FSA::CheckIntegrity() const
	{
		CriticalSectionScope lock(m_CriticalSection);

		m_FreePageList.CheckIntegrity();

		VMEM_ASSERT_MEM(m_FreePageList.GetIterEnd()->mp_FreeSlots == NULL, m_FreePageList.GetIterEnd()->mp_FreeSlots);

		const FSAPageHeader* p_end = m_FreePageList.GetIterEnd();
		FSAPageHeader* p_prev = NULL;
		for(FSAPageHeader* p_page=m_FreePageList.GetHead(); p_page!=p_end; p_page=p_page->mp_Next)
		{
			CheckIntegrity(p_page);
			#ifndef VMEM_DISABLE_BIASING
				VMEM_ASSERT(p_page > p_prev, "FSA free list is not sorted correctly");
			#endif
			VMEM_UNREFERENCED_PARAM(p_prev);
			p_prev = p_page;
		}

#ifdef VMEM_CHECK_ALLOCATED_MARKERS
		const FSAPageHeader* p_full_end = m_FullPageList.GetIterEnd();
		for (FSAPageHeader* p_page = m_FullPageList.GetHead(); p_page != p_full_end; p_page = p_page->mp_Next)
			CheckIntegrity(p_page);
#endif
			
#ifdef VMEM_TRAIL_GUARDS
		m_TrailGuard.CheckIntegrity();
#endif
	}

	//------------------------------------------------------------------------
	void FSA::CheckIntegrity(FSAPageHeader* p_page) const
	{
		#ifdef VMEM_FSA_HEADER_MARKER
			VMEM_MEM_CHECK(&p_page->m_Marker, VMEM_FSA_PAGE_HEADER_MARKER);
		#endif

		VMEM_ASSERT_CODE(VMEM_MEM_CHECK(&p_page->mp_FSA->m_Marker, VMEM_FSA_MARKER));

		#ifdef VMEM_ASSERTS
			int page_size = mp_PageHeap->GetPageSize();
			int used_size = m_AlignedHeaderSize + m_SlotsPerPage * m_SlotSize;
			int unused_size = page_size - used_size;
			if (unused_size)
				CheckMemory((byte*)p_page + used_size, unused_size, VMEM_GUARD_MEM);
		#endif

		// check p_slot pointers are valid and the free slot count matches
		int free_slot_count = 0;
		void* p_slot = p_page->mp_FreeSlots;
		
		VMEM_ASSERT_CODE(void* p_slot_addr = &p_page->mp_FreeSlots);

		// if the assert fails re-get the slot because the assert callback in the test framework will have repaired it
		VMEM_ASSERT_MEM(!p_slot || (p_slot >= p_page+1 && p_slot < (byte*)p_page + mp_PageHeap->GetPageSize()), p_slot_addr);
		p_slot = p_page->mp_FreeSlots;

		while(p_slot && free_slot_count < m_SlotsPerPage)
		{
			++free_slot_count;
			VMEM_ASSERT_CODE(p_slot_addr = p_slot);
			void* p_old_slot = p_slot;
			p_slot = *(void**)p_old_slot;

			// if the assert fails re-get the slot because the assert callback in the test framework will have repaired it
			VMEM_ASSERT_MEM(!p_slot || (p_slot >= p_page+1 && p_slot < (byte*)p_page + mp_PageHeap->GetPageSize()), p_slot_addr);
			p_slot = *(void**)p_old_slot;
		}
		VMEM_ASSERT(free_slot_count == m_SlotsPerPage-p_page->m_UsedSlotCount, "FSA free slot list or m_UsedSlotCount corrupted");

		// check the memory contents
#ifdef VMEM_ENABLE_MEMSET
		if(m_Size > (int)sizeof(void*))
		{
			int check_size = m_SlotSize - sizeof(void*);
			void* p_slot_iter = p_page->mp_FreeSlots;
			while(p_slot_iter)
			{
				CheckMemory((void**)p_slot_iter+1, check_size, VMEM_FREED_MEM);
				p_slot_iter = *(void**)p_slot_iter;
			}
		}
#endif

		// check markers for allocated slots
#ifdef VMEM_CHECK_ALLOCATED_MARKERS
		if(m_SlotSize > 2 * sizeof(void*))
		{
			void* p_start_slot = AlignUp((byte*)p_page + sizeof(FSAPageHeader), m_Alignment);
			void* p_end_slot = (byte*)p_start_slot + m_SlotSize * m_SlotsPerPage;
			for (void* p_slot_iter = p_start_slot; p_slot_iter != p_end_slot; p_slot_iter = (byte*)p_slot_iter + m_SlotSize)
			{
				unsigned int* p_mem = (unsigned int*)((byte*)p_slot_iter + sizeof(void*));
				unsigned int value = *p_mem;
				bool is_free_slot = value == VMEM_FREED_MEM || value == VMEM_TRAIL_GUARD_MEM;		// will miss allocated memory that is set to free mem but that's acceptable
				if (!is_free_slot)
					CheckGuards(p_slot_iter, m_Size, m_SlotSize);
			}
		}
#endif
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	size_t FSA::WriteAllocs(int* p_allocs_written_count, int max_allocs_to_write, bool write_header) const
	{
		CriticalSectionScope lock(m_CriticalSection);

		if(write_header)
		{
			DebugWrite(_T("-------------------------\n"));
			DebugWrite(_T("FSA %d\n"), m_Size);
		}

		bool empty = true;

		int allocs_written = p_allocs_written_count ? *p_allocs_written_count : 0;

		int alloc_count = 0;
		size_t bytes_allocated = 0;

		// write all allocs in the full pages
		for(FSAPageHeader* p_page=m_FullPageList.GetHead(); p_page!=m_FullPageList.GetIterEnd(); p_page=p_page->mp_Next)
		{
			const byte* p_slot = (byte*)AlignUp((byte*)p_page + sizeof(FSAPageHeader), m_Alignment);
			const byte* p_end_slot = p_slot + m_SlotsPerPage*m_SlotSize;
			for(; p_slot!=p_end_slot; p_slot+=m_SlotSize)
			{
				if(allocs_written < max_allocs_to_write)
				{
					WriteAlloc(p_slot, m_Size);
					++allocs_written;
					empty = false;
				}
				++alloc_count;
				bytes_allocated += m_Size;
			}
		}

		// write any allocs in the partially free pages
		const FSAPageHeader* p_end = m_FreePageList.GetIterEnd();
		for(const FSAPageHeader* p_page = m_FreePageList.GetHead(); p_page!=p_end; p_page=p_page->mp_Next)
		{
			const byte* p_slot = (byte*)AlignUp((byte*)p_page + sizeof(FSAPageHeader), m_Alignment);
			const byte* p_end_slot = p_slot + m_SlotsPerPage*m_SlotSize;
			for(; p_slot!=p_end_slot; p_slot+=m_SlotSize)
			{
				// find out if this slot if on the free list
				bool allocated = true;
				for(void* p_iter=p_page->mp_FreeSlots; p_iter!=NULL; p_iter=*(void**)p_iter)
				{
					if(p_iter == p_slot)
					{
						allocated = false;
						break;
					}
				}

				if(allocated)
				{
					if(allocs_written < max_allocs_to_write)
					{
						WriteAlloc(p_slot, m_Size);
						++allocs_written;
					}
					++alloc_count;
					bytes_allocated += m_Size;
				}
			}
		}

		if(!empty)
		{
			if(!p_allocs_written_count)
			{
				if(allocs_written == max_allocs_to_write)
					DebugWrite(_T("Only showing first %d allocs\n"), max_allocs_to_write);

				DebugWrite(_T("%d bytes allocated across %d allocations\n"), bytes_allocated, alloc_count);
			}
		}

		if(p_allocs_written_count)
			*p_allocs_written_count = allocs_written;

		return bytes_allocated;
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	void FSA::SendStatsToMemPro(MemProSendFn send_fn, void* p_context)
	{
		CriticalSectionScope lock(m_CriticalSection);

		MemProStats::FSAStats stats;
		stats.m_Size = m_Size;
		stats.m_SlotSize = m_SlotSize;
		stats.m_Alignment = m_Alignment;
		stats.m_Stats = m_Stats;

		stats.m_FreePageCount = 0;
		FSAPageHeader* p_end = m_FreePageList.GetIterEnd();
		for(FSAPageHeader* p_page=m_FreePageList.GetHead(); p_page!=p_end; p_page=p_page->mp_Next)
			++stats.m_FreePageCount;

		stats.m_FullPageCount = 0;
		for(FSAPageHeader* p_page=m_FullPageList.GetHead(); p_page!=m_FullPageList.GetIterEnd(); p_page=p_page->mp_Next)
			++stats.m_FullPageCount;

		SendToMemPro(stats, send_fn, p_context);
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_FSA_CACHE
	void FSA::AddAllocToStats()
	{
		VMEM_STATS(m_Stats.m_Unused -= m_SlotSize);
		VMEM_STATS(m_Stats.m_Used += m_Size);
		VMEM_STATS(m_Stats.m_Overhead += m_SlotSize - m_Size);
		#ifdef VMEM_CUSTOM_ALLOC_INFO
			VMEM_STATS(m_Stats.m_Used -= sizeof(VMemCustomAllocInfo));
			VMEM_STATS(m_Stats.m_Overhead += sizeof(VMemCustomAllocInfo));
		#endif
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_FSA_CACHE
	void FSA::RemoveAllocFromStats()
	{
		VMEM_STATS(m_Stats.m_Unused += m_SlotSize);
		VMEM_STATS(m_Stats.m_Used -= m_Size);
		VMEM_STATS(m_Stats.m_Overhead -= m_SlotSize - m_Size);
		#ifdef VMEM_CUSTOM_ALLOC_INFO
			VMEM_STATS(m_Stats.m_Used += sizeof(VMemCustomAllocInfo));
			VMEM_STATS(m_Stats.m_Overhead -= sizeof(VMemCustomAllocInfo));
		#endif
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_FSA_PAGE_CACHE
	void FSA::AddPageToEmptyList(FSAPageHeader* p_page)
	{
		// find where to insert into the list (sorted by page address)
		FSAPageHeader* p_iter = m_EmptyPageList.GetHead();
		FSAPageHeader* p_iter_end = m_EmptyPageList.GetIterEnd();
		while(p_iter != p_iter_end && p_iter < p_page)
			p_iter = p_iter->mp_Next;

		m_EmptyPageList.Insert(p_iter->mp_Prev, p_page);
		int empty_page_list_count = ++m_EmptyPageListCount;

		// remove a page if we have reached the limit
		VMEM_ASSERT(empty_page_list_count == m_EmptyPageList.GetCount(), "Empty page list has been corrupted");
		if(empty_page_list_count == VMEM_FSA_PAGE_CACHE_COUNT_MAX)
		{
			FSAPageHeader* p_last = m_EmptyPageList.RemoveTail();
			FreePage(p_last);
			--m_EmptyPageListCount;
		}
		else if(!m_InUpdateList && empty_page_list_count > VMEM_FSA_PAGE_CACHE_COUNT_MIN)
		{
			m_UpdateList.Add(this);
			m_InUpdateList = true;
		}
	}
#endif
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

