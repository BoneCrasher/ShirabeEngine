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
#ifndef VMEM_PHYSICALARRAY_H_INCLUDED
#define VMEM_PHYSICALARRAY_H_INCLUDED

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#include "VMemCore.hpp"

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	template<typename T>
	class PhysicalArray
	{
	public:
		//------------------------------------------------------------------------
		struct Page
		{
			PhysicalAlloc m_Alloc;
			Page* mp_Next;
		};

		//------------------------------------------------------------------------
		PhysicalArray(int page_size)
		:	mp_Pages(NULL),
			m_CountPerPage((page_size - sizeof(Page)) / sizeof(T)),
			m_Count(0),
			m_Capacity(0),
			m_PageSize(page_size)
		{
		}

		//------------------------------------------------------------------------
		~PhysicalArray()
		{
			VMEM_ASSERT(mp_Pages == NULL, "PhysicalArray has been corrupted");
		}

		//------------------------------------------------------------------------
		int GetCount() const
		{
			return m_Count;
		}

		//------------------------------------------------------------------------
		int GetCapacity() const
		{
			return m_Capacity;
		}

		//------------------------------------------------------------------------
		size_t GetMemorySize() const
		{
			size_t size = 0;

			for(Page* p_page = mp_Pages; p_page != NULL; p_page = p_page->mp_Next)
				size += m_PageSize;

			return size;
		}

		//------------------------------------------------------------------------
		bool IsFull() const
		{
			return m_Count == m_Capacity;
		}

		//------------------------------------------------------------------------
		T& operator[](int index)
		{
			VMEM_ASSERT(index >= 0 && index < m_Count, "PhysicalArray index out of range");
			return Get(index);
		}

		//------------------------------------------------------------------------
		// should always check IsFull first before calling add and call GiveMemory if full
		void Add(const T& value)
		{
			VMEM_ASSERT(m_Count != m_Capacity, "PhysicalArray has been corrupted");
			Get(m_Count++) = value;
		}

		//------------------------------------------------------------------------
		void RemoveLast()
		{
			VMEM_ASSERT(m_Count > 0, "VMemArray has been corrupted");
			--m_Count;
		}

		//------------------------------------------------------------------------
		void GiveMemory(PhysicalAlloc alloc)
		{
			Page* p_page = (Page*)alloc.mp_Virtual;
			p_page->m_Alloc = alloc;
			p_page->mp_Next = NULL;

			if (mp_Pages)
			{
				Page* p_last_page = mp_Pages;
				while (p_last_page->mp_Next)
					p_last_page = p_last_page->mp_Next;

				p_last_page->mp_Next = p_page;
			}
			else
			{
				mp_Pages = p_page;
			}

			m_Capacity += m_CountPerPage;
		}

		//------------------------------------------------------------------------
		void SetCount(int new_count)
		{
			VMEM_ASSERT(new_count <= m_Count, "PhysicalArray has been corrupted");
			m_Count = new_count;
		}

		//------------------------------------------------------------------------
		void Trim(Page*& p_unused_pages)
		{
			if(!m_Count)
			{
				p_unused_pages = mp_Pages;
				mp_Pages = NULL;
				m_Capacity = 0;
				return;
			}

			p_unused_pages = NULL;

			if (!mp_Pages)
				return;

			int last_page_index = (m_Count - 1) / m_CountPerPage;

			Page* p_page = mp_Pages;
			for (int i = 0; i<last_page_index; ++i)
				p_page = p_page->mp_Next;

			p_unused_pages = p_page->mp_Next;
			p_page->mp_Next = NULL;

			m_Capacity = (last_page_index + 1) * m_CountPerPage;
		}

		//------------------------------------------------------------------------
		bool PhysicalMemoryInUse(PhysicalAlloc alloc)
		{
			void* p_alloc_start = alloc.mp_Physical;
			void* p_alloc_end = (byte*)alloc.mp_Physical + alloc.m_Size;

			int index = 0;
			for (Page* p_page = mp_Pages; p_page != NULL; p_page = p_page->mp_Next)
			{
				PhysicalAlloc page_alloc = p_page->m_Alloc;
				if(page_alloc.mp_Physical >= p_alloc_start && page_alloc.mp_Physical < p_alloc_end)
					return true;

				index += m_CountPerPage;
				if(index >= m_Count)
					break;
			}

			return false;
		}

	private:
		//------------------------------------------------------------------------
		T& Get(int index)
		{
			VMEM_ASSERT(index >= 0 && index < m_Count, "PhysicalArray index out of range");

			int page_index = index / m_CountPerPage;

			Page* p_page = mp_Pages;
			for(int i=0; i<page_index; ++i)
				p_page = p_page->mp_Next;

			int index_in_page = index - (page_index * m_CountPerPage);
			T* p_array = (T*)(p_page + 1);

			return p_array[index_in_page];
		}

		//------------------------------------------------------------------------
		PhysicalArray<T>(const PhysicalArray<T>&);
		PhysicalArray<T>& operator=(const PhysicalArray<T>&);

		//------------------------------------------------------------------------
		// data
	private:
		Page* mp_Pages;

		int m_CountPerPage;

		int m_Count;
		int m_Capacity;

		int m_PageSize;

		PhysicalAlloc m_PhysicalAlloc;
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_PHYSICALARRAY_H_INCLUDED

