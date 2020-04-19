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
#ifndef VMEM_VMEMARRAY_H_INCLUDED
#define VMEM_VMEMARRAY_H_INCLUDED

//------------------------------------------------------------------------
#include "VMemCore.hpp"
#include "VMemStats.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	template<typename T>
	class VMemArray
	{
	public:
		//------------------------------------------------------------------------
		VMemArray(InternalHeap& internal_heap)
		:	mp_Array(NULL),
			m_Count(0),
			m_Capacity(0),
			m_InternalHeap(internal_heap)
		{
		}

		//------------------------------------------------------------------------
		bool Initialise()
		{
			return Grow();
		}

		//------------------------------------------------------------------------
		~VMemArray()
		{
			if(mp_Array)
				m_InternalHeap.Free_WithFalseSharingBuffer(mp_Array);
		}

		//------------------------------------------------------------------------
		int GetCount() const
		{
			return m_Count;
		}

		//------------------------------------------------------------------------
		T& operator[](int index)
		{
			VMEM_ASSERT(index >= 0 && index < m_Count, "VMemArray index out of range");
			return mp_Array[index];
		}

		//------------------------------------------------------------------------
		const T& operator[](int index) const
		{
			VMEM_ASSERT(index >= 0 && index < m_Count, "VMemArray index out of range");
			return mp_Array[index];
		}

		//------------------------------------------------------------------------
		bool Add(const T& value)
		{
			if(m_Count == m_Capacity)
			{
				if(!Grow())
					return false;
			}

			mp_Array[m_Count++] = value;

			return true;
		}

		//------------------------------------------------------------------------
		void RemoveAt(int index)
		{
			VMEM_ASSERT(index >= 0 && index < m_Count, "VMemArray index out of range");

			--m_Count;

			for(int i=index; i!=m_Count; ++i)
				mp_Array[i] = mp_Array[i+1];
		}

		//------------------------------------------------------------------------
		int GetMemoryUsage() const
		{
			return m_Capacity * sizeof(T) + InternalHeap::GetFalseSharingBufferOverhead();
		}

	private:
		//------------------------------------------------------------------------
		bool Grow()
		{
			m_Capacity = m_Capacity ? 2*m_Capacity : m_DefaultCapacity;
			
			T* p_new_array = (T*)m_InternalHeap.Alloc_WithFalseSharingBuffer(m_Capacity * sizeof(T));

			if(!p_new_array)
			{
				m_Capacity = 0;
				return false;
			}

			if(mp_Array)
			{
				memcpy(p_new_array, mp_Array, m_Count * sizeof(T));
				m_InternalHeap.Free_WithFalseSharingBuffer(mp_Array);
			}

			mp_Array = p_new_array;

			return true;
		}

		//------------------------------------------------------------------------
		VMemArray<T>(const VMemArray<T>&);
		VMemArray<T>& operator=(const VMemArray<T>&);

		//------------------------------------------------------------------------
		// data
	private:
		static const int m_DefaultCapacity = 32;

		T* mp_Array;
		int m_Count;
		int m_Capacity;

		InternalHeap& m_InternalHeap;
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_VMEMARRAY_H_INCLUDED

