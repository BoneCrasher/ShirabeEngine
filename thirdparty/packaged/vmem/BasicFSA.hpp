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
#ifndef VMEM_BASICFSA_H_INCLUDED
#define VMEM_BASICFSA_H_INCLUDED

//------------------------------------------------------------------------
#include "VMemCore.hpp"
#include "VMemStats.hpp"
#include "VMemSys.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	template<typename T>
	class BasicFSA
	{
	public:
		//------------------------------------------------------------------------
		BasicFSA(int page_size)
		:	m_PageSize(page_size),
			mp_FreeList(NULL),
			mp_BlockList(NULL),
			m_ItemsPerBlock(ToInt((m_PageSize - AlignUp(sizeof(void*), (size_t)ItemSize())) / ItemSize()))
		{
		}

		//------------------------------------------------------------------------
		~BasicFSA()
		{
			void* p_block = mp_BlockList;
			while(p_block)
			{
				void* p_next_block = *(void**)p_block;

				VMem::VirtualDecommit(p_block, m_PageSize, m_PageSize, VMEM_DEFAULT_COMMIT_FLAGS);
				VMem::VirtualRelease(p_block, m_PageSize);

				VMEM_STATS(m_Stats.m_Reserved -= m_PageSize);
				VMEM_STATS(m_Stats.m_Overhead -= m_PageSize);

				p_block = p_next_block;
			}
		}

		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE T* Alloc()
		{
			T* p = mp_FreeList;

			if(p)
				mp_FreeList = *(T**)p;
			else
				p = AllocateBlock();

			VMEM_ASSERT(((uint64)p & (VMEM_INTERNAL_ALIGNMENT-1)) == 0,"BasicFSA Alloc returned misaligned pointer");

			return p;
		}

		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE void Free(T* p)
		{
			*(void**)p = mp_FreeList;
			mp_FreeList = p;
		}

		//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
		const Stats& GetStats() const
		{
			return m_Stats;
		}
#endif

	private:
		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE static int ItemSize()
		{
			return ToInt(AlignSizeUpPow2(sizeof(T), VMEM_INTERNAL_ALIGNMENT));
		}

		//------------------------------------------------------------------------
		VMEM_NO_INLINE T* AllocateBlock()
		{
			void* p_block = VMem::VirtualReserve(m_PageSize, m_PageSize, VMEM_DEFAULT_RESERVE_FLAGS);
			if(!p_block)
			{
				return NULL;
			}

			if(!VMem::VirtualCommit(p_block, m_PageSize, m_PageSize, VMEM_DEFAULT_COMMIT_FLAGS))
			{
				VMem::VirtualRelease(p_block, m_PageSize);
				return NULL;
			}

			*(void**)p_block = mp_BlockList;
			mp_BlockList = p_block;

			T* p_first_item = (T*)((byte*)p_block + AlignUp(sizeof(void*), ItemSize()));

			T* p_item = p_first_item + 1;
			mp_FreeList = p_item;

			int count = m_ItemsPerBlock;
			for(int i=2; i<count; ++i, ++p_item)
				*(void**)p_item = p_item + 1;
			*(void**)p_item = NULL;

			VMEM_STATS(m_Stats.m_Reserved += m_PageSize);
			VMEM_STATS(m_Stats.m_Overhead += m_PageSize);

			return p_first_item;
		}

		//------------------------------------------------------------------------
		// data
	private:
		int m_PageSize;

		T* mp_FreeList;

		void* mp_BlockList;
#ifdef VMEM_ENABLE_STATS
		Stats m_Stats;
#endif
		int m_ItemsPerBlock;
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_BASICFSA_H_INCLUDED

