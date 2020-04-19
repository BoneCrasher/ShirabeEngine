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
#ifndef VMEM_LIST_H_INCLUDED
#define VMEM_LIST_H_INCLUDED

//------------------------------------------------------------------------
#include "VMemCore.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#define VMEM_LIST_FULL_CHECKING 0

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	template<class NodeT>
	class List
	{
	public:
		//------------------------------------------------------------------------
		List()
		{
			VMEM_ASSERT_CODE(m_Count = 0);
			m_Node.mp_Prev = m_Node.mp_Next = &m_Node;
		}

		//------------------------------------------------------------------------
		bool Empty() const
		{
			return m_Node.mp_Next == &m_Node;
		}

		//------------------------------------------------------------------------
		NodeT* GetHead() const
		{
			return m_Node.mp_Next;
		}

		//------------------------------------------------------------------------
		NodeT* GetTail() const
		{
			return m_Node.mp_Prev;
		}

		//------------------------------------------------------------------------
		NodeT* GetIterEnd()
		{
			return &m_Node;
		}

		//------------------------------------------------------------------------
		const NodeT* GetIterEnd() const
		{
			return &m_Node;
		}

		//------------------------------------------------------------------------
		void AddHead(NodeT* p_node)
		{
			#if VMEM_LIST_FULL_CHECKING
				CheckIntegrity();
				VMEM_ASSERT(!Contains(p_node), "VMem List has been corrupted");
			#endif

			Insert(&m_Node, p_node);
		}

		//------------------------------------------------------------------------
		void AddTail(NodeT* p_node)
		{
			#if VMEM_LIST_FULL_CHECKING
				CheckIntegrity();
				VMEM_ASSERT(!Contains(p_node), "VMem List has been corrupted");
			#endif

			Insert(m_Node.mp_Prev, p_node);
		}

		//------------------------------------------------------------------------
		// insert p_node after p_prev
		void Insert(NodeT* p_prev, NodeT* p_node)
		{
			#if VMEM_LIST_FULL_CHECKING
				CheckIntegrity();
				VMEM_ASSERT(!Contains(p_node), "VMem List has been corrupted");
				VMEM_ASSERT(p_prev == &m_Node || Contains(p_prev), "VMem List has been corrupted");
			#endif
			VMEM_ASSERT(!p_node->mp_Next && !p_node->mp_Prev, "VMem list has been corrupted");

			NodeT* p_next = p_prev->mp_Next;
			p_prev->mp_Next = p_node;
			p_node->mp_Prev = p_prev;
			p_node->mp_Next = p_next;
			p_next->mp_Prev = p_node;

			VMEM_ASSERT_CODE(++m_Count);
		}

		//------------------------------------------------------------------------
		void Remove(NodeT* p_node)
		{
			#if VMEM_LIST_FULL_CHECKING
				CheckIntegrity();
				VMEM_ASSERT(Contains(p_node), "VMem List has been corrupted");
			#endif

			NodeT* p_prev = p_node->mp_Prev;
			NodeT* p_next = p_node->mp_Next;

			p_prev->mp_Next = p_next;
			p_next->mp_Prev = p_prev;

			p_node->mp_Prev = p_node->mp_Next = NULL;

			VMEM_ASSERT_CODE(--m_Count);
		}

		//------------------------------------------------------------------------
		NodeT* RemoveHead()
		{
			#if VMEM_LIST_FULL_CHECKING
				CheckIntegrity();
			#endif
			VMEM_ASSERT(!Empty(), "VMem List had been corrupted");

			NodeT* p_node = m_Node.mp_Next;
			Remove(p_node);

			return p_node;
		}

		//------------------------------------------------------------------------
		NodeT* RemoveTail()
		{
			#if VMEM_LIST_FULL_CHECKING
				CheckIntegrity();
			#endif
			VMEM_ASSERT(!Empty(), "VMem List had been corrupted");

			NodeT* p_node = m_Node.mp_Prev;
			Remove(p_node);

			return p_node;
		}

		//------------------------------------------------------------------------
		void CheckIntegrity() const
		{
#ifdef VMEM_ASSERTS
			int count = 0;
			const NodeT* p_iter_end = &m_Node;
			for(const NodeT* p_iter = m_Node.mp_Next; p_iter != p_iter_end; p_iter = p_iter->mp_Next)
			{
				++count;
				VMEM_ASSERT(count <= m_Count, "VMem list has been corrupted");
			}

			VMEM_ASSERT(count == m_Count, "VMem list has been corrupted");
#endif
		}

		//------------------------------------------------------------------------
#ifdef VMEM_ASSERTS
		int GetCount() const
		{
			return m_Count;
		}
#endif

	private:
		//------------------------------------------------------------------------
#if VMEM_LIST_FULL_CHECKING
		bool Contains(NodeT* p_node) const
		{
			const NodeT* p_iter_end = &m_Node;
			for(const NodeT* p_iter = m_Node.mp_Next; p_iter != p_iter_end; p_iter = p_iter->mp_Next)
			{
				if(p_iter == p_node)
					return true;
			}

			return false;
		}
#endif

		//------------------------------------------------------------------------
		// data
	private:
		NodeT m_Node;

		#ifdef VMEM_ASSERTS
			int m_Count;
		#endif
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_LIST_H_INCLUDED

