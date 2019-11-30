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
#include "PhysicalPageHeap.hpp"
#include "VMemCore.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//-----------------------------------------------------------------------------
namespace VMem
{
	//-----------------------------------------------------------------------------
	PhysicalPageHeap::PhysicalPageHeap()
	:	m_Initialised(false),
		m_PageSize(0),
		m_CommitFlags(0),
		m_MaxFreeRangeSize(0),
		m_MaxFreeRangeSizeDirty(0)
	{
	}

	//-----------------------------------------------------------------------------
	void PhysicalPageHeap::Initialise(int page_size, int commit_flags)
	{
		m_Initialised = true;

		m_PageSize = page_size;

		m_CommitFlags = commit_flags;
	}

	//-----------------------------------------------------------------------------
	void PhysicalPageHeap::Clear()
	{
		for (int i = 0; i<m_RangeNodeListCount; ++i)
		{
			List<RangeNode>& list = m_RangeNodeLists[i];
			while (!list.Empty())
				m_RangeNodeFSA.Free(list.RemoveHead());
		}

		m_RangeMap.Clear();

		m_MaxFreeRangeSize = 0;
	}

	//-----------------------------------------------------------------------------
	bool PhysicalPageHeap::FindAndRemoveRange(size_t size, Range& range)
	{
		int range_list_index = GetRangeNodeListIndex(size);

		while (range_list_index < m_RangeNodeListCount - 1 && m_RangeNodeLists[range_list_index].Empty())
			++range_list_index;

		List<RangeNode>& list = m_RangeNodeLists[range_list_index];

		if(range_list_index < m_RangeNodeListCount - 1)
		{
			RangeNode* p_node = list.RemoveHead();
			VMEM_ASSERT(p_node->m_Range.m_Size >= size, "RangeHeap has been corrupted");
			range = p_node->m_Range;
			m_RangeNodeFSA.Free(p_node);
			return true;
		}
		else
		{
			RangeNode* p_node = list.GetHead();
			while(p_node != list.GetIterEnd() && p_node->m_Range.m_Size < size)
				p_node = p_node->mp_Next;

			if(p_node != list.GetIterEnd())
			{
				range = p_node->m_Range;
				list.Remove(p_node);
				m_RangeNodeFSA.Free(p_node);
				return true;
			}
		}

		return false;
	}

	//-----------------------------------------------------------------------------
	void* PhysicalPageHeap::Alloc(size_t size)
	{
		VMEM_ASSERT((size % m_PageSize) == 0, "invalid size in PhysicalPageHeap");

		Range range;
		if(FindAndRemoveRange(size, range))
		{
			RangeMap<RangeNode*>::Range range_map_range(range.mp_Mem, (byte*)range.mp_Mem + range.m_Size, 0);
			m_RangeMap.Remove(range_map_range);

			VMEM_ASSERT(range.m_Size >= size, "PhysicalPageHeap has been corrupted");
			size_t offcut_size = range.m_Size - size;
			if (offcut_size)
			{
				bool added = Free((byte*)range.mp_Mem + size, offcut_size);
				VMEM_ASSERT(added, "m_RangeMap has been corrupted");	// should always succeed because we've just removed an item
				VMEM_UNREFERENCED_PARAM(added);
			}

			if (range.m_Size == m_MaxFreeRangeSize)
				m_MaxFreeRangeSizeDirty = true;

			return range.mp_Mem;
		}

		return NULL;
	}

	//-----------------------------------------------------------------------------
	bool PhysicalPageHeap::Free(void* p, size_t size)
	{
		VMEM_ASSERT((size % m_PageSize) == 0, "invalid size in PhysicalPageHeap");

		RangeMap<RangeNode*>::Range prev_range;
		bool coalesce_prev = m_RangeMap.TryGetRange((byte*)p - 1, prev_range);

		void* p_end = (byte*)p + size;
		RangeMap<RangeNode*>::Range next_range;
		bool coalesce_next = m_RangeMap.TryGetRange(p_end, next_range);
	
		if(coalesce_prev)
		{
			// coalesce prev

			if(coalesce_next)
			{
				// coalesce prev and next
				p_end = next_range.mp_End;
				m_RangeMap.Remove(next_range);
				RemoveRange(next_range.m_Value);
			}

			m_RangeMap.Remove(prev_range);
			prev_range.mp_End = p_end;
			bool added = m_RangeMap.Add(prev_range);
			VMEM_ASSERT(added, "m_RangeMap has been corrupted");	// should always succeed because we've just removed an item
			VMEM_UNREFERENCED_PARAM(added);

			RangeNode* p_node = prev_range.m_Value;
			RemoveRange(p_node);
			p_node->m_Range.m_Size = (byte*)p_end - (byte*)prev_range.mp_Start;
			AddRange(p_node);
		}
		else if(coalesce_next)
		{
			// coalesce next
			m_RangeMap.Remove(next_range);
			next_range.mp_Start = p;
			bool added = m_RangeMap.Add(next_range);
			VMEM_ASSERT(added, "m_RangeMap has been corrupted");	// should always succeed because we've just removed an item
			VMEM_UNREFERENCED_PARAM(added);

			RangeNode* p_node = next_range.m_Value;
			RemoveRange(p_node);
			p_node->m_Range.mp_Mem = p;
			p_node->m_Range.m_Size += size;
			AddRange(p_node);
		}
		else
		{
			// no coalesce, just add
			RangeNode* p_node = m_RangeNodeFSA.Alloc();
			if(!p_node)
				return false;

			p_node->mp_Prev = p_node->mp_Next = NULL;
			p_node->m_Range.mp_Mem = p;
			p_node->m_Range.m_Size = size;
			AddRange(p_node);

			RangeMap<RangeNode*>::Range range;
			range.mp_Start = p;
			range.mp_End = (byte*)p + size;
			range.m_Value = p_node;

			if(!m_RangeMap.Add(range))
			{
				RemoveRange(p_node);
				m_RangeNodeFSA.Free(p_node);
				return false;
			}
		}

		if(size > m_MaxFreeRangeSize)
			m_MaxFreeRangeSize = size;

		return true;
	}

	//------------------------------------------------------------------------
	void PhysicalPageHeap::AddRange(RangeNode* p_node)
	{
		size_t size = p_node->m_Range.m_Size;

		if (size > m_MaxFreeRangeSize)
			m_MaxFreeRangeSize = size;

		int range_list_index = GetRangeNodeListIndex(size);
		m_RangeNodeLists[range_list_index].AddHead(p_node);
	}

	//------------------------------------------------------------------------
	void PhysicalPageHeap::RemoveRange(RangeNode* p_node)
	{
		size_t size = p_node->m_Range.m_Size;

		if (size == m_MaxFreeRangeSize)
			m_MaxFreeRangeSizeDirty = true;

		int range_list_index = GetRangeNodeListIndex(size);
		m_RangeNodeLists[range_list_index].Remove(p_node);
	}

	//------------------------------------------------------------------------
	int PhysicalPageHeap::GetRangeNodeListIndex(size_t size)
	{
		return VMin(ToInt(size / m_PageSize), m_RangeNodeListCount - 1);
	}

	//------------------------------------------------------------------------
	void PhysicalPageHeap::GiveMemory(void* p_mem, size_t size)
	{
		bool range_map_full = m_RangeMap.IsFull();
		bool fsa_empty = m_RangeNodeFSA.IsEmpty();

		if(range_map_full && fsa_empty)
		{
			int map_size = AlignDownPow2(ToInt(size / 2), VMEM_INTERNAL_ALIGNMENT);
			m_RangeMap.GiveMemory(p_mem, map_size);

			int fsa_size = AlignDownPow2(ToInt(size - map_size), VMEM_INTERNAL_ALIGNMENT);
			m_RangeNodeFSA.GiveMemory((byte*)p_mem + map_size, fsa_size);
		}
		else if(range_map_full)
		{
			m_RangeMap.GiveMemory(p_mem, size);
		}
		else
		{
			m_RangeNodeFSA.GiveMemory(p_mem, size);
		}
	}

	//------------------------------------------------------------------------
	void PhysicalPageHeap::FindMaxFreeRangeSize()
	{
		m_MaxFreeRangeSize = 0;

		// the last list holds multiple sizes, all other lists hold one size only
		int last_index = m_RangeNodeListCount - 1;
		List<RangeNode>& end_list = m_RangeNodeLists[last_index];
		if(!end_list.Empty())
		{
			for (RangeNode* p_node = end_list.GetHead(); p_node != end_list.GetIterEnd(); p_node = p_node->mp_Next)
			{
				size_t node_size = p_node->m_Range.m_Size;
				if (node_size > m_MaxFreeRangeSize)
					m_MaxFreeRangeSize = node_size;
			}
		}
		else
		{
			for(int i = last_index-1; i >= 0; --i)
			{
				List<RangeNode>& list = m_RangeNodeLists[i];
				RangeNode* p_node = list.GetHead();
				if(p_node != list.GetIterEnd())
				{
					m_MaxFreeRangeSize = p_node->m_Range.m_Size;
					break;
				}
			}
		}

		m_MaxFreeRangeSizeDirty = false;
	}

	//------------------------------------------------------------------------
	size_t PhysicalPageHeap::GetMaxFreeRangeSize()
	{
		if(m_MaxFreeRangeSizeDirty)
			FindMaxFreeRangeSize();

		#ifdef VMEM_ASSERTS
			size_t check_max = 0;
			for(int i=0; i<m_RangeNodeListCount; ++i)
			{
				List<RangeNode>& list = m_RangeNodeLists[i];
				for(RangeNode* p_node = list.GetHead(); p_node != list.GetIterEnd(); p_node = p_node->mp_Next)
					if(p_node->m_Range.m_Size > check_max)
						check_max = p_node->m_Range.m_Size;
			}
			VMEM_ASSERT(m_MaxFreeRangeSize == check_max, "m_RangeNodeLists has been corrupted");
		#endif

		return m_MaxFreeRangeSize;
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	size_t PhysicalPageHeap::GetMemorySize() const
	{
		return m_RangeMap.GetMemorySize();
	}
#endif
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE


