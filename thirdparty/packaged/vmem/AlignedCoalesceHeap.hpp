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
#ifndef VMEM_ALIGNEDCOALESCEHEAP_H_INCLUDED
#define VMEM_ALIGNEDCOALESCEHEAP_H_INCLUDED

//------------------------------------------------------------------------
#include "VMemCore.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	class AlignedCoalesceHeap
	{
	public:
		//------------------------------------------------------------------------
		AlignedCoalesceHeap(
			int region_size,
			int min_size,
			int max_size,
			InternalHeap& internal_heap,
			HeapRegions& heap_regions,
			RegionType::Enum region_type,
			VirtualMem& virtual_mem,
			int reserve_flags,
			int commit_flags)
		:
			m_CoalesceHeap(
				region_size,
				min_size,
				max_size,
				internal_heap,
				heap_regions,
				region_type,
				reserve_flags,
				commit_flags,
				virtual_mem)
		{
		}

		//------------------------------------------------------------------------
		bool Initialise()
		{
			return m_CoalesceHeap.Initialise();
		}

		//------------------------------------------------------------------------
		void* Alloc(size_t size, size_t alignment)
		{
			size_t aligned_size = size + sizeof(AlignedHeader) + alignment;

			void* p = m_CoalesceHeap.Alloc(aligned_size);
			if(!p)
				return NULL;

#ifdef VMEM_CUSTOM_ALLOC_INFO
			void* aligned_p = (byte*)AlignUp((byte*)p + sizeof(AlignedHeader) + sizeof(VMemCustomAllocInfo), alignment) - sizeof(VMemCustomAllocInfo);
#else
			void* aligned_p = AlignUp((byte*)p + sizeof(AlignedHeader), alignment);
#endif
			AlignedHeader* p_header = (AlignedHeader*)aligned_p - 1;
			p_header->p = p;

			return aligned_p;
		}

		//------------------------------------------------------------------------
		void Free(void* p)
		{
			AlignedHeader* p_header = (AlignedHeader*)p - 1;
			m_CoalesceHeap.Free(p_header->p);
		}

		//------------------------------------------------------------------------
		static size_t GetSize(void* p)
		{
			AlignedHeader* p_header = (AlignedHeader*)p - 1;
			return CoalesceHeap::GetSize(p_header->p) - ((byte*)p - (byte*)p_header->p);
		}

		//------------------------------------------------------------------------
		void Lock() const { m_CoalesceHeap.Lock(); }
		void Release() const { m_CoalesceHeap.Release(); }

		//------------------------------------------------------------------------
		void Update()
		{
			m_CoalesceHeap.Update();
		}

		//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
		Stats GetStats() const { return m_CoalesceHeap.GetStats(); }
		Stats GetStatsNoLock() const { return m_CoalesceHeap.GetStatsNoLock(); }
		size_t WriteAllocs() const { return m_CoalesceHeap.WriteAllocs(); }
		void SendStatsToMemPro(MemProSendFn send_fn, void* p_context) { m_CoalesceHeap.SendStatsToMemPro(send_fn, p_context); }
#endif
		//------------------------------------------------------------------------
		// data
	private:
		CoalesceHeap m_CoalesceHeap;
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_ALIGNEDCOALESCEHEAP_H_INCLUDED

