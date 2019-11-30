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
//--------------------------------------------------------------
#ifndef VMEM_RANGEMAP_H_INCLUDED
#define VMEM_RANGEMAP_H_INCLUDED

//--------------------------------------------------------------
#include "VMemRedBlackTree.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//--------------------------------------------------------------
namespace VMem
{
	//--------------------------------------------------------------
	template<class TRangeValue>
	class RangeMap
	{
	public:
		//--------------------------------------------------------------
		struct Range
		{
			Range()
			:	mp_Start(NULL),
				mp_End(NULL),
				m_Value(TRangeValue())
			{
			}

			Range(void* p_start, void* p_end, TRangeValue value)
			:	mp_Start(p_start),
				mp_End(p_end),
				m_Value(value)
			{
			}

			bool operator<(const Range& other) const
			{
				return mp_Start < other.mp_Start;
			}

			bool operator==(const Range& other) const
			{
				VMEM_ASSERT(!(mp_Start == other.mp_Start && mp_End != other.mp_End), "RangeMap range has been corrupted");
				return mp_Start == other.mp_Start;
			}

			void* mp_Start;
			void* mp_End;		// end is exclusive (size = mp_End - mp_Start)
			TRangeValue m_Value;
		};

		//--------------------------------------------------------------
		RangeMap(InternalAllocator* p_allocator = NULL, int page_size = 0)
		:	m_RedBlackTree(p_allocator, page_size)
		{
		}

		//--------------------------------------------------------------
		void Clear()
		{
			m_RedBlackTree.Clear();
		}

		//--------------------------------------------------------------
		bool IsFull() const
		{
			return m_RedBlackTree.IsFull();
		}

		//--------------------------------------------------------------
		bool Add(const Range& range)
		{
			VMEM_ASSERT(range.mp_End > range.mp_Start, "bad range passed in to RangeMap::Add");

			Range existing_range;
			VMEM_ASSERT(!TryGetRange(range.mp_Start, existing_range) && !TryGetRange((byte*)range.mp_End - 1, existing_range), "RangeMap has been corrupted. Adding overlapping range");
			VMEM_UNREFERENCED_PARAM(existing_range);

			return m_RedBlackTree.Insert(range);
		}

		//--------------------------------------------------------------
		void Remove(const Range& range)
		{
			m_RedBlackTree.Remove(range);
		}

		//--------------------------------------------------------------
		bool TryGetRange(void* p, Range& range)
		{
			const typename RedBlackTree<Range>::Node* p_node = m_RedBlackTree.GetRoot();

			if (!p_node)
				return false;

			while (!p_node->IsNull())
			{
				Range node_range = p_node->m_Value;

				if (p < node_range.mp_Start)
				{
					p_node = p_node->mp_Child[0];
				}
				else if (p < (byte*)node_range.mp_End)
				{
					range = node_range;
					return true;
				}
				else
				{
					p_node = p_node->mp_Child[1];
				}
			}

			return false;
		}

		//--------------------------------------------------------------
		bool TryRemoveRange(void* p, size_t size, TRangeValue& value)
		{
			void* p_start = p;
			void* p_end = (byte*)p + size;

			typename RedBlackTree<Range>::Node* p_node = m_RedBlackTree.GetRoot();

			if (!p_node)
				return false;

			typename RedBlackTree<Range>::Node* p_overlapping_node = NULL;

			while (!p_node->IsNull())
			{
				Range node_range = p_node->m_Value;

				if (p_start < node_range.mp_Start)
				{
					if (p_end > node_range.mp_Start)
						p_overlapping_node = p_node;

					p_node = p_node->mp_Child[0];
				}
				else if (p_start < (byte*)node_range.mp_End)
				{
					value = node_range.m_Value;
					m_RedBlackTree.Remove(p_node);
					return true;
				}
				else
				{
					p_node = p_node->mp_Child[1];
				}
			}

			if(p_overlapping_node)
			{
				value = p_overlapping_node->m_Value.m_Value;
				m_RedBlackTree.Remove(p_overlapping_node);
				return true;
			}

			return false;
		}

		//--------------------------------------------------------------
		void GiveMemory(void* p, size_t size)
		{
			m_RedBlackTree.GiveMemory(p, size);
		}

		//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
		size_t GetMemorySize() const
		{
			return m_RedBlackTree.GetMemorySize();
		}
#endif

		//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
		const Stats& GetStats() const
		{
			return m_RedBlackTree.GetStats();
		}
#endif

	private:
		//--------------------------------------------------------------
		RangeMap(const RangeMap&);
		RangeMap& operator=(const RangeMap&);

		//--------------------------------------------------------------
		// data
	private:
		RedBlackTree<Range> m_RedBlackTree;
	};
}

//--------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//--------------------------------------------------------------
#endif		// #ifndef VMEM_RANGEMAP_H_INCLUDED

