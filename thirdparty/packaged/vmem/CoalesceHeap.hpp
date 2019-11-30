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
#ifndef VMEM_COALESCEHEAP_H_INCLUDED
#define VMEM_COALESCEHEAP_H_INCLUDED

//------------------------------------------------------------------------
#include "VMemCore.hpp"
#include "VMemStats.hpp"
#include "BasicFSA.hpp"
#include "TrailGuard.hpp"
#include "HeapRegions.hpp"
#include "VMemCriticalSection.hpp"
#include "CoalesceHeapCache.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	class InternalHeap;

	//------------------------------------------------------------------------
	namespace Coalesce
	{
		//------------------------------------------------------------------------
		struct Region;
		struct Header;
		struct Node;

		//------------------------------------------------------------------------
		// each free block points to a node. Allocated blocks do not have nodes.
		// Instead of embedding this data in the memory block we store it as a
		// seperate object to allow for fast cache friendly iteration when sorting
		// and looking up nodes.
		struct Node
		{
			void* mp_Mem;
			Node* mp_Prev;
			Node* mp_Next;
			int m_Size;
			VMEM_X64_ONLY(unsigned int m_Padding);
		};

#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE
		//------------------------------------------------------------------------
		struct NodeBucket
		{
			NodeBucket()
			:	mp_PrevNeedsSortBucket(NULL),
				mp_NextNeedsSortBucket(NULL)
#ifdef VMEM_SORT_COALESCE_HEAP_ON_SERVICE_THREAD
				,m_NeedsSort(false)
				,m_SortByAddressOnly(false)
#endif
			{
				m_Head.mp_Mem = NULL;
				m_Head.mp_Prev = &m_Head;
				m_Head.mp_Next = &m_Head;
				m_Head.m_Size = 0;
			}

			Node m_Head;
			NodeBucket* mp_PrevNeedsSortBucket;
			NodeBucket* mp_NextNeedsSortBucket;
#ifdef VMEM_SORT_COALESCE_HEAP_ON_SERVICE_THREAD
			bool m_NeedsSort;
			bool m_SortByAddressOnly;
#endif
		};
#endif
	}

	//------------------------------------------------------------------------
	// This coalesce heap used best fit and immediate coalesceing. It also decommits
	// free pages in and free blocks, and performs biasing to reduce fragmentation.
	// Unlike many coalesce heaps the free nodes are not embedded in the memory blocks
	// but are seperate allocations to increase cache coherency when looking up and sorting.
	// This allocator aligns to VMEM_NATURAL_ALIGNMENT bytes.
	class CoalesceHeap
	{
	public:
		CoalesceHeap(
			int region_size,
			int min_size,
			int max_size,
			InternalHeap& internal_heap,
			HeapRegions& heap_regions,
			RegionType::Enum region_type,
			int reserve_flags,
			int commit_flags,
			VirtualMem& virtual_mem,
			bool cache_enabled=false,
			bool add_initial_region_to_heap_regions=true);

		~CoalesceHeap();

		bool Initialise();

		void* Alloc(size_t size);

		void Free(void* p);

		static int GetSize(void* p);

		void CheckIntegrity() const;

		void DebugWrite() const;

#ifdef VMEM_ENABLE_STATS
		inline Stats GetStats() const;
		inline Stats GetStatsNoLock() const;
		size_t WriteAllocs() const;
		void SendStatsToMemPro(MemProSendFn send_fn, void* p_context);
#endif

#ifdef VMEM_TRAIL_GUARDS
		inline void InitialiseTrailGuard(int size, int check_freq);
#endif

		void Lock() const { m_CriticalSection.Enter(); }

		void Release() const { m_CriticalSection.Leave(); }

		void Flush();

		void Update();

		void* GetFirstRegion() { return mp_RegionList; }

	private:
		bool FreeInternal(void* p);

		Coalesce::Region* CreateRegion(bool add_to_heap_regions=true);

		void ClearRegion(Coalesce::Region* p_region);

		void DestroyRegion(Coalesce::Region* p_region);

		VMEM_FORCE_INLINE Coalesce::Node* AllocNode();

		VMEM_FORCE_INLINE void FreeNode(Coalesce::Node* p_node);

		Coalesce::Node* GetNode(int size, Coalesce::Region*& p_region);

		VMEM_FORCE_INLINE void InsertNode(Coalesce::Node* p_node, Coalesce::Region* p_region);

		VMEM_FORCE_INLINE bool UnlinkNode(Coalesce::Node* p_node);

		VMEM_FORCE_INLINE void RemoveNode(Coalesce::Node* p_node, Coalesce::Region* p_region);

		inline int GetFreeListIndex(int size) const;

		Coalesce::Region* GetRegion(void* p) const;

		inline bool CommitRange(void* p, int size);

		inline void DecommitRange(void* p, int size);

		inline Coalesce::Header* FindNextAllocatedHeader(Coalesce::Region* p_region, Coalesce::Header* p_header) const;

		void CheckIntegrity(Coalesce::Header* p_header) const;

		void CheckIntegrity_NoLock() const;

#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE
		VMEM_FORCE_INLINE int SizeToBucketValue(int size) const;
		void RemoveNode(Coalesce::Node* p_node, Coalesce::Region* p_region, int bucket_index);
#endif

#ifdef VMEM_COALESCE_GUARD_PAGES
		bool SetupGuardPages(Coalesce::Region* p_region);
		void RemoveGuardPages(Coalesce::Region* p_region);
#endif

#ifdef VMEM_COALESCE_HEAP_CACHE
		void FlushCache(size_t max_mem);
#endif

#ifdef VMEM_SORT_COALESCE_HEAP_ON_SERVICE_THREAD
		void SortBuckets();
#endif
		CoalesceHeap(const CoalesceHeap&);
		CoalesceHeap& operator=(const CoalesceHeap&);

		//------------------------------------------------------------------------
		// data
	private:
#ifdef VMEM_COALESCE_HEAP_CACHE
		bool m_CacheEnabled;
		CoalesceHeapCache m_Cache;
#endif
		int m_RegionSize;

		VirtualMem& m_VirtualMem;

		// the minimum and maximum size that this coalesce heap can allocate. This is
		// used to skew the freelist map indices, deciding what to do with offcuts and
		// asserting the allocation sizes are in range.
		const int m_MinSize;
		const int m_MaxSize;
		const int m_SizeRange;

		Coalesce::Region* mp_RegionList;		// linked list of regions

		BasicFSA<Coalesce::Node> m_NodeFSA;	// the allocator to use to allocate nodes

		// free can't fail, so every alloc we allocate a "spare" node and add it to this list. If free needs a new node it can always be guaranteed to find one here
		Coalesce::Node* mp_SpareNodeFreeList;

		InternalHeap& m_InternalHeap;

		HeapRegions& m_HeapRegions;
		RegionType::Enum m_RegionType;

#if defined(VMEM_SORT_COALESCE_HEAP_ON_SERVICE_THREAD) && !defined(VMEM_COALESCE_HEAP_BUCKET_TREE)
		Coalesce::NodeBucket m_BucketsToSortList;
#endif
		bool m_AddInitialRegionToHeapRegions;

#ifdef VMEM_ENABLE_STATS
		Stats m_Stats;
#endif

#ifdef VMEM_TRAIL_GUARDS
		TrailGuard m_TrailGuard;
#endif
		mutable CriticalSection m_CriticalSection;

		int m_ReserveFlags;
		int m_CommitFlags;
	};

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	Stats CoalesceHeap::GetStats() const
	{
		CriticalSectionScope lock(m_CriticalSection);
		return GetStatsNoLock();
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	Stats CoalesceHeap::GetStatsNoLock() const
	{
		Stats stats = m_Stats + m_NodeFSA.GetStats();
		#ifdef VMEM_COALESCE_HEAP_CACHE
			if(m_CacheEnabled)
				stats.m_Overhead += m_Cache.GetMemoryUsage();
		#endif
		return stats;
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_TRAIL_GUARDS
	void CoalesceHeap::InitialiseTrailGuard(int size, int check_freq)
	{
		CriticalSectionScope lock(m_CriticalSection);

		m_TrailGuard.Initialise(size, check_freq);
	}
#endif
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_COALESCEHEAP_H_INCLUDED

