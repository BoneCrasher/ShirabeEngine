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
#include "TrailGuard.hpp"
#include "VMemCore.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	inline int GetAllocSize(void* p, int fixed_alloc_size)
	{
		return fixed_alloc_size != -1 ? fixed_alloc_size : *(int*)((VMem::byte*)p + sizeof(void*));
	}

	//------------------------------------------------------------------------
	TrailGuard::TrailGuard()
	:	m_Size(0),
		m_CurSize(0),
		m_FixedAllocSize(0),
		mp_Head(NULL),
		mp_Tail(NULL),
		m_FullCheckFreq(0),
		m_FullCheckCounter(0)
	{
	}

	//------------------------------------------------------------------------
	void TrailGuard::Initialise(int size, int check_freq, int alloc_size)
	{
		m_Size = size;
		m_FullCheckFreq = check_freq;
		m_FixedAllocSize = alloc_size;
	}

	//------------------------------------------------------------------------
	void* TrailGuard::Shutdown()
	{
		void* p_trail = mp_Head;
		mp_Head = NULL;
		mp_Tail = NULL;
		m_Size = 0;
		m_CurSize = 0;
		return p_trail;
	}

	//------------------------------------------------------------------------
	void* TrailGuard::Add(void* p, int size)
	{
		if(!m_Size) return NULL;

		// add alloc to tail
		if(mp_Tail)
		{
			*(void**)mp_Tail = p;
			mp_Tail = p;
		}
		else
		{
			mp_Head = mp_Tail = p;
		}
		*(void**)p = NULL;

		// set the guard
		int guard_size = size - sizeof(void*);
		void* p_guard = (byte*)p + sizeof(void*);
		if(m_FixedAllocSize == -1)
		{
			VMEM_ASSERT((unsigned int)size >= sizeof(void*) + sizeof(int), "Variable size guards must be >= sizeof(void*) + sizeof(int)");
			guard_size -= sizeof(int);
			*(int*)p_guard = size;
			p_guard = (byte*)p_guard + sizeof(int);

		}
		if(guard_size)
			memset(p_guard, VMEM_TRAIL_GUARD_MEM, guard_size);

		// increase trail size
		m_CurSize += size;

		// check the entire trail
		if(m_FullCheckCounter == m_FullCheckFreq)
		{
			m_FullCheckCounter = 0;
			CheckIntegrity();
		}
		++m_FullCheckCounter;

		// return NULL if trail has not reached limit yet
		if(m_CurSize < m_Size)
			return NULL;

		// remove alloc from head
		void* new_p = mp_Head;
		mp_Head = *(void**)new_p;

		int new_alloc_size = GetAllocSize(new_p, m_FixedAllocSize);
		m_CurSize -= new_alloc_size;

		int new_guard_size = new_alloc_size - sizeof(void*);
		void* p_new_guard = (byte*)new_p + sizeof(void*);
		if(m_FixedAllocSize == -1)
		{
			new_guard_size -= sizeof(int);
			p_new_guard = (byte*)p_new_guard + sizeof(int);
		}

		// check that the guard is still intact
		CheckMemory(p_new_guard, new_guard_size, VMEM_TRAIL_GUARD_MEM);

		return new_p;
	}

	//------------------------------------------------------------------------
	void TrailGuard::CheckIntegrity() const
	{
		VMEM_ASSERT_CODE(int total_trail_size = 0);
		void* p_alloc = mp_Head;

		while(p_alloc)
		{
			int size = GetAllocSize(p_alloc, m_FixedAllocSize);

			void* p_guard = (byte*)p_alloc + sizeof(void*);
			int guard_size = size - sizeof(void*);

			if(m_FixedAllocSize == -1)
			{
				guard_size -= sizeof(int);
				p_guard = (byte*)p_guard + sizeof(int);
			}

			CheckMemory(p_guard, guard_size, VMEM_TRAIL_GUARD_MEM);

			VMEM_ASSERT_CODE(total_trail_size += size);
			VMEM_ASSERT(total_trail_size <= m_CurSize, "Trail guard fail");

			// get the next alloc
			p_alloc = *(void**)p_alloc;
		}

		VMEM_ASSERT(total_trail_size == m_CurSize, "Trail guard fail");
	}

	//------------------------------------------------------------------------
	bool TrailGuard::Contains(void* p) const
	{
		void* p_alloc = mp_Head;

		while(p_alloc)
		{
			if(p_alloc == p)
				return true;
			p_alloc = *(void**)p_alloc;
		}

		return false;
	}
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

