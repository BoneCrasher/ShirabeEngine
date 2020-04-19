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
#ifndef VMEM_PHYSICALHEAP_H_INCLUDED
#define VMEM_PHYSICALHEAP_H_INCLUDED

//------------------------------------------------------------------------
#include "VMemStats.hpp"
#include "VMemCriticalSection.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	class PhysicalHeap
	{
		// the header is located immediately before the allocation or at the start of the free block in the coalesce heap
		struct Header
		{
#if VMEM_DEBUG_LEVEL >= 1
			unsigned int m_Marker[4];
#endif
			Header* mp_Prev;		// the prev and next free block of memory.
			Header* mp_Next;		// if the header is allocated prev and next pointers will be null.
			int m_Size;				// the size includes the size of the header
			int m_PrevSize;
#ifdef VMEM_X64
			int m_Padding[2];
#endif
		};

		// the coalesce heap region
		struct Region
		{
			Header m_FreeList;
			Region* mp_Next;
#ifdef VMEM_X64
			int m_Padding[2];
#else
			int m_Padding[3];
#endif
		};

		struct PhysicalAllocInfo
		{
			void* mp_Mem;
			size_t m_Size;
		};

		class PhysicalAllocArray
		{
		public:
			PhysicalAllocArray();

			~PhysicalAllocArray();

			bool Add(void* p, size_t size);

			bool Remove(void* p, size_t& size);

			size_t GetSize(void* p);

			bool Contains(void* p);

			int GetMemorySize() const { return m_Capacity * sizeof(PhysicalAllocInfo); }

			int GetCount() const { return m_Count; }

			PhysicalAllocInfo operator[](int index) const
			{
				VMEM_ASSERT(index >= 0 && index < m_Count, "VMem index out of bound in PhysicalAllocArray");
				return mp_Array[index];
			}

		private:
			bool Grow();

			// data
			PhysicalAllocInfo* mp_Array;
			int m_Capacity;
			int m_Count;
		};

		//------------------------------------------------------------------------
	public:
		PhysicalHeap(size_t coalesce_heap_region_size, size_t coalesce_heap_max_size, int flags);

		~PhysicalHeap();

		void* Alloc(size_t size);

		bool Free(void* p);

		size_t GetSize(void* p);

		bool Owns(void* p);

		void* AllocAligned(size_t size, size_t alignment);

		bool FreeAligned(void* p);

		size_t GetSizeAligned(void* p);

		void CheckIntegrity();

		void DebugWrite() const;

		void Trim();

		size_t GetMaxAllocSize() const { return m_CoalesceHeapMaxSize; }

		void WriteStats();

	private:
		Region* CreateRegion();

		void DestroyRegion(Region* p_region);

		void UnlinkRegion(Region* p_region);

		static void DebugWrite(Header* p_header);

		static void DebugWrite(const PhysicalAllocArray& array);

		void ClearRegion(Region* p_region);

		bool CoalesceOwns(void* p) const;

		static inline void Unlink(Header* p_header);

		static void InitialiseHeader(Header* p_header);

		static void CheckHeader(Header* p_header);

		void* PhysicalAllocDirect(size_t size);

		bool PhysicalFreeDirect(void* p);

		void* AllocatePhysical(size_t size);

		void PhysicalFree(void* p);

		static void* IntrnalAlloc(size_t size);

		static void InternalFree(void* p);

		void* CoalesceAlloc(size_t size);

		bool CoalesceFree(void* p);

		void* PhysicalAllocDirectAligned(size_t size);

		bool PhysicalFreeDirectAligned(void* p);

		bool TrackPhysicalAlloc(void* p, size_t size);

		bool TrackPhysicalAllocAligned(void* p, size_t size);

		bool CoalesceEmpty() const;

		size_t GetSize_NoLock(void* p);

		static void InitialiseAlignmentHeader(struct PhysicalAlignHeader* p_header);

		static void CheckAlignmentHeader(struct PhysicalAlignHeader* p_header);

		//------------------------------------------------------------------------
		// data
	private:
		Region* mp_RegionList;			// the coalesce heap regions
		int m_RegionSize;				// the size of each coalesce region.
		size_t m_CoalesceHeapMaxSize;	// allocations larger than this size get allocated with the platform physical alloc function directly

#ifdef VMEM_PLATFORM_XBOX360
		int m_Flags;					// flags that are passed through to the platfom specific physical alloc
#endif
		mutable CriticalSection m_CriticalSection;

		PhysicalAllocArray m_PhysicalAllocs;			// allocations too big for the coalesce heap that don't have an alignment header
		PhysicalAllocArray m_AlignedPhysicalAllocs;		// allocations too big for the coalesce heap that do have an alignment header

		// debug stuff to make sure that everything is cleaned up in the destructor
#if VMEM_DEBUG_LEVEL >= 1
		int m_PhysicalAllocCount;
		#ifdef VMEM_PLATFORM_WIN
			size_t m_PhysicalAllocTotal;
		#endif
#endif

#ifdef VMEM_ENABLE_STATS
		Stats m_Stats;
		size_t m_PhysicalAllocDirectTotalSize;
#endif
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_PHYSICAL_HEAP_SUPPORTED

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_PHYSICALHEAP_H_INCLUDED

