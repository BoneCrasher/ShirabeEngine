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
#ifndef VMEMATOMIC_H_INCLUDED
#define VMEMATOMIC_H_INCLUDED

//------------------------------------------------------------------------
#include "VMemCore.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#ifdef VMEM_USE_BOOST_ATOMICS
	#include <boost/atomic.hpp>
#else
	#include <atomic>
#endif

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	template<typename T>
	class Atomic
	{
	public:
		//------------------------------------------------------------------------
		Atomic()
		{
		}

		//------------------------------------------------------------------------
		Atomic(T value)
		:	m_Value(value)
		{
		}

		//------------------------------------------------------------------------
		bool operator==(const Atomic<T>& other) const
		{
			return m_Value == other.m_Value;
		}

		//------------------------------------------------------------------------
		void operator+=(const Atomic<T>& other)
		{
			m_Value += other.m_Value;
		}

		//------------------------------------------------------------------------
		void operator-=(const Atomic<T>& other)
		{
			m_Value -= other.m_Value;
		}

		//------------------------------------------------------------------------
		bool operator>(T value) const
		{
			return m_Value > value;
		}

		//------------------------------------------------------------------------
		bool operator>=(T value) const
		{
			return m_Value >= value;
		}

		//------------------------------------------------------------------------
		operator T() const
		{
#ifdef VMEM_USE_BOOST_ATOMICS
			return m_Value.load(boost::memory_order_relaxed);
#else
			return m_Value.load(std::memory_order_relaxed);
#endif
		}

		//------------------------------------------------------------------------
		bool operator==(T value) const
		{
			return m_Value == value;
		}

		//------------------------------------------------------------------------
		void operator=(T value)
		{
			m_Value = value;
		}

		//------------------------------------------------------------------------
		// data
	private:
#ifdef VMEM_USE_BOOST_ATOMICS
		boost::atomic<T> m_Value;
#else
		std::atomic<T> m_Value;
#endif
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEMATOMIC_H_INCLUDED

