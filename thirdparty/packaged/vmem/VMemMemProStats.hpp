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
#ifndef VMEM_MEMPROSTATS_H_INCLUDED
#define VMEM_MEMPROSTATS_H_INCLUDED

//------------------------------------------------------------------------
#include "VMemStats.hpp"
#include "VMemCore.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	namespace MemProStats
	{
		//------------------------------------------------------------------------
		// make everything use uint64 to avoid packing issues and simplify endian byte swapping
		struct Stats
		{
			Stats& operator=(const VMem::Stats& other)
			{
				m_Used = other.m_Used;
				m_Unused = other.m_Unused;
				m_Overhead = other.m_Overhead;
				m_Reserved = other.m_Reserved;
				return *this;
			}

			uint64 m_Used;
			uint64 m_Unused;
			uint64 m_Overhead;
			uint64 m_Reserved;
		};

		//------------------------------------------------------------------------
		struct BasicCoalesceHeapStats
		{
			uint64 m_RegionCount;
			uint64 m_RegionSize;
			Stats m_Stats;
		};

		//------------------------------------------------------------------------
		struct PageHeapStats
		{
			uint64 m_PageSize;
			uint64 m_RegionSize;
			uint64 m_RegionCount;
			Stats m_Stats;
		};

		//------------------------------------------------------------------------
		struct FSAHeapStats
		{
			uint64 m_FSACount;
			Stats m_Stats;
		};

		//------------------------------------------------------------------------
		struct FSAStats
		{
			uint64 m_Size;
			uint64 m_SlotSize;
			uint64 m_Alignment;
			uint64 m_FreePageCount;
			uint64 m_FullPageCount;
			Stats m_Stats;
		};

		//------------------------------------------------------------------------
		struct CoalesceHeapStats
		{
			uint64 m_MinSize;
			uint64 m_MaxSize;
			uint64 m_RegionSize;
			uint64 m_RegionCount;
			Stats m_Stats;
		};

		//------------------------------------------------------------------------
		struct LageHeapStats
		{
			uint64 m_AllocCount;
			Stats m_Stats;
		};
	}
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_MEMPROSTATS_H_INCLUDED

