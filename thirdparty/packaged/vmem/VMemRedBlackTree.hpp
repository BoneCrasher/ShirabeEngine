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
#ifndef VMEM_REDBLACKTREE_H_INCLUDED
#define VMEM_REDBLACKTREE_H_INCLUDED

//--------------------------------------------------------------
#include "VMemStats.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//--------------------------------------------------------------
namespace VMem
{
	//--------------------------------------------------------------
	template<typename T>
	class RedBlackTree
	{
		//--------------------------------------------------------------
		struct MemPool
		{
			MemPool* mp_Next;
			size_t m_Size;
		};

	public:
		//--------------------------------------------------------------
		struct Node
		{
			Node(Node* p_parent)
			:	mp_Parent(p_parent),
				m_Red(false)
			{
				mp_Child[0] = mp_Child[1] = NULL;
			}

			bool IsNull() const { return mp_Child[0] == NULL; }

			bool IsBlack() const { return !m_Red; }

			Node* mp_Parent;
			Node* mp_Child[2];
			T m_Value;
			bool m_Red;
		};

		//--------------------------------------------------------------
		RedBlackTree(InternalAllocator* p_allocator, int page_size)
		:	mp_Root(NULL),
			m_Count(0),
			m_NextMemPoolSize(page_size),
			mp_MemPools(NULL),
			mp_FreeNodeSlots(NULL),
			mp_Allocator(p_allocator)
		{
		}

		//--------------------------------------------------------------
		~RedBlackTree()
		{
			DeleteMemPools();
		}

		//--------------------------------------------------------------
		void Clear()
		{
			mp_Root = NULL;
			m_Count = 0;

			mp_FreeNodeSlots = NULL;
			DeleteMemPools();

			#ifdef VMEM_ENABLE_STATS
				m_Stats = Stats();
			#endif
		}

		//--------------------------------------------------------------
		bool IsFull() const
		{
			// always need at least 2 free nodes for an add
			return !mp_FreeNodeSlots || !*(void**)mp_FreeNodeSlots;
		}

		//--------------------------------------------------------------
		bool Insert(T value)
		{
			Node* p_insert_node = FindInsertNode(value);
			if(!p_insert_node)
				return false;		// oom

			if(!p_insert_node->IsNull() && p_insert_node->m_Value == value)
			{
				p_insert_node->m_Value = value;
			}
			else
			{
				Node* p_new_child1 = AllocNode(p_insert_node);
				if (!p_new_child1)
					return false;

				Node* p_new_child2 = AllocNode(p_insert_node);
				if (!p_new_child2)
				{
					FreeNode(p_new_child1);
					return false;
				}

				p_insert_node->m_Value = value;
				p_insert_node->mp_Child[0] = p_new_child1;
				p_insert_node->mp_Child[1] = p_new_child2;

				if(p_insert_node != mp_Root)
				{
					p_insert_node->m_Red = true;
					FixupTreeAfterInsert(p_insert_node);
				}

				++m_Count;
			}

			return true;
		}

		//--------------------------------------------------------------
		void Remove(T value)
		{
			Node* p_node = Find(value);
			VMEM_ASSERT(p_node, "RedBlackTree has been corrupted");

			Remove(p_node);
		}

		//--------------------------------------------------------------
		void Remove(Node* p_node)
		{
			VMEM_ASSERT(p_node, "RedBlackTree has been corrupted");

			bool child1_is_null = p_node->mp_Child[0]->IsNull();
			bool child2_is_null = p_node->mp_Child[1]->IsNull();

			// case where deleting the only node in the tree
			if(p_node == mp_Root && child1_is_null && child2_is_null)
			{
				mp_Root = p_node->mp_Child[0];
				mp_Root->mp_Parent = NULL;
				FreeNode(p_node->mp_Child[1]);
				FreeNode(p_node);
			}
			else
			{
				// case 1: convert to deleting a node with 0 or 1 non-null children
				if(!child1_is_null && !child2_is_null)
				{
					// find next
					Node* p_next = p_node->mp_Child[1];
					while(!p_next->mp_Child[0]->IsNull())
						p_next = p_next->mp_Child[0];

					// swap with next
					p_node->m_Value = p_next->m_Value;
					p_node = p_next;

					child1_is_null = p_node->mp_Child[0]->IsNull();
					child2_is_null = p_node->mp_Child[1]->IsNull();
				}

				// case 2: node is red so simply delete
				if(p_node->m_Red)
				{
					DeleteLeaf(p_node);
				}
				else
				{
					// case 3: black node with one red child. Swap with child and colour black
					if(!child1_is_null && child2_is_null && p_node->mp_Child[0]->m_Red)
					{
						p_node->m_Value = p_node->mp_Child[0]->m_Value;
						p_node->m_Red = false;
						DeleteLeaf(p_node->mp_Child[0]);
					}
					else if(!child2_is_null && child1_is_null && p_node->mp_Child[1]->m_Red)
					{
						p_node->m_Value = p_node->mp_Child[1]->m_Value;
						p_node->m_Red = false;
						DeleteLeaf(p_node->mp_Child[1]);
					}
					else
					{
						// delete the node and point to the new null node
						Node* p_new_null_node = p_node->mp_Child[0];
						DeleteLeaf(p_node);
						p_node = p_new_null_node;

						FixupDoubleBlackAfterRemove(p_node);
					}
				}
			}

			VMEM_ASSERT(m_Count > 0, "RedBlackTree has been corrupted");
			--m_Count;
		}

		//--------------------------------------------------------------
		Node* GetRoot()
		{
			return mp_Root;
		}

		//--------------------------------------------------------------
		void GiveMemory(void* p, size_t size)
		{
			int count = ToInt(size / sizeof(Node) - 1);
			VMEM_ASSERT(count, "bad size passed to GiveMemory");
			p = VMem::AlignUp(p, sizeof(Node));

			void* p_first_slot = p;
			void* p_slot = p_first_slot;
			for(int i=0; i<count - 1; ++i)
			{
				void* p_next = (byte*)p_slot + sizeof(Node);
				*(void**)p_slot = p_next;
				p_slot = p_next;
			}
			*(void**)p_slot = mp_FreeNodeSlots;

			mp_FreeNodeSlots = p_first_slot;
		}

		//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
		const Stats& GetStats() const
		{
			return m_Stats;
		}
#endif

		//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
		size_t GetMemorySize() const
		{
			size_t size = 0;

			for(MemPool* p_mem_pool = mp_MemPools; p_mem_pool != NULL; p_mem_pool = p_mem_pool->mp_Next)
				size += p_mem_pool->m_Size;

			return size;
		}
#endif

	private:
		//--------------------------------------------------------------
		static bool BothChildrenBlack(Node* p_node)
		{
			return p_node->mp_Child[0]->IsBlack() && p_node->mp_Child[1]->IsBlack();
		}

		//--------------------------------------------------------------
		void FixupTreeAfterInsert(Node* p_node)
		{
			Node* p_parent = p_node->mp_Parent;
			if(p_parent->IsBlack())
				return;

			Node* p_grandparent = p_parent->mp_Parent;

			// case 1: re-colour
			Node* p_uncle = p_parent == p_grandparent->mp_Child[0] ? p_grandparent->mp_Child[1] : p_grandparent->mp_Child[0];
			while (p_parent->m_Red && p_uncle->m_Red)
			{
				p_parent->m_Red = false;
				p_uncle->m_Red = false;
				p_grandparent->m_Red = true;
				p_node = p_grandparent;
				p_parent = p_node->mp_Parent;

				if (!p_parent)
				{
					p_node->m_Red = false;
					return;
				}

				if(!p_parent->m_Red)
					return;

				p_grandparent = p_parent->mp_Parent;
				p_uncle = p_parent == p_grandparent->mp_Child[0] ? p_grandparent->mp_Child[1] : p_grandparent->mp_Child[0];
			}

			// parent to the left of grandparent
			if(p_parent == p_grandparent->mp_Child[0])
			{
				if(p_node == p_parent->mp_Child[1])
				{
					// case 2: rotate left
					RotateLeft(p_parent);
					p_node = p_parent;
					p_parent = p_node->mp_Parent;
				}

				// case 3: re-colour and rotate right
				p_parent->m_Red = false;
				p_grandparent->m_Red = true;
				RotateRight(p_grandparent);
			}
			// parent to the right of grandparent
			else
			{
				if (p_node == p_parent->mp_Child[0])
				{
					// case 2: rotate right
					RotateRight(p_parent);
					p_node = p_parent;
					p_parent = p_node->mp_Parent;
				}

				// case 3: re-colour and rotate left
				p_parent->m_Red = false;
				p_grandparent->m_Red = true;
				RotateLeft(p_grandparent);
			}
		}

		//--------------------------------------------------------------
		void FixupDoubleBlackAfterRemove(Node* p_node)
		{
			for (;;)
			{
				// case 1: double black root node
				Node* p_parent = p_node->mp_Parent;
				if (!p_parent)
					break;

				bool red_parent = p_parent->m_Red;

				// case 2
				int sibling_index = p_node == p_parent->mp_Child[0] ? 1 : 0;
				Node* p_sibling = p_parent->mp_Child[sibling_index];
				bool sibling_red = p_sibling->m_Red;
				bool sibling_children_black = BothChildrenBlack(p_sibling);

				if (!red_parent && sibling_red && sibling_children_black)
				{
					p_parent->m_Red = true;
					p_sibling->m_Red = false;

					if (sibling_index)		// if sibling to the right
						RotateLeft(p_parent);
					else
						RotateRight(p_parent);

					continue;
				}

				// case 3
				if (!red_parent && !sibling_red && sibling_children_black)
				{
					p_sibling->m_Red = true;
					p_node = p_parent;
					continue;	// p_sibling has changed, so go back to the start and get the parent and sibling again
				}

				// case 4
				if (red_parent && !sibling_red && sibling_children_black)
				{
					p_parent->m_Red = false;
					p_sibling->m_Red = true;
					break;
				}
				else
				{
					// case 5
					Node* p_sibling_child = p_sibling->mp_Child[sibling_index];
					Node* p_other_sibling_child = p_sibling->mp_Child[1 - sibling_index];
					bool sibling_child_red = p_sibling_child->m_Red;

					if (!sibling_red && !sibling_child_red && p_other_sibling_child->m_Red)
					{
						p_other_sibling_child->m_Red = false;
						p_sibling->m_Red = true;

						if (sibling_index)		// if sibling to the right
							RotateRight(p_sibling);
						else
							RotateLeft(p_sibling);

						p_sibling_child = p_sibling;
						p_sibling = p_other_sibling_child;

						goto case6;
					}

					// case 6
					if (!sibling_red && sibling_child_red)
					{
					case6:
						p_sibling->m_Red = red_parent;
						p_parent->m_Red = false;
						p_sibling_child->m_Red = false;

						if (sibling_index)		// if sibling to the right
							RotateLeft(p_parent);
						else
							RotateRight(p_parent);

						break;
					}
				}
			}
		}

		//--------------------------------------------------------------
		void DeleteLeaf(Node* p_node)
		{
			Node* p_null_node = p_node->mp_Child[0];
			Node* p_parent = p_node->mp_Parent;

			if (p_parent->mp_Child[0] == p_node)
				p_parent->mp_Child[0] = p_null_node;
			else
				p_parent->mp_Child[1] = p_null_node;
			p_null_node->mp_Parent = p_parent;

			FreeNode(p_node->mp_Child[1]);
			FreeNode(p_node);
		}

		//--------------------------------------------------------------
		void RotateLeft(Node* p_node)
		{
			Node* y = p_node;
			Node* z = y->mp_Parent;
			Node* x = y->mp_Child[1];
			Node* b = x->mp_Child[0];

			if(z)
			{
				if(z->mp_Child[0] == y)
					z->mp_Child[0] = x;
				else
					z->mp_Child[1] = x;
			}
			else
			{
				mp_Root = x;
			}

			x->mp_Parent = z;

			x->mp_Child[0] = y;
			y->mp_Parent = x;

			y->mp_Child[1] = b;
			b->mp_Parent = y;
		}

		//--------------------------------------------------------------
		void RotateRight(Node* p_node)
		{
			Node* y = p_node;
			Node* z = y->mp_Parent;
			Node* x = y->mp_Child[0];
			Node* b = x->mp_Child[1];

			if(z)
			{
				if(z->mp_Child[0] == y)
					z->mp_Child[0] = x;
				else
					z->mp_Child[1] = x;
			}
			else
			{
				mp_Root = x;
			}

			x->mp_Parent = z;

			x->mp_Child[1] = y;
			y->mp_Parent = x;

			y->mp_Child[0] = b;
			b->mp_Parent = y;
		}

		//--------------------------------------------------------------
		Node* FindInsertNode(T value)
		{
			Node* p_node = mp_Root;
			
			if(p_node)
			{
				while (!p_node->IsNull())
				{
					if(p_node->m_Value < value)
						p_node = p_node->mp_Child[1];
					else if(p_node->m_Value == value)
						break;
					else
						p_node = p_node->mp_Child[0];
				}
			}
			else
			{
				p_node = AllocNode(NULL);
				if(!p_node)
					return NULL;

				mp_Root = p_node;
			}

			return p_node;
		}

		//--------------------------------------------------------------
		// assert if value not found
		Node* Find(T value)
		{
			Node* p_node = mp_Root;
			VMEM_ASSERT(p_node, "RedBlackTree has been corrupted");

			while (!p_node->IsNull())
			{
				if (p_node->m_Value < value)
					p_node = p_node->mp_Child[1];
				else if (p_node->m_Value == value)
					break;
				else
					p_node = p_node->mp_Child[0];
			}

			VMEM_ASSERT(!p_node->IsNull(), "RedBlackTree has been corrupted");
			return p_node;
		}

		//--------------------------------------------------------------
		static size_t AlignUp(size_t size, size_t alignment)
		{
			return ((size + alignment - 1) / alignment) * alignment;
		}

		//--------------------------------------------------------------
		Node* AllocNode(Node* p_parent)
		{
			void* p_slot = mp_FreeNodeSlots;
			if(!p_slot)
			{
				if(!AllocateNewMemPool())
					return NULL;

				p_slot = mp_FreeNodeSlots;
				VMEM_ASSERT(p_slot, "mp_FreeNodeSlots has been corrupted");
			}

			mp_FreeNodeSlots = *(void**)p_slot;

			Node* p_node = (Node*)p_slot;

			new (p_node)Node(p_parent);

			return p_node;
		}

		//--------------------------------------------------------------
		void FreeNode(Node* p_node)
		{
			p_node->~Node();

			*(void**)p_node = mp_FreeNodeSlots;
			mp_FreeNodeSlots = p_node;
		}

		//--------------------------------------------------------------
		bool AllocateNewMemPool()
		{
			if(!mp_Allocator)
				return false;

			VMEM_ASSERT(m_NextMemPoolSize, "m_NextMemPoolSize has been corrupted");
			size_t pool_size = m_NextMemPoolSize;

			void* p = mp_Allocator->Alloc(pool_size);
			if (!p)
				return false;

			MemPool* p_mem_pool = (MemPool*)p;

			VMEM_STATS(m_Stats.m_Reserved += pool_size);
			VMEM_STATS(m_Stats.m_Overhead += pool_size);

			p_mem_pool->mp_Next = mp_MemPools;
			p_mem_pool->m_Size = pool_size;
			mp_MemPools = p_mem_pool;

			void* p_first_slot = (byte*)p_mem_pool + AlignUp(sizeof(MemPool), sizeof(Node));

			size_t slot_count = (pool_size - ((byte*)p_first_slot - (byte*)p_mem_pool)) / sizeof(Node);
			VMEM_ASSERT(slot_count > 0, "RedBlackTree has been corrupted");

			void* p_end_slot = (byte*)p_first_slot + (slot_count - 1) * sizeof(Node);

			void* p_slot = p_first_slot;
			while(p_slot != p_end_slot)
			{
				void* p_next_slot = (byte*)p_slot + sizeof(Node);
				*(void**)p_slot = p_next_slot;
				p_slot = p_next_slot;
			}
			*(void**)p_slot = NULL;

			VMEM_ASSERT(!mp_FreeNodeSlots, "RedBlackTree has been corrupted");
			mp_FreeNodeSlots = p_first_slot;

			m_NextMemPoolSize *= 2;

			return true;
		}

		//--------------------------------------------------------------
		void FreeMemPool(MemPool* p_mem_pool)
		{
			VMEM_STATS(m_Stats.m_Reserved -= p_mem_pool->m_Size);
			VMEM_STATS(m_Stats.m_Overhead -= p_mem_pool->m_Size);

			mp_Allocator->Free(p_mem_pool, p_mem_pool->m_Size);
		}

		//--------------------------------------------------------------
		void DeleteMemPools()
		{
			MemPool* p_mem_pool = mp_MemPools;
			while(p_mem_pool)
			{
				MemPool* p_next = p_mem_pool->mp_Next;
				FreeMemPool(p_mem_pool);
				p_mem_pool = p_next;
			}

			mp_MemPools = NULL;
		}

		//--------------------------------------------------------------
		RedBlackTree(const RedBlackTree<T>&);
		RedBlackTree& operator=(const RedBlackTree<T>&);

		//--------------------------------------------------------------
		// data
	private:
		Node* mp_Root;
		int m_Count;

		size_t m_NextMemPoolSize;
		MemPool* mp_MemPools;
		void* mp_FreeNodeSlots;

		InternalAllocator* mp_Allocator;

#ifdef VMEM_ENABLE_STATS
		Stats m_Stats;
#endif
	};
}

//--------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//--------------------------------------------------------------
#endif		// #ifndef VMEM_REDBLACKTREE_H_INCLUDED

