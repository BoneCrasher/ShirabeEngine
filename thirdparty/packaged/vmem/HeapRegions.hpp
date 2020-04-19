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
#ifndef VMEM_HEAPREGIONS_H_INCLUDED
#define VMEM_HEAPREGIONS_H_INCLUDED

//------------------------------------------------------------------------
#include "VMemCore.hpp"
#include "InternalHeap.hpp"
#include "VMemCriticalSection.hpp"
#include "VMemArray.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	namespace RegionType
	{
		enum Enum
		{
			PageHeap1 = 0,
			PageHeap2,
			CoalesceHeap1,
			CoalesceHeap2,
			AlignedCoalesceHeap,
#ifdef VMEM_COALESCE_HEAP_PER_THREAD
			TLSCoalesceHeap,
#endif
			Invalid,
		};
	};

	//------------------------------------------------------------------------
	class HeapRegions
	{
		struct Region
		{
			void* mp_Mem;
			int m_Size;
			RegionType::Enum m_RegionType;

			#ifdef VMEM_COALESCE_HEAP_PER_THREAD
				void* mp_Heap;
			#endif
		};

	public:
		//------------------------------------------------------------------------
#ifdef VMEM_COALESCE_HEAP_PER_THREAD
		struct RegionAndHeap
		{
			RegionType::Enum m_RegionType;
			void* mp_Heap;
		};
#endif
		//------------------------------------------------------------------------
		HeapRegions(InternalHeap& internal_heap)
		:	m_Regions(internal_heap)
		{
		}

		//------------------------------------------------------------------------
		bool Initialise()
		{
			return m_Regions.Initialise();
		}

		//------------------------------------------------------------------------
#ifdef VMEM_COALESCE_HEAP_PER_THREAD
		VMEM_FORCE_INLINE RegionAndHeap GetRegionAndHeap(void* p) const
		{
			ReadLockScope lock(m_ReadWriteLock);

			int count = m_Regions.GetCount();
			for(int i=0; i<count; ++i)
			{
				const Region& region = m_Regions[i];

				void* p_mem = region.mp_Mem;
				if(p >= p_mem && p < (byte*)p_mem + region.m_Size)
				{
					RegionAndHeap region_and_heap;
					region_and_heap.m_RegionType = region.m_RegionType;
					region_and_heap.mp_Heap = region.mp_Heap;
					return region_and_heap;
				}
			}

			RegionAndHeap region_and_heap;
			region_and_heap.m_RegionType = RegionType::Invalid;
			region_and_heap.mp_Heap = NULL;
			return region_and_heap;
		}
#else
		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE RegionType::Enum GetRegion(void* p) const
		{
			ReadLockScope lock(m_ReadWriteLock);

			int count = m_Regions.GetCount();
			for(int i=0; i<count; ++i)
			{
				const Region& region = m_Regions[i];

				void* p_mem = region.mp_Mem;
				if(p >= p_mem && p < (byte*)p_mem + region.m_Size)
					return region.m_RegionType;
			}

			return RegionType::Invalid;
		}
#endif
		//------------------------------------------------------------------------
#ifdef VMEM_COALESCE_HEAP_PER_THREAD
		bool AddRegion(void* p_heap, void* p_mem, int size, RegionType::Enum region_type)
		{
			WriteLockScope lock(m_ReadWriteLock);

			Region region;
			region.mp_Heap = p_heap;
			region.mp_Mem = p_mem;
			region.m_Size = size;
			region.m_RegionType = region_type;

			return m_Regions.Add(region);
		}
#else
		bool AddRegion(void* p_mem, int size, RegionType::Enum region_type)
		{
			WriteLockScope lock(m_ReadWriteLock);

			Region region;
			region.mp_Mem = p_mem;
			region.m_Size = size;
			region.m_RegionType = region_type;

			return m_Regions.Add(region);
		}
#endif
		//------------------------------------------------------------------------
		void RemoveRegion(void* p_mem)
		{
			WriteLockScope lock(m_ReadWriteLock);

			int index = FindRegion(p_mem);
			VMEM_ASSERT(index != -1, "unable to find region");

			m_Regions.RemoveAt(index);
		}

		//------------------------------------------------------------------------
		int GetMemoryUsage() const
		{
			ReadLockScope lock(m_ReadWriteLock);
			return m_Regions.GetMemoryUsage();
		}

		//------------------------------------------------------------------------
		int GetMemoryUsage_NoLock() const
		{
			return m_Regions.GetMemoryUsage();
		}

	private:
		//------------------------------------------------------------------------
		int FindRegion(void* p_mem) const
		{
			int count = m_Regions.GetCount();
			for(int i=0; i<count; ++i)
				if(m_Regions[i].mp_Mem == p_mem)
					return i;

			return -1;
		}

		//------------------------------------------------------------------------
		// data
	private:
		VMemArray<Region> m_Regions;

		mutable ReadWriteLock m_ReadWriteLock;
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_HEAPREGIONS_H_INCLUDED

