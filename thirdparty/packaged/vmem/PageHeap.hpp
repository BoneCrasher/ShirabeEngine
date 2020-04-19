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
#ifndef VMEM_PAGEHEAP_H_INCLUDED
#define VMEM_PAGEHEAP_H_INCLUDED

//------------------------------------------------------------------------
#include "VMemCore.hpp"
#include "VMemStats.hpp"
#include "VMemCriticalSection.hpp"
#include "HeapRegions.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	struct PageHeapRegion;
	class InternalHeap;

	//------------------------------------------------------------------------
	// PageHeap is used to allocate pages of a specific size. The page size must
	// be an exact multiple of the system page size, or vice versa.
	//
	// Note that the regions array was added to allow the Owns function to be lock free
	// because this wsas a point of contention for Realloc. The reason the region list
	// is still used is because it is ordered with the oldest region first. This biases
	// the allocs to the oldest regions allowing the new regions to be cleaned up. The
	// regions array is not ordered, it simply uses the first empty element. The regions
	// array could not be sorted and still be lockfree, so both the array and list are needed.
	//
	class PageHeap
	{
	public:
		PageHeap(
			int page_size,
			int region_size,
			InternalHeap& internal_heap,
			HeapRegions& heap_regions,
			RegionType::Enum region_type,
			int reserve_flags,
			int commit_flags,
			VirtualMem& virtual_mem);

		~PageHeap();

		bool Initialise();

		void* Alloc();

		void Free(void* p_page);

		VMEM_FORCE_INLINE bool Empty() const;

		VMEM_FORCE_INLINE int GetPageSize() const;

		void* GetFirstRegion();

#ifdef VMEM_ENABLE_STATS
		inline const Stats& GetStats() const;
		inline const Stats& GetStatsNoLock() const;
		void SendStatsToMemPro(MemProSendFn send_fn, void* p_context);
#endif

	private:
		PageHeapRegion* CreateRegion();

		void UnlinkRegion(PageHeapRegion* p_region);

		void DestroyRegion(PageHeapRegion* p_region);

		PageHeapRegion* GetRegion(void* p) const;

		inline bool SysPageClear(int index, PageHeapRegion* p_region) const;

		PageHeap& operator=(const PageHeap& other);

		//------------------------------------------------------------------------
		// data
	private:
		mutable CriticalSection m_CriticalSection;

		int m_PageSize;
		int m_SysPageSize;
		int m_RegionSize;
		int m_PagesPerRegion;
		int m_PagesPerSysPage;

		int m_ReserveFlags;
		int m_CommitFlags;

		PageHeapRegion* mp_RegionList;

		HeapRegions& m_HeapRegions;
		RegionType::Enum m_RegionType;

		InternalHeap& m_InternalHeap;

		VirtualMem& m_VirtualMem;

#ifdef VMEM_ENABLE_STATS
		Stats m_Stats;
#endif
	};

	//------------------------------------------------------------------------
	int PageHeap::GetPageSize() const
	{
		return m_PageSize;
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	const Stats& PageHeap::GetStats() const
	{
		CriticalSectionScope lock(m_CriticalSection);
		return m_Stats;
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	const Stats& PageHeap::GetStatsNoLock() const
	{
		return m_Stats;
	}
#endif
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_PAGEHEAP_H_INCLUDED

