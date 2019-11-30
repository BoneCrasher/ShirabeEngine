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
#ifndef VMEM_TIEREDBITFIELD_H_INCLUDED
#define VMEM_TIEREDBITFIELD_H_INCLUDED

//------------------------------------------------------------------------
#include "VMemCore.hpp"
#include "InternalHeap.hpp"
#include "VMemStats.hpp"
#include "Bitfield.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	class TieredBitfield
	{
		//------------------------------------------------------------------------
		typedef unsigned int uint;

		//------------------------------------------------------------------------
		enum
		{
			UIntShift = 5,
			UIntBitCount = 1<<UIntShift,
			UIntMask = UIntBitCount-1,
		};

		//------------------------------------------------------------------------
		class Bitfield
		{
		public:
			Bitfield()
			:	mp_Bits(NULL),
				m_UIntCount(0)
			{
			}

			void Initialise(uint* p_bits, int uint_count)
			{
				mp_Bits = p_bits;
				m_UIntCount = uint_count;

				memset(p_bits, 0, uint_count * sizeof(uint));
			}

			// return true if he uint was clear before we set the bit
			VMEM_FORCE_INLINE bool Set(int index)
			{
				int uint_index = index >> UIntShift;
				int shift = index & UIntMask;
				uint bit = 1 << shift;

				uint old_bits = mp_Bits[uint_index];
				if((old_bits & bit) == 0)
					mp_Bits[uint_index] = old_bits | bit;

				return !old_bits;
			}

			// return true if the entire uint has no set bits after the clear
			VMEM_FORCE_INLINE bool Clear(int index)
			{
				int uint_index = index >> UIntShift;
				int shift = index & UIntMask;
				uint bit = 1 << shift;

				VMEM_ASSERT((mp_Bits[uint_index] & bit) != 0, "bit not set");

				uint new_bits = mp_Bits[uint_index] & ~bit;
				mp_Bits[uint_index] = new_bits;
				return !new_bits;
			}

			VMEM_FORCE_INLINE int FindNextSetBit_InCurrentUInt(int index) const
			{
				int uint_index = index >> UIntShift;
				VMEM_ASSERT(uint_index < m_UIntCount, "index out of range");

				uint bits = mp_Bits[uint_index];

				if(bits)
				{
					for(int i=index&UIntMask; i<UIntBitCount; ++i)
						if((bits & (1<<i)) != 0)
							return uint_index * UIntBitCount + i;
				}

				return -1;
			}

			VMEM_FORCE_INLINE int FindPrevSetBit_InCurrentUInt(int index) const
			{
				int uint_index = index >> UIntShift;
				VMEM_ASSERT(uint_index < m_UIntCount, "index out of range");

				uint bits = mp_Bits[uint_index];

				if(bits)
				{
					for(int i=index&UIntMask; i>=0; --i)
						if((bits & (1<<i)) != 0)
							return uint_index * UIntBitCount + i;
				}

				return -1;
			}

			VMEM_FORCE_INLINE int FindNextSetBit(int index) const
			{
				int uint_index = index >> UIntShift;
				if(uint_index == m_UIntCount)
					return -1;
				VMEM_ASSERT(uint_index < m_UIntCount, "index out of range");

				uint* p_bits = mp_Bits + uint_index;
				uint bits = *p_bits;

				if(bits)
				{
					for(int i=index&UIntMask; i<UIntBitCount; ++i)
						if((bits & (1<<i)) != 0)
							return uint_index * UIntBitCount + i;
				}

				uint* p_bits_end = mp_Bits + m_UIntCount;
				do
				{
					++p_bits;
					if(p_bits == p_bits_end)
						return -1;
				} while(*p_bits == 0);

				uint bits2 = *p_bits;
				VMEM_ASSERT(bits2, "bits2 is 0");
				int start_index = ToInt(p_bits - mp_Bits) * UIntBitCount;
				for(int i=0; ; ++i)
					if((bits2 & (1<<i)) != 0)
						return start_index + i;
			}

			VMEM_FORCE_INLINE int FindPrevSetBit(int index) const
			{
				if(index == -1)
					return -1;
				int uint_index = index >> UIntShift;
				VMEM_ASSERT(uint_index < m_UIntCount, "index out of range");

				uint* p_bits = mp_Bits + uint_index;
				uint bits = *p_bits;

				if(bits)
				{
					for(int i=index&UIntMask; i>=0; --i)
						if((bits & (1<<i)) != 0)
							return uint_index * UIntBitCount + i;
				}

				uint* p_bits_end = mp_Bits - 1;
				do
				{
					--p_bits;
					if(p_bits == p_bits_end)
						return -1;
				} while(*p_bits == 0);

				uint bits2 = *p_bits;
				VMEM_ASSERT(bits2, "bits2 is 0");
				int start_index = ToInt(p_bits - mp_Bits) * UIntBitCount;
				for(int i=UIntBitCount-1; ; --i)
					if((bits2 & (1<<i)) != 0)
						return start_index + i;
			}

			//------------------------------------------
			// data
		private:
			uint* mp_Bits;
			int m_UIntCount;
		};

	public:
		//------------------------------------------------------------------------
		TieredBitfield(InternalHeap& internal_heap)
		:	m_InternalHeap(internal_heap),
			mp_Bits(NULL),
			m_Count(0),
			m_MemoryUsage(0)
		{
		}

		//------------------------------------------------------------------------
		~TieredBitfield()
		{
			if(mp_Bits)
				m_InternalHeap.Free_WithFalseSharingBuffer(mp_Bits);
		}

		//------------------------------------------------------------------------
		bool Initialise(int count)
		{
			m_Count = count;

			int tier1_uint_count = ((count + UIntBitCount-1) >> UIntShift);
			int tier2_uint_count = ((tier1_uint_count + UIntBitCount-1) >> UIntShift);

			int total_count = tier1_uint_count + tier2_uint_count;
			mp_Bits = (uint*)m_InternalHeap.Alloc_WithFalseSharingBuffer(total_count * sizeof(uint));
			if(!mp_Bits)
			{
				m_Count = 0;
				return false;
			}

			m_MemoryUsage = total_count * sizeof(uint) + InternalHeap::GetFalseSharingBufferOverhead();

			m_Tier1Bits.Initialise(mp_Bits, tier1_uint_count);
			m_Tier2Bits.Initialise(mp_Bits + tier1_uint_count, tier2_uint_count);

			return true;
		}

		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE void Set(int index)
		{
			VMEM_ASSERT(index >= 0 && index < m_Count, "index out of range");

			if(m_Tier1Bits.Set(index))
				m_Tier2Bits.Set(index >> UIntShift);
		}

		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE void Clear(int index)
		{
			VMEM_ASSERT(index >= 0 && index < m_Count, "index out of range");

			if(m_Tier1Bits.Clear(index))
				m_Tier2Bits.Clear(index >> UIntShift);
		}

		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE int FindNextSetBit(int index) const
		{
			VMEM_ASSERT(index >= 0 && index < m_Count, "index out of range");

			int set_index = m_Tier1Bits.FindNextSetBit_InCurrentUInt(index);
			if(set_index != -1)
				return set_index;

			int tier2_index = m_Tier2Bits.FindNextSetBit((index >> UIntShift) + 1);
			if(tier2_index == -1)
				return -1;

			return m_Tier1Bits.FindNextSetBit_InCurrentUInt(tier2_index << UIntShift);
		}

		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE int FindPrevSetBit(int index) const
		{
			VMEM_ASSERT(index >= 0 && index < m_Count, "index out of range");

			int set_index = m_Tier1Bits.FindPrevSetBit_InCurrentUInt(index);
			if(set_index != -1)
				return set_index;

			int tier2_index = m_Tier2Bits.FindPrevSetBit((index >> UIntShift) - 1);
			if(tier2_index == -1)
				return -1;

			return m_Tier1Bits.FindPrevSetBit_InCurrentUInt((tier2_index << UIntShift) + UIntBitCount-1);
		}

		//------------------------------------------------------------------------
		int GetMemoryUsage() const
		{
			return m_MemoryUsage;
		}

		//------------------------------------------------------------------------
		TieredBitfield(const TieredBitfield&);
		TieredBitfield& operator=(const TieredBitfield&);

		//------------------------------------------------------------------------
		// data
	private:
		Bitfield m_Tier1Bits;
		Bitfield m_Tier2Bits;

		InternalHeap& m_InternalHeap;
		uint* mp_Bits;

		int m_Count;
		int m_MemoryUsage;
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_COALESCE_HEAP_BUCKET_TREE

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_TIEREDBITFIELD_H_INCLUDED

