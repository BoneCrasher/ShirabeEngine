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
#include "CoalesceHeap.hpp"
#include "VirtualMem.hpp"
#include "VMemMemProStats.hpp"
#include "BucketTree.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#define VMEM_SKEW_SIZES

//------------------------------------------------------------------------
#ifdef VMEM_OS_WIN
	#pragma warning(push)
	#pragma warning(disable : 4100)
#endif

//------------------------------------------------------------------------
#ifdef VMEM_COALESCE_HEAP_MARKER
	#define VMEM_ASSERT_COALESCE_MARKER(p_value) VMEM_MEM_CHECK(p_value, VMEM_COALESCE_ALLOC_MARKER)
#else
	#define VMEM_ASSERT_COALESCE_MARKER(p_value)
#endif

//------------------------------------------------------------------------
//
// The free list map.
// -------------------
// Each free block of memory has a node. These nodes are stored in the free list map.
//
// This is a simple example of a free node map. It has a min and max size of
// 0 and 50, and there are 6 buckets (in reality there are 256 buckets and they
// are skewed).
// There is an array of 6 lists L, each contain nodes in a size range.
// Each list L contains a linked list of nodes n.
// The nodes in each list are sorted by size and then by address, in an ascending order.
//
// to find a node for a specific size, it first works out the first list L that
// could contain such a node. It then iterates down the list to find a node that is
// big enough. If no such node is found it moves onto the next non-empty list
// and uses the first node.
//
//   L0(0-10)  L1(10-20)  L2(20-30)  L3(30-40)  L4(40-50)  L5(>50)
//       |          |          |          |                   |
//       n2         n14        n20        n33                 n55
//       |                     |                              |
//       n6                    n27                            n200
//       |
//       n10
//

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	const int g_FreeNodeMapSize = 255;
	const int g_CoalesceHeapAlignment = VMEM_NATURAL_ALIGNMENT;

	//------------------------------------------------------------------------
	// The node pointer is aliased with this enum, so the enum values must not be
	// valid pointers. The order is important, fragment must come after allocated
	// so that we can do > allocated to test if the memory is free.
	enum ENodeType
	{
		nt_Fixed = 1,			// fixed memory block at start or end of memory that is never freed
		nt_Allocated = 2,		// allocated memory
		nt_Fragment = 3			// offcut that is too small to be re-used and so doesn't need a free node
								// > than nt_Fragment means that it is a pointer to a free node. We are
								// relying on the fact that 1,2,3 are invalid pointers.
	};

	//------------------------------------------------------------------------
	namespace Coalesce
	{
		//------------------------------------------------------------------------
		const size_t g_MaxCheckMemorySize = 128;

		//------------------------------------------------------------------------
#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE
		const int g_FreeNodeBucketSize = 32;
		typedef BucketTree<NodeBucket> NodeBucketTree;
#endif

		//------------------------------------------------------------------------
		// A reserved range of memory from which we commit memory for the allocations
		struct Region
		{
#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE
			Region(int value_count, int bucket_size, InternalHeap& internal_heap)
			:	m_BucketTree(value_count, bucket_size, internal_heap),
#else
			Region() :
#endif
				mp_Next(NULL)
			{
				for(int i=0; i<(int)(sizeof(m_Marker)/sizeof(unsigned int)); ++i)
					m_Marker[i] = VMEM_COALESCE_ALLOC_MARKER;
			}

			Region* mp_Next;
#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE
			NodeBucketTree m_BucketTree;
			unsigned int m_Marker[1];		// pack to maintain alignment
#else
			int m_LargestFreeNodeSize;
			unsigned int m_Marker[1];
			VMEM_X64_ONLY(unsigned int padding[4]);
			Node mp_FreeNodeLists[g_FreeNodeMapSize];	// essentially a map from alloc size to free list
#endif
		};

		//------------------------------------------------------------------------
		// embedded before the start of the allocation. The size and prev size are
		// needed so that we can coalesce previous and next allocation when deallocating.
		struct Header
		{
#ifdef VMEM_COALESCE_HEAP_MARKER
			unsigned int m_Marker;
			VMEM_X64_ONLY(unsigned int m_Padding[3]);		// unused
#endif

			int m_Size;					// includes the size of the header
			int m_PrevSize;				// size of the previous sequential block in memory

			// pointer to a node or one of the node flag defines
			union
			{
				size_t m_NodeType;		// ENodeType - use size_t to ensure sizeof(m_NodeType) == sizeof(Node*)
				Node* mp_Node;
			};

#if VMEM_NATURAL_ALIGNMENT == 32 && !defined(VMEM_COALESCE_HEAP_MARKER)
			int padding[4];
#endif
		};

		//------------------------------------------------------------------------
		// check alignment
		VMEM_STATIC_ASSERT((sizeof(Header) & (g_CoalesceHeapAlignment-1)) == 0, "Header size not aligned");
		VMEM_STATIC_ASSERT((sizeof(Region) & (g_CoalesceHeapAlignment-1)) == 0, "Region size not aligned");
		VMEM_STATIC_ASSERT((VMEM_COALESCE_GUARD_SIZE & (g_CoalesceHeapAlignment-1)) == 0, "guard size not aligned");

		//------------------------------------------------------------------------
		const int g_RegionOverhead = sizeof(Region) + 2*sizeof(Header);

		//------------------------------------------------------------------------
		inline void SetupHeader(Header* p_header)
		{
#ifdef VMEM_COALESCE_HEAP_MARKER
			p_header->m_Marker = VMEM_COALESCE_ALLOC_MARKER;
			VMEM_X64_ONLY(p_header->m_Padding[0] = p_header->m_Padding[1] = p_header->m_Padding[2] = 0);
#endif
		}

		//------------------------------------------------------------------------
#ifndef VMEM_COALESCE_HEAP_BUCKET_TREE
		void UpdateLargestFreeNodeSize(Region* p_region)
		{
			Coalesce::Node* p_free_node_lists = p_region->mp_FreeNodeLists;

			Coalesce::Node* p_list_head = p_free_node_lists + g_FreeNodeMapSize - 1;
			int max_size = 0;
			for (Coalesce::Node* p_node = p_list_head->mp_Next; p_node != p_list_head; p_node = p_node->mp_Next)
			{
				if(p_node->m_Size > max_size)
					max_size = p_node->m_Size;
			}

			if (max_size)
			{
				p_region->m_LargestFreeNodeSize = max_size;
			}
			else
			{
				--p_list_head;
				while(p_list_head >= p_region->mp_FreeNodeLists)
				{
					Coalesce::Node* p_node = p_list_head->mp_Next;
					if (p_node != p_list_head)
					{
						while (p_node->mp_Next != p_list_head)
							p_node = p_node->mp_Next;
						p_region->m_LargestFreeNodeSize = p_node->m_Size;
						return;
					}
					--p_list_head;
				}
				p_region->m_LargestFreeNodeSize = 0;
			}
		}
#endif
		//------------------------------------------------------------------------
#ifdef VMEM_SORT_COALESCE_HEAP_ON_SERVICE_THREAD
		VMEM_FORCE_INLINE void SwapWithNextNode(Node* p_node1)
		{
			Node* p_node2 = p_node1->mp_Next;

			Node* p_prev = p_node1->mp_Prev;
			Node* p_next = p_node2->mp_Next;

			p_prev->mp_Next = p_node2;
			p_node2->mp_Prev = p_prev;
			p_node2->mp_Next = p_node1;
			p_node1->mp_Prev = p_node2;
			p_node1->mp_Next = p_next;
			p_next->mp_Prev = p_node1;
		}
#endif

		//------------------------------------------------------------------------
#ifdef VMEM_SORT_COALESCE_HEAP_ON_SERVICE_THREAD
		void AddNodeBucketToSortList(Coalesce::NodeBucket* p_prev, Coalesce::NodeBucket* p_bucket)
		{
			Coalesce::NodeBucket* p_prev_bucket = p_prev;
			Coalesce::NodeBucket* p_next_bucket = p_prev_bucket->mp_NextNeedsSortBucket;
			p_prev_bucket->mp_NextNeedsSortBucket = p_bucket;
			p_bucket->mp_PrevNeedsSortBucket = p_prev_bucket;
			p_bucket->mp_NextNeedsSortBucket = p_next_bucket;
			p_next_bucket->mp_PrevNeedsSortBucket = p_bucket;
		}

		//------------------------------------------------------------------------
		void RemoveNodeBucketFromSortList(Coalesce::NodeBucket* p_bucket)
		{
			Coalesce::NodeBucket* p_prev = p_bucket->mp_PrevNeedsSortBucket;
			Coalesce::NodeBucket* p_next = p_bucket->mp_NextNeedsSortBucket;

			p_prev->mp_NextNeedsSortBucket = p_next;
			p_next->mp_PrevNeedsSortBucket = p_prev;
			p_bucket->mp_PrevNeedsSortBucket = p_bucket->mp_NextNeedsSortBucket = NULL;
		}
#endif
	}

	//------------------------------------------------------------------------
	// The region size will be reserved, but not necessarily committed.
	// All allocation sizes requested must be in this min/max range.
	CoalesceHeap::CoalesceHeap(
		int region_size,
		int min_size,
		int max_size,
		InternalHeap& internal_heap,
		HeapRegions& heap_regions,
		RegionType::Enum region_type,
		int reserve_flags,
		int commit_flags,
		VirtualMem& virtual_mem,
		bool cache_enabled,
		bool add_initial_region_to_heap_regions)
	:
#ifdef VMEM_COALESCE_HEAP_CACHE
		m_CacheEnabled(cache_enabled),
		m_Cache(internal_heap),
#endif
		m_RegionSize(AlignUpPow2(region_size, virtual_mem.GetPageSize())),
		m_VirtualMem(virtual_mem),
		m_MinSize(AlignDownPow2(min_size, g_CoalesceHeapAlignment)),
		m_MaxSize(AlignUpPow2(max_size, g_CoalesceHeapAlignment)),
		m_SizeRange(m_MaxSize - m_MinSize - 1),
		mp_RegionList(NULL),
		m_NodeFSA(virtual_mem.GetPageSize()),
		mp_SpareNodeFreeList(NULL),
		m_InternalHeap(internal_heap),
		m_HeapRegions(heap_regions),
		m_RegionType(region_type),
		m_AddInitialRegionToHeapRegions(add_initial_region_to_heap_regions),
		m_ReserveFlags(reserve_flags),
		m_CommitFlags(commit_flags)
	{
#ifndef VMEM_COALESCE_HEAP_CACHE
		VMEM_UNREFERENCED_PARAM(cache_enabled);
#endif
	}

	//------------------------------------------------------------------------
	bool CoalesceHeap::Initialise()
	{
#ifdef VMEM_COALESCE_HEAP_CACHE
		if(m_CacheEnabled)
		{
			if(!m_Cache.Initialise(m_MinSize, VMin(m_MaxSize, VMEM_COALESCE_HEAP_CACHE_MAX_BUCKET_SIZE)))
				return false;
		}
#endif

#ifdef VMEM_SORT_COALESCE_HEAP_ON_SERVICE_THREAD
		m_BucketsToSortList.mp_PrevNeedsSortBucket = m_BucketsToSortList.mp_NextNeedsSortBucket = &m_BucketsToSortList;
#endif

		VMEM_ASSERT(m_MaxSize + (int)sizeof(Coalesce::Header) <= m_RegionSize - Coalesce::g_RegionOverhead, "region size not big enough to hold dmax alloc (+overhead)");

		mp_RegionList = CreateRegion(m_AddInitialRegionToHeapRegions);

		return mp_RegionList != NULL;
	}

	//------------------------------------------------------------------------
	CoalesceHeap::~CoalesceHeap()
	{
		CriticalSectionScope lock(m_CriticalSection);

#ifdef VMEM_COALESCE_HEAP_CACHE
		FlushCache(0);
#endif

	#ifdef VMEM_TRAIL_GUARDS
		// free the trail allocs
		void* p_trail_alloc = m_TrailGuard.Shutdown();
		while(p_trail_alloc)
		{
			void* p_next = *(void**)p_trail_alloc;
			FreeInternal(p_trail_alloc);
			p_trail_alloc = p_next;
		}
	#endif

		// ideally the region will be empty when the coalesce heap is destructed
		// but we will clean up properly anyway. We can't just release all the
		// virtual memory otherwise the committed pages count will be wrong and
		// the free nodes won't be deleted. So we go through all allocations and
		// delete them using the normal method.
		if(mp_RegionList)
		{
			Coalesce::Region* p_region = mp_RegionList;
			while(p_region)
			{
				Coalesce::Region* p_next_region = p_region->mp_Next;
				ClearRegion(p_region);
				p_region = p_next_region;
			}

			// the initial region is not destroyed automatically, so have to destroy it explicitly
			DestroyRegion(mp_RegionList);
		}

		// free the spare node free list
		Coalesce::Node* p_node = mp_SpareNodeFreeList;
		while(p_node)
		{
			Coalesce::Node* p_next = p_node->mp_Next;
			VMEM_ASSERT_CODE(p_node->mp_Prev = p_node->mp_Next = NULL);
			FreeNode(p_node);
			p_node = p_next;
		}
	}

	//------------------------------------------------------------------------
	void* CoalesceHeap::Alloc(size_t size)
	{
		int i_size = ToInt(size);

		if(i_size < m_MinSize)
			i_size = m_MinSize;

		VMEM_ASSERT(i_size <= m_MaxSize, "size out of range for this coalesce heap");

		// work out the actual size of the memory block
		int aligned_size = AlignUpPow2(i_size, g_CoalesceHeapAlignment);

#ifdef VMEM_COALESCE_HEAP_CACHE
		if(m_CacheEnabled && size <= VMEM_COALESCE_HEAP_CACHE_MAX_BUCKET_SIZE)
		{
			void* p_cached_alloc = m_Cache.Remove(aligned_size);
			if(p_cached_alloc)
			{
				VMEM_STATS(CriticalSectionScope lock(m_CriticalSection));
				VMEM_STATS(size_t alloc_size = GetSize(p_cached_alloc));
				VMEM_STATS(m_Stats.m_Unused -= alloc_size);
				VMEM_STATS(m_Stats.m_Used += alloc_size);
				#ifdef VMEM_CUSTOM_ALLOC_INFO
					VMEM_STATS(m_Stats.m_Used -= sizeof(VMemCustomAllocInfo));
					VMEM_STATS(m_Stats.m_Overhead += sizeof(VMemCustomAllocInfo));
				#endif
				return p_cached_alloc;
			}
		}
#endif

		int total_size = sizeof(Coalesce::Header) + aligned_size;
		#ifdef VMEM_COALESCE_GUARDS
			total_size += 2*VMEM_COALESCE_GUARD_SIZE;
		#endif

		// find a node that will fit this allocation size
		Coalesce::Region* p_region = NULL;
		CriticalSectionScope lock(m_CriticalSection);		// entering critical section here -------------------

		// allocate a spare node so that free can never fail
		Coalesce::Node* p_node_for_free = AllocNode();
		if(!p_node_for_free)
			return NULL;
		p_node_for_free->mp_Next = mp_SpareNodeFreeList;
		mp_SpareNodeFreeList = p_node_for_free;

		Coalesce::Node* VMEM_RESTRICT p_node = GetNode(total_size, p_region);	// this also removes the node
		if(!p_node)
			return NULL;	// this means out of memory

		void* VMEM_RESTRICT p_mem = p_node->mp_Mem;

		// work out the offcut size
		int offcut_size = p_node->m_Size - total_size;
		VMEM_ASSERT(offcut_size >= 0, "node returned from GetNode is not big enough");

		// if the offcut size is too small ignore it and add it on to the alloc
		if(offcut_size < (int)sizeof(Coalesce::Header))
		{
			total_size += offcut_size;
			offcut_size = 0;
		}

		// work out the range to commit
		void* VMEM_RESTRICT p_commit_start = (byte*)p_node->mp_Mem + sizeof(Coalesce::Header);
		int commit_size = total_size - sizeof(Coalesce::Header);

		// setup the header for this allocation
		Coalesce::Header* VMEM_RESTRICT p_header = (Coalesce::Header*)p_mem;
		Coalesce::Header* VMEM_RESTRICT p_next_header = (Coalesce::Header*)((byte*)p_header + p_header->m_Size);
		VMEM_ASSERT_MEM(p_header->m_NodeType > nt_Fragment, &p_header->m_NodeType);		// GetNode returned an allocated node?
		VMEM_ASSERT_COALESCE_MARKER(&p_header->m_Marker);

		int page_size = m_VirtualMem.GetPageSize();

		// deal with the offcut
		if(offcut_size)
		{
			// get the pointer to the end of the range to commit. The end pointer is exclusive.
			// +sizeof(Coalesce::Header) because we always need to commit the new header for the new offcut
			void* p_commit_end = (byte*)p_commit_start + commit_size + sizeof(Coalesce::Header);

			// Commit the page for the offcut header.
			// If the next block header is in the same page as the offcut then it will
			// already be committed, otherwise we need to commit it here
			void* p_last_page = AlignUpPow2(p_commit_end, page_size);
			VMEM_ASSERT_COALESCE_MARKER(&p_next_header->m_Marker);
			if(p_last_page <= p_next_header)
				p_commit_end = p_last_page;

			// need to commit the memory before writing to the new offcut header
			int offcut_commit_size = ToInt((byte*)p_commit_end - (byte*)p_commit_start);
			if(!CommitRange(p_commit_start, offcut_commit_size))
			{
				InsertNode(p_node, p_region);

				// destroy the region if we just created it
				if (p_region != mp_RegionList)
				{
					int total_free_size = m_RegionSize - sizeof(Coalesce::Region) - 2 * sizeof(Coalesce::Header);
					if (p_node->m_Size == total_free_size)
						DestroyRegion(p_region);
				}

				return NULL;		// out of memory
			}

			// set the offcut header
			void* VMEM_RESTRICT p_offcut_mem = (byte*)p_mem + total_size;
			Coalesce::Header* VMEM_RESTRICT p_offcut_header = (Coalesce::Header*)p_offcut_mem;
			SetupHeader(p_offcut_header);
			p_offcut_header->m_Size = offcut_size;
			p_offcut_header->m_PrevSize = total_size;

			p_next_header->m_PrevSize = offcut_size;

			if(offcut_size < m_MinSize)
			{
				p_offcut_header->m_NodeType = nt_Fragment;
				FreeNode(p_node);
			}
			else
			{
				// re-use the free node for the offcut
				p_offcut_header->mp_Node = p_node;

				// update the offcut node
				p_node->mp_Mem = p_offcut_mem;
				p_node->m_Size = offcut_size;

				// reinsert the offcut node
				InsertNode(p_node, p_region);
			}

			#ifdef VMEM_ENABLE_MEMSET
				if(offcut_size > ToInt(sizeof(Coalesce::Header)))
				{
					void* p_offcut_memset_mem = (byte*)p_offcut_mem + sizeof(Coalesce::Header);
					size_t memset_size = offcut_size - sizeof(Coalesce::Header);

					// only commit up to the end of the page if the next page is not committed
					void* p_start_page = AlignDownPow2((byte*)p_offcut_memset_mem-1, page_size);
					void* p_end_page = AlignDownPow2((byte*)p_offcut_mem+offcut_size, page_size);
					if((byte*)p_end_page - (byte*)p_start_page > page_size)
						memset_size = (byte*)p_start_page + page_size - (byte*)p_offcut_memset_mem;

					if(memset_size)
						VMEM_MEMSET(p_offcut_memset_mem, VMEM_FREED_MEM, memset_size);
				}
			#endif

			VMEM_STATS(m_Stats.m_Overhead += sizeof(Coalesce::Header));
			VMEM_STATS(m_Stats.m_Unused -= sizeof(Coalesce::Header));
		}
		else
		{
			// must try and commit memory before setting anything
			if(!CommitRange(p_commit_start, commit_size))
			{
				InsertNode(p_node, p_region);

				// destroy the region if we just created it
				if (p_region != mp_RegionList)
				{
					int total_free_size = m_RegionSize - sizeof(Coalesce::Region) - 2 * sizeof(Coalesce::Header);
					if (p_node->m_Size == total_free_size)
						DestroyRegion(p_region);
				}

				return NULL;		// out of memory!
			}

			FreeNode(p_node);

			p_next_header->m_PrevSize = total_size;
		}

		p_header->m_NodeType = nt_Allocated;
		p_header->m_Size = total_size;

		void* p = p_header + 1;
	#ifdef VMEM_ENABLE_MEMSET
		CheckMemory(p, total_size-sizeof(Coalesce::Header), VMEM_FREED_MEM);
	#endif
		VMEM_MEMSET(p, VMEM_ALLOCATED_MEM, total_size-sizeof(Coalesce::Header));

	#ifdef VMEM_COALESCE_GUARDS
		SetGuards(p, VMEM_COALESCE_GUARD_SIZE);
		SetGuards((byte*)p_header + total_size - VMEM_COALESCE_GUARD_SIZE, VMEM_COALESCE_GUARD_SIZE);
		p = (byte*)p + VMEM_COALESCE_GUARD_SIZE;
	#endif

	#ifdef VMEM_ENABLE_STATS
		int allocated_size = p_header->m_Size - sizeof(Coalesce::Header);
	#ifdef VMEM_COALESCE_GUARDS
		allocated_size -= 2*VMEM_COALESCE_GUARD_SIZE;
	#endif
		VMEM_STATS(m_Stats.m_Used += allocated_size);
		VMEM_STATS(m_Stats.m_Unused -= allocated_size);
		#ifdef VMEM_CUSTOM_ALLOC_INFO
			VMEM_STATS(m_Stats.m_Used -= sizeof(VMemCustomAllocInfo));
			VMEM_STATS(m_Stats.m_Overhead += sizeof(VMemCustomAllocInfo));
		#endif
	#endif

		return p;
	}

	//------------------------------------------------------------------------
	void CoalesceHeap::Free(void* p)
	{
#ifdef VMEM_COALESCE_HEAP_CACHE
		if(m_CacheEnabled)
		{
			int size = GetSize(p);
			if(size <= VMEM_COALESCE_HEAP_CACHE_MAX_BUCKET_SIZE && m_Cache.Add(p, size))
			{
#ifdef VMEM_ENABLE_STATS
				CriticalSectionScope lock(m_CriticalSection);
				#ifdef VMEM_CUSTOM_ALLOC_INFO
					VMEM_STATS(m_Stats.m_Used += sizeof(VMemCustomAllocInfo));
					VMEM_STATS(m_Stats.m_Overhead -= sizeof(VMemCustomAllocInfo));
				#endif
				VMEM_STATS(m_Stats.m_Unused += size);
				VMEM_STATS(m_Stats.m_Used -= size);
				if(m_Cache.GetSize() > VMEM_COALESCE_HEAP_CACHE_SIZE)
					FlushCache(90*VMEM_COALESCE_HEAP_CACHE_SIZE/100);
#else
				if(m_Cache.GetSize() > VMEM_COALESCE_HEAP_CACHE_SIZE)
				{
					CriticalSectionScope lock(m_CriticalSection);
					FlushCache(90*VMEM_COALESCE_HEAP_CACHE_SIZE/100);
				}
#endif
				return;
			}
		}
#endif
		CriticalSectionScope lock(m_CriticalSection);
		FreeInternal(p);
	}

	//------------------------------------------------------------------------
	bool CoalesceHeap::FreeInternal(void* p)
	{
		// find the region that owns this alloc
		Coalesce::Region* p_region = GetRegion(p);
		VMEM_ASSERT(p_region, "CoalesceHeap trying to free allocation that it doesn't own");

#ifdef VMEM_COALESCE_GUARDS
		p = (byte*)p - VMEM_COALESCE_GUARD_SIZE;
#endif
		// get the header for this alloc
		Coalesce::Header* VMEM_RESTRICT p_header = (Coalesce::Header*)p - 1;

		// if this hits it usually means that you are trying to free an
		// allocation that wasn't allocated from VMem (ie. something
		// within a coalesce alloc)
		VMEM_ASSERT_COALESCE_MARKER(&p_header->m_Marker);

		int size = p_header->m_Size;

#ifdef VMEM_TRAIL_GUARDS
		if(m_TrailGuard.GetSize())
		{
			// push the alloc onto the tail list and pop another alloc off the end
			#ifdef VMEM_COALESCE_GUARDS
				p = (byte*)p + VMEM_COALESCE_GUARD_SIZE;
				size -= 2*VMEM_COALESCE_GUARD_SIZE;
			#endif
			size -= sizeof(Coalesce::Header);

			// move stats from used to overhead
			#ifdef VMEM_CUSTOM_ALLOC_INFO
				VMEM_STATS(m_Stats.m_Used += sizeof(VMemCustomAllocInfo));
				VMEM_STATS(m_Stats.m_Overhead -= sizeof(VMemCustomAllocInfo));
			#endif
			VMEM_STATS(m_Stats.m_Used -= size);
			VMEM_STATS(m_Stats.m_Overhead += size);

			// put the alloc onto the trail, and pop another alloc to free
			p = m_TrailGuard.Add(p, size);
			if(!p) return true;					// return here if TrailGuard didn't return an alloc to free

			// get the details of the new alloc to free
			#ifdef VMEM_COALESCE_GUARDS
				p = (byte*)p - VMEM_COALESCE_GUARD_SIZE;
			#endif
			p_header = (Coalesce::Header*)p - 1;
			size = p_header->m_Size;

			// move stats back from overhead to used
			int stats_size = size - sizeof(Coalesce::Header);
			#ifdef VMEM_COALESCE_GUARDS
				stats_size -= 2*VMEM_COALESCE_GUARD_SIZE;
			#endif
			VMEM_STATS(m_Stats.m_Used += stats_size);
			VMEM_STATS(m_Stats.m_Overhead -= stats_size);
			#ifdef VMEM_CUSTOM_ALLOC_INFO
				VMEM_STATS(m_Stats.m_Used -= sizeof(VMemCustomAllocInfo));
				VMEM_STATS(m_Stats.m_Overhead += sizeof(VMemCustomAllocInfo));
			#endif

			// the region could have changed
			p_region = GetRegion(p);
			VMEM_ASSERT(p_region, "unable to find region");
		}
	#endif

	#ifdef VMEM_COALESCE_GUARDS
		CheckMemory(p, VMEM_COALESCE_GUARD_SIZE, VMEM_GUARD_MEM);
		CheckMemory((byte*)p_header + size - VMEM_COALESCE_GUARD_SIZE, VMEM_COALESCE_GUARD_SIZE, VMEM_GUARD_MEM);
	#endif

		VMEM_MEMSET(p, VMEM_FREED_MEM, size-sizeof(Coalesce::Header));

		// get the range of memory we can definitely try and decommit. The end pointer is exclusive.
		void* VMEM_RESTRICT p_decommit_start = p;
		void* p_decommit_end = (byte*)p + size - sizeof(Coalesce::Header);

		// get the prev and next headers
		Coalesce::Header* p_prev_header = (Coalesce::Header*)((byte*)p_header - p_header->m_PrevSize);
		Coalesce::Header* p_next_header = (Coalesce::Header*)((byte*)p_header + size);

		VMEM_ASSERT_COALESCE_MARKER(&p_prev_header->m_Marker);
		VMEM_ASSERT_COALESCE_MARKER(&p_next_header->m_Marker);

		int merged_size = size;

		int page_size = m_VirtualMem.GetPageSize();

		// see if we can merge with the prev block (a valid node pointer means it's free)
		Coalesce::Node* VMEM_RESTRICT p_new_node = NULL;
		size_t prev_node_type = p_prev_header->m_NodeType;
		if(prev_node_type > nt_Allocated)	// if block is free
		{
			CheckIntegrity(p_prev_header);

			merged_size += p_prev_header->m_Size;

			Coalesce::Node* VMEM_RESTRICT p_prev_node = p_prev_header->mp_Node;			// take the node pointer before we memset it

			VMEM_MEMSET(p_header, VMEM_FREED_MEM, sizeof(Coalesce::Header));

			p_header = p_prev_header;

			p_next_header->m_PrevSize = merged_size;

			// if it's not a fragment update the free node
			if(prev_node_type != nt_Fragment)
			{
				RemoveNode(p_prev_node, p_region);
				p_new_node = p_prev_node;
			}

			// we can decommit the original header if we are merging with the prev block
			p_decommit_start = (byte*)p_decommit_start - sizeof(Coalesce::Header);

			// we can also decommit the current page if it doesn't contain the prev block header
			void* VMEM_RESTRICT p_cur_page = AlignDownPow2(p_decommit_start, page_size);
			if(p_cur_page >= p_header+1)
				p_decommit_start = p_cur_page;

			VMEM_STATS(m_Stats.m_Overhead -= sizeof(Coalesce::Header));
			VMEM_STATS(m_Stats.m_Unused += sizeof(Coalesce::Header));
		}

		// see if we can merge with the next alloc
		size_t next_node_type = p_next_header->m_NodeType;
		if(next_node_type > nt_Allocated)	// if block is free
		{
			CheckIntegrity(p_next_header);

			merged_size += p_next_header->m_Size;

			Coalesce::Node* p_next_node = p_next_header->mp_Node;			// take the node pointer before we memset it

			VMEM_MEMSET(p_next_header, VMEM_FREED_MEM, sizeof(Coalesce::Header));

			// see if we can re-use the next node
			if(next_node_type != nt_Fragment)
			{
				RemoveNode(p_next_node, p_region);

				if(p_new_node)
				{
					// we already have a node, so free the next node
					FreeNode(p_next_node);
				}
				else
				{
					// don't have a node so re-use the next node
					p_new_node = p_next_node;
					p_new_node->mp_Mem = p_header;
				}
			}

			// update the next header prev size
			Coalesce::Header* VMEM_RESTRICT p_next_next_header = (Coalesce::Header*)((byte*)p_header + merged_size);
			p_next_next_header->m_PrevSize = merged_size;

			// we can decommit the next header
			p_decommit_end = (byte*)p_decommit_end + sizeof(Coalesce::Header);

			// we can also decommit the page that the next header is on if it doesn't contain the next next header
			void* VMEM_RESTRICT p_next_page = AlignUpPow2(p_decommit_end, page_size);
			if(p_next_page <= p_next_next_header)
				p_decommit_end = p_next_page;

			VMEM_STATS(m_Stats.m_Overhead -= sizeof(Coalesce::Header));
			VMEM_STATS(m_Stats.m_Unused += sizeof(Coalesce::Header));
		}

		p_header->m_Size = merged_size;

		// get the spare node that was allocated by the alloc function
		VMEM_ASSERT_MEM(mp_SpareNodeFreeList, &mp_SpareNodeFreeList);
		VMEM_ASSUME(mp_SpareNodeFreeList);
		Coalesce::Node* p_spare_node = mp_SpareNodeFreeList;
		mp_SpareNodeFreeList = p_spare_node->mp_Next;

		if(!p_new_node)
		{
			// didn't merge, so use spare node
			p_new_node = p_spare_node;
			p_new_node->mp_Mem = p_header;
		}
		else
		{
			// don't need spare node, so free it
			VMEM_ASSERT_CODE(p_spare_node->mp_Prev = p_spare_node->mp_Next = NULL);
			FreeNode(p_spare_node);
		}

		// setup the new node
		p_new_node->m_Size = merged_size;
		p_new_node->mp_Next = NULL;
		p_new_node->mp_Prev = NULL;
		VMEM_X64_ONLY(p_new_node->m_Padding = 0);
		p_header->mp_Node = p_new_node;

		// (re)insert the new free node
		InsertNode(p_new_node, p_region);

	#ifdef VMEM_ENABLE_STATS
		int alloc_size = size - sizeof(Coalesce::Header);
		#ifdef VMEM_COALESCE_GUARDS
			alloc_size -= 2*VMEM_COALESCE_GUARD_SIZE;
		#endif
	#endif
		#ifdef VMEM_CUSTOM_ALLOC_INFO
			VMEM_STATS(m_Stats.m_Used += sizeof(VMemCustomAllocInfo));
			VMEM_STATS(m_Stats.m_Overhead -= sizeof(VMemCustomAllocInfo));
		#endif
		VMEM_STATS(m_Stats.m_Used -= alloc_size);
		VMEM_STATS(m_Stats.m_Unused += alloc_size);

		// decommit the range
		int range_size = ToInt((byte*)p_decommit_end - (byte*)p_decommit_start);
		DecommitRange(p_decommit_start, range_size);

		// destroy region if empty (unless it's the initial region
		if(p_region != mp_RegionList)
		{
			int total_free_size = m_RegionSize - sizeof(Coalesce::Region) - 2*sizeof(Coalesce::Header);
			if(merged_size == total_free_size)
				DestroyRegion(p_region);
		}

		return true;
	}

	//------------------------------------------------------------------------
	int CoalesceHeap::GetSize(void* p)
	{
	#ifdef VMEM_COALESCE_GUARDS
		p = (byte*)p - VMEM_COALESCE_GUARD_SIZE;
	#endif

		// get the header for this alloc
		Coalesce::Header* p_header = (Coalesce::Header*)p - 1;
		VMEM_ASSERT_COALESCE_MARKER(&p_header->m_Marker);

		int size = p_header->m_Size - sizeof(Coalesce::Header);

	#ifdef VMEM_COALESCE_GUARDS
		size -= 2*VMEM_COALESCE_GUARD_SIZE;
	#endif

		return size;
	}

	//------------------------------------------------------------------------
	Coalesce::Region* CoalesceHeap::CreateRegion(bool add_to_heap_regions)
	{
		int page_size = m_VirtualMem.GetPageSize();

		// reserve region memory
		void* p_region_mem = m_VirtualMem.Reserve(m_RegionSize, page_size, m_ReserveFlags);
		if(!p_region_mem)
			return NULL;		// out of virtual memory!

		// we have a fixed header at the start and end of the region that are never
		// allocated or freed. This makes the logic for coalescing simpler because
		// we can always assume there is a prev and next header.

		// commit the first page to store the start fixed alloc header and the free node header
		int offset = sizeof(Coalesce::Region) + 2*sizeof(Coalesce::Header);
		int start_commit_size = AlignUpPow2(offset, page_size);
		if(!m_VirtualMem.Commit(p_region_mem, start_commit_size, m_CommitFlags))
		{
			m_VirtualMem.Release(p_region_mem);
			return NULL;		// out of memory
		}
		VMEM_MEMSET((byte*)p_region_mem + offset, VMEM_FREED_MEM, start_commit_size - offset);
		VMEM_STATS(m_Stats.m_Unused += start_commit_size);

		// add a node for the entire free memory
		Coalesce::Node* p_node = AllocNode();
		if(!p_node)
		{
			VMEM_STATS(m_Stats.m_Unused -= start_commit_size);
			m_VirtualMem.Decommit(p_region_mem, start_commit_size, m_CommitFlags);
			m_VirtualMem.Release(p_region_mem);
			return NULL;		// out of memory
		}

		// commit the last page to store the end fixed alloc
		int end_commit_size = AlignUpPow2(sizeof(Coalesce::Header), page_size);
		void* p_last_page = (byte*)p_region_mem + m_RegionSize - end_commit_size;
		if(p_last_page >= (byte*)p_region_mem + start_commit_size)
		{
			if(!m_VirtualMem.Commit(p_last_page, end_commit_size, m_CommitFlags))
			{
				FreeNode(p_node);
				VMEM_STATS(m_Stats.m_Unused -= start_commit_size);
				m_VirtualMem.Decommit(p_region_mem, start_commit_size, m_CommitFlags);
				m_VirtualMem.Release(p_region_mem);
				return NULL;		// out of memory
			}
			VMEM_STATS(m_Stats.m_Unused += end_commit_size);
			VMEM_MEMSET(p_last_page, VMEM_FREED_MEM, end_commit_size);
		}

		// allocate region object at the start of the region memory
		Coalesce::Region* p_region = (Coalesce::Region*)p_region_mem;
#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE
		new (p_region)Coalesce::Region(m_MaxSize - m_MinSize, Coalesce::g_FreeNodeBucketSize, m_InternalHeap);
#else
		new (p_region)Coalesce::Region();
#endif

		void* p_mem = (byte*)p_region_mem + sizeof(Coalesce::Region);

		// clear free node list head nodes
#ifndef VMEM_COALESCE_HEAP_BUCKET_TREE
		Coalesce::Node* p_free_node_lists = p_region->mp_FreeNodeLists;
		memset(p_free_node_lists, 0, sizeof(p_region->mp_FreeNodeLists));

		// the free lists are circular so setup the prev and next pointers for the list heads.
		for(int i=0; i<g_FreeNodeMapSize; ++i)
		{
			Coalesce::Node& node_list = p_free_node_lists[i];
			node_list.mp_Prev = &node_list;
			node_list.mp_Next = &node_list;
		}
#endif
		// we need to put a fixed allocation at the start and end of the memory so that
		// we dont have to check if we are the first or last allocation when coalescing.

		// setup the fixed start alloc
		Coalesce::Header* p_start_header = (Coalesce::Header*)p_mem;
		SetupHeader(p_start_header);
		p_start_header->m_Size = sizeof(Coalesce::Header);
		p_start_header->m_PrevSize = 0;
		p_start_header->m_NodeType = nt_Allocated;

		// setup the main free node
		Coalesce::Header* p_header = p_start_header + 1;
		int total_free_size = m_RegionSize - Coalesce::g_RegionOverhead;
		SetupHeader(p_header);
		p_header->m_Size = total_free_size;
		p_header->m_PrevSize = sizeof(Coalesce::Header);

		// setup the fixed end alloc
		Coalesce::Header* p_end_header = (Coalesce::Header*)((byte*)p_header + total_free_size);
		SetupHeader(p_end_header);
		p_end_header->m_Size = sizeof(Coalesce::Header);
		p_end_header->m_PrevSize = total_free_size;
		p_end_header->m_NodeType = nt_Allocated;

		p_node->mp_Mem = p_header;
		p_node->m_Size = total_free_size;
		p_header->mp_Node = p_node;

		// insert the node into the last free node list
#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE
		p_node->mp_Prev = p_node->mp_Next = NULL;
		InsertNode(p_node, p_region);
#else
		Coalesce::Node& node_list_head = p_free_node_lists[g_FreeNodeMapSize-1];
		node_list_head.mp_Next = p_node;
		node_list_head.mp_Prev = p_node;
		p_node->mp_Prev = &node_list_head;
		p_node->mp_Next = &node_list_head;

		p_region->m_LargestFreeNodeSize = total_free_size;
#endif
		VMEM_STATS(m_Stats.m_Unused -= sizeof(Coalesce::Region) + 3*sizeof(Coalesce::Header));
		VMEM_STATS(m_Stats.m_Overhead += sizeof(Coalesce::Region) + 3*sizeof(Coalesce::Header));
		VMEM_STATS(m_Stats.m_Reserved += m_RegionSize);

#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE
		VMEM_STATS(m_Stats.m_Overhead += p_region->m_BucketTree.GetMemoryUsage());
#endif

	#ifdef VMEM_COALESCE_GUARD_PAGES
		if(!SetupGuardPages(p_region))
		{
			RemoveGuardPages(p_region);
			p_region->~Region();
			if(p_last_page >= (byte*)p_region_mem + start_commit_size)
				m_VirtualMem.Decommit(p_last_page, end_commit_size, m_CommitFlags);
			m_VirtualMem.Decommit(p_region_mem, start_commit_size, m_CommitFlags);
			m_VirtualMem.Release(p_region_mem);
			return NULL;		// out of memory
		}
	#endif

		// add to heap regions array
		if(add_to_heap_regions)
		{
		#ifdef VMEM_COALESCE_HEAP_PER_THREAD
			if(!m_HeapRegions.AddRegion(this, p_region, m_RegionSize, m_RegionType))
		#else
			if(!m_HeapRegions.AddRegion(p_region, m_RegionSize, m_RegionType))
		#endif
			{
				p_region->~Region();
				if(p_last_page >= (byte*)p_region_mem + start_commit_size)
					m_VirtualMem.Decommit(p_last_page, end_commit_size, m_CommitFlags);
				m_VirtualMem.Decommit(p_region_mem, start_commit_size, m_CommitFlags);
				m_VirtualMem.Release(p_region_mem);
				return NULL;		// out of memory
			}
		}

		return p_region;
	}

	//------------------------------------------------------------------------
	Coalesce::Header* CoalesceHeap::FindNextAllocatedHeader(Coalesce::Region* p_region, Coalesce::Header* p_header) const
	{
		Coalesce::Header* p_end_header = (Coalesce::Header*)((byte*)p_region + m_RegionSize) - 1;

		// find the next allocated header by skipping over free nodes and fragment nodes
		while(p_header->m_NodeType != nt_Allocated)
		{
			p_header = (Coalesce::Header*)((byte*)p_header + p_header->m_Size);
			VMEM_ASSERT_COALESCE_MARKER(&p_header->m_Marker);
			VMEM_ASSERT((byte*)p_header >= (byte*)p_region && (byte*)p_header < (byte*)p_region + m_RegionSize, "p_header out of range");
		}
		return p_header != p_end_header ? p_header : NULL;
	}

	//------------------------------------------------------------------------
	// warning: this deletes p_region, don't use p_region after calling this function
	void CoalesceHeap::ClearRegion(Coalesce::Region* p_region)
	{
		// get the first header
		Coalesce::Header* p_header = (Coalesce::Header*)(p_region + 1) + 1;		// +1 for the fixed start alloc

		// get the first allocated header
		p_header = FindNextAllocatedHeader(p_region, p_header);

		while(p_header)
		{
			// check the header
			VMEM_ASSERT_MEM(p_header->m_NodeType == nt_Allocated, &p_header->m_NodeType);
			VMEM_ASSERT((byte*)p_header >= (byte*)p_region && (byte*)p_header < (byte*)p_region + m_RegionSize, "p_header out of range");

			// find the next allocated header by skipping over free nodes and fragment nodes
			Coalesce::Header* p_next_header =  (Coalesce::Header*)((byte*)p_header + p_header->m_Size);
			p_next_header = FindNextAllocatedHeader(p_region, p_next_header);

			// free the alloc
			// warning: this deletes p_region when it's empty, so be careful not to access p_region after last free
			void* p = p_header + 1;
			#ifdef VMEM_COALESCE_GUARDS
				p = (byte*)p + VMEM_COALESCE_GUARD_SIZE;
			#endif
			FreeInternal(p);

			p_header = p_next_header;
		}

	#ifdef VMEM_COALESCE_GUARD_PAGES
		// region won't have been destroyed because it still contains the alloc guards
		RemoveGuardPages(p_region);

		if(p_region != mp_RegionList)
			DestroyRegion(p_region);
#endif
	}

	//------------------------------------------------------------------------
	void CoalesceHeap::DestroyRegion(Coalesce::Region* p_region)
	{
		CheckIntegrity_NoLock();

#if defined(VMEM_SORT_COALESCE_HEAP_ON_SERVICE_THREAD) && defined(VMEM_COALESCE_HEAP_BUCKET_TREE)
		// remove buckets from update list
		int bucket_count = p_region->m_BucketTree.GetBucketCount();
		for(int i=0; i<bucket_count; ++i)
		{
			Coalesce::NodeBucket& bucket = p_region->m_BucketTree[i];
			if(bucket.m_NeedsSort)
			{
				bucket.m_NeedsSort = false;
				RemoveNodeBucketFromSortList(&bucket);
			}
		}
#endif

		if(p_region != mp_RegionList || m_AddInitialRegionToHeapRegions)
			m_HeapRegions.RemoveRegion(p_region);

#ifdef VMEM_ASSERTS
		int total_free_size = m_RegionSize - Coalesce::g_RegionOverhead;
		Coalesce::Header* p_header = (Coalesce::Header*)((byte*)p_region + sizeof(Coalesce::Region) + sizeof(Coalesce::Header));
		VMEM_ASSERT(p_header->m_Size == total_free_size, "trying to destrroy non-empty region");
#endif

		// take region off list
		if(p_region == mp_RegionList)
		{
			mp_RegionList = p_region->mp_Next;
		}
		else
		{
			Coalesce::Region* p_srch_region = mp_RegionList;
			while(p_srch_region)
			{
				Coalesce::Region* p_next = p_srch_region->mp_Next;
				VMEM_ASSERT(p_next, "unable to find region to remove");
				VMEM_ASSUME(p_next);

				if(p_next == p_region)
				{
					p_srch_region->mp_Next = p_region->mp_Next;
					break;
				}

				p_srch_region = p_next;
			}
		}

		// free the last free node that was alloced when creating the region
		Coalesce::Header* p_free_header = (Coalesce::Header*)(p_region + 1) + 1;
		Coalesce::Node* p_node = p_free_header->mp_Node;
		VMEM_ASSERT_COALESCE_MARKER(&p_free_header->m_Marker);
		VMEM_ASSERT_MEM(p_free_header->m_NodeType != nt_Allocated && p_free_header->m_NodeType != nt_Fragment, &p_free_header->m_NodeType);
		VMEM_ASSERT_CODE(VMEM_ASSERT_MEM(p_free_header->m_Size == total_free_size, &p_free_header->m_Size));
		RemoveNode(p_node, p_region);
		FreeNode(p_node);

		VMEM_STATS(m_Stats.m_Unused += sizeof(Coalesce::Region) + 3*sizeof(Coalesce::Header));
		VMEM_STATS(m_Stats.m_Overhead -= sizeof(Coalesce::Region) + 3*sizeof(Coalesce::Header));

		int page_size = m_VirtualMem.GetPageSize();

#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE
		VMEM_STATS(m_Stats.m_Overhead -= p_region->m_BucketTree.GetMemoryUsage());
#endif
		// decommit the start fixed header and region struct
		int start_offset = sizeof(Coalesce::Region) + 2*sizeof(Coalesce::Header);
		int start_commit_size = AlignUpPow2(start_offset, page_size);

#ifdef VMEM_ENABLE_MEMSET
		Coalesce::Header* p_first_header = (Coalesce::Header*)((byte*)p_region + sizeof(Coalesce::Region) + sizeof(Coalesce::Header));
		int free_size = p_first_header->m_Size - sizeof(Coalesce::Header);
		int commit_size = start_commit_size - start_offset;
		int check_size = VMem::VMin(free_size, commit_size);
		CheckMemory(p_first_header+1, check_size, VMEM_FREED_MEM);
#endif

		p_region->~Region();

		m_VirtualMem.Decommit(p_region, start_commit_size, m_CommitFlags);
		VMEM_STATS(m_Stats.m_Unused -= start_commit_size);

		// decommit the end fixed header
		int end_commit_size = AlignUpPow2(sizeof(Coalesce::Header), page_size);
		void* p_last_header = (byte*)p_region + m_RegionSize - sizeof(Coalesce::Header);
		void* p_last_page = AlignDownPow2(p_last_header, page_size);
		if(p_last_page >= (byte*)p_region + start_commit_size)
		{
			#if defined(VMEM_ENABLE_MEMSET) && !defined(VMEM_MEMSET_ONLY_SMALL)
				if(p_last_header > p_last_page)
					CheckMemory(p_last_page, (byte*)p_last_header - (byte*)p_last_page, VMEM_FREED_MEM);
			#endif

			m_VirtualMem.Decommit(p_last_page, end_commit_size, m_CommitFlags);
			VMEM_STATS(m_Stats.m_Unused -= end_commit_size);
		}

		m_VirtualMem.Release(p_region);

		VMEM_STATS(m_Stats.m_Reserved -= m_RegionSize);
	}

	//------------------------------------------------------------------------
	// ensure all the complete pages in the range are committed
	// |-------|---C---|-------|	<- only commit middle page, other pages will already be committed
	//      xxxxxxxxxxxxxx			<- range
	bool CoalesceHeap::CommitRange(void* p, int size)
	{
		VMEM_ASSERT(p, "invalid pointer passed to CommitRange");
		VMEM_ASSERT(size > 0, "invalid size passed to CommitRange");

		int page_size = m_VirtualMem.GetPageSize();

		void* p_page_start = AlignUpPow2(p, page_size);
		void* p_page_end = AlignDownPow2((byte*)p + size, page_size);

		if(p_page_start < p_page_end)
		{
			size_t commit_size = (byte*)p_page_end - (byte*)p_page_start;

			if(!m_VirtualMem.Commit(p_page_start, commit_size, m_CommitFlags))
				return false;

			VMEM_STATS(m_Stats.m_Unused += commit_size);
			VMEM_MEMSET(p_page_start, VMEM_FREED_MEM, commit_size);
		}

		return true;
	}

	//------------------------------------------------------------------------
	// ensure all the complete pages in the range are decommitted
	// |-------|---D---|-------|	<- decommit just the middle page
	//      xxxxxxxxxxxxxx			<- range
	void CoalesceHeap::DecommitRange(void* p, int size)
	{
		VMEM_ASSERT(p, "invalid pointer passed to DecommitRange");
		VMEM_ASSERT(size > 0, "invalid size passed to DecommitRange");

		int page_size = m_VirtualMem.GetPageSize();

		void* p_page_start = AlignUpPow2(p, page_size);
		void* p_page_end = AlignDownPow2((byte*)p + size, page_size);

		if(p_page_start < p_page_end)
		{
			size_t decommit_size = (byte*)p_page_end - (byte*)p_page_start;

			m_VirtualMem.Decommit(p_page_start, decommit_size, m_CommitFlags);
			VMEM_STATS(m_Stats.m_Unused -= decommit_size);
		}
	}

	//------------------------------------------------------------------------
	Coalesce::Node* CoalesceHeap::AllocNode()
	{
		Coalesce::Node* p_node = m_NodeFSA.Alloc();
		VMEM_ASSERT_CODE(if(p_node) { p_node->mp_Prev = p_node->mp_Next = NULL; });
		return p_node;
	}

	//------------------------------------------------------------------------
	void CoalesceHeap::FreeNode(Coalesce::Node* p_node)
	{
		VMEM_ASSERT_MEM(!p_node->mp_Prev, &p_node->mp_Prev);		// trying to free node that is still on a list?
		VMEM_ASSERT_MEM(!p_node->mp_Next, &p_node->mp_Next);		// trying to free node that is still on a list?
		m_NodeFSA.Free(p_node);
	}

	//------------------------------------------------------------------------
	int CoalesceHeap::GetFreeListIndex(int size) const
	{
		// fragments don't go into the free list
		VMEM_ASSERT(size >= m_MinSize, "size passed to GetFreeListIndex is < min size");
		int r = m_MaxSize - m_MinSize;	// the range of possible sizes
		long long s = size - m_MinSize;	// the size (in range space)
#ifdef VMEM_SKEW_SIZES
		if(s > r) s = r;				// coalesced blocks can be bigger than the max size.

		// the skewing loses resolution if the range is not big enough, we are better of with a linear mapping
		// 36 seems about right for 32 bit ints.
		int max_index = g_FreeNodeMapSize - 1;
		if(r > 36 * g_FreeNodeMapSize)
		{
			// the skew formula is
			// skew = r - (s*s*s) / (r*r)
			// where s = r - s
			// this skews it so that there are more indices for smaller sizes which is usually beneficial
			s = r - s;
			s = r - (s * ((s*s)/r) ) / r;
		}

		int index = (int)((s * max_index) / r);
#else
		int index = VMin((int)(s * g_FreeNodeMapSize / r), g_FreeNodeMapSize-1);
#endif
		VMEM_ASSERT(index >= 0 && index < g_FreeNodeMapSize, "GetFreeListIndex returning index out of range");
		return index;
	}

	//------------------------------------------------------------------------
#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE
	int CoalesceHeap::SizeToBucketValue(int size) const
	{
		VMEM_ASSERT(m_MaxSize >= m_MinSize, "CoalesceHeap: Corrupt m_MinSize or m_MaxSize");
		return VMin(size - m_MinSize, m_SizeRange);
	}
#endif

	//------------------------------------------------------------------------
	Coalesce::Node* CoalesceHeap::GetNode(int size, Coalesce::Region*& p_region)
	{
		// get the first region
		p_region = mp_RegionList;
		VMEM_ASSERT(p_region, "something has corrupted mp_RegionList");

#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE
		int node_size = size + Coalesce::g_FreeNodeBucketSize - 1;	// increase to the next bucket size so that we can be guranteed that the first node we find will be big enough

		for(;;)
		{
			int bucket_index = p_region->m_BucketTree.FindExistingBucket(SizeToBucketValue(node_size));
			if(bucket_index != -1)
			{
				Coalesce::NodeBucket& node_bucket = p_region->m_BucketTree[bucket_index];

				Coalesce::Node* p_list = &node_bucket.m_Head;
				Coalesce::Node* p_node = p_list->mp_Next;
				while(p_node != p_list)
				{
					if(p_node->m_Size >= size)
					{
						RemoveNode(p_node, p_region, bucket_index);
						return p_node;
					}

					p_node = p_node->mp_Next;
				}
			}

			Coalesce::Region* p_next_region = p_region->mp_Next;

			if(!p_next_region)
			{
				p_next_region = CreateRegion();
				if(!p_next_region)
					return NULL;

				p_region->mp_Next = p_next_region;
			}

			p_region = p_next_region;
		}
#else
		// get the best list to start searching in
		int start_index = GetFreeListIndex(size);
		VMEM_ASSERT(start_index >= 0 && start_index < g_FreeNodeMapSize, "invalid index returned by GetFreeListIndex");

		int region_count = 0;

		// search for a free node list big enough to hold the alloc
		while(p_region)
		{
			if(p_region->m_LargestFreeNodeSize == INT_MAX)
				UpdateLargestFreeNodeSize(p_region);

			if (p_region->m_LargestFreeNodeSize >= size)
			{
				Coalesce::Node* p_free_node_lists = p_region->mp_FreeNodeLists;

				int index = start_index;
				Coalesce::Node* p_list_head = p_free_node_lists + index;
				Coalesce::Node* p_node = p_list_head->mp_Next;

				// Best Fit - find the smallest node that fits the request (the list is sorted by size, small to large)
				while(p_node != p_list_head && p_node->m_Size < size)
					p_node = p_node->mp_Next;

				// if we haven't found a node that is big enough in the list move on to the next
				// non-empty list. Note that if we move onto another list we don't need to iterate
				// down the list, we can just use the first node in the list because it is always
				// guaranteed to be big enough.
				while(p_node == p_list_head && index < g_FreeNodeMapSize)
				{
					++index;
					++p_list_head;
					p_node = p_list_head->mp_Next;
				}

				// if we've found a node return it
				if(index < g_FreeNodeMapSize && p_node != p_list_head)
				{
					RemoveNode(p_node, p_region);
					return p_node;
				}
			}

			// no next region, so create one
			if(!p_region->mp_Next)
			{
				p_region->mp_Next = CreateRegion();

				if(p_region->mp_Next && region_count > 10)
					VMem::DebugWrite(_T("*** VMem warning: CoalesceHeap with more than 10 regions. Consider making the region size larger.\n"));
			}

			// not found a node, so try the next region
			p_region = p_region->mp_Next;

			++region_count;
		}

		return NULL;		// out of memory
#endif
	}

	//------------------------------------------------------------------------
	void CoalesceHeap::InsertNode(Coalesce::Node* p_node, Coalesce::Region* p_region)
	{
		VMEM_ASSERT(p_node->mp_Mem, "p_node->mp_Mem has been corrupted");
		VMEM_ASSERT_MEM(!p_node->mp_Prev, &p_node->mp_Prev);		// node already inserted?
		VMEM_ASSERT_MEM(!p_node->mp_Next, &p_node->mp_Next);
		VMEM_ASSERT(p_region == GetRegion(p_node->mp_Mem) || !GetRegion(p_node->mp_Mem), "Inserting node from another region");

		int node_size = p_node->m_Size;

#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE
		int bucket_index = p_region->m_BucketTree.GetBucket(SizeToBucketValue(node_size));
		Coalesce::NodeBucket& node_bucket = p_region->m_BucketTree[bucket_index];

		Coalesce::Node* p_prev = &node_bucket.m_Head;

		#ifdef VMEM_SORT_COALESCE_HEAP_ON_SERVICE_THREAD
			if(!node_bucket.m_NeedsSort)
			{
				node_bucket.m_NeedsSort = true;
				node_bucket.m_SortByAddressOnly = bucket_index == p_region->m_BucketTree.GetBucketCount()-1;
				AddNodeBucketToSortList(m_BucketsToSortList.mp_NextNeedsSortBucket, &node_bucket);
			}
		#else
			Coalesce::Node* p_end = &node_bucket.m_Head;
			void* p_node_mem = p_node->mp_Mem;

			bool sort_by_address_only = bucket_index == p_region->m_BucketTree.GetBucketCount()-1;

			if(sort_by_address_only)
			{
				Coalesce::Node* p_next = p_prev->mp_Next;
				while(p_next != p_end && p_next < p_node_mem)
				{
					p_prev = p_next;
					p_next = p_next->mp_Next;
				}
			}
			else
			{
				Coalesce::Node* p_next = p_prev->mp_Next;
				while(p_next != p_end && p_next->m_Size < node_size && p_next < p_node_mem)
				{
					p_prev = p_next;
					p_next = p_next->mp_Next;
				}
			}
		#endif
#else
		// get the list for this node size
		int index = GetFreeListIndex(node_size);
		Coalesce::Node* p_list_head = p_region->mp_FreeNodeLists + index;

		// find the best place to insert
		Coalesce::Node* p_prev = p_list_head;
		if(index == g_FreeNodeMapSize-1)
		{
	#ifndef VMEM_DISABLE_BIASING
			// always sort the top bucket by address only. No point doing best fit on blocks
			// that are much larger than our maximum allocation size, just take the lowest
			// in memory. this biasing reduces fragmentation.
			for(Coalesce::Node* p_iter=p_prev->mp_Next; p_iter!=p_list_head; p_iter=p_iter->mp_Next)
			{
				if(p_node->mp_Mem < p_iter->mp_Mem)
					break;
				p_prev = p_iter;
			}
	#endif
		}
		else
		{
			for(Coalesce::Node* p_iter=p_prev->mp_Next; p_iter!=p_list_head; p_iter=p_iter->mp_Next)
			{
				// sort by size first and then by address, smallest to largest (in both)
	#ifdef VMEM_DISABLE_BIASING
				if(p_iter->m_Size > node_size)
	#else
				if(p_iter->m_Size > node_size ||
					(p_iter->m_Size == node_size && p_node->mp_Mem < p_iter->mp_Mem))
	#endif
				{
					break;
				}
				p_prev = p_iter;
			}
		}

		// update the largest node size
		if(p_node->m_Size > p_region->m_LargestFreeNodeSize)
			p_region->m_LargestFreeNodeSize = p_node->m_Size;
#endif
		// link the node in
		Coalesce::Node* p_next = p_prev->mp_Next;
		p_prev->mp_Next = p_node;
		p_node->mp_Prev = p_prev;
		p_node->mp_Next = p_next;
		p_next->mp_Prev = p_node;
	}

	//------------------------------------------------------------------------
	bool CoalesceHeap::UnlinkNode(Coalesce::Node* p_node)
	{
		Coalesce::Node* p_prev = p_node->mp_Prev;
		Coalesce::Node* p_next = p_node->mp_Next;

		VMEM_ASSERT(GetRegion(p_node->mp_Mem) == GetRegion(p_prev->mp_Mem) || !p_prev->mp_Mem, "Removing node from another region");
		VMEM_ASSERT(GetRegion(p_node->mp_Mem) == GetRegion(p_next->mp_Mem) || !p_next->mp_Mem, "Removing node from another region");

		p_prev->mp_Next = p_next;
		p_next->mp_Prev = p_prev;

		p_node->mp_Prev = NULL;
		p_node->mp_Next = NULL;

		return p_prev == p_next;
	}

	//------------------------------------------------------------------------
	// unlink the node form the list
	void CoalesceHeap::RemoveNode(Coalesce::Node* p_node, Coalesce::Region* p_region)
	{
#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE
		// if the node list is empty tell the BucketTree
		if(UnlinkNode(p_node))
		{
			int bucket_index = p_region->m_BucketTree.GetBucket(SizeToBucketValue(p_node->m_Size));
			p_region->m_BucketTree.ReleaseBucket(bucket_index);
		}
#else
		UnlinkNode(p_node);

		if(p_node->m_Size == p_region->m_LargestFreeNodeSize)
			p_region->m_LargestFreeNodeSize = INT_MAX;
#endif
	}

	//------------------------------------------------------------------------
	// unlink the node form the list
#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE
	void CoalesceHeap::RemoveNode(Coalesce::Node* p_node, Coalesce::Region* p_region, int bucket_index)
	{
		// if the node list is empty tell the BucketTree
		if(UnlinkNode(p_node))
			p_region->m_BucketTree.ReleaseBucket(bucket_index);
	}
#endif

	//------------------------------------------------------------------------
	// return the region that contains the specified address.
	Coalesce::Region* CoalesceHeap::GetRegion(void* p) const
	{
		int region_size = m_RegionSize;
		Coalesce::Region* p_region = mp_RegionList;
		while(p_region)
		{
			byte* p_mem = (byte*)p_region;
			if(p >= p_mem && p < p_mem + region_size)
				return p_region;

			p_region = p_region->mp_Next;
		}
		return NULL;
	}

	//------------------------------------------------------------------------
	void CoalesceHeap::DebugWrite() const
	{
		CriticalSectionScope lock(m_CriticalSection);

		for(Coalesce::Region* p_region = mp_RegionList; p_region!=NULL; p_region=p_region->mp_Next)
		{
			VMem::DebugWrite(_T("--------------------------------\n"), p_region);
			VMem::DebugWrite(_T("Region 0x%08x\n"), p_region);

			// write the sequential memory blocks
			Coalesce::Header* p_header = (Coalesce::Header*)(p_region + 1) + 1;
			Coalesce::Header* p_last_header = (Coalesce::Header*)((byte*)p_region + m_RegionSize - sizeof(Coalesce::Header));
			while(p_header != p_last_header)
			{
				VMEM_ASSERT_COALESCE_MARKER(&p_header->m_Marker);

				int size = p_header->m_Size;
				void* p_start = p_header;
				void* p_end = (byte*)p_header + size;

				const _TCHAR* p_status;
				if(p_header->m_NodeType == nt_Fragment) p_status = _T("frag");
				else if(p_header->m_NodeType == nt_Fixed) p_status = _T("fixed");
				else if(p_header->m_NodeType == nt_Allocated) p_status = _T("alloc");
				else p_status = _T("free");

				VMem::DebugWrite(_T("\t0x%08x - 0x%08x %7d\t%s\n"), p_start, p_end, size, p_status);

				p_header = (Coalesce::Header*)((byte*)p_header + p_header->m_Size);
			}

			// write the free node map
			VMem::DebugWrite(_T("\n"));
			VMem::DebugWrite(_T("Free nodes:\n"));

#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE
			int bucket_count = p_region->m_BucketTree.GetBucketCount();
			for(int i=0; i<bucket_count; ++i)
			{
				const Coalesce::NodeBucket& bucket = p_region->m_BucketTree[i];
				const Coalesce::Node* p_list_head = &bucket.m_Head;

				if(p_list_head->mp_Next != p_list_head)
				{
					VMem::DebugWrite(_T("free list %d\n"), i);

					for(Coalesce::Node* p_node = p_list_head->mp_Next; p_node!=p_list_head; p_node=p_node->mp_Next)
						VMem::DebugWrite(_T("\t%08x %d\n"), p_node->mp_Mem, p_node->m_Size);
				}
			}
#else
			for(int i=0; i<g_FreeNodeMapSize; ++i)
			{
				Coalesce::Node* p_list_head = p_region->mp_FreeNodeLists + i;
				if(p_list_head->mp_Next != p_list_head)
				{
					VMem::DebugWrite(_T("free list %d\n"), i);

					for(Coalesce::Node* p_node = p_list_head->mp_Next; p_node!=p_list_head; p_node=p_node->mp_Next)
					{
						VMEM_ASSUME(p_node);
						VMem::DebugWrite(_T("\t%08x %d\n"), p_node->mp_Mem, p_node->m_Size);
					}
				}
			}
#endif
		}
	}

	//------------------------------------------------------------------------
	void CoalesceHeap::CheckIntegrity() const
	{
#if defined(VMEM_ASSERTS) || defined(VMEM_TRAIL_GUARDS)
		CriticalSectionScope lock(m_CriticalSection);
#endif
		CheckIntegrity_NoLock();
	}

	//------------------------------------------------------------------------
	void CoalesceHeap::CheckIntegrity_NoLock() const
	{
#ifdef VMEM_ASSERTS
		for(Coalesce::Region* p_region = mp_RegionList; p_region!=NULL; p_region=p_region->mp_Next)
		{
			// check headers
			Coalesce::Header* p_header = (Coalesce::Header*)(p_region + 1) + 1;
			Coalesce::Header* p_last_header = (Coalesce::Header*)((byte*)p_region + m_RegionSize - sizeof(Coalesce::Header));
			size_t last_node_type = nt_Allocated;
			int prev_size = -1;
			while(p_header != p_last_header)
			{
				VMEM_ASSERT_COALESCE_MARKER(&p_header->m_Marker);
				VMEM_ASSERT(prev_size == -1 || p_header->m_PrevSize == prev_size, "header prev size is incorrect");
				prev_size = p_header->m_Size;

				VMEM_ASSERT_MEM(p_header->m_NodeType == nt_Allocated || p_header->m_NodeType != last_node_type, &p_header->m_NodeType);		// two consecutive free nodes of the same status
				last_node_type = p_header->m_NodeType;

				#ifdef VMEM_COALESCE_GUARDS
					if(p_header->m_NodeType == nt_Allocated)
					{
						CheckMemory(p_header + 1, VMEM_COALESCE_GUARD_SIZE, VMEM_GUARD_MEM);
						CheckMemory((byte*)p_header + p_header->m_Size - VMEM_COALESCE_GUARD_SIZE, VMEM_COALESCE_GUARD_SIZE, VMEM_GUARD_MEM);
					}
				#endif

				p_header = (Coalesce::Header*)((byte*)p_header + p_header->m_Size);
			}

			// check free nodes
			prev_size = 0;
#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE
			int bucket_count = p_region->m_BucketTree.GetBucketCount();
			for(int i=0; i<bucket_count; ++i)
			{
				const Coalesce::NodeBucket& bucket = p_region->m_BucketTree[i];
				const Coalesce::Node* p_list_head = &bucket.m_Head;
#else
			int largest_free_node_size = 0;
			for(int i=0; i<g_FreeNodeMapSize; ++i)
			{
				Coalesce::Node* p_list_head = p_region->mp_FreeNodeLists + i;
#endif
				VMEM_ASSERT_MEM(p_list_head->mp_Next, &p_list_head->mp_Next);		// found a list in the free node map with a null next pointer

				void* p_prev_addr = NULL;

				for(Coalesce::Node* p_node = p_list_head->mp_Next; p_node!=p_list_head; p_node=p_node->mp_Next)
				{
					VMEM_ASSERT(p_node, "null node");
					VMEM_ASSUME(p_node);
					Coalesce::Header* p_node_header = (Coalesce::Header*)p_node->mp_Mem;
#if !defined(VMEM_DISABLE_BIASING) && !defined(VMEM_COALESCE_HEAP_BUCKET_TREE)
					if(p_node->m_Size == prev_size || i == g_FreeNodeMapSize-1)
						VMEM_ASSERT_MEM(p_node->mp_Mem > p_prev_addr, &p_node->mp_Mem);		// Coalesce heap free node map list out of order
					else
						VMEM_ASSERT_MEM(p_node->m_Size > prev_size, &p_node->m_Size);		// Coalesce heap free node map list out of order
#endif
					// check memory
					CheckIntegrity(p_node_header);

					// remember sizes
					prev_size = p_node->m_Size;
					p_prev_addr = p_node->mp_Mem;

					VMEM_ASSERT(p_node_header, "Corrupted coalesce node");
					VMEM_ASSERT(p_node->m_Size == p_node_header->m_Size, "coalesce heap block size doens't match node size");
					VMEM_ASSERT_COALESCE_MARKER(&p_node_header->m_Marker);
					VMEM_ASSERT_MEM(p_node_header->mp_Node == p_node, &p_node_header->mp_Node);		// Header doesn't point back to free node

#ifndef VMEM_COALESCE_HEAP_BUCKET_TREE
					VMEM_ASSUME(p_node);
					if(p_node->m_Size > largest_free_node_size)
						largest_free_node_size = p_node->m_Size;
#endif
				}
			}

			VMEM_ASSERT_MEM(p_region->m_Marker[0] == VMEM_COALESCE_ALLOC_MARKER, &p_region->m_Marker[0]);
#ifndef VMEM_COALESCE_HEAP_BUCKET_TREE
			VMEM_ASSERT_MEM(p_region->m_LargestFreeNodeSize == largest_free_node_size || p_region->m_LargestFreeNodeSize == INT_MAX, &p_region->m_LargestFreeNodeSize);
#endif
		}
#endif

#ifdef VMEM_TRAIL_GUARDS
		m_TrailGuard.CheckIntegrity();
#endif
	}

	//------------------------------------------------------------------------
	void CoalesceHeap::CheckIntegrity(Coalesce::Header* p_header) const
	{
		void* p_check_mem = p_header + 1;
		size_t mem_check_size = p_header->m_Size - sizeof(Coalesce::Header);
		int page_size = m_VirtualMem.GetPageSize();
		void* p_start_page = AlignDownPow2((byte*)p_check_mem-1, page_size);
		void* p_end_page = AlignDownPow2((byte*)p_check_mem + mem_check_size, page_size);
		if((byte*)p_end_page - (byte*)p_start_page > page_size)
			mem_check_size = (byte*)p_start_page + page_size - (byte*)p_check_mem;
		mem_check_size = VMem::VMin(mem_check_size, Coalesce::g_MaxCheckMemorySize);

#ifdef VMEM_ENABLE_MEMSET
		CheckMemory(p_check_mem, mem_check_size, VMEM_FREED_MEM);
#endif
	}

	//------------------------------------------------------------------------
	void CoalesceHeap::Flush()
	{
#ifdef VMEM_COALESCE_HEAP_CACHE
		CriticalSectionScope lock(m_CriticalSection);
		FlushCache(0);
#endif
	}

	//------------------------------------------------------------------------
	void CoalesceHeap::Update()
	{
		#ifdef VMEM_COALESCE_HEAP_CACHE
			if(m_CacheEnabled)
				m_Cache.Update();
		#endif

		#ifdef VMEM_SORT_COALESCE_HEAP_ON_SERVICE_THREAD
			CriticalSectionScope lock(m_CriticalSection);
			SortBuckets();
		#endif
	}

	//------------------------------------------------------------------------
#ifdef VMEM_SORT_COALESCE_HEAP_ON_SERVICE_THREAD
	void CoalesceHeap::SortBuckets()
	{
		Coalesce::NodeBucket* VMEM_RESTRICT p_bucket = m_BucketsToSortList.mp_NextNeedsSortBucket;
		Coalesce::NodeBucket* VMEM_RESTRICT p_end_bucket = &m_BucketsToSortList;
		m_BucketsToSortList.mp_PrevNeedsSortBucket = m_BucketsToSortList.mp_NextNeedsSortBucket = &m_BucketsToSortList;

		while(p_bucket != p_end_bucket)
		{
			Coalesce::NodeBucket* p_next_bucket = p_bucket->mp_NextNeedsSortBucket;

			bool swapped = false;
			Coalesce::Node* p_head = &p_bucket->m_Head;
			bool sort_by_address_only = p_bucket->m_SortByAddressOnly;
			for(Coalesce::Node* p_node=p_head->mp_Next; p_node!=p_head && p_node->mp_Next!=p_head; p_node=p_node->mp_Next)
			{
				Coalesce::Node* p_next = p_node->mp_Next;
				if((!sort_by_address_only && p_node->m_Size < p_next->m_Size) || p_node->mp_Mem > p_next->mp_Mem)
				{
					SwapWithNextNode(p_node);
					swapped = true;
				}
			}

			if(swapped)
			{
				AddNodeBucketToSortList(m_BucketsToSortList.mp_NextNeedsSortBucket, p_bucket);
			}
			else
			{
				p_bucket->m_NeedsSort = false;
				p_bucket->mp_PrevNeedsSortBucket = p_bucket->mp_NextNeedsSortBucket = NULL;
			}

			p_bucket = p_next_bucket;
		}
	}
#endif

	//------------------------------------------------------------------------
	#ifdef VMEM_ENABLE_STATS
	size_t CoalesceHeap::WriteAllocs() const
	{
		CriticalSectionScope lock(m_CriticalSection);

		VMem::DebugWrite(_T("CoalesceHeap %d-%d\n"), m_MinSize, m_MaxSize);

		int allocs_written = 0;
		int alloc_count = 0;
		size_t bytes_allocated = 0;

		for(Coalesce::Region* p_region = mp_RegionList; p_region!=NULL; p_region=p_region->mp_Next)
		{
			// check headers
			Coalesce::Header* p_header = (Coalesce::Header*)(p_region + 1) + 1;
			Coalesce::Header* p_last_header = (Coalesce::Header*)((byte*)p_region + m_RegionSize - sizeof(Coalesce::Header));
			while(p_header != p_last_header)
			{
				if(p_header->m_NodeType == nt_Allocated)
				{
					if(allocs_written != VMEM_WRITE_ALLOCS_MAX)
					{
						const void* p_alloc = p_header + 1;
	#ifdef VMEM_COALESCE_GUARDS
						p_alloc = (byte*)p_alloc + VMEM_COALESCE_GUARD_SIZE;
	#endif
						WriteAlloc(p_alloc, p_header->m_Size);
						++allocs_written;
					}
					++alloc_count;
					bytes_allocated += p_header->m_Size - sizeof(Coalesce::Header);
	#ifdef VMEM_COALESCE_GUARDS
					bytes_allocated -= 2*VMEM_COALESCE_GUARD_SIZE;
	#endif
				}

				p_header = (Coalesce::Header*)((byte*)p_header + p_header->m_Size);
			}
		}

		if(allocs_written == VMEM_WRITE_ALLOCS_MAX)
			VMem::DebugWrite(_T("only showing first %d allocs\n"), VMEM_WRITE_ALLOCS_MAX);

		VMem::DebugWrite(_T("%d bytes allocated across %d allocations\n"), bytes_allocated, alloc_count);

		return bytes_allocated;
	}
	#endif

	//------------------------------------------------------------------------
	#ifdef VMEM_ENABLE_STATS
	void CoalesceHeap::SendStatsToMemPro(MemProSendFn send_fn, void* p_context)
	{
		CriticalSectionScope lock(m_CriticalSection);

		MemProStats::CoalesceHeapStats stats;
		stats.m_MinSize = m_MinSize;
		stats.m_MaxSize = m_MaxSize;
		stats.m_RegionSize = m_RegionSize;

		stats.m_RegionCount = 0;
		for(Coalesce::Region* p_region = mp_RegionList; p_region!=NULL; p_region=p_region->mp_Next)
			++stats.m_RegionCount;

		stats.m_Stats = m_Stats;

		SendEnumToMemPro(vmem_CoalesceHeap, send_fn, p_context);
		SendToMemPro(stats, send_fn, p_context);
	}
	#endif

	//------------------------------------------------------------------------
	#ifdef VMEM_COALESCE_GUARD_PAGES
	bool CoalesceHeap::SetupGuardPages(Coalesce::Region* p_region)
	{
		Coalesce::Header* p_free_header = (Coalesce::Header*)(p_region+1) + 1;
		Coalesce::Header* p_last_header = (Coalesce::Header*)((byte*)p_free_header + p_free_header->m_Size);

		int page_size = m_VirtualMem.GetPageSize();

		void* p_prev_committed_page = AlignDownPow2(p_free_header, page_size);
		void* p_last_committed_page = AlignDownPow2(p_last_header, page_size);

		const size_t required_free_size = VMEM_COALESCE_GUARD_PAGES * m_MaxSize + 2* page_size;

		// we put a guard page every # times the max allocation size
		while(p_free_header->m_Size > (int)required_free_size)
		{
			//--------------------------------
			// create the new decommitted page block

			// find the page that we will leave decommitted
			void* p_page = AlignUpPow2((byte*)p_free_header + VMEM_COALESCE_GUARD_PAGES * m_MaxSize, page_size);
			VMEM_ASSERT((byte*)p_page + page_size <= (byte*)p_region + m_RegionSize, "Possible corrupted RegionSize. Guard page out of range");

			// get the guard header
			int guard_size = sizeof(Coalesce::Header) + page_size;
			Coalesce::Header* p_guard_header = (Coalesce::Header*)((byte*)p_page - sizeof(Coalesce::Header));

			// create a new node
			Coalesce::Node* p_node = AllocNode();
			if(!p_node)
				return false;

			// commit the page for the guard header.
			void* p_commit_page = AlignDownPow2(p_guard_header, page_size);
			bool commit_page_for_guard_header = p_commit_page != p_prev_committed_page;
			if(commit_page_for_guard_header)
			{
				if(!m_VirtualMem.Commit(p_commit_page, page_size, m_CommitFlags))
				{
					FreeNode(p_node);
					return false;		// out of memory
				}
				VMEM_STATS(m_Stats.m_Unused += page_size);
				VMEM_MEMSET(p_commit_page, VMEM_FREED_MEM, page_size);
				p_prev_committed_page = p_commit_page;
			}

			// commit the page that contains the next free header
			Coalesce::Header* p_next_free_header = (Coalesce::Header*)((byte*)p_guard_header + guard_size);
			void* p_next_free_commit_page = AlignDownPow2(p_next_free_header, page_size);
			if(p_next_free_commit_page != p_last_committed_page)
			{
				if(!m_VirtualMem.Commit(p_next_free_commit_page, page_size, m_CommitFlags))
				{
					FreeNode(p_node);
					if(commit_page_for_guard_header)
					{
						m_VirtualMem.Decommit(p_commit_page, page_size, m_CommitFlags);
						VMEM_STATS(m_Stats.m_Unused -= page_size);
					}

					return false;		// out of memory
				}
				VMEM_MEMSET(p_next_free_commit_page, VMEM_FREED_MEM, page_size);
				VMEM_STATS(m_Stats.m_Unused += page_size);
				p_prev_committed_page = p_next_free_commit_page;
			}

			// reduce the size of the prev free block
			int new_free_size = (int)((byte*)p_guard_header - (byte*)p_free_header);
			p_free_header->m_Size = new_free_size;
			RemoveNode(p_free_header->mp_Node, p_region);
			p_free_header->mp_Node->m_Size = new_free_size;
			InsertNode(p_free_header->mp_Node, p_region);

			// setup the guard header
			SetupHeader(p_guard_header);
			p_guard_header->m_Size = guard_size;
			p_guard_header->m_PrevSize = p_free_header->m_Size;
			p_guard_header->m_NodeType = nt_Fixed;

			//--------------------------------
			// setup the next free block

			int next_free_size = (int)((byte*)p_last_header - (byte*)p_next_free_header);

			// initialise the new node
			if(next_free_size < m_MinSize)
			{
				FreeNode(p_node);
				p_next_free_header->m_NodeType = nt_Fragment;
			}
			else
			{
				p_node->mp_Mem = p_next_free_header;
				p_node->m_Size = next_free_size;
				p_node->mp_Prev = p_node->mp_Next = NULL;
				InsertNode(p_node, p_region);
				p_next_free_header->mp_Node = p_node;
			}

			// setup the header
			SetupHeader(p_next_free_header);
			p_next_free_header->m_Size = next_free_size;
			p_next_free_header->m_PrevSize = guard_size;

			p_last_header->m_PrevSize = next_free_size;

			VMEM_STATS(m_Stats.m_Unused -= 2 * sizeof(Coalesce::Header));
			VMEM_STATS(m_Stats.m_Overhead += 2 * sizeof(Coalesce::Header));

			// move to the next free header
			p_free_header = p_next_free_header;
		}

		return true;
	}
	#endif

	//------------------------------------------------------------------------
	// this function assumes that all other allocs have been removed and only
	// the guard allocs remain. All guard allocs will be removed and the region
	// destroyed.
	#ifdef VMEM_COALESCE_GUARD_PAGES
	void CoalesceHeap::RemoveGuardPages(Coalesce::Region* p_region)
	{
		int page_size = m_VirtualMem.GetPageSize();

		// if the region is too small no guard pages will have been setup and the region will alreddy have been decallocated
		if(m_RegionSize - Coalesce::g_RegionOverhead < VMEM_COALESCE_GUARD_PAGES * m_MaxSize + 2* page_size)
			return;

		Coalesce::Header* p_first_free_header = (Coalesce::Header*)(p_region+1) + 1;
		Coalesce::Header* p_last_header = (Coalesce::Header*)((byte*)p_region + m_RegionSize - sizeof(Coalesce::Header));

		void* p_first_committed_page = AlignDownPow2(p_first_free_header, page_size);

		Coalesce::Header* p_guard_header = (Coalesce::Header*)((byte*)p_first_free_header + p_first_free_header->m_Size);

		while(p_guard_header != p_last_header)
		{
			VMEM_MEM_CHECK(&p_guard_header->m_NodeType, nt_Fixed);		// expected guard node

			// get the next free header
			Coalesce::Header* p_free_header = (Coalesce::Header*)((byte*)p_guard_header + p_guard_header->m_Size);
			VMEM_ASSERT_MEM(p_free_header->m_NodeType > nt_Allocated, &p_free_header->m_NodeType);		// expected free node
			Coalesce::Header* p_next_guard_header = (Coalesce::Header*)((byte*)p_free_header + p_free_header->m_Size);
			VMEM_MEMSET(p_guard_header, VMEM_FREED_MEM, sizeof(Coalesce::Header));

			// delete the free node
			if(p_free_header->m_NodeType != nt_Fragment)
			{
				RemoveNode(p_free_header->mp_Node, p_region);
				FreeNode(p_free_header->mp_Node);
			}
			VMEM_MEMSET(p_free_header, VMEM_FREED_MEM, sizeof(Coalesce::Header));

			// decommit the guard header page
			void* p_guard_header_page = AlignDownPow2(p_guard_header, page_size);
			void* p_free_header_page = AlignDownPow2(p_free_header, page_size);
			if(p_guard_header_page != p_free_header_page && p_guard_header_page != p_first_committed_page)
			{
				m_VirtualMem.Decommit(p_guard_header_page, page_size, m_CommitFlags);
				VMEM_STATS(m_Stats.m_Unused -= page_size);
			}
			VMEM_STATS(m_Stats.m_Unused += sizeof(Coalesce::Header));
			VMEM_STATS(m_Stats.m_Overhead -= sizeof(Coalesce::Header));

			// decommit the free header page
			void* p_next_guard_header_page = AlignDownPow2(p_next_guard_header, page_size);
			if(p_free_header_page != p_next_guard_header_page && p_free_header_page != p_first_committed_page)
			{
				m_VirtualMem.Decommit(p_free_header_page, page_size, m_CommitFlags);
				VMEM_STATS(m_Stats.m_Unused -= page_size);
			}
			VMEM_STATS(m_Stats.m_Unused += sizeof(Coalesce::Header));
			VMEM_STATS(m_Stats.m_Overhead -= sizeof(Coalesce::Header));

			p_guard_header = p_next_guard_header;
		}

		int total_free_size = m_RegionSize - Coalesce::g_RegionOverhead;
		p_first_free_header->m_Size = total_free_size;
		p_first_free_header->mp_Node->m_Size = total_free_size;
#ifndef VMEM_COALESCE_HEAP_BUCKET_TREE
		p_region->m_LargestFreeNodeSize = total_free_size;
#endif
		p_last_header->m_PrevSize = total_free_size;

		CheckIntegrity_NoLock();
	}
	#endif

	//------------------------------------------------------------------------
#ifdef VMEM_COALESCE_HEAP_CACHE
	void CoalesceHeap::FlushCache(size_t max_mem)
	{
		if(m_CacheEnabled)
		{
			void* p_allocs_to_free = NULL;
			m_Cache.Trim(max_mem, p_allocs_to_free);

			void* p_alloc = p_allocs_to_free;
			while(p_alloc)
			{
				void* p_next = *(void**)p_alloc;

				VMEM_STATS(int size = GetSize(p_alloc));
				VMEM_STATS(m_Stats.m_Unused -= size);
				VMEM_STATS(m_Stats.m_Used += size);
				#ifdef VMEM_CUSTOM_ALLOC_INFO
					VMEM_STATS(m_Stats.m_Used -= sizeof(VMemCustomAllocInfo));
					VMEM_STATS(m_Stats.m_Overhead += sizeof(VMemCustomAllocInfo));
				#endif

				FreeInternal(p_alloc);

				p_alloc = p_next;
			}
		}
	}
#endif
}

//------------------------------------------------------------------------
#ifdef VMEM_OS_WIN
	#pragma warning(pop)
#endif

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

