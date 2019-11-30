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

	VMem is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with VMem. If not, see <http://www.gnu.org/licenses/>.

	VMem can only be used in a commercial product if a commercial license has been purchased.
	Please see http://www.puredevsoftware.com/vmem/License.htm.
*/
//------------------------------------------------------------------------
#include "VMem_PCH.hpp"
#include "VMemCore.hpp"
#include "PhysicalHeap.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED

//------------------------------------------------------------------------
#define VMEM_PHYSICAL_HEAP_HEADER_MAGIC_MARKER	0xbeebbeeb
#define VMEM_PHYSICAL_HEAP_ALIGNED_ALLOC_MARKER 0xa1a1a1a1
#define VMEM_PHYSICAL_PAGE_SIZE 4096

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	const size_t g_PhysicalHeapAlignment = 16;

	//------------------------------------------------------------------------
	struct PhysicalAlignHeader
	{
		void* p;
#ifdef VMEM_X64
		int m_Padding[2];
#else
		int m_Padding[3];
#endif
	};

	//------------------------------------------------------------------------
	void PhysicalHeap_DebugWriteMem(size_t size)
	{
		float mb = (float)(size/1024.0/1024.0);
#ifdef VMEM_X64
		DebugWrite(_T("%10lld (%0.1fMB)"), size, mb);
#else
		DebugWrite(_T("%10d (%0.1fMB)"), size, mb);
#endif

		if(mb < 10.0f)			DebugWrite(_T("   "));
		else if(mb < 100.0f)	DebugWrite(_T("  "));
		else if(mb < 1000.0f)	DebugWrite(_T(" "));
	}

	//------------------------------------------------------------------------
	PhysicalHeap::PhysicalHeap(size_t coalesce_heap_region_size, size_t coalesce_heap_max_size, int flags)
	:	mp_RegionList(NULL),
		m_RegionSize(ToInt(coalesce_heap_region_size)),
		m_CoalesceHeapMaxSize(coalesce_heap_max_size)
#ifdef VMEM_PLATFORM_XBOX360
		,m_Flags(flags)
#endif
#if VMEM_DEBUG_LEVEL >= 1
		,m_PhysicalAllocCount(0)
		#ifdef VMEM_PLATFORM_WIN
			,m_PhysicalAllocTotal(0)
		#endif
#endif
	{
#ifndef VMEM_PLATFORM_XBOX360
		VMEM_UNREFERENCED_PARAM(flags);
#endif
		VMEM_STATIC_ASSERT((sizeof(Region) % g_PhysicalHeapAlignment) == 0, "PhysicalHeap Region not aligned correctly");
		VMEM_STATIC_ASSERT((sizeof(Header) % g_PhysicalHeapAlignment) == 0, "PhysicalHeap Header not aligned correctly");
		VMEM_STATIC_ASSERT((sizeof(PhysicalAlignHeader) % g_PhysicalHeapAlignment) == 0, "PhysicalHeap PhysicalAlignHeader not aligned correctly");

		VMEM_STATS(m_PhysicalAllocDirectTotalSize = 0);

		VMEM_ASSERT(m_CoalesceHeapMaxSize < m_RegionSize - sizeof(Region) - 2 * sizeof(Header), "coalesce_heap_max_size must be smaller than region size");
	}

	//------------------------------------------------------------------------
	PhysicalHeap::~PhysicalHeap()
	{
		if(!CoalesceEmpty() || m_PhysicalAllocs.GetCount() || m_AlignedPhysicalAllocs.GetCount())
		{
			VMem::DebugWrite(_T("ERROR: VMem PhysicalHeap Memory Leak"));
			DebugWrite();
			VMEM_ASSERT(false, "VMem PhysicalHeap Memory Leak");
		}

		Trim();

#if VMEM_DEBUG_LEVEL >= 1
		VMEM_ASSERT(m_PhysicalAllocCount == 0, "Internal leak in VMem PhysicalHeap");
		#ifdef VMEM_PLATFORM_WIN
			VMEM_ASSERT(m_PhysicalAllocTotal == 0, "Internal leak in VMem PhysicalHeap");
		#endif
#endif
	}

	//------------------------------------------------------------------------
	PhysicalHeap::Region* PhysicalHeap::CreateRegion()
	{
		// commit the region memory
		// the region struct is at the start of the committed memory
		Region* p_region = (Region*)AllocatePhysical(m_RegionSize);

		VMEM_STATS(m_Stats.m_Reserved += m_RegionSize);
		VMEM_STATS(m_Stats.m_Unused += m_RegionSize - sizeof(Region));
		VMEM_STATS(m_Stats.m_Overhead += sizeof(Region));

		if(!p_region)
			return NULL;		// out of memory

		// initialise the region
		p_region->mp_Next = NULL;

		// initialise the free list header
		Header& free_list_head = p_region->m_FreeList;
		memset(&free_list_head, 0, sizeof(Header));

		// setup first header 'allocated' header. This header is never used
		// but makes the coalesce logic a simpler.
		Header* p_first_header = (Header*)(p_region + 1);
		InitialiseHeader(p_first_header);
		p_first_header->mp_Prev = NULL;				// NULL prev and next pointers mean header is allocated
		p_first_header->mp_Next = NULL;
		p_first_header->m_Size = sizeof(Header);
		p_first_header->m_PrevSize = 0;
		VMEM_STATS(m_Stats.m_Unused -= sizeof(Header));
		VMEM_STATS(m_Stats.m_Overhead += sizeof(Header));

		// setup free header and link it to the first header
		Header* p_header = p_first_header + 1;
		InitialiseHeader(p_header);
		p_header->mp_Prev = &free_list_head;		// circular linked list
		p_header->mp_Next = &free_list_head;
		int total_free_size = ToInt(m_RegionSize - sizeof(Region) - 2 * sizeof(Header));
		p_header->m_Size = total_free_size;			// includes size of the header
		p_header->m_PrevSize = sizeof(Header);
		free_list_head.mp_Prev = p_header;
		free_list_head.mp_Next = p_header;
		VMEM_STATS(m_Stats.m_Unused -= sizeof(Header));
		VMEM_STATS(m_Stats.m_Overhead += sizeof(Header));

		// setup last header guard
		Header* p_last_header = (Header*)((byte*)p_header + p_header->m_Size);
		InitialiseHeader(p_last_header);
		p_last_header->mp_Prev = NULL;				// NULL prev and next pointers mean header is allocated
		p_last_header->mp_Next = NULL;
		p_last_header->m_Size = sizeof(Header);
		p_last_header->m_PrevSize = total_free_size;
		VMEM_STATS(m_Stats.m_Unused -= sizeof(Header));
		VMEM_STATS(m_Stats.m_Overhead += sizeof(Header));

		return p_region;
	}

	//------------------------------------------------------------------------
	// WARNING: this deletes p_region, don't use p_region after calling this function.
	// This function calls the standard Free function on each alloc in the region until
	// the region disappears. It's slightly messy, but avoids duplicating the code in
	// the Free function.
	void PhysicalHeap::ClearRegion(Region* p_region)
	{
		// get the first alloc that we can free
		Header* p_header = (Header*)(p_region + 1) + 1;

		// first we work out how many bytes have been allocated so that we
		// know when we are done. This is needed because the last Free call
		// will delete the region and all headers, so we need a simple way
		// of telling when this happens.
		int size = 0;
		Header* p_end_header = (Header*)((byte*)p_region + m_RegionSize) - 1;
		for(Header* p_h = p_header; p_h != p_end_header; p_h=(Header*)((byte*)p_h + p_h->m_Size))
		{
			VMEM_ASSERT_CODE(CheckHeader(p_h));

			if(!p_h->mp_Prev)
				size += p_h->m_Size;
		}

		// now keep deleting the first allocated block in the list until all
		// allocations are freed (which frees the region)
		while(size)
		{
			Header* p_alloc_header = p_header;

			// if p_header is free the next header is garanteed to be allocated
			if(p_alloc_header->mp_Prev)
				p_alloc_header = (Header*)((byte*)p_alloc_header + p_alloc_header->m_Size);
			VMEM_ASSERT(!p_alloc_header->mp_Prev, "expected block to be allocated");

			size -= p_alloc_header->m_Size;
			VMEM_ASSERT(size >= 0, "size negative");

			// warning: this deletes p_region when it's empty, so be careful not to access p_region after last free
			void* p = p_alloc_header + 1;
			Free(p);
		}
	}

	//------------------------------------------------------------------------
	// releases the memory associated with the region. Do not use p_region after this.
	void PhysicalHeap::DestroyRegion(Region* p_region)
	{
		// check it's empty
		#if VMEM_DEBUG_LEVEL >= 1
			int total_free_size = m_RegionSize - sizeof(Region) - 2 * sizeof(Header);
			Header* p_header = (Header*)(p_region + 1) + 1;
			VMEM_ASSERT(p_header->m_Size == total_free_size, "Destroying non-empty region");
		#endif

		VMEM_STATS(m_Stats.m_Unused -= m_RegionSize - sizeof(Region) - 3*sizeof(Header));
		VMEM_STATS(m_Stats.m_Overhead -= sizeof(Region) + 3*sizeof(Header));
		VMEM_STATS(m_Stats.m_Reserved -= m_RegionSize);

		// release memory
		PhysicalFree(p_region);
	}

	//------------------------------------------------------------------------
	void* PhysicalHeap::AllocAligned(size_t size, size_t alignment)
	{
		CriticalSectionScope lock(m_CriticalSection);

		size_t aligned_size = AlignUp(size + sizeof(PhysicalAlignHeader), g_PhysicalHeapAlignment);

		if(aligned_size >= m_CoalesceHeapMaxSize && (alignment == 0 || (VMEM_PHYSICAL_PAGE_SIZE % alignment) == 0))
		{
			void* p = PhysicalAllocDirect(size);

			if(!p || !TrackPhysicalAlloc(p, size))
				return NULL;

			return p;
		}
		else
		{
			if(alignment != g_PhysicalHeapAlignment)
				aligned_size += alignment;

			if(aligned_size < m_CoalesceHeapMaxSize)
			{
				void* p = CoalesceAlloc(aligned_size);
				if(!p)
					return NULL;		// out of memory

				void* aligned_p = AlignUp((byte*)p + sizeof(PhysicalAlignHeader), alignment);

				PhysicalAlignHeader* p_header = (PhysicalAlignHeader*)aligned_p - 1;
				InitialiseAlignmentHeader(p_header);
				p_header->p = p;
				return aligned_p;
			}
			else
			{
				void* p = PhysicalAllocDirectAligned(aligned_size);
				if(!p)
					return NULL;		// out of memory

				void* aligned_p = AlignUp((byte*)p + sizeof(PhysicalAlignHeader), alignment);

				PhysicalAlignHeader* p_header = (PhysicalAlignHeader*)aligned_p - 1;
				InitialiseAlignmentHeader(p_header);
				p_header->p = p;

				if(!TrackPhysicalAllocAligned(aligned_p, size))
					return NULL;		// out of memory

				return aligned_p;
			}
		}
	}

	//------------------------------------------------------------------------
	void* PhysicalHeap::Alloc(size_t size)
	{
		CriticalSectionScope lock(m_CriticalSection);

		if(size < m_CoalesceHeapMaxSize)
		{
			return CoalesceAlloc(size);
		}
		else
		{
			void* p = PhysicalAllocDirect(size);

			if(!p || !TrackPhysicalAlloc(p, size))
				return NULL;

			return p;
		}
	}

	//------------------------------------------------------------------------
	void* PhysicalHeap::CoalesceAlloc(size_t size)
	{
		VMEM_ASSERT(size < m_CoalesceHeapMaxSize, "invalid size passed in to PhysicalHeap::CoalesceAlloc");

		int aligned_size = AlignUp(ToInt(size), ToInt(g_PhysicalHeapAlignment));

		VMEM_STATIC_ASSERT((sizeof(Header) & (g_PhysicalHeapAlignment-1)) == 0, "PhysicalHeap sizeof(Header) not aligned correctly");

		// work out the size of the block
		int total_size = aligned_size + sizeof(Header);

		#if VMEM_DEBUG_LEVEL >= 1
			int total_free_size = m_RegionSize - sizeof(Region) - 2 * sizeof(Header);
			VMEM_ASSERT(total_size < total_free_size, "allocation bigger than region size");
		#endif

		// get the first region (or create one)
		Region* p_region = mp_RegionList;
		if(!p_region)
			p_region = mp_RegionList = CreateRegion();

		// linear search through the regions for a big enough free block
		while(p_region)
		{
			Header* p_list_head = &p_region->m_FreeList;
		
			// linear search through the free block list for a big enough block
			for(Header* p_header=p_list_head->mp_Next; p_header != p_list_head; p_header=p_header->mp_Next)
			{
				VMEM_ASSERT_CODE(CheckHeader(p_header));
				VMEM_ASSERT(p_header->mp_Prev, "header already allocated");

				// work out what the offcut would be
				int offcut_size = p_header->m_Size - total_size;

				// if we have found an allocation big enough
				if(offcut_size >= 0)
				{
					Header* p_prev_free_header = p_header->mp_Prev;
					Header* p_next_free_header = p_header->mp_Next;

					// ignore offcut if too small
					if(offcut_size < sizeof(Header))
					{
						total_size += offcut_size;
						offcut_size = 0;
					}

					// get the next header
					Header* p_next_header = (Header*)((byte*)p_header + p_header->m_Size);
					VMEM_ASSERT_CODE(CheckHeader(p_next_header));
					if(offcut_size)
					{
						// create a new header for the offcut
						Header* p_offcut_header = (Header*)((byte*)p_header + total_size);
						VMEM_ASSERT_CODE(InitialiseHeader(p_offcut_header));
						p_offcut_header->m_Size = offcut_size;
						p_offcut_header->m_PrevSize = total_size;

						// link new offcut header into free list
						p_prev_free_header->mp_Next = p_offcut_header;
						p_offcut_header->mp_Prev = p_prev_free_header;
						p_offcut_header->mp_Next = p_next_free_header;
						p_next_free_header->mp_Prev = p_offcut_header;

						// update the prev size of the next header in memory
						p_next_header->m_PrevSize = offcut_size;

						VMEM_STATS(m_Stats.m_Unused -= sizeof(Header));
						VMEM_STATS(m_Stats.m_Overhead += sizeof(Header));
					}
					else
					{
						// no offcut, so remove the original header from the free list
						p_prev_free_header->mp_Next = p_next_free_header;
						p_next_free_header->mp_Prev = p_prev_free_header;

						// update the prev size of the next header in memory
						p_next_header->m_PrevSize = total_size;
					}

					// setup the header for this alloc
					p_header->mp_Prev = NULL;		// NULL prev and next pointers mean header is allocated
					p_header->mp_Next = NULL;
					p_header->m_Size = total_size;

					void* p = p_header + 1;
					memset(p, VMEM_ALLOCATED_MEM, aligned_size);

					VMEM_STATS(m_Stats.m_Unused -= aligned_size);
					VMEM_STATS(m_Stats.m_Used += aligned_size);

					return p;
				}
			}

			// region full, try the next one
			if(!p_region->mp_Next)
				p_region->mp_Next = CreateRegion();
			p_region = p_region->mp_Next;
		}

		return NULL;	// out of memory
	}

	//------------------------------------------------------------------------
	bool PhysicalHeap::FreeAligned(void* p)
	{
		CriticalSectionScope lock(m_CriticalSection);

		if(CoalesceOwns(p))
		{
			PhysicalAlignHeader* p_header = (PhysicalAlignHeader*)p - 1;
			CheckAlignmentHeader(p_header);

			return CoalesceFree(p_header->p);
		}
		else
		{
			return PhysicalFreeDirectAligned(p);
		}
	}

	//------------------------------------------------------------------------
	bool PhysicalHeap::Free(void* p)
	{
		CriticalSectionScope lock(m_CriticalSection);

		VMEM_ASSERT(!m_AlignedPhysicalAllocs.Contains(p), "Free call on aligned pointer. Use FreeAligned.");

		return CoalesceFree(p) || PhysicalFreeDirect(p);
	}

	//------------------------------------------------------------------------
	bool PhysicalHeap::CoalesceFree(void* p)
	{
		// find the region that contains this alloc
		Region* p_region = mp_RegionList;
		while(p_region && (p < p_region || p >= (byte*)p_region + m_RegionSize))
			p_region = p_region->mp_Next;
		
		if(!p_region)
			return false;

		// check the header
		Header* p_header = (Header*)p - 1;
		VMEM_ASSERT_CODE(CheckHeader(p_header));
		VMEM_ASSERT(!p_header->mp_Prev, "allocation already freed");
		VMEM_ASSERT(!p_header->mp_Next, "header already on free list");

		int size = p_header->m_Size;

		// clear the memory
		memset(p, VMEM_FREED_MEM, size - sizeof(Header));

		// get the prev and next headers
		VMEM_ASSERT(p_header->m_PrevSize, "p_header->m_PrevSize is 0");
		VMEM_ASSERT(size, "size is 0");
		Header* p_prev_header = (Header*)((byte*)p_header - p_header->m_PrevSize);
		Header* p_next_header = (Header*)((byte*)p_header + size);
		int merged_size = size;

		//----------------------------
		// merge with prev header if it's free (mp_Prev!=NULL means free)
		if(p_prev_header && p_prev_header->mp_Prev)
		{
			VMEM_ASSERT_CODE(CheckHeader(p_prev_header));

			// merge with prev header
			merged_size += p_prev_header->m_Size;

			p_prev_header->m_Size = merged_size;
			p_next_header->m_PrevSize = merged_size;

			memset(p_header, VMEM_FREED_MEM, sizeof(Header));

			p_header = p_prev_header;

			// remove from free list
			Unlink(p_header);

			VMEM_STATS(m_Stats.m_Overhead -= sizeof(Header));
			VMEM_STATS(m_Stats.m_Unused += sizeof(Header));
		}

		//----------------------------
		// merge with next header if it's free (mp_Prev!=NULL means free)
		if(p_next_header->mp_Prev)
		{
			VMEM_ASSERT_CODE(CheckHeader(p_next_header));

			merged_size += p_next_header->m_Size;

			Header* p_next_next_header = (Header*)((byte*)p_next_header + p_next_header->m_Size);
			if(p_next_next_header != (Header*)((byte*)p_region + m_RegionSize))
			{
				VMEM_ASSERT_CODE(CheckHeader(p_next_next_header));
				p_next_next_header->m_PrevSize = merged_size;
			}

			p_header->m_Size = merged_size;

			// remove next header from free list
			Unlink(p_next_header);

			memset(p_next_header, VMEM_FREED_MEM, sizeof(Header));

			VMEM_STATS(m_Stats.m_Overhead -= sizeof(Header));
			VMEM_STATS(m_Stats.m_Unused += sizeof(Header));
		}

		//----------------------------
		// add header to free list
		Header* p_prev = &p_region->m_FreeList;
		Header* p_next = p_prev->mp_Next;
		p_prev->mp_Next = p_header;
		p_header->mp_Prev = p_prev;
		p_header->mp_Next = p_next;
		p_next->mp_Prev = p_header;

		//----------------------------
		// remove the region if it's empty
		int total_free_size = m_RegionSize - sizeof(Region) - 2 * sizeof(Header);
		if(merged_size == total_free_size && !(p_region == mp_RegionList && !p_region->mp_Next))
		{
			UnlinkRegion(p_region);
			DestroyRegion(p_region);
		}

		VMEM_STATS(m_Stats.m_Used -= size);
		VMEM_STATS(m_Stats.m_Unused += size);

		return true;
	}

	//------------------------------------------------------------------------
	size_t PhysicalHeap::GetSizeAligned(void* p)
	{
		CriticalSectionScope lock(m_CriticalSection);

		if(CoalesceOwns(p))
		{
			PhysicalAlignHeader* p_header = (PhysicalAlignHeader*)p - 1;
			void* p_alloc = p_header->p;
			size_t aligned_size = GetSize_NoLock(p_alloc);
			return aligned_size - ((byte*)p - (byte*)p_alloc);
		}
		else
		{
			size_t size = m_AlignedPhysicalAllocs.GetSize(p);

			// if the size was >= m_CoalesceHeapMaxSize and the alignment was 16 or
			// VMEM_PHYSICAL_PAGE_SIZE then it will have been allocated directly without an
			// alignment header
			if(size == VMEM_INVALID_SIZE)
				size = m_PhysicalAllocs.GetSize(p);

			return size;
		}
	}

	//------------------------------------------------------------------------
	size_t PhysicalHeap::GetSize(void* p)
	{
		CriticalSectionScope lock(m_CriticalSection);

		return GetSize_NoLock(p);
	}

	//------------------------------------------------------------------------
	size_t PhysicalHeap::GetSize_NoLock(void* p)
	{
		if (CoalesceOwns(p))
		{
			Header* p_header = (Header*)p - 1;
			return p_header->m_Size - sizeof(Header);
		}
		else
		{
			return m_PhysicalAllocs.GetSize(p);
		}
	}

	//------------------------------------------------------------------------
	bool PhysicalHeap::Owns(void* p)
	{
		CriticalSectionScope lock(m_CriticalSection);

		return CoalesceOwns(p) || m_PhysicalAllocs.Contains(p) || m_AlignedPhysicalAllocs.Contains(p);
	}

	//------------------------------------------------------------------------
	bool PhysicalHeap::CoalesceOwns(void* p) const
	{
		// find the region that contains this alloc
		Region* p_region = mp_RegionList;
		while(p_region && (p < p_region || p >= (byte*)p_region + m_RegionSize))
			p_region = p_region->mp_Next;

		return p_region ? true : false;
	}

	//------------------------------------------------------------------------
	void PhysicalHeap::UnlinkRegion(Region* p_region)
	{
		Region* p_prev_region = NULL;
		for(Region* p_srch_region = mp_RegionList; p_srch_region!=p_region; p_srch_region=p_srch_region->mp_Next)
			p_prev_region = p_srch_region;

		Region* p_next_region = p_region->mp_Next;
		if(p_prev_region)
			p_prev_region->mp_Next = p_next_region;
		else
			mp_RegionList = p_next_region;
	}

	//------------------------------------------------------------------------
	void PhysicalHeap::InitialiseHeader(Header* p_header)
	{
#if VMEM_DEBUG_LEVEL >= 1
		int marker_count = sizeof(p_header->m_Marker)/sizeof(unsigned int);
		for(int i=0; i<marker_count; ++i)
			p_header->m_Marker[i] = VMEM_PHYSICAL_HEAP_HEADER_MAGIC_MARKER;
#else
		VMEM_UNREFERENCED_PARAM(p_header);
#endif
	}

	//------------------------------------------------------------------------
	void PhysicalHeap::CheckHeader(Header* p_header)
	{
#if VMEM_DEBUG_LEVEL >= 1
		int marker_count = sizeof(p_header->m_Marker)/sizeof(unsigned int);
		VMEM_ASSERT(*(int*)&p_header->mp_Next != VMEM_PHYSICAL_HEAP_ALIGNED_ALLOC_MARKER, "This looks like an allocation allocated with AllocAligned. Please free with FreeAligned");
		for(int i=0; i<marker_count; ++i)
			VMEM_ASSERT(p_header->m_Marker[i] == VMEM_PHYSICAL_HEAP_HEADER_MAGIC_MARKER, "header is corrupt");
#else
		VMEM_UNREFERENCED_PARAM(p_header);
#endif
	}

	//------------------------------------------------------------------------
	void PhysicalHeap::Unlink(Header* p_header)
	{
		// prev and next pointers always guaranteed to be valid because of the fixed start and end headers
		Header* p_prev = p_header->mp_Prev;
		Header* p_next = p_header->mp_Next;
		p_prev->mp_Next = p_next;
		p_next->mp_Prev = p_prev;
	}

	//------------------------------------------------------------------------
	void PhysicalHeap::CheckIntegrity()
	{
#if VMEM_DEBUG_LEVEL >= 1
		CriticalSectionScope lock(m_CriticalSection);

		for(Region* p_region = mp_RegionList; p_region!=NULL; p_region=p_region->mp_Next)
		{
			int total_size = sizeof(Region);

			//----------------------------
			// check all headers
			Header* p_last_header = (Header*)((byte*)p_region + m_RegionSize);
			Header* p_header = (Header*)(p_region + 1);
			bool last_free = false;
			int prev_size = 0;
			while(p_header != p_last_header)
			{
				// check marker
				CheckHeader(p_header);

				int size = p_header->m_Size;
				total_size += size;

				// check prev size
				VMEM_ASSERT(p_header->m_PrevSize == prev_size, "header has incorrect prev size");
				prev_size = size;

				// check no two consecutive free blocks
				bool free = p_header->mp_Prev != NULL;
				VMEM_ASSERT(!(free && last_free), "two consecutive free blocks, they should have been coalesced");
				last_free = free;

				p_header = (Header*)((byte*)p_header + p_header->m_Size);
			}

			VMEM_ASSERT(total_size == m_RegionSize, "blocks don't add up to region size");

			//----------------------------
			// check free list
			Header* p_head = &p_region->m_FreeList;
			Header* p_prev_header = p_head;
			for(Header* p_check_header = p_head->mp_Next; p_check_header!=p_head; p_check_header=p_check_header->mp_Next)
			{
				// check marker
				CheckHeader(p_check_header);

				// check prev header pointer
				VMEM_ASSERT(p_check_header->mp_Prev == p_prev_header, "prev header pointer incorrect");
				p_prev_header = p_check_header;
			}
		}

		// check stats
		#ifdef VMEM_ENABLE_STATS
			int region_count = 0;
			for(Region* p_region = mp_RegionList; p_region!=NULL; p_region=p_region->mp_Next)
				++region_count;
			
			size_t total_mem =
				region_count * m_RegionSize +
				m_PhysicalAllocs.GetMemorySize() +
				m_AlignedPhysicalAllocs.GetMemorySize() +
				m_PhysicalAllocDirectTotalSize;

			VMEM_ASSERT(m_Stats.GetCommittedBytes() == total_mem, "VMem PhysicalHeap Stats incorrect");
			VMEM_ASSERT(m_Stats.m_Reserved == total_mem, "VMem PhysicalHeap Stats incorrect");
		#endif
#endif
	}

	//------------------------------------------------------------------------
	void PhysicalHeap::DebugWrite(Header* p_header)
	{
		int size = p_header->m_Size;
		void* p_start = p_header;
		void* p_end = (byte*)p_header + size;

		VMem::DebugWrite(_T("0x%08x - 0x%08x\t%7d\t%s\n"), p_start, p_end, size, p_header->mp_Prev ? _T("free") : _T("alloc"));
	}

	//------------------------------------------------------------------------
	void PhysicalHeap::DebugWrite() const
	{
		CriticalSectionScope lock(m_CriticalSection);

		for(Region* p_region = mp_RegionList; p_region!=NULL; p_region=p_region->mp_Next)
		{
			VMem::DebugWrite(_T("--------------------------------\n"), p_region);
			VMem::DebugWrite(_T("Region 0x%08x\n"), p_region);

			// write all headers
			Header* p_last_header = (Header*)((byte*)p_region + m_RegionSize);
			Header* p_header = (Header*)(p_region + 1);
			while(p_header != p_last_header)
			{
				DebugWrite(p_header);
				p_header = (Header*)((byte*)p_header + p_header->m_Size);
			}

			// write free list
			VMem::DebugWrite(_T("\nFree list\n"));

			Header* p_list_head = &p_region->m_FreeList;
			for(Header* p_write_header=p_list_head->mp_Next; p_write_header!=p_list_head; p_write_header=p_write_header->mp_Next)
			{
				DebugWrite(p_write_header);
			}
		}

		VMem::DebugWrite(_T("\n-------------------\nPhysical Allocs:\n"));
		DebugWrite(m_PhysicalAllocs);

		VMem::DebugWrite(_T("\n-------------------\nAligned Physical Allocs:\n"));
		DebugWrite(m_AlignedPhysicalAllocs);
	}

	//------------------------------------------------------------------------
	void PhysicalHeap::DebugWrite(const PhysicalAllocArray& array)
	{
		int physical_alloc_count = array.GetCount();
		for(int i=0; i<physical_alloc_count; ++i)
		{
			PhysicalAllocInfo alloc = array[i];
			VMem::DebugWrite(_T("0x%p : size %lld\n"), alloc.mp_Mem, (long long)alloc.m_Size);
		}
	}

	//------------------------------------------------------------------------
	void PhysicalHeap::Trim()
	{
		CriticalSectionScope lock(m_CriticalSection);

		Region* p_region = mp_RegionList;
		int total_free_size = m_RegionSize - sizeof(Region) - 2 * sizeof(Header);

		// if there is only 1 region left and it's empty, destroy it
		if (p_region && !p_region->mp_Next && p_region->m_FreeList.mp_Next->m_Size == total_free_size)
		{
			UnlinkRegion(p_region);
			DestroyRegion(p_region);
		}
	}

	//------------------------------------------------------------------------
	void* PhysicalHeap::AllocatePhysical(size_t size)
	{
		size_t aligned_size = AlignUp(size, (size_t)VMEM_PHYSICAL_PAGE_SIZE);

#if VMEM_DEBUG_LEVEL >= 1
		++m_PhysicalAllocCount;
		#ifdef VMEM_PLATFORM_WIN
			m_PhysicalAllocTotal += aligned_size;
		#endif
#endif

#ifdef VMEM_PLATFORM_XBOX360
		return XPhysicalAlloc(size, MAXULONG_PTR, 0, m_Flags);
#else
		return VirtualAlloc(NULL, aligned_size, MEM_COMMIT, PAGE_READWRITE);
#endif
	}

	//------------------------------------------------------------------------
	void PhysicalHeap::PhysicalFree(void* p)
	{
#if VMEM_DEBUG_LEVEL >= 1
		--m_PhysicalAllocCount;
		#ifdef VMEM_PLATFORM_WIN
			MEMORY_BASIC_INFORMATION info;
			VirtualQuery(p, &info, sizeof(info));
			VMEM_ASSERT(m_PhysicalAllocTotal >= info.RegionSize, "Internal VMem error");
			m_PhysicalAllocTotal -= info.RegionSize;
		#endif
#endif

#ifdef VMEM_PLATFORM_XBOX360
		XPhysicalFree(p);
#else
		VirtualFree(p, 0, MEM_RELEASE);
#endif
	}

	//------------------------------------------------------------------------
	void* PhysicalHeap::IntrnalAlloc(size_t size)
	{
		return new char[size];
	}

	//------------------------------------------------------------------------
	void PhysicalHeap::InternalFree(void* p)
	{
		char* pc = (char*)p;
		delete[] pc;
	}

	//------------------------------------------------------------------------
	PhysicalHeap::PhysicalAllocArray::PhysicalAllocArray()
	:	mp_Array(NULL),
		m_Capacity(0),
		m_Count(0)
	{
	}

	//------------------------------------------------------------------------
	PhysicalHeap::PhysicalAllocArray::~PhysicalAllocArray()
	{
		PhysicalHeap::InternalFree(mp_Array);
	}

	//------------------------------------------------------------------------
	bool PhysicalHeap::PhysicalAllocArray::Grow()
	{
		int new_capacity = m_Capacity ? 2 * m_Capacity : 1024;
		PhysicalAllocInfo* p_new_array = (PhysicalAllocInfo*)PhysicalHeap::IntrnalAlloc(new_capacity * sizeof(PhysicalAllocInfo));
		if(!p_new_array)
			return false;

		if(m_Count)
		{
			memcpy(p_new_array, mp_Array, m_Count*sizeof(PhysicalAllocInfo));
			PhysicalHeap::InternalFree(mp_Array);
		}
		mp_Array = p_new_array;
		m_Capacity = new_capacity;

		return true;
	}

	//------------------------------------------------------------------------
	bool PhysicalHeap::PhysicalAllocArray::Add(void* p, size_t size)
	{
		if(m_Capacity == m_Count)
			if(!Grow())
				return false;

		PhysicalAllocInfo alloc;
		alloc.mp_Mem = p;
		alloc.m_Size = size;

		mp_Array[m_Count++] = alloc;
		return true;
	}

	//------------------------------------------------------------------------
	// it is expected that there are no more than a few hundred pointers in the
	// array, so a cache friendly linear search should suffice.
	bool PhysicalHeap::PhysicalAllocArray::Remove(void* p, size_t& size)
	{
		int count = m_Count;
		for(int i=0; i<count; ++i)
		{
			if(mp_Array[i].mp_Mem == p)
			{
				size = mp_Array[i].m_Size;
				mp_Array[i] = mp_Array[--m_Count];
				return true;
			}
		}

		return false;
	}

	//------------------------------------------------------------------------
	size_t PhysicalHeap::PhysicalAllocArray::GetSize(void* p)
	{
		int count = m_Count;
		for(int i=0; i<count; ++i)
			if(mp_Array[i].mp_Mem == p)
				return mp_Array[i].m_Size;

		return VMEM_INVALID_SIZE;
	}

	//------------------------------------------------------------------------
	bool PhysicalHeap::PhysicalAllocArray::Contains(void* p)
	{
		int count = m_Count;
		for(int i=0; i<count; ++i)
		{
			if(p == mp_Array[i].mp_Mem)
				return true;
		}

		return false;
	}

	//------------------------------------------------------------------------
	void* PhysicalHeap::PhysicalAllocDirect(size_t size)
	{
		VMEM_STATS(m_Stats.m_Reserved += size);
		VMEM_STATS(m_Stats.m_Used += size);
		VMEM_STATS(m_PhysicalAllocDirectTotalSize += size);

		return AllocatePhysical(size);
	}

	//------------------------------------------------------------------------
	void* PhysicalHeap::PhysicalAllocDirectAligned(size_t size)
	{
		VMEM_STATS(m_Stats.m_Reserved += size);
		VMEM_STATS(m_Stats.m_Used += size);
		VMEM_STATS(m_PhysicalAllocDirectTotalSize += size);

		return AllocatePhysical(size);
	}

	//------------------------------------------------------------------------
	bool PhysicalHeap::PhysicalFreeDirect(void* p)
	{
		size_t size = 0;
		if(m_PhysicalAllocs.Remove(p, size))
		{
			VMEM_STATS(m_Stats.m_Reserved -= size);
			VMEM_STATS(m_Stats.m_Used -= size);
			VMEM_STATS(m_PhysicalAllocDirectTotalSize -= size);

			PhysicalFree(p);
			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------
	bool PhysicalHeap::PhysicalFreeDirectAligned(void* p)
	{
		size_t size = 0;
		if(m_AlignedPhysicalAllocs.Remove(p, size))
		{
			VMEM_STATS(m_Stats.m_Reserved -= size);
			VMEM_STATS(m_Stats.m_Used -= size);
			VMEM_STATS(m_PhysicalAllocDirectTotalSize -= size);

			PhysicalAlignHeader* p_header = (PhysicalAlignHeader*)p - 1;

			PhysicalFree(p_header->p);
			return true;
		}
		else
		{
			return PhysicalFreeDirect(p);
		}
	}

	//------------------------------------------------------------------------
	void PhysicalHeap::WriteStats()
	{
#ifdef VMEM_ENABLE_STATS
		CriticalSectionScope lock(m_CriticalSection);

		VMem::DebugWrite(_T("                       Used                 Unused               Overhead                  Total               Reserved\n"));
		VMem::DebugWrite(_T(" Physical:  "));	

		size_t committed_bytes = m_Stats.GetCommittedBytes();
		int usage_percent = committed_bytes ? (int)((100 * (long long)m_Stats.m_Used) / committed_bytes) : 0;
		VMem::DebugWrite(_T("%3d%% "), usage_percent);

		PhysicalHeap_DebugWriteMem(m_Stats.m_Used);
		VMem::DebugWrite(_T("  "));
		PhysicalHeap_DebugWriteMem(m_Stats.m_Unused);
		VMem::DebugWrite(_T("  "));
		PhysicalHeap_DebugWriteMem(m_Stats.m_Overhead);
		VMem::DebugWrite(_T("  "));
		PhysicalHeap_DebugWriteMem(committed_bytes);
		VMem::DebugWrite(_T("  "));
		PhysicalHeap_DebugWriteMem(m_Stats.m_Reserved);
		VMem::DebugWrite(_T("\n"));
#endif
	}

	//------------------------------------------------------------------------
	bool PhysicalHeap::TrackPhysicalAlloc(void* p, size_t size)
	{
		VMEM_STATS(m_Stats.m_Reserved -= m_PhysicalAllocs.GetMemorySize());
		VMEM_STATS(m_Stats.m_Overhead -= m_PhysicalAllocs.GetMemorySize());

		if(!m_PhysicalAllocs.Add(p, size))
			return false;		// out of memory

		VMEM_STATS(m_Stats.m_Reserved += m_PhysicalAllocs.GetMemorySize());
		VMEM_STATS(m_Stats.m_Overhead += m_PhysicalAllocs.GetMemorySize());

		return true;
	}

	//------------------------------------------------------------------------
	bool PhysicalHeap::TrackPhysicalAllocAligned(void* p, size_t size)
	{
		VMEM_STATS(m_Stats.m_Reserved -= m_AlignedPhysicalAllocs.GetMemorySize());
		VMEM_STATS(m_Stats.m_Overhead -= m_AlignedPhysicalAllocs.GetMemorySize());

		if(!m_AlignedPhysicalAllocs.Add(p, size))
			return false;		// out of memory

		VMEM_STATS(m_Stats.m_Reserved += m_AlignedPhysicalAllocs.GetMemorySize());
		VMEM_STATS(m_Stats.m_Overhead += m_AlignedPhysicalAllocs.GetMemorySize());

		return true;
	}

	//------------------------------------------------------------------------
	bool PhysicalHeap::CoalesceEmpty() const
	{
		return
			!mp_RegionList ||
			mp_RegionList->m_FreeList.mp_Next->m_Size == (int)(m_RegionSize - sizeof(Region) - 2 * sizeof(Header));
	}

	//------------------------------------------------------------------------
	void PhysicalHeap::InitialiseAlignmentHeader(PhysicalAlignHeader* p_header)
	{
#if VMEM_DEBUG_LEVEL >= 1
		for(int i=0; i<sizeof(p_header->m_Padding)/sizeof(int); ++i)
			p_header->m_Padding[i] = VMEM_PHYSICAL_HEAP_ALIGNED_ALLOC_MARKER;
#else
		VMEM_UNREFERENCED_PARAM(p_header);
#endif
	}

	//------------------------------------------------------------------------
	void PhysicalHeap::CheckAlignmentHeader(PhysicalAlignHeader* p_header)
	{
#if VMEM_DEBUG_LEVEL >= 1
		for(int i=0; i<sizeof(p_header->m_Padding)/sizeof(int); ++i)
			VMEM_ASSERT(p_header->m_Padding[i] == VMEM_PHYSICAL_HEAP_ALIGNED_ALLOC_MARKER, "Corrupt memory on physical aligned alloc, or alloc not allocated with AllocAligned");
#else
		VMEM_UNREFERENCED_PARAM(p_header);
#endif
	}
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_PHYSICAL_HEAP_SUPPORTED

