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
#include "FSAHeap.hpp"
#include "FSA.hpp"
#include "InternalHeap.hpp"
#include "VMemMemProStats.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	FSAHeap::FSAHeap(PageHeap* p_page_heap, InternalHeap& internal_heap, int page_size)
	:	mp_PageHeap(p_page_heap),
#ifdef VMEM_INC_INTEG_CHECK
		m_FSAIntegCheckIndex(0),
#endif
		m_InternalHeap(internal_heap),
		m_AllocatedFSAs(m_MaxFSASize, internal_heap),
		m_PageSize(page_size)
	{
		memset(m_FSAMap, 0, sizeof(m_FSAMap));
	}

	//------------------------------------------------------------------------
	bool FSAHeap::Initialise()
	{
		return m_AllocatedFSAs.Initialise();
	}

	//------------------------------------------------------------------------
	FSAHeap::~FSAHeap()
	{
		if(m_AllocatedFSAs.Initialised())
		{
			for(int i=0; i<m_MaxFSACount; ++i)
			{
				if(m_AllocatedFSAs.Get(i))
					m_InternalHeap.Delete_WithFalseSharingBuffer(m_FSAMap[i]);
			}
		}
	}

	//------------------------------------------------------------------------
	bool FSAHeap::InitialiseFSA(int size, int alignment)
	{
		int index = size / VMEM_NATURAL_ALIGNMENT;
		VMEM_ASSERT(index < m_MaxFSASize/VMEM_NATURAL_ALIGNMENT, "increase m_MaxFSASize");
		VMEM_ASSUME(index < m_MaxFSASize/VMEM_NATURAL_ALIGNMENT);
		VMEM_ASSERT_MEM(!m_FSAMap[index], &m_FSAMap[index]);		// FSA size already setup?

		FSA* p_fsa = m_InternalHeap.New_WithFalseSharingBufferRef45<FSA>(
			size,
			alignment,
			mp_PageHeap,
			m_InternalHeap,
			m_FSAUpdateList,
			m_PageSize);

		if(!p_fsa)
			return false;

		if(!p_fsa->Initialise())
		{
			m_InternalHeap.Delete_WithFalseSharingBuffer(p_fsa);
			return false;
		}

		m_FSAMap[index] = p_fsa;
		m_AllocatedFSAs.Set(index);

		// set all the lower indices that haven't been setup up in map to use this fsa
		int i = index - 1;
		while(i >= 0 && !m_AllocatedFSAs.Get(i))
		{
			m_FSAMap[i] = p_fsa;
			--i;
		}

		return true;
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	Stats FSAHeap::GetStats() const
	{
		Stats stats;

		for(int i=0; i<m_MaxFSACount; ++i)
		{
			if(m_AllocatedFSAs.Get(i))
			{
				stats += m_FSAMap[i]->GetStats();
				stats.m_Overhead += sizeof(FSA) + InternalHeap::GetFalseSharingBufferOverhead();
			}
		}

		stats.m_Overhead += m_AllocatedFSAs.GetSizeInBytes();

		return stats;
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	Stats FSAHeap::GetStatsNoLock() const
	{
		Stats stats;

		for(int i=0; i<m_MaxFSACount; ++i)
		{
			if(m_AllocatedFSAs.Get(i))
			{
				stats += m_FSAMap[i]->GetStatsNoLock();
				stats.m_Overhead += sizeof(FSA) + InternalHeap::GetFalseSharingBufferOverhead();
			}
		}

		stats.m_Overhead += m_AllocatedFSAs.GetSizeInBytes();

		return stats;
	}
#endif

	//------------------------------------------------------------------------
	void FSAHeap::Flush()
	{
		for(int i=0; i<m_MaxFSACount; ++i)
		{
			if(m_AllocatedFSAs.Get(i))
				m_FSAMap[i]->Flush();
		}
	}

	//------------------------------------------------------------------------
#if defined(VMEM_FSA_PAGE_CACHE) || defined(VMEM_POPULATE_FSA_CACHE)
	void FSAHeap::Update()
	{
		// Note: Thread safely is not obvious here.
		// This is safe because only FSA's can add and remove themselves to/from the list,
		// and this will always be done inside of the FSA critical sction. An FSA will only
		// add itself if it isn't already added. The FSA is still part of the list after 
		// GetAndClearList is called so it won't be added again. It is removed from the
		// list in the update, and any new FSA's that are added while this is Update happening
		// will go onto the main m_FSAUpdateList.
		FSA* p_fsa = m_FSAUpdateList.GetAndClearList();
		while(p_fsa)
		{
			FSA* p_next = p_fsa->GetNextUpdateFSA();
			p_fsa->Update();
			p_fsa = p_next;
		}
	}
#endif

	//------------------------------------------------------------------------
	void FSAHeap::CheckIntegrity() const
	{
		for(int i=0; i<m_MaxFSACount; ++i)
		{
			if(m_AllocatedFSAs.Get(i))
				m_FSAMap[i]->CheckIntegrity();
		}
	}

	//------------------------------------------------------------------------
#ifdef VMEM_INC_INTEG_CHECK
	void FSAHeap::IncIntegrityCheck()
	{
		int i = 0;
		while(!m_FSAMap[m_FSAIntegCheckIndex] && i++ < m_MaxFSACount)
			m_FSAIntegCheckIndex = (m_FSAIntegCheckIndex + 1) % m_MaxFSACount;

		FSA* p_fsa = m_FSAMap[m_FSAIntegCheckIndex];
		if(p_fsa)
			p_fsa->CheckIntegrity();

		m_FSAIntegCheckIndex = (m_FSAIntegCheckIndex + 1) % m_MaxFSACount;
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	size_t FSAHeap::WriteAllocs() const
	{
		size_t allocated_bytes = 0;
		int allocs_written = 0;

		for(int i=0; i<m_MaxFSACount; ++i)
		{
			if(m_AllocatedFSAs.Get(i))
				allocated_bytes += m_FSAMap[i]->WriteAllocs(&allocs_written, VMEM_WRITE_ALLOCS_MAX, false);
		}

		if(allocs_written == VMEM_WRITE_ALLOCS_MAX)
			DebugWrite(_T("Only showing first %d allocs\n"), VMEM_WRITE_ALLOCS_MAX);

		return allocated_bytes;
	}
#endif

	//------------------------------------------------------------------------
	void FSAHeap::Lock() const
	{
		for(int i=0; i<m_MaxFSACount; ++i)
		{
			if(m_AllocatedFSAs.Get(i))
				m_FSAMap[i]->Lock();
		}
	}

	//------------------------------------------------------------------------
	void FSAHeap::Release() const
	{
		for(int i=0; i<m_MaxFSACount; ++i)
		{
			if(m_AllocatedFSAs.Get(i))
				m_FSAMap[i]->Release();
		}
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	void FSAHeap::SendStatsToMemPro(MemProSendFn send_fn, void* p_context)
	{
		SendEnumToMemPro(vmem_FSAHeap, send_fn, p_context);

		MemProStats::FSAHeapStats stats;

		stats.m_FSACount = 0;
		for(int i=0; i<m_MaxFSACount; ++i)
		{
			if(m_AllocatedFSAs.Get(i))
				++stats.m_FSACount;
		}
		stats.m_Stats = GetStats();
		SendToMemPro(stats, send_fn, p_context);

		for(int i=0; i<m_MaxFSACount; ++i)
		{
			if(m_AllocatedFSAs.Get(i))
				m_FSAMap[i]->SendStatsToMemPro(send_fn, p_context);
		}
	}
#endif
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

