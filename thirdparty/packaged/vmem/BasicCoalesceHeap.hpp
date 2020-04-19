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
#ifndef VMEM_BASICCOALESCEHEAP_H_INCLUDED
#define VMEM_BASICCOALESCEHEAP_H_INCLUDED

//------------------------------------------------------------------------
#include "VMemCore.hpp"
#include "VMemStats.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	class VirtualMem;

	//------------------------------------------------------------------------
	// BasicCoalesceHeap is different from the standard Coalesce heap in that it
	// doesn't allocate seperate nodes for the free list and doesn't do any biasing.
	// BasicCoalesceHeap doesn't rely on any other allocators and gets its memory
	// directly from the system.
	// It only has a very small overhead and does a simple linear search for a big
	// enough block of memory for each alloc. BasicCoalesceHeap is only intended for
	// internal use by VMem. All allocations are guaranteed to be aligned to 4 bytes.
	// BasicCoalesceHeap is NOT thread safe.
	class BasicCoalesceHeap
	{
		// the header is located immediately before the allocation or at the start of the free block
		struct Header
		{
#ifdef VMEM_ASSERTS
			unsigned int m_Marker;
#endif
			Header* mp_Prev;		// the prev and next free block of memory.
			Header* mp_Next;		// if the header is allocated prev and next pointers will be null.
			int m_Size;				// the size includes the size of the header
			int m_PrevSize;
#ifdef VMEM_ENABLE_STATS
			int m_RequestedSize;
#endif
		};

		struct Region
		{
			Header m_FreeList;
			Region* mp_Next;
		};

		VMEM_STATIC_ASSERT((sizeof(Header) & (VMEM_INTERNAL_ALIGNMENT-1)) == 0, "BasicCoalesceHeap Header not aligned");
		VMEM_STATIC_ASSERT((sizeof(Region) & (VMEM_INTERNAL_ALIGNMENT-1)) == 0, "BasicCoalesceHeap Region not aligned");

		//------------------------------------------------------------------------
	public:
		BasicCoalesceHeap(int region_size, VirtualMem& virtual_mem);

		~BasicCoalesceHeap();

		void* Alloc(size_t size);

		void Free(void* p);

		void CheckIntegrity();

		void DebugWrite() const;

#ifdef VMEM_ENABLE_STATS
		inline const Stats& GetStats() const;
		void SendStatsToMemPro(MemProSendFn send_fn, void* p_context);
#endif

	private:
		Region* CreateRegion();

		void DestroyRegion(Region* p_region);

		void DebugWrite(Header* p_header) const;

		void ClearRegion(Region* p_region);

		VMEM_FORCE_INLINE void Unlink(Header* p_header);

		BasicCoalesceHeap(const BasicCoalesceHeap&);
		void operator=(const BasicCoalesceHeap&);

		//------------------------------------------------------------------------
		// data
	private:
		Region* mp_RegionList;
		int m_RegionSize;

		VirtualMem& m_VirtualMem;

#ifdef VMEM_ENABLE_STATS
		Stats m_Stats;
#endif
	};

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	const Stats& BasicCoalesceHeap::GetStats() const
	{
		return m_Stats;
	}
#endif
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_BASICCOALESCEHEAP_H_INCLUDED

