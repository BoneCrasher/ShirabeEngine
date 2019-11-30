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
#ifndef VMEM_PHYSICALPAGEHEAP_H_INCLUDED
#define VMEM_PHYSICALPAGEHEAP_H_INCLUDED

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#include "VMemCore.hpp"
#include "RangeMap.hpp"
#include "PhysicalFSA.hpp"
#include "List.hpp"
#include "VMemStats.hpp"

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	struct Range
	{
		Range()
		:	mp_Mem(0),
			m_Size(0)
		{
		}

		Range(void* p, size_t size)
		:	mp_Mem(p),
			m_Size(size)
		{
		}

		// data
		void* mp_Mem;
		size_t m_Size;
	};

	//------------------------------------------------------------------------
	class PhysicalPageHeap
	{
		struct RangeNode
		{
			RangeNode* mp_Prev;
			RangeNode* mp_Next;
			Range m_Range;
		};

	public:
		PhysicalPageHeap();

		void Initialise(int page_size, int commit_flags);

		bool Initialised() const { return m_Initialised; }

		void Clear();

		void* Alloc(size_t size);

		bool Free(void* p, size_t size);

		void GiveMemory(void* p_mem, size_t size);

		size_t GetMaxFreeRangeSize();

		int GetCommitFlags() const { return m_CommitFlags; }

#ifdef VMEM_ENABLE_STATS
		size_t GetMemorySize() const;
#endif

	private:
		void AddRange(RangeNode* p_node);

		void RemoveRange(RangeNode* p_node);

		bool FindAndRemoveRange(size_t size, Range& range);

		int GetRangeNodeListIndex(size_t size);

		void FindMaxFreeRangeSize();

		//------------------------------------------------------------------------
		// data
	private:
		static const int m_RangeNodeListCount = 128;

		bool m_Initialised;

		int m_PageSize;

		int m_CommitFlags;

		RangeMap<RangeNode*> m_RangeMap;

		List<RangeNode> m_RangeNodeLists[m_RangeNodeListCount];	// the last list holds multiple sizes, all other lists hold one size only

		PhysicalFSA<RangeNode> m_RangeNodeFSA;

		size_t m_MaxFreeRangeSize;
		bool m_MaxFreeRangeSizeDirty;
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_PHYSICALPAGEHEAP_H_INCLUDED

