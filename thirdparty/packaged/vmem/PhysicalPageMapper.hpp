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
#ifndef VMEM_PHYSICALPAGEMAPPER_H_INCLUDED
#define VMEM_PHYSICALPAGEMAPPER_H_INCLUDED

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#include "VMemCore.hpp"
#include "PhysicalArray.hpp"
#include "PhysicalPageHeap.hpp"

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	class PhysicalPageMapper
	{
	public:
		PhysicalPageMapper(int page_size);

		~PhysicalPageMapper();

		bool MapPhysicalPages(void* p_virtual, size_t size, int commit_flags);

		void UnMapPhysicalPages(void* p_virtual, size_t size, int commit_flags);

		void CheckIntegrity();

#ifdef VMEM_ENABLE_STATS
		Stats GetStats() const;
#endif

	private:
		PhysicalPageHeap* GetPhysicalPageHeap(int commit_flags);

		PhysicalAlloc InternalAlloc(size_t size, int commit_flags);

		void InternalFree(PhysicalAlloc alloc);

		void AddMappedRange(void* p_virtual, size_t size, void* p_physical, void* p_internal_page_physical, bool& used_internal_page);

		void FreePhysical(void* p_physical, size_t size, int commit_flags);

		void AddExcessMemoryToHeap();

		void AddInternalPage(void* p_internal_page);

		//------------------------------------------------------------------------
		// data
	private:
		int m_PageSize;

		RangeMap<void*> m_MappedRangeMap;	// virtual range => physical range

		static const int m_MaxPhysicalPageHeaps = 2;
		PhysicalPageHeap m_PhysicalPageHeaps[m_MaxPhysicalPageHeaps];

		PhysicalArray<PhysicalAlloc> m_AllocatedRanges;

		struct ExcessRange
		{
			ExcessRange(void* p, size_t size, int commit_flags)
			:	mp_Mem(p),
				m_Size(size),
				m_CommitFlags(commit_flags)
			{
			}

			void* mp_Mem;
			size_t m_Size;
			int m_CommitFlags;
		};
		PhysicalArray<ExcessRange> m_ExcessPhysicalMemory;

		void* mp_InternalPages;		// pages given to m_MappedRangeMap and m_PhysicalPageHeaps

#ifdef VMEM_ENABLE_STATS
		Stats m_Stats;
#endif
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_PHYSICALPAGEMAPPER_H_INCLUDED

