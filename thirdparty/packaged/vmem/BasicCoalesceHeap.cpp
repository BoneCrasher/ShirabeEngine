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
#include "BasicCoalesceHeap.hpp"
#include "VirtualMem.hpp"
#include "VMemMemProStats.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	BasicCoalesceHeap::BasicCoalesceHeap(int region_size, VirtualMem& virtual_mem)
	:	mp_RegionList(NULL),
		m_RegionSize(AlignUpPow2(region_size, virtual_mem.GetPageSize())),
		m_VirtualMem(virtual_mem)
	{
	}

	//------------------------------------------------------------------------
	BasicCoalesceHeap::~BasicCoalesceHeap()
	{
		// ideally the region will be empty when the coalesce heap is destructed
		// but we will clean up properly anyway. We can't just release all the
		// virtual memory otherwise the committed pages count will be wrong and
		// the free nodes won't be deleted. So we go through all allocations and 
		// delete them using the normal method.
		while(mp_RegionList)
			ClearRegion(mp_RegionList);
	}

	//------------------------------------------------------------------------
	BasicCoalesceHeap::Region* BasicCoalesceHeap::CreateRegion()
	{
		int page_size = m_VirtualMem.GetPageSize();

		// commit the region memory
		// the region struct is at the start of the committed memory
		Region* p_region = (Region*)m_VirtualMem.Reserve(m_RegionSize, page_size, VMEM_DEFAULT_RESERVE_FLAGS);
		if(!p_region)
			return NULL;		// out of memory

		if(!m_VirtualMem.Commit(p_region, m_RegionSize, VMEM_DEFAULT_COMMIT_FLAGS))
		{
			m_VirtualMem.Release(p_region);
			return NULL;		// out of memory
		}

		// initialise the region
		p_region->mp_Next = NULL;

		// initialise the free list header
		Header& free_list_head = p_region->m_FreeList;
		memset(&free_list_head, 0, sizeof(Header));

		// setup first header 'allocated' header. This header is never used
		// but makes the coalesce logic a simpler.
		Header* p_first_header = (Header*)(p_region + 1);
		VMEM_ASSERT_CODE(p_first_header->m_Marker = VMEM_COALESCE_ALLOC_MARKER);
		p_first_header->mp_Prev = NULL;				// NULL prev and next pointers mean header is allocated
		p_first_header->mp_Next = NULL;
		p_first_header->m_Size = sizeof(Header);
		VMEM_STATS(p_first_header->m_RequestedSize = 0);
		p_first_header->m_PrevSize = 0;

		// setup free header and link it to the first header
		Header* p_header = p_first_header + 1;
		VMEM_ASSERT_CODE(p_header->m_Marker = VMEM_COALESCE_ALLOC_MARKER);
		p_header->mp_Prev = &free_list_head;		// circular linked list
		p_header->mp_Next = &free_list_head;
		int total_free_size = m_RegionSize - sizeof(Region) - 2 * sizeof(Header);
		p_header->m_Size = total_free_size;			// includes size of the header
		VMEM_STATS(p_header->m_RequestedSize = 0);
		p_header->m_PrevSize = sizeof(Header);
		free_list_head.mp_Prev = p_header;
		free_list_head.mp_Next = p_header;

		// setup last header guard
		Header* p_last_header = (Header*)((byte*)p_header + p_header->m_Size);
		VMEM_ASSERT_CODE(p_last_header->m_Marker = VMEM_COALESCE_ALLOC_MARKER);
		p_last_header->mp_Prev = NULL;				// NULL prev and next pointers mean header is allocated
		p_last_header->mp_Next = NULL;
		p_last_header->m_Size = sizeof(Header);
		VMEM_STATS(p_last_header->m_RequestedSize = 0);
		p_last_header->m_PrevSize = total_free_size;

		// update stats
		VMEM_STATS(m_Stats.m_Reserved += m_RegionSize);
		VMEM_STATS(m_Stats.m_Unused += m_RegionSize - sizeof(Region) - 3*sizeof(Header));
		VMEM_STATS(m_Stats.m_Overhead += sizeof(Region) + 3*sizeof(Header));

		return p_region;
	}

	//------------------------------------------------------------------------
	// WARNING: this deletes p_region, don't use p_region after calling this function.
	// This function calls the standard Free function on each alloc in the region until
	// the region disappears. It's slightly messy, but avoids duplicating the code in
	// the Free function.
	void BasicCoalesceHeap::ClearRegion(Region* p_region)
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
			VMEM_MEM_CHECK(&p_h->m_Marker, VMEM_COALESCE_ALLOC_MARKER);

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
			VMEM_MEM_CHECK(&p_alloc_header->mp_Prev, NULL);

			size -= p_alloc_header->m_Size;
			VMEM_ASSERT_MEM(p_alloc_header->m_Size >= 0, &p_alloc_header->m_Size);

			// warning: this deletes p_region when it's empty, so be careful not to access p_region after last free
			void* p = p_alloc_header + 1;
#ifdef VMEM_COALESCE_GUARDS
			p = (byte*)p + VMEM_COALESCE_GUARD_SIZE;
#endif
			Free(p);
		}
	}

	//------------------------------------------------------------------------
	// releases the memory associated with the region. Do not use p_region after this.
	void BasicCoalesceHeap::DestroyRegion(Region* p_region)
	{
		// check it's empty
#ifdef VMEM_ASSERTS
		int total_free_size = m_RegionSize - sizeof(Region) - 2 * sizeof(Header);
		Header* p_header = (Header*)(p_region + 1) + 1;
		VMEM_ASSERT_MEM(p_header->m_Size == total_free_size, &p_header->m_Size);		// destroying non-empty region
#endif

		// release memory
		m_VirtualMem.Decommit(p_region, m_RegionSize, VMEM_DEFAULT_COMMIT_FLAGS);
		m_VirtualMem.Release(p_region);

		// update stats
		VMEM_STATS(m_Stats.m_Reserved -= m_RegionSize);
		VMEM_STATS(m_Stats.m_Unused -= m_RegionSize - sizeof(Region) - 3*sizeof(Header));
		VMEM_STATS(m_Stats.m_Overhead -= sizeof(Region) + 3*sizeof(Header));
	}

	//------------------------------------------------------------------------
	void* BasicCoalesceHeap::Alloc(size_t size)
	{
		size = AlignSizeUpPow2(size, VMEM_INTERNAL_ALIGNMENT);

		int i_size = ToInt(size);

		VMEM_ASSERT((i_size&3) == 0, "internal allocs must be aligned to 4 or stats go wrong");

		// work out the size of the block
		int total_size = i_size + sizeof(Header);
#ifdef VMEM_COALESCE_GUARDS
		total_size += 2*VMEM_COALESCE_GUARD_SIZE;
#endif
		// handle the case where the allocation is too big for this heap, fall back to VirtualAlloc
		int total_free_size = m_RegionSize - sizeof(Region) - 2 * sizeof(Header);
		if(total_size > total_free_size)
		{
			int page_size = m_VirtualMem.GetPageSize();
			size_t page_aligned_size = AlignSizeUpPow2(size + sizeof(size_t), page_size);
			
			void* p = m_VirtualMem.Reserve(page_aligned_size, page_size, VMEM_DEFAULT_RESERVE_FLAGS);
			if(!p)
				return NULL;

			if(!m_VirtualMem.Commit(p, page_aligned_size, VMEM_DEFAULT_COMMIT_FLAGS))
			{
				m_VirtualMem.Release(p);
				return NULL;
			}

			*(size_t*)p = size;
			p = (byte*)p + sizeof(size_t);

			VMEM_MEMSET(p, VMEM_ALLOCATED_MEM, i_size);

			VMEM_STATS(m_Stats.m_Reserved += page_aligned_size);
			VMEM_STATS(m_Stats.m_Used += size);
			VMEM_STATS(m_Stats.m_Overhead += page_aligned_size - size);

			VMEM_ASSERT(((uint64)p & (VMEM_INTERNAL_ALIGNMENT-1)) == 0, "pointer returned by BasicCoalescHeap not aligned correctly");

			return p;
		}

		// get the first region (or create one)
		Region* p_region = mp_RegionList;
		if(!p_region)
		{
			p_region = mp_RegionList = CreateRegion();
			if(!p_region)
				return NULL;
		}

		// linear search through the regions for a big enough free block
		while(p_region)
		{
			Header* p_list_head = &p_region->m_FreeList;

			// linear search through the free block list for a big enough block
			for(Header* p_header=p_list_head->mp_Next; p_header != p_list_head; p_header=p_header->mp_Next)
			{
				VMEM_MEM_CHECK(&p_header->m_Marker, VMEM_COALESCE_ALLOC_MARKER);
				VMEM_ASSERT_MEM(p_header->mp_Prev, &p_header->mp_Prev);

				// work out what the offcut would be
				int offcut_size = p_header->m_Size - total_size;

				// if we have found an allocation big enough
				if(offcut_size >= 0)
				{
					Header* p_prev_free_header = p_header->mp_Prev;
					Header* p_next_free_header = p_header->mp_Next;

					// ignore offcut if too small
					if(offcut_size < (int)sizeof(Header))
					{
						total_size += offcut_size;
						offcut_size = 0;
					}

					// get the next header
					Header* p_next_header = (Header*)((byte*)p_header + p_header->m_Size);
					VMEM_MEM_CHECK(&p_next_header->m_Marker, VMEM_COALESCE_ALLOC_MARKER);

					if(offcut_size)
					{
						// create a new header for the offcut
						Header* p_offcut_header = (Header*)((byte*)p_header + total_size);
						VMEM_ASSERT_CODE(p_offcut_header->m_Marker = VMEM_COALESCE_ALLOC_MARKER);
						p_offcut_header->m_Size = offcut_size;
						VMEM_STATS(p_offcut_header->m_RequestedSize = 0);
						p_offcut_header->m_PrevSize = total_size;

						// link new offcut header into free list
						VMEM_ASSERT_MEM(p_header->mp_Prev, &p_header->mp_Prev);
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
						VMEM_ASSERT_MEM(p_header->mp_Prev, &p_header->mp_Prev);
						p_prev_free_header->mp_Next = p_next_free_header;
						p_next_free_header->mp_Prev = p_prev_free_header;

						// update the prev size of the next header in memory
						p_next_header->m_PrevSize = total_size;
					}

					// setup the header for this alloc
					p_header->mp_Prev = NULL;		// NULL prev and next pointers mean header is allocated
					p_header->mp_Next = NULL;
					p_header->m_Size = total_size;
					VMEM_STATS(p_header->m_RequestedSize = i_size);

					void* p = p_header + 1;
					VMEM_MEMSET(p, VMEM_ALLOCATED_MEM, i_size);

					// set guards
#ifdef VMEM_COALESCE_GUARDS
					SetGuards(p, VMEM_COALESCE_GUARD_SIZE);
					SetGuards((byte*)p_header + total_size - VMEM_COALESCE_GUARD_SIZE, VMEM_COALESCE_GUARD_SIZE);
					p = (byte*)p + VMEM_COALESCE_GUARD_SIZE;
#endif
					// update stats
					VMEM_STATS(m_Stats.m_Unused -= total_size - sizeof(Header));
					VMEM_STATS(m_Stats.m_Used += i_size);
					VMEM_STATS(m_Stats.m_Overhead += total_size - sizeof(Header) - i_size);

					VMEM_ASSERT(((uint64)p & (VMEM_INTERNAL_ALIGNMENT-1)) == 0, "pointer returned by BasicCoalescHeap not aligned correctly");

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
	void BasicCoalesceHeap::Free(void* p)
	{
		// find the region that contains this alloc
		Region* p_region = mp_RegionList;
		while(p_region && (p < p_region || p >= (byte*)p_region + m_RegionSize))
			p_region = p_region->mp_Next;

		// handle the case where this allocation was too big for this heap and was allocated with VirtualAlloc
		if(!p_region)
		{
			void* p_base_alloc = (byte*)p - sizeof(size_t);
			size_t size = *(size_t*)p_base_alloc;
			int page_size = m_VirtualMem.GetPageSize();
			size_t page_aligned_size = AlignSizeUpPow2(size + sizeof(size_t), page_size);
			m_VirtualMem.Decommit(p_base_alloc, page_aligned_size, VMEM_DEFAULT_COMMIT_FLAGS);
			m_VirtualMem.Release(p_base_alloc);
			VMEM_STATS(m_Stats.m_Reserved -= page_aligned_size);
			VMEM_STATS(m_Stats.m_Used -= size);
			VMEM_STATS(m_Stats.m_Overhead -= page_aligned_size - size);
			return;
		}

#ifdef VMEM_COALESCE_GUARDS
		p = (byte*)p - VMEM_COALESCE_GUARD_SIZE;
#endif

		// check the header
		Header* p_header = (Header*)p - 1;
		VMEM_MEM_CHECK(&p_header->m_Marker, VMEM_COALESCE_ALLOC_MARKER);
		VMEM_ASSERT_MEM(!p_header->mp_Prev, &p_header->mp_Prev);		// allocation already freed?
		VMEM_ASSERT_MEM(!p_header->mp_Next, &p_header->mp_Next);		// header already on free list?

		VMEM_ASSERT_MEM(p_header->m_Size, &p_header->m_Size);
		int size = p_header->m_Size;

		// check the guards
#ifdef VMEM_COALESCE_GUARDS
		CheckMemory(p, VMEM_COALESCE_GUARD_SIZE, VMEM_GUARD_MEM);
		CheckMemory((byte*)p_header + size - VMEM_COALESCE_GUARD_SIZE, VMEM_COALESCE_GUARD_SIZE, VMEM_GUARD_MEM);
#endif
		// stats
		VMEM_STATS(m_Stats.m_Unused += p_header->m_Size - sizeof(Header));
		VMEM_STATS(m_Stats.m_Used -= p_header->m_RequestedSize);
		VMEM_STATS(m_Stats.m_Overhead -= p_header->m_Size - sizeof(Header) - p_header->m_RequestedSize);

		// clear the memory
		VMEM_MEMSET(p, VMEM_FREED_MEM, size - sizeof(Header));

		// get the prev and next headers
		VMEM_ASSERT_MEM(p_header->m_PrevSize, &p_header->m_PrevSize);
		Header* p_prev_header = (Header*)((byte*)p_header - p_header->m_PrevSize);
		Header* p_next_header = (Header*)((byte*)p_header + size);
		int merged_size = size;

		//----------------------------
		// merge with prev header if it's free (mp_Prev!=NULL means free)
		if(p_prev_header && p_prev_header->mp_Prev)
		{
			VMEM_MEM_CHECK(&p_prev_header->m_Marker, VMEM_COALESCE_ALLOC_MARKER);

			// merge with prev header
			merged_size += p_prev_header->m_Size;

			p_prev_header->m_Size = merged_size;
			p_next_header->m_PrevSize = merged_size;

			VMEM_MEMSET(p_header, VMEM_FREED_MEM, sizeof(Header));

			p_header = p_prev_header;

			// remove from free list
			Unlink(p_header);

			VMEM_STATS(m_Stats.m_Unused += sizeof(Header));
			VMEM_STATS(m_Stats.m_Overhead -= sizeof(Header));
		}

		//----------------------------
		// merge with next header if it's free (mp_Prev!=NULL means free)
		if(p_next_header->mp_Prev)
		{
			VMEM_MEM_CHECK(&p_next_header->m_Marker, VMEM_COALESCE_ALLOC_MARKER);

			merged_size += p_next_header->m_Size;

			Header* p_next_next_header = (Header*)((byte*)p_next_header + p_next_header->m_Size);
			if(p_next_next_header != (Header*)((byte*)p_region + m_RegionSize))
			{
				VMEM_MEM_CHECK(&p_next_next_header->m_Marker, VMEM_COALESCE_ALLOC_MARKER);
				p_next_next_header->m_PrevSize = merged_size;
			}

			p_header->m_Size = merged_size;

			// remove next header from free list
			Unlink(p_next_header);

			VMEM_MEMSET(p_next_header, VMEM_FREED_MEM, sizeof(Header));

			VMEM_STATS(m_Stats.m_Unused += sizeof(Header));
			VMEM_STATS(m_Stats.m_Overhead -= sizeof(Header));
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
		if(merged_size == total_free_size)
		{
			Region* p_prev_region = NULL;
			for(Region* p_srch_region = mp_RegionList; p_srch_region!=p_region; p_srch_region=p_srch_region->mp_Next)
				p_prev_region = p_srch_region;

			Region* p_next_region = p_region->mp_Next;
			if(p_prev_region)
				p_prev_region->mp_Next = p_next_region;
			else
				mp_RegionList = p_next_region;

			DestroyRegion(p_region);
		}
	}

	//------------------------------------------------------------------------
	void BasicCoalesceHeap::Unlink(Header* p_header)
	{
		// prev and next pointers always guaranteed to be valid because of the fixed start and end headers
		Header* p_prev = p_header->mp_Prev;
		Header* p_next = p_header->mp_Next;
		p_prev->mp_Next = p_next;
		p_next->mp_Prev = p_prev;
	}

	//------------------------------------------------------------------------
	void BasicCoalesceHeap::CheckIntegrity()
	{
#ifdef VMEM_ASSERTS
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
				VMEM_MEM_CHECK(&p_header->m_Marker, VMEM_COALESCE_ALLOC_MARKER);

				int size = p_header->m_Size;
				total_size += size;

				// check prev size
				VMEM_ASSERT_MEM(p_header->m_PrevSize == prev_size, &p_header->m_PrevSize);
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
			for(Header* p_iter = p_head->mp_Next; p_iter!=p_head; p_iter=p_iter->mp_Next)
			{
				// check marker
				VMEM_MEM_CHECK(&p_iter->m_Marker, VMEM_COALESCE_ALLOC_MARKER);

				// check prev header pointer
				VMEM_MEM_CHECK(&p_iter->mp_Prev, p_prev_header);
				p_prev_header = p_iter;
			}
		}
#endif
	}

	//------------------------------------------------------------------------
	void BasicCoalesceHeap::DebugWrite(Header* p_header) const
	{
		int size = p_header->m_Size;
		void* p_start = p_header;
		void* p_end = (byte*)p_header + size;

		VMem::DebugWrite(_T("0x%08x - 0x%08x\t%7d\t%s\n"), p_start, p_end, size, p_header->mp_Prev ? _T("free") : _T("alloc"));
	}

	//------------------------------------------------------------------------
	void BasicCoalesceHeap::DebugWrite() const
	{
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
			for(Header* p_iter=p_list_head->mp_Next; p_iter!=p_list_head; p_iter=p_iter->mp_Next)
			{
				DebugWrite(p_iter);
			}
		}
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	void BasicCoalesceHeap::SendStatsToMemPro(MemProSendFn send_fn, void* p_context)
	{
		MemProStats::BasicCoalesceHeapStats stats;

		stats.m_RegionCount = 0;
		for(Region* p_region = mp_RegionList; p_region!=NULL; p_region=p_region->mp_Next)
			++stats.m_RegionCount;

		stats.m_RegionSize = m_RegionSize;

		stats.m_Stats = m_Stats;

		SendEnumToMemPro(vmem_BasicCoalesceHeap, send_fn, p_context);
		SendToMemPro(stats, send_fn, p_context);
	}
#endif
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

