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
#ifndef VMEM_BUCKET_TREE_H_INCLUDED
#define VMEM_BUCKET_TREE_H_INCLUDED

//------------------------------------------------------------------------
#include "VMemCore.hpp"
#include "InternalHeap.hpp"
#include "VMemStats.hpp"
#include "TieredBitfield.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE

//------------------------------------------------------------------------
#define VMEM_BUCKET_TREE_MARKER 0xbbbbbbbb

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	template<typename BucketT>
	class BucketTree
	{
	public:
		//------------------------------------------------------------------------
		BucketTree(int value_count, int bucket_size, InternalHeap& internal_heap)
		:	m_Bitfield(internal_heap),
			m_BucketShift(0),
			m_BucketCount(0),
			m_ValueCount(value_count),
			mp_Buckets(NULL),
			m_InternalHeap(internal_heap),
			m_MemoryUsage(0),
			m_LastUsedBucketIndex(-1),
			m_Padding(VMEM_BUCKET_TREE_MARKER)
		{
			VMEM_ASSERT(IsPow2(bucket_size), "bucket size must be a power of 2");
			while((1 << m_BucketShift) != bucket_size)
				++m_BucketShift;

			m_BucketCount = (value_count+bucket_size-1) >> m_BucketShift;
		}

		//------------------------------------------------------------------------
		bool Initialise()
		{
			if(!m_Bitfield.Initialise(m_BucketCount))
				return false;

			int buckets_size = ToInt(m_BucketCount * sizeof(BucketT));
			mp_Buckets = (BucketT*)m_InternalHeap.Alloc(buckets_size);
			if(!mp_Buckets)
				return false;

			for(int i=0; i<m_BucketCount; ++i)
				new (mp_Buckets+i)BucketT();

			m_MemoryUsage = buckets_size + m_Bitfield.GetMemoryUsage();

			return true;
		}

		//------------------------------------------------------------------------
		~BucketTree()
		{
			if(mp_Buckets)
			{
				for(int i=0; i<m_BucketCount; ++i)
					mp_Buckets[i].~BucketT();

				m_InternalHeap.Free(mp_Buckets);
			}
		}

		//------------------------------------------------------------------------
		int GetBucket(int value)
		{
			VMEM_ASSERT(value >= 0 && value < m_ValueCount, "value of of range");

			int bucket_index = value >> m_BucketShift;
			m_Bitfield.Set(bucket_index);

			if(bucket_index > m_LastUsedBucketIndex)
				m_LastUsedBucketIndex = bucket_index;

			return bucket_index;
		}

		//------------------------------------------------------------------------
		// find the first non-empty bucket big enough to hold value
		int FindExistingBucket(int value)
		{
			VMEM_ASSERT(value >= 0 && value < m_ValueCount, "value of of range");

			int bucket_index = value >> m_BucketShift;
			return m_Bitfield.FindNextSetBit(bucket_index);
		}

		//------------------------------------------------------------------------
		void ReleaseBucket(int bucket_index)
		{
			VMEM_ASSERT(bucket_index >= 0 && bucket_index < m_BucketCount, "bucket_index out of range");
			m_Bitfield.Clear(bucket_index);

			if(bucket_index == m_LastUsedBucketIndex)
				m_LastUsedBucketIndex = m_Bitfield.FindPrevSetBit(bucket_index);
		}

		//------------------------------------------------------------------------
		int GetLastBucketIndex() const
		{
			return m_LastUsedBucketIndex;
		}

		//------------------------------------------------------------------------
		int GetBucketCount() const
		{
			return m_BucketCount;
		}

		//------------------------------------------------------------------------
		BucketT& operator[](int index)
		{
			return mp_Buckets[index];
		}

		//------------------------------------------------------------------------
		const BucketT& operator[](int index) const
		{
			return mp_Buckets[index];
		}

		//------------------------------------------------------------------------
		int GetMemoryUsage() const
		{
			return m_MemoryUsage;
		}

	private:
		//------------------------------------------------------------------------
		static bool IsPow2(unsigned int value)
		{
			return (value & (value-1)) == 0;
		}

		//------------------------------------------------------------------------
		// data
	private:
		TieredBitfield m_Bitfield;

		int m_BucketShift;
		int m_BucketCount;
		int m_ValueCount;
		int m_MemoryUsage;
		int m_LastUsedBucketIndex;
		int m_Padding;

		BucketT* mp_Buckets;

		InternalHeap& m_InternalHeap;
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_COALESCE_HEAP_BUCKET_TREE

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_BUCKET_TREE_H_INCLUDED

