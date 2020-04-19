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
#ifndef VMEM_VMEMSTATS_H_INCLUDED
#define VMEM_VMEMSTATS_H_INCLUDED

//------------------------------------------------------------------------
#include <stddef.h>
#include "VMemDefs.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	class Stats
	{
	public:
		inline Stats();

		inline Stats& operator+=(const Stats& other);

		inline size_t GetCommittedBytes() const;

		//------------------------------------------------------------------------
		// data
	public:
		size_t m_Used;			// memory that the allocator has allocated (in use by the app)
		size_t m_Unused;		// memory that the allocator has committed but not currently in use,
		size_t m_Overhead;		// memory used internally by the allocator to manage data structures.
		size_t m_Reserved;		// reserved memory (includes all committed memory)
	};

	//------------------------------------------------------------------------
	Stats::Stats()
	:	m_Used(0),
		m_Unused(0),
		m_Overhead(0),
		m_Reserved(0)
	{
	}

	//------------------------------------------------------------------------
	struct VMemHeapStats
	{
		VMemHeapStats& operator+=(const VMemHeapStats& other)
		{
			m_FSAHeap1 += other.m_FSAHeap1;
			m_FSAHeap2 += other.m_FSAHeap2;
			m_CoalesceHeap1 += other.m_CoalesceHeap1;
			m_CoalesceHeap2 += other.m_CoalesceHeap2;
			m_LargeHeap += other.m_LargeHeap;
			m_AlignedCoalesceHeap += other.m_AlignedCoalesceHeap;
			m_AlignedLargeHeap += other.m_AlignedLargeHeap;
			m_Internal += other.m_Internal;
			m_Total += other.m_Total;
			return *this;
		}

		Stats m_FSAHeap1;
		Stats m_FSAHeap2;
		Stats m_CoalesceHeap1;
		Stats m_CoalesceHeap2;
		Stats m_LargeHeap;
		Stats m_AlignedCoalesceHeap;
		Stats m_AlignedLargeHeap;
		Stats m_Internal;
		Stats m_Total;
	};

	//------------------------------------------------------------------------
	Stats& Stats::operator+=(const Stats& other)
	{
		m_Used += other.m_Used;
		m_Unused += other.m_Unused;
		m_Overhead += other.m_Overhead;
		m_Reserved += other.m_Reserved;
		return *this;
	}

	//------------------------------------------------------------------------
	size_t Stats::GetCommittedBytes() const
	{
		return m_Used + m_Unused + m_Overhead;
	}

	//------------------------------------------------------------------------
	inline Stats operator+(const Stats& s1, const Stats& s2)
	{
		Stats stats;
		stats += s1;
		stats += s2;
		return stats;
	}
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_VMEMSTATS_H_INCLUDED

