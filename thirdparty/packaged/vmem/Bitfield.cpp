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
#include "Bitfield.hpp"
#include "InternalHeap.hpp"
#include <memory.h>

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	Bitfield::Bitfield(int size, InternalHeap& internal_heap)
	:	m_Size(size),
		mp_Bits(NULL),
		m_FirstClearBitHint(0),
		m_Count(0),
		m_InternalHeap(internal_heap)
	{
		VMEM_ASSERT(size, "size must > 0");
	}

	//------------------------------------------------------------------------
	bool Bitfield::Initialise()
	{
		int size_in_bytes = GetSizeInBytes();

		unsigned int* p_bits = (unsigned int*)m_InternalHeap.Alloc(size_in_bytes);
		if(!p_bits)
			return false;

		mp_Bits = p_bits;
		memset(p_bits, 0, size_in_bytes);

		// if the size is not a multiple of 32 set all trailing bits to 1
		int left_over = m_Size & 31;
		if(left_over)
		{
			int size_in_uints = size_in_bytes / sizeof(unsigned int);
			unsigned int& last_uint = p_bits[size_in_uints - 1];
			for(int i=left_over; i<32; ++i)
				last_uint |= (1 << i);
		}

		return true;
	}

	//------------------------------------------------------------------------
	Bitfield::~Bitfield()
	{
		if(mp_Bits)
			m_InternalHeap.Free(mp_Bits);
	}
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

