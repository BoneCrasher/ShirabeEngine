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
#ifndef VMEM_PHYSICALFSA_H_INCLUDED
#define VMEM_PHYSICALFSA_H_INCLUDED

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#include "VMemCore.hpp"

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	template<typename T>
	class PhysicalFSA
	{
		struct Block
		{
			Block* mp_Next;
			PhysicalAlloc m_PhysicalAlloc;
		};

	public:
		//------------------------------------------------------------------------
		PhysicalFSA()
		:	mp_FreeList(NULL)
		{
		}

		//------------------------------------------------------------------------
		bool IsEmpty() const
		{
			return mp_FreeList == NULL;
		}

		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE T* Alloc()
		{
			T* p = mp_FreeList;

			if (p)
				mp_FreeList = *(T**)p;

			VMEM_ASSERT(((uint64)p & (VMEM_INTERNAL_ALIGNMENT - 1)) == 0, "BasicFSA Alloc returned misaligned pointer");

			return p;
		}

		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE void Free(T* p)
		{
			*(void**)p = mp_FreeList;
			mp_FreeList = p;
		}

		//------------------------------------------------------------------------
		void GiveMemory(void* p, size_t size)
		{
			VMEM_ASSERT(((uint64)p & (VMEM_INTERNAL_ALIGNMENT - 1)) == 0, "BasicFSA Alloc returned misaligned pointer");

			int count = ToInt(size / ItemSize());
			VMEM_ASSERT(count, "bad size passed to GiveMemory");

			T* p_first_item = (T*)p;
			T* p_item = p_first_item;
			for (int i = 1; i<count; ++i, ++p_item)
				*(void**)p_item = p_item + 1;
			*(void**)p_item = mp_FreeList;

			mp_FreeList = p_first_item;
		}

	private:
		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE static int ItemSize()
		{
			return ToInt(AlignSizeUpPow2(sizeof(T), VMEM_INTERNAL_ALIGNMENT));
		}

		//------------------------------------------------------------------------
		// data
	private:
		T* mp_FreeList;
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_PHYSICALFSA_H_INCLUDED

