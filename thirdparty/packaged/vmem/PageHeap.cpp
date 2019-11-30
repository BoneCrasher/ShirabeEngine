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
#include "PageHeap.hpp"
#include "VirtualMem.hpp"
#include "Bitfield.hpp"
#include "InternalHeap.hpp"
#include "VMemMemProStats.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	struct PageHeapRegion
	{
		PageHeapRegion(int page_count, InternalHeap& internal_heap)
		:	mp_Mem(NULL),
			m_Bitfield(page_count, internal_heap),
			mp_Next(NULL)
#ifdef VMEM_MEMORY_CORRUPTION_FINDER
			,m_CommittedPagesBitfield(page_count, internal_heap)
#endif
		{}

		bool Initialise()
		{
			if(!m_Bitfield.Initialise())
				return false;

#ifdef VMEM_MEMORY_CORRUPTION_FINDER
			if(!m_CommittedPagesBitfield.Initialise())
				return false;
#endif
			return true;
		}

		void* mp_Mem;
		Bitfield m_Bitfield;
		PageHeapRegion* mp_Next;
#ifdef VMEM_MEMORY_CORRUPTION_FINDER
		Bitfield m_CommittedPagesBitfield;
#endif
	};

	//------------------------------------------------------------------------
	PageHeap::PageHeap(
		int page_size,
		int region_size,
		InternalHeap& internal_heap,
		HeapRegions& heap_regions,
		RegionType::Enum region_type,
		int reserve_flags,
		int commit_flags,
		VirtualMem& virtual_mem)
	:	
		m_PageSize(page_size),
		m_SysPageSize(VMem::VMax(page_size, virtual_mem.GetPageSize())),
		m_RegionSize(AlignUpPow2(region_size, m_PageSize)),
		m_PagesPerRegion(region_size / m_PageSize),
		m_PagesPerSysPage(m_SysPageSize / page_size),
		m_ReserveFlags(reserve_flags),
		m_CommitFlags(commit_flags),
		mp_RegionList(NULL),
		m_HeapRegions(heap_regions),
		m_RegionType(region_type),
		m_InternalHeap(internal_heap),
		m_VirtualMem(virtual_mem)
	{
		VMEM_ASSERT((m_RegionSize % virtual_mem.GetPageSize()) == 0, "regionsize must be a multiple of the system page size");
		VMEM_ASSERT((m_SysPageSize % page_size) == 0, "page size must be a multple of the system page size or vice versa");
		VMEM_ASSERT((m_SysPageSize % virtual_mem.GetPageSize()) == 0, "page size must be a multple of the system page size or vice versa");
	}

	//------------------------------------------------------------------------
	bool PageHeap::Initialise()
	{
		return CreateRegion() != NULL;
	}

	//------------------------------------------------------------------------
	PageHeap::~PageHeap()
	{
		while(mp_RegionList)
		{
			PageHeapRegion* p_region = mp_RegionList;
			UnlinkRegion(p_region);
			DestroyRegion(p_region);
		}
	}

	//------------------------------------------------------------------------
	void* PageHeap::GetFirstRegion()
	{
		return mp_RegionList->mp_Mem;
	}

	//------------------------------------------------------------------------
	void* PageHeap::Alloc()
	{
		CriticalSectionScope lock(m_CriticalSection);

		// find the first region that has a free page
		PageHeapRegion* p_region = mp_RegionList;
		while(p_region && p_region->m_Bitfield.GetCount() == m_PagesPerRegion)
			p_region = p_region->mp_Next;

		if(!p_region)
			p_region = CreateRegion();

		if(!p_region)
			return NULL;

		int page_index = p_region->m_Bitfield.GetFirstClearBit();
		void* p_page = (byte*)p_region->mp_Mem + page_index * m_PageSize;

		// commit the sys page if it's clear (there can be multiple pages in a system page)
		if(SysPageClear(page_index, p_region))
		{
			if(!m_VirtualMem.Commit(AlignDownPow2(p_page, m_VirtualMem.GetPageSize()), m_SysPageSize, m_CommitFlags))
			{
				p_region->m_Bitfield.Set(page_index);	// must call Set after calling GetFirstClearBit
				p_region->m_Bitfield.Clear(page_index);
				return NULL;		// out of memory
			}

			VMEM_STATS(m_Stats.m_Unused += m_SysPageSize);

			for(int i=0; i<m_PagesPerSysPage; ++i)
				VMEM_MEMSET((byte*)p_page + i*m_PageSize, VMEM_UNUSED_PAGE, m_PageSize);
		}

#ifdef VMEM_ENABLE_MEMSET
		CheckMemory(p_page, m_PageSize, VMEM_UNUSED_PAGE);
#endif

		p_region->m_Bitfield.Set(page_index);
#ifdef VMEM_MEMORY_CORRUPTION_FINDER
		p_region->m_CommittedPagesBitfield.Set(page_index);
#endif

		VMEM_STATS(m_Stats.m_Unused -= m_PageSize);
		VMEM_STATS(m_Stats.m_Used += m_PageSize);

		return p_page;
	}

	//------------------------------------------------------------------------
	void PageHeap::Free(void* p_page)
	{
		CriticalSectionScope lock(m_CriticalSection);

		// get the region
		PageHeapRegion* p_region = GetRegion(p_page);
		VMEM_ASSERT(p_region, "can't find region for page");
		VMEM_ASSUME(p_region);

		// clear the bitfield bit
		int index = ToInt(((byte*)p_page - (byte*)p_region->mp_Mem) / m_PageSize);
		p_region->m_Bitfield.Clear(index);
#ifdef VMEM_MEMORY_CORRUPTION_FINDER
		p_region->m_CommittedPagesBitfield.Clear(index);
#endif

		VMEM_STATS(m_Stats.m_Unused += m_PageSize);
		VMEM_STATS(m_Stats.m_Used -= m_PageSize);

		// decommit the sys page if it's clear (there can be multiple pages in a system page)
		if(SysPageClear(index, p_region))
		{
			m_VirtualMem.Decommit(AlignDownPow2(p_page, m_VirtualMem.GetPageSize()), m_SysPageSize, m_CommitFlags);
			VMEM_STATS(m_Stats.m_Unused -= m_SysPageSize);
#ifdef VMEM_MEMORY_CORRUPTION_FINDER
			// don't re-use these pages if we are looking for corruptions
			int start_index = AlignDownPow2(index, m_PagesPerSysPage);
			int end_index = start_index + m_PagesPerSysPage;
			for(int i=start_index; i<end_index; ++i)
				p_region->m_Bitfield.Set(i);
#endif
		}
		else
		{
			VMEM_MEMSET(p_page, VMEM_UNUSED_PAGE, m_PageSize);
		}

		if(!p_region->m_Bitfield.GetCount() && p_region != mp_RegionList)		// do not destroy the first region
		{
			UnlinkRegion(p_region);
			DestroyRegion(p_region);
		}
	}

	//------------------------------------------------------------------------
	// create the region and add it to the regions list
	PageHeapRegion* PageHeap::CreateRegion()
	{
		PageHeapRegion* p_region = m_InternalHeap.NewRef2<PageHeapRegion>(m_PagesPerRegion, m_InternalHeap);
		if(!p_region)
			return NULL;

		if(!p_region->Initialise())
		{
			m_InternalHeap.Delete(p_region);
			return NULL;
		}

		p_region->mp_Mem = m_VirtualMem.Reserve(m_RegionSize, m_SysPageSize, m_ReserveFlags);
		if(!p_region->mp_Mem)
		{
			m_InternalHeap.Delete(p_region);
			return NULL;
		}

		p_region->mp_Next = NULL;

		VMEM_STATS(m_Stats.m_Reserved += m_RegionSize);
		VMEM_STATS(m_Stats.m_Overhead += sizeof(PageHeapRegion));
		VMEM_STATS(m_Stats.m_Overhead += p_region->m_Bitfield.GetSizeInBytes());
#ifdef VMEM_MEMORY_CORRUPTION_FINDER
		VMEM_STATS(m_Stats.m_Overhead += p_region->m_CommittedPagesBitfield.GetSizeInBytes());
#endif

#if defined(VMEM_ALTERNATE_PAGES)
		int sys_pages_per_region = m_RegionSize / m_SysPageSize;
		for(int i=1; i<sys_pages_per_region; i+=2)		// start from 1 so that we always have at least one valid page
		{
			for(int a=0; a<m_PagesPerSysPage; ++a)
				p_region->m_Bitfield.Set(i*m_PagesPerSysPage + a);
		}
#endif
		// add to heap regions array
		if(mp_RegionList)				// don't add the first region to the heap regions, it is handled differently to avoid locks
		{
		#ifdef VMEM_COALESCE_HEAP_PER_THREAD
			if(!m_HeapRegions.AddRegion(this, p_region->mp_Mem, m_RegionSize, m_RegionType))
		#else
			if(!m_HeapRegions.AddRegion(p_region->mp_Mem, m_RegionSize, m_RegionType))
		#endif
			{
				VMEM_STATS(m_Stats.m_Reserved -= m_RegionSize);
				VMEM_STATS(m_Stats.m_Overhead -= sizeof(PageHeapRegion));
				VMEM_STATS(m_Stats.m_Overhead -= p_region->m_Bitfield.GetSizeInBytes());
				m_VirtualMem.Release(p_region->mp_Mem);
				m_InternalHeap.Delete(p_region);
				return NULL;
			}
		}

		// add to end of region list
		PageHeapRegion* p_iter = mp_RegionList;
		if(p_iter)
		{
			while(p_iter->mp_Next)
				p_iter = p_iter->mp_Next;
			p_iter->mp_Next = p_region;
		}
		else
		{
			mp_RegionList = p_region;
		}

		return p_region;
	}

	//------------------------------------------------------------------------
	void PageHeap::UnlinkRegion(PageHeapRegion* p_region)
	{
		if(p_region != mp_RegionList)
			m_HeapRegions.RemoveRegion(p_region->mp_Mem);

		// unlink from region list
		if(mp_RegionList == p_region)
		{
			mp_RegionList = p_region->mp_Next;
		}
		else
		{
			VMEM_ASSERT(mp_RegionList, "unable to find region");
			VMEM_ASSUME(mp_RegionList);
			PageHeapRegion* p = mp_RegionList;
			while(p->mp_Next != p_region)
			{
				p = p->mp_Next;
				VMEM_ASSERT(p, "unable to find region");
				VMEM_ASSUME(p);
			}
			p->mp_Next = p_region->mp_Next;
		}
	}

	//------------------------------------------------------------------------
	void PageHeap::DestroyRegion(PageHeapRegion* p_region)
	{
#if defined(VMEM_ALTERNATE_PAGES)
		int sys_pages_per_region = m_RegionSize / m_SysPageSize;
		for(int i=1; i<sys_pages_per_region; i+=2)		// remember to start from 1
		{
			for(int a=0; a<m_PagesPerSysPage; ++a)
				p_region->m_Bitfield.Clear(i*m_PagesPerSysPage + a);
		}
#endif

		// decommit pages
		byte* p_page = (byte*)p_region->mp_Mem;
		for(int i=0; i<m_PagesPerRegion; i+=m_PagesPerSysPage, p_page+=m_SysPageSize)
		{
			if(!SysPageClear(i, p_region))
				m_VirtualMem.Decommit(p_page, m_SysPageSize, m_CommitFlags);
		}

		// release the memory
		m_VirtualMem.Release(p_region->mp_Mem);

		VMEM_STATS(m_Stats.m_Reserved -= m_RegionSize);
		VMEM_STATS(m_Stats.m_Overhead -= sizeof(PageHeapRegion));
		VMEM_STATS(m_Stats.m_Overhead -= p_region->m_Bitfield.GetSizeInBytes());

		m_InternalHeap.Delete(p_region);
	}

	//------------------------------------------------------------------------
	PageHeapRegion* PageHeap::GetRegion(void* p) const
	{
		for(PageHeapRegion* p_region=mp_RegionList; p_region!=NULL; p_region=p_region->mp_Next)
		{
			void* p_mem = p_region->mp_Mem;
			if(p >= p_mem && p < (byte*)p_mem + m_RegionSize)
				return p_region;
		}
		return NULL;
	}

	//------------------------------------------------------------------------
	// we can always assume that the index passed in is clear, which covers the
	// case of m_PagesPerSysPage being zero.
	bool PageHeap::SysPageClear(int index, PageHeapRegion* p_region) const
	{
#ifdef VMEM_MEMORY_CORRUPTION_FINDER
		const Bitfield& bitfield = p_region->m_CommittedPagesBitfield;
#else
		const Bitfield& bitfield = p_region->m_Bitfield;
#endif

		int start_index = AlignDownPow2(index, m_PagesPerSysPage);
		for(int i=0; i<m_PagesPerSysPage; ++i)
		{
			if(bitfield.Get(start_index + i))
				return false;
		}

		return true;
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	void PageHeap::SendStatsToMemPro(MemProSendFn send_fn, void* p_context)
	{
		CriticalSectionScope lock(m_CriticalSection);

		MemProStats::PageHeapStats stats;
		stats.m_PageSize = m_PageSize;
		stats.m_RegionSize = m_RegionSize;

		stats.m_RegionCount = 0;
		for(PageHeapRegion* p_region = mp_RegionList; p_region!=NULL; p_region=p_region->mp_Next)
			++stats.m_RegionCount;

		stats.m_Stats = m_Stats;

		SendEnumToMemPro(vmem_PageHeap, send_fn, p_context);
		SendToMemPro(stats, send_fn, p_context);
	}
#endif
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

