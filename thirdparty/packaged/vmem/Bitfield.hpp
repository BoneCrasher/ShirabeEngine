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
#ifndef VMEM_BITFIELD_H_INCLUDED
#define VMEM_BITFIELD_H_INCLUDED

//------------------------------------------------------------------------
#include "VMemCore.hpp"
#include "VMemStats.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	class InternalHeap;

	//------------------------------------------------------------------------
	// This bitfield is designed to be very fast at finding the lowest clear bit
	// in the array of bits. It uses a hint index that points to the lowest bit
	// that might be clear. See GetFirstClearBit for more details.
	class Bitfield
	{
	public:
		Bitfield(int size, InternalHeap& internal_heap);

		~Bitfield();

		bool Initialise();

		bool Initialised() const { return mp_Bits != NULL; }

		VMEM_FORCE_INLINE bool Get(int index) const;

		VMEM_FORCE_INLINE void Set(int index);

		VMEM_FORCE_INLINE void Clear(int index);

		VMEM_FORCE_INLINE int GetFirstClearBit();

		VMEM_FORCE_INLINE int GetCount() const;

		VMEM_FORCE_INLINE int GetSizeInBytes() const;

	private:
		Bitfield& operator=(const Bitfield& other);

		//------------------------------------------------------------------------
		// data
	private:
		int m_Size;					// size in bits
		unsigned int* mp_Bits;		// the bit array
		int m_FirstClearBitHint;	// always garanteed to be <= the first clear bit index. Usually the exact index.
		int m_Count;				// number of set bits

		InternalHeap& m_InternalHeap;	// heap that is used to allocate the bit array
	};

	//------------------------------------------------------------------------
	bool Bitfield::Get(int index) const
	{
		VMEM_ASSERT(index >= 0 && index < m_Size, "Bitfield index out of range");
		unsigned int u32_index = index / 32;
		unsigned int bit_index = index & 0x1f;
		return (mp_Bits[u32_index] & (1 << bit_index)) != 0;
	}

	//------------------------------------------------------------------------
	void Bitfield::Set(int index)
	{
		VMEM_ASSERT(index >= 0 && index < m_Size, "Bitfield index out of range");

		unsigned int u32_index = index / 32;
		unsigned int bit_index = index & 0x1f;

		int bit_mask = 1 << bit_index;
		VMEM_ASSERT_MEM((mp_Bits[u32_index] & bit_mask) == 0, &mp_Bits[u32_index]);		// Bitfield bit already set
		mp_Bits[u32_index] |= bit_mask;

		++m_Count;
	}

	//------------------------------------------------------------------------
	void Bitfield::Clear(int index)
	{
		VMEM_ASSERT(index >= 0 && index < m_Size, "Bitfield index out of range");

		unsigned int u32_index = index / 32;
		unsigned int bit_index = index & 0x1f;

		int bit_mask = 1 << bit_index;
		VMEM_ASSERT_MEM((mp_Bits[u32_index] & bit_mask) != 0, &mp_Bits[u32_index]);		// Bitfield bit already clear
		mp_Bits[u32_index] &= ~bit_mask;

		if(index < m_FirstClearBitHint)
			m_FirstClearBitHint = index;

		VMEM_ASSERT_MEM(m_Count, &m_Count);		// can't claer bit, bitfield is empty
		--m_Count;
	}

	//------------------------------------------------------------------------
	int Bitfield::GetCount() const
	{
		return m_Count;
	}

	//------------------------------------------------------------------------
	int Bitfield::GetSizeInBytes() const
	{
		return AlignUpPow2(m_Size, 32) / 8;
	}

	//------------------------------------------------------------------------
	// NOTE: the returned bit must be set after this call to keep m_FirstClearButHint valid.
	// m_FirstClearButHint is incremented by this function to avoid having a conditional in the
	// Set function.
	int Bitfield::GetFirstClearBit()
	{
		VMEM_ASSERT_MEM(m_FirstClearBitHint >= 0, &m_FirstClearBitHint);
		VMEM_ASSERT_MEM(m_FirstClearBitHint < m_Size, &m_FirstClearBitHint);
		VMEM_ASSERT_MEM(m_Count >= 0 && m_Count < m_Size, &m_Count);

		unsigned int u32_index = m_FirstClearBitHint / 32;
		unsigned int bit_index = m_FirstClearBitHint & 0x1f;
		unsigned int* p_u32 = mp_Bits + u32_index;

		int index = m_FirstClearBitHint;

		// if m_FirstClearBitHint is in use look for the next one
		if((*p_u32 & (1 << bit_index)) != 0)
		{
			// look for the next u32 that is not full
#ifdef VMEM_ASSERTS
			unsigned int u32_count = (m_Size + 31) / 32;
#endif
			while(mp_Bits[u32_index] == 0xffffffff)
			{
				++u32_index;
				VMEM_ASSERT_CODE(VMEM_ASSERT_MEM(u32_index < u32_count, &mp_Bits[u32_index]));
			}

			// find the clear bit in the u32
			unsigned int bits = mp_Bits[u32_index];
			unsigned int bit_mask = 1;
			bit_index = 0;
			while(bits & bit_mask)
			{
				bit_mask <<= 1;
				++bit_index;
			}

			index = u32_index * 32 + bit_index;
		}

		// might be free, but at least always guaranteed to be <= the free index
		m_FirstClearBitHint = index + 1;

		return index;
	}
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_BITFIELD_H_INCLUDED

