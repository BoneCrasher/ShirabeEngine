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
#include "ProtectedHeap.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#ifdef VMEM_PROTECTED_HEAP

//------------------------------------------------------------------------
#include "VMemSys.hpp"

//------------------------------------------------------------------------
namespace VMem
{
	const int g_ProtectedAllocAlignment = VMEM_NATURAL_ALIGNMENT;

	extern size_t g_ReservedBytes;

	typedef bool (*VMemSHouldProtectFn)(int i, size_t size);
	extern VMemSHouldProtectFn g_VMemSHouldProtectFn;

	inline bool ShouldProtect(int i, size_t size)
	{
		if(g_VMemSHouldProtectFn)
			return g_VMemSHouldProtectFn(i, size);
		else
			return VMemShouldProtect(i, size);
	}

	//------------------------------------------------------------------------
	inline void CheckMemoryBytes(void* p, size_t size, unsigned int value)
	{
#ifdef VMEM_ASSERTS
		byte* p_check = (byte*)p;
		for(size_t i=0; i<size; ++i, ++p_check)
			VMEM_MEM_CHECK(p_check, (byte)value);
#else
		VMEM_UNREFERENCED_PARAM(p);
		VMEM_UNREFERENCED_PARAM(size);
		VMEM_UNREFERENCED_PARAM(value);
#endif
	}

	//------------------------------------------------------------------------
	ProtectedHeap::ProtectedHeap(int page_size, int reserve_flags, int commit_flags)
	:	m_Allocs(page_size),
		m_PageSize(page_size),
		m_ReserveFlags(reserve_flags),
		m_CommitFlags(commit_flags),
		m_AllocIndex(rand()),
		m_Overhead(0)
	{
		m_UnreleasedAllocsPageList.mp_Prev = &m_UnreleasedAllocsPageList;
		m_UnreleasedAllocsPageList.mp_Next = &m_UnreleasedAllocsPageList;
	}

	//------------------------------------------------------------------------
	ProtectedHeap::~ProtectedHeap()
	{
		// free any unfreed allocs
		HashMap<AddrKey, size_t>::Iterator iter=m_Allocs.GetIterator();
		while(iter.MoveNext())
			Free(iter.GetKey().GetAddr());

		while(ReleaseOldestProtectedAlloc())
			;
	}

	//------------------------------------------------------------------------
	void* ProtectedHeap::Alloc(size_t size)
	{
		CriticalSectionScope lock(m_CriticalSection);

		if(!ShouldProtect(m_AllocIndex++, size))
			return NULL;

		return AllocInternal(size);
	}

	//------------------------------------------------------------------------
	void* ProtectedHeap::AllocInternal(size_t size)
	{
		if(size == 0)
			size = 4;

		size_t aligned_size = AlignSizeUpPow2(size, g_ProtectedAllocAlignment);

		size_t page_aligned_size = AlignSizeUpPow2(aligned_size, m_PageSize);
		if(m_Overhead + page_aligned_size - size >= VMEM_PROTECTED_HEAP_MAX_OVERHEAD)
			return NULL;

		void* p_page = VMem::VirtualReserve(page_aligned_size, m_PageSize, m_ReserveFlags);
		while (!p_page && ReleaseOldestProtectedAlloc())
			p_page = VMem::VirtualReserve(page_aligned_size, m_PageSize, m_ReserveFlags);
		if(!p_page)
			return NULL;

		if(!VMem::VirtualCommit(p_page, page_aligned_size, m_PageSize, m_CommitFlags))
		{
			VMem::VirtualRelease(p_page, page_aligned_size);
			return NULL;
		}

		size_t alloc_offset = page_aligned_size - aligned_size;
		void* p_alloc = (byte*)p_page + alloc_offset;
		if(alloc_offset)
			memset(p_page, VMEM_PROTECTED_PAGE, alloc_offset);

		size_t end_guard_size = aligned_size - size;
		if(end_guard_size)
			memset((byte*)p_page + page_aligned_size - end_guard_size, VMEM_PROTECTED_PAGE, end_guard_size);

		if(!m_Allocs.Add(p_alloc, size))
		{
			VMem::VirtualDecommit(p_page, page_aligned_size, m_PageSize, m_CommitFlags);
			VMem::VirtualRelease(p_page, page_aligned_size);
			return NULL;
		}

		size_t overhead = page_aligned_size - size;
		m_Overhead += overhead;

		VMEM_STATS(m_Stats.m_Reserved += page_aligned_size);
		VMEM_STATS(m_Stats.m_Used += size);
		VMEM_STATS(m_Stats.m_Overhead += overhead);
		#ifdef VMEM_CUSTOM_ALLOC_INFO
			VMEM_STATS(m_Stats.m_Used -= sizeof(VMemCustomAllocInfo));
			VMEM_STATS(m_Stats.m_Overhead += sizeof(VMemCustomAllocInfo));
		#endif

		return p_alloc;
	}

	//------------------------------------------------------------------------
	bool ProtectedHeap::Free(void* p)
	{
		CriticalSectionScope lock(m_CriticalSection);

		void* p_alloc = p;

		size_t size = 0;
		AddrKey key(p_alloc);
		if(!m_Allocs.TryGetValue(key, size))
			return false;

		m_Allocs.Remove(key);

		size_t aligned_size = AlignSizeUpPow2(size, g_ProtectedAllocAlignment);
		size_t page_aligned_size = AlignSizeUpPow2(aligned_size, m_PageSize);

		size_t alloc_offset = page_aligned_size - aligned_size;

		void* p_page = (byte*)p_alloc - alloc_offset;

		if(alloc_offset)
			CheckMemory(p_page, alloc_offset, VMEM_PROTECTED_PAGE);

		size_t end_guard_size = aligned_size - size;
		if(end_guard_size)
			CheckMemoryBytes((byte*)p_page + page_aligned_size - end_guard_size, end_guard_size, VMEM_PROTECTED_PAGE);

		// we only decommit, do not release. This protects the page and
		// stops the virtual address being used again
		VMem::VirtualDecommit(p_page, page_aligned_size, m_PageSize, m_CommitFlags);

		size_t overhead = page_aligned_size - size;
		m_Overhead -= overhead;

		#ifdef VMEM_CUSTOM_ALLOC_INFO
			VMEM_STATS(m_Stats.m_Used += sizeof(VMemCustomAllocInfo));
			VMEM_STATS(m_Stats.m_Overhead -= sizeof(VMemCustomAllocInfo));
		#endif
		VMEM_STATS(m_Stats.m_Used -= size);
		VMEM_STATS(m_Stats.m_Overhead -= overhead);

		if(!AddUnreleasedPage(p_page, page_aligned_size))
		{
			// in an out of memory situation release the memory immediatley (and return true because we freed it)
			VirtualRelease(p_page, page_aligned_size);
			VMEM_STATS(m_Stats.m_Reserved -= page_aligned_size);
		}

		return true;
	}

	//------------------------------------------------------------------------
	size_t ProtectedHeap::GetSize(void* p) const
	{
		CriticalSectionScope lock(m_CriticalSection);

		size_t size = 0;
		if(m_Allocs.TryGetValue(p, size))
			return size;

		return VMEM_INVALID_SIZE;
	}

	//------------------------------------------------------------------------
	bool ProtectedHeap::AddUnreleasedPage(void* p_page, size_t size)
	{
		UnreleasedAllocsPage* p_unreleased_allocs_page = m_UnreleasedAllocsPageList.mp_Prev;

		int unreleased_allocs_per_page = ((int)((m_PageSize - sizeof(UnreleasedAllocsPage)) / sizeof(UnreleasedAlloc)));
		if(p_unreleased_allocs_page == &m_UnreleasedAllocsPageList || p_unreleased_allocs_page->m_UsedCount == unreleased_allocs_per_page)
		{
			void* p_new_page = VMem::VirtualReserve(m_PageSize, m_PageSize, m_ReserveFlags);
			if(!p_new_page)
				return false;

			if(!VMem::VirtualCommit(p_new_page, m_PageSize, m_PageSize, m_CommitFlags))
			{
				VMem::VirtualRelease(p_new_page, m_PageSize);
				return false;
			}

			memset(p_new_page, 0, m_PageSize);

			p_unreleased_allocs_page = (UnreleasedAllocsPage*)p_new_page;

			p_unreleased_allocs_page->m_UsedCount = 0;
			p_unreleased_allocs_page->m_ReleasedCount = 0;

			// add to the end of the list
			p_unreleased_allocs_page->mp_Next = &m_UnreleasedAllocsPageList;
			p_unreleased_allocs_page->mp_Prev = m_UnreleasedAllocsPageList.mp_Prev;
			p_unreleased_allocs_page->mp_Prev->mp_Next = p_unreleased_allocs_page;
			m_UnreleasedAllocsPageList.mp_Prev = p_unreleased_allocs_page;

			m_Overhead += m_PageSize;

			VMEM_STATS(m_Stats.m_Reserved += m_PageSize);
			VMEM_STATS(m_Stats.m_Overhead += m_PageSize);
		}

		// add to the last block page
		UnreleasedAlloc* p_unreleased_blocks = (UnreleasedAlloc*)(p_unreleased_allocs_page + 1);
		UnreleasedAlloc* p_unreleased_block = p_unreleased_blocks + p_unreleased_allocs_page->m_UsedCount;

		p_unreleased_block->mp_Page = p_page;
		p_unreleased_block->m_Size = size;

		++p_unreleased_allocs_page->m_UsedCount;

		return true;
	}

	//------------------------------------------------------------------------
	bool ProtectedHeap::ReleaseOldestProtectedAlloc()
	{
		// check if there are any block pages
		UnreleasedAllocsPage* p_unreleased_allocs_page = m_UnreleasedAllocsPageList.mp_Next;
		if(p_unreleased_allocs_page == &m_UnreleasedAllocsPageList)
			return false;

		// get the next block to release from the first block page
		UnreleasedAlloc* p_unreleased_blocks = (UnreleasedAlloc*)(p_unreleased_allocs_page + 1);
		UnreleasedAlloc unreleased_block = p_unreleased_blocks[p_unreleased_allocs_page->m_ReleasedCount];

		// release the block
		VMEM_ASSERT(unreleased_block.mp_Page, "unreleased_block has been corrupted");
		VirtualRelease(unreleased_block.mp_Page, unreleased_block.m_Size);
		VMEM_STATS(m_Stats.m_Reserved -= unreleased_block.m_Size);

		++p_unreleased_allocs_page->m_ReleasedCount;
		VMEM_ASSERT(p_unreleased_allocs_page->m_ReleasedCount <= p_unreleased_allocs_page->m_UsedCount, "p_unreleased_allocs_page->m_UsedCount has been corrupted");

		// if all blocks in the current page have been totally released release the page
		if(p_unreleased_allocs_page->m_ReleasedCount == p_unreleased_allocs_page->m_UsedCount)
		{
			// unlink the page
			p_unreleased_allocs_page->mp_Prev->mp_Next = p_unreleased_allocs_page->mp_Next;
			p_unreleased_allocs_page->mp_Next->mp_Prev = p_unreleased_allocs_page->mp_Prev;

			// free the page
			VMem::VirtualDecommit(p_unreleased_allocs_page, m_PageSize, m_PageSize, m_CommitFlags);
			VMem::VirtualRelease(p_unreleased_allocs_page, m_PageSize);

			m_Overhead -= m_PageSize;

			VMEM_STATS(m_Stats.m_Reserved -= m_PageSize);
			VMEM_STATS(m_Stats.m_Overhead -= m_PageSize);
		}

		return true;
	}

	//------------------------------------------------------------------------
	void ProtectedHeap::CheckIntegrity()
	{
		CriticalSectionScope lock(m_CriticalSection);

		HashMap<AddrKey, size_t>::Iterator iter = m_Allocs.GetIterator();
		while(iter.MoveNext())
		{
			void* p_alloc = (void*)iter.GetKey().GetAddr();
			size_t size = iter.GetValue();

			size_t aligned_size = AlignSizeUpPow2(size, g_ProtectedAllocAlignment);
			size_t page_aligned_size = AlignSizeUpPow2(aligned_size, m_PageSize);

			size_t alloc_offset = page_aligned_size - aligned_size;

			void* p_page = (byte*)p_alloc - alloc_offset;

			if(alloc_offset)
				CheckMemory(p_page, alloc_offset, VMEM_PROTECTED_PAGE);

			size_t end_guard_size = aligned_size - size;
			if(end_guard_size)
				CheckMemoryBytes((byte*)p_page + page_aligned_size - end_guard_size, end_guard_size, VMEM_PROTECTED_PAGE);
		}
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	Stats ProtectedHeap::GetStats() const
	{
		CriticalSectionScope lock(m_CriticalSection);
		return GetStatsNoLock();
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	Stats ProtectedHeap::GetStatsNoLock() const
	{
		int map_size = m_Allocs.GetAllocedMemory();

		Stats internal_stats;
		internal_stats.m_Reserved += map_size;
		internal_stats.m_Overhead += map_size;

		return m_Stats + internal_stats;
	}
#endif
}

//------------------------------------------------------------------------
#endif			// #ifdef VMEM_PROTECTED_HEAP

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

