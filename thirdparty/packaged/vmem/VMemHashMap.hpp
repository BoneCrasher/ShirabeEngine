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
//-----------------------------------------------------------------
#ifndef VMEM_VMEMHASHMAP_H_INCLUDED
#define VMEM_VMEMHASHMAP_H_INCLUDED

//-----------------------------------------------------------------
#include "VMemCore.hpp"
#include "VMemSys.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//-----------------------------------------------------------------
//#define VMEM_PROFILE_HASHMAP

//-----------------------------------------------------------------
#ifdef VMEM_PROFILE_HASHMAP
	#include <time.h>
#endif

//-----------------------------------------------------------------
namespace VMem
{
	//-----------------------------------------------------------------
	template<typename TKey, typename TValue>
	class HashMap
	{
	public:
		//-----------------------------------------------------------------
		struct Pair
		{
			TKey m_Key;
			TValue m_Value;
		};

		//-----------------------------------------------------------------
		// Iterator used to iterate over the items of the set.
		// Must call MoveNext to move to the first element. To be used like
		// this:
		//	HashMap<TKey, TValue>::Iteartor iter(set);
		//	while(iter.MoveNext())
		//	{
		//		const HashMap<TKey, TValue>::Pair& pair = iter.GetValue();
		//		//...
		//	}
		class Iterator
		{
			friend class HashMap;

		public:
			//-----------------------------------------------------------------
			Iterator(const HashMap& hash_map)
			{
				Initialise(&hash_map);
			}

			//-----------------------------------------------------------------
			// if this iterator has reached the end do nothing and return false
			// otherwise move to the next item and return true.
			// The first time this is called moves to the first element.
			bool MoveNext()
			{
				const int capacity = mp_HashMap->m_Capacity;
				if(m_Index == capacity)
				{
					return false;
				}

				// move to the next non-empty item
				++m_Index;
				++mp_Pair;

				if(m_Index == capacity)
				{
					mp_Pair = NULL;
					return false;
				}

				while(!mp_HashMap->IsItemInUse(m_Index))
				{
					++m_Index;
					++mp_Pair;

					if(m_Index == capacity)
					{
						mp_Pair = NULL;
						return false;
					}
				}

				return true;
			}

			//-----------------------------------------------------------------
			const TKey& GetKey() const
			{
				VMEM_ASSERT(mp_Pair, "");		// Iterator not at a valid location
				VMEM_ASSUME(mp_Pair);
				return (*mp_Pair)->m_Key;
			}

			//-----------------------------------------------------------------
			TValue& GetValue()
			{
				VMEM_ASSERT(mp_Pair, "");		// Iterator not at a valid location
				VMEM_ASSUME(mp_Pair);
				return (*mp_Pair)->m_Value;
			}

		private:
			//-----------------------------------------------------------------
			void Initialise(const HashMap* p_hash_map)
			{
				mp_HashMap = p_hash_map;

				if(p_hash_map->m_Capacity)
				{
					m_Index = -1;
					mp_Pair = p_hash_map->mp_Table - 1;
				}
				else
				{
					m_Index = 0;
					mp_Pair = NULL;
				}
			}

			//-----------------------------------------------------------------
			// data
		private:
			const HashMap<TKey, TValue>* mp_HashMap;
			int m_Index;
			Pair** mp_Pair;
		};

		//-----------------------------------------------------------------
		// The default capacity of the set. The capacity is the number
		// of elements that the set is expected to hold. The set will resized
		// when the item count is greater than the capacity;
		HashMap(int page_size)
		:	m_PageSize(page_size),
			m_Capacity(0),
			mp_Table(NULL),
			m_Count(0),
			mp_ItemPool(NULL),
			mp_FreePair(NULL),
			m_AllocedMemory(0)
#ifdef VMEM_PROFILE_HASHMAP
			,m_IterAcc(0)
			,m_IterCount(0)
#endif
		{
		}

		//-----------------------------------------------------------------
		bool Initialise(int capacity=m_DefaultCapacity)
		{
			if(capacity)
			{
				if(!AllocTable(GetNextPow2((256 * capacity) / m_Margin)))
					return false;
			}

			return true;
		}

		//-----------------------------------------------------------------
		bool Initialised() const
		{
			return mp_Table != NULL;
		}

		//-----------------------------------------------------------------
		~HashMap()
		{
			Clear();
		}

		//-----------------------------------------------------------------
		void Clear()
		{
			if(mp_Table)
			{
				for(int i=0; i<m_Capacity; ++i)
				{
					Pair* p_pair = mp_Table[i];
					if(p_pair)
						p_pair->~Pair();
				}

				InternalFree(mp_Table, m_Capacity * sizeof(Pair*));
			}

			m_Capacity = 0;
			mp_Table = NULL;
			m_Count = 0;

			FreePools();
		}

		//-----------------------------------------------------------------
		// Add a value to this set.
		// If this set already contains the value does nothing.
		bool Add(const TKey& key, const TValue& value)
		{
			if(m_Capacity == 0 || m_Count == (m_Margin * m_Capacity) / 256)
			{
				if(!Grow())
					return false;
			}

			const int index = GetItemIndex(key);
			
			VMEM_ASSERT(mp_Table, "mp_Table is null");

			if(IsItemInUse(index))
			{
				mp_Table[index]->m_Value = value;
			}
			else
			{
				// make a copy of the value
				Pair* p_pair = AllocPair();
				if(!p_pair)
					return false;
				p_pair->m_Key = key;
				p_pair->m_Value = value;

				// add to table
				mp_Table[index] = p_pair;

				++m_Count;
			}

			return true;
		}

		//-----------------------------------------------------------------
		// if this set contains the value set value to the existing value and
		// return true, otherwise set to the default value and return false.
		bool TryGetValue(const TKey& key, TValue& value) const
		{
			if(!mp_Table)
				return false;

			const int index = GetItemIndex(key);
			if(IsItemInUse(index))
			{
				value = mp_Table[index]->m_Value;
				return true;
			}
			else
			{
				return false;
			}
		}

		//-----------------------------------------------------------------
		// If this set contains the specifed value remove it
		// and return true, otherwise do nothing and return false.
		TValue Remove(const TKey& key)
		{
			VMEM_ASSERT(mp_Table, "null table");
			VMEM_ASSUME(mp_Table);

			int remove_index = GetItemIndex(key);
			VMEM_ASSERT(IsItemInUse(remove_index), "item no in use");

			Pair* p_pair = mp_Table[remove_index];

			TValue value = p_pair->m_Value;

			// find first index in this array
			int srch_index = remove_index;
			int first_index = remove_index;
			if(!srch_index)
			{
				srch_index = m_Capacity;
			}
			--srch_index;
			while(IsItemInUse(srch_index))
			{
				first_index = srch_index;
				if(!srch_index)
				{
					srch_index = m_Capacity;
				}
				--srch_index;
			}

			bool found = false;
			for(;;)
			{
				// find the last item in the array that can replace the item being removed
				int i = (remove_index + 1) & (m_Capacity-1);

				int swap_index = m_InvalidIndex;
				while(IsItemInUse(i))
				{
					const unsigned int srch_hash_code = mp_Table[i]->m_Key.GetHashCode();
					const int srch_insert_index = srch_hash_code & (m_Capacity-1);

					if(InRange(srch_insert_index, first_index, remove_index))
					{
						swap_index = i;
						found = true;
					}

					i = (i + 1) & (m_Capacity-1);
				}

				// swap the item
				if(found)
				{
					mp_Table[remove_index] = mp_Table[swap_index];
					remove_index = swap_index;
					found = false;
				}
				else
				{
					break;
				}
			}

			// remove the last item
			mp_Table[remove_index] = NULL;

			// free this item
			FreePair(p_pair);

			--m_Count;
			return value;
		}

		//-----------------------------------------------------------------
		Iterator GetIterator() const
		{
			return Iterator(*this);
		}

		//-----------------------------------------------------------------
		int GetCount() const
		{
			return m_Count;
		}

		//-----------------------------------------------------------------
		const TValue& operator[](const TKey& key) const
		{
			const int index = GetItemIndex(key);
			VMEM_ASSERT(IsItemInUse(index), "");
			return mp_Table[index]->m_Value;
		}

		//-----------------------------------------------------------------
		bool Resize(int new_capacity)
		{
			new_capacity = GetNextPow2(new_capacity);

			// keep a copy of the old table
			Pair** p_old_table = mp_Table;
			const int old_capacity = m_Capacity;

			// allocate the new table
			if(!AllocTable(new_capacity))
				return false;

			// copy the values from the old to the new table
			Pair** p_old_pair = p_old_table;
			for(int i=0; i<old_capacity; ++i, ++p_old_pair)
			{
				Pair* p_pair = *p_old_pair;
				if(p_pair)
				{
					const int index = GetItemIndex(p_pair->m_Key);
					mp_Table[index] = p_pair;
				}
			}

			if(p_old_table)
				InternalFree(p_old_table, old_capacity * sizeof(Pair*));

			return true;
		}

		//-----------------------------------------------------------------
		int GetAllocedMemory() const
		{
			return m_AllocedMemory;
		}

	private:
		//-----------------------------------------------------------------
		void* InternalAlloc(int size)
		{
			size = AlignUpPow2(size, m_PageSize);
			void* p = VMem::VirtualReserve(size, m_PageSize, VMEM_DEFAULT_RESERVE_FLAGS);
			if(!p)
				return NULL;

			if(!VMem::VirtualCommit(p, size, m_PageSize, VMEM_DEFAULT_COMMIT_FLAGS))
			{
				VMem::VirtualRelease(p, size);
				return NULL;
			}

			m_AllocedMemory += size;

			return p;
		}

		//-----------------------------------------------------------------
		void InternalFree(void* p, int size)
		{
			size = AlignUpPow2(size, m_PageSize);
			VMem::VirtualDecommit(p, size, m_PageSize, VMEM_DEFAULT_COMMIT_FLAGS);
			VMem::VirtualRelease(p, size);
			m_AllocedMemory -= size;
		}

		//-----------------------------------------------------------------
		static int GetNextPow2(int value)
		{
			int p = 2;
			while(p < value)
				p *= 2;
			return p;
		}

		//-----------------------------------------------------------------
		bool AllocTable(const int capacity)
		{
			VMEM_ASSERT(capacity < m_MaxCapacity, "");

			// allocate a block of memory for the table
			if(capacity > 0)
			{
				const int size = capacity * sizeof(Pair*);
				Pair** p_table = (Pair**)InternalAlloc(size);
				if(!p_table)
					return false;

				mp_Table = p_table;
				memset(p_table, 0, size);
			}

			m_Capacity = capacity;

			return true;
		}

		//-----------------------------------------------------------------
		bool IsItemInUse(const int index) const
		{
			return mp_Table[index] != NULL;
		}

		//-----------------------------------------------------------------
		int GetItemIndex(const TKey& key) const
		{
			VMEM_ASSERT(mp_Table, "");
			VMEM_ASSUME(mp_Table);

			const unsigned int hash = key.GetHashCode();
			int srch_index = hash & (m_Capacity-1);
			while(IsItemInUse(srch_index) && !(mp_Table[srch_index]->m_Key == key))
			{
				srch_index = (srch_index + 1) & (m_Capacity-1);
#ifdef VMEM_PROFILE_HASHMAP
				++m_IterAcc;
#endif
			}

#ifdef VMEM_PROFILE_HASHMAP
			++m_IterCount;
			double average = m_IterAcc / (double)m_IterCount;
			if(average > 2.0)
			{
				static int last_write_time = 0;
				int now = clock();
				if(now - last_write_time > 1000)
				{
					last_write_time = now;
					DebugWrite("WARNING: HashMap average: %f\n", (float)average);
				}
			}
#endif
			return srch_index;
		}

		//-----------------------------------------------------------------
		// Increase the capacity of the table.
		bool Grow()
		{
			const int new_capacity = m_Capacity ? 2*m_Capacity : m_DefaultCapacity;
			return Resize(new_capacity);
		}

		//-----------------------------------------------------------------
		static bool InRange(
			const int index,
			const int start_index,
			const int end_index)
		{
			return (start_index <= end_index) ?
				index >= start_index && index <= end_index :
				index >= start_index || index <= end_index;
		}

		//-----------------------------------------------------------------
		void FreePools()
		{
			byte* p_pool = mp_ItemPool;
			while(p_pool)
			{
				byte* p_next_pool = *(byte**)p_pool;
				InternalFree(p_pool, m_PageSize);
				p_pool = p_next_pool;
			}
			mp_ItemPool = NULL;
			mp_FreePair = NULL;
		}

		//-----------------------------------------------------------------
		Pair* AllocPair()
		{
			if(!mp_FreePair)
			{
				// allocate a new pool and link to pool list
				byte* p_new_pool = (byte*)InternalAlloc(m_PageSize);
				if(!p_new_pool)
					return NULL;
				*(byte**)p_new_pool = mp_ItemPool;
				mp_ItemPool = p_new_pool;

				// link all items onto free list
				mp_FreePair = p_new_pool + sizeof(Pair);
				byte* p = (byte*)mp_FreePair;
				int item_count = m_PageSize / sizeof(Pair) - 2;	// subtract 2 for pool pointer and last item
				VMEM_ASSERT(item_count, "");
				for(int i=0; i<item_count; ++i, p+=sizeof(Pair))
				{
					*(byte**)p = p + sizeof(Pair);
				}
				*(byte**)p = NULL;
			}

			// take item off free list
			Pair* p_pair = (Pair*)mp_FreePair;
			mp_FreePair = *(byte**)mp_FreePair;

			// construct the pair
			new (p_pair)Pair;

			return p_pair;
		}

		//-----------------------------------------------------------------
		void FreePair(Pair* p_pair)
		{
			p_pair->~Pair();

			*(byte**)p_pair = mp_FreePair;
			mp_FreePair = (byte*)p_pair;
		}

		//-----------------------------------------------------------------
		// data
	private:
		static const int m_DefaultCapacity = 1024;
		static const int m_InvalidIndex = 0xffffffff;
		static const int m_MaxCapacity = 0x7fffffff;
		static const int m_Margin = (40 * 256) / 100;

		int m_PageSize;

		int m_Capacity;			// the current capacity of this set, will always be >= m_Margin*m_Count/256
		Pair** mp_Table;		// NULL for a set with capacity 0
		int m_Count;			// the current number of items in this set, will always be <= m_Margin*m_Count/256

		byte* mp_ItemPool;
		byte* mp_FreePair;

		int m_AllocedMemory;

#ifdef VMEM_PROFILE_HASHMAP
		mutable uint64 m_IterAcc;
		mutable uint64 m_IterCount;
#endif
	};

	//------------------------------------------------------------------------
	struct AddrKey
	{
		AddrKey() : m_Addr(0) {}

		AddrKey(void* p)
		:	m_Addr(p),
			m_HashCode(CalculateHashCode())
		{
		}

		bool operator==(const AddrKey& other) const { return m_Addr == other.m_Addr; }

		void* GetAddr() const { return m_Addr; }

		unsigned int GetHashCode() const { return m_HashCode; }

	private:
		unsigned int CalculateHashCode() const
		{
#ifdef VMEM_X64
			uint64 key = (uint64)m_Addr;
			key = (~key) + (key << 18);
			key = key ^ (key >> 31);
			key = key * 21;
			key = key ^ (key >> 11);
			key = key + (key << 6);
			key = key ^ (key >> 22);
			return (unsigned int)key;
#else
			unsigned int a = (unsigned int)m_Addr;
			a = (a ^ 61) ^ (a >> 16);
			a = a + (a << 3);
			a = a ^ (a >> 4);
			a = a * 0x27d4eb2d;
			a = a ^ (a >> 15);
			return a;
#endif
		}

		//------------------------------------------------------------------------
		// data
	private:
		void* m_Addr;
		unsigned int m_HashCode;
	};
}

//-----------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//-----------------------------------------------------------------
#endif		// #ifndef VMEM_VMEMHASHMAP_H_INCLUDED

