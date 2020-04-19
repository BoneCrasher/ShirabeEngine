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
#ifndef VMEM_FSAHEAP_H_INCLUDED
#define VMEM_FSAHEAP_H_INCLUDED

//------------------------------------------------------------------------
#include "FSA.hpp"
#include "PageHeap.hpp"
#include "VMemCore.hpp"
#include "VMemStats.hpp"
#include "Bitfield.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	class InternalHeap;

	//------------------------------------------------------------------------
	// note: this class doesn't need to be thread safe. Once it's setup it's read only.
	class FSAHeap
	{
	public:
		FSAHeap(PageHeap* p_page_heap, InternalHeap& internal_heap, int page_size);

		~FSAHeap();

		bool Initialise();

		bool InitialiseFSA(int size, int alignment);

		VMEM_FORCE_INLINE void* Alloc(size_t size);

		VMEM_FORCE_INLINE void Free(void* p);

		VMEM_FORCE_INLINE void Free(void* p, int page_size);

		void Flush();

#if defined(VMEM_FSA_PAGE_CACHE) || defined(VMEM_POPULATE_FSA_CACHE)
		void Update();
#endif
		void CheckIntegrity() const;

#ifdef VMEM_INC_INTEG_CHECK
		void IncIntegrityCheck();
#endif

#ifdef VMEM_ENABLE_STATS
		Stats GetStats() const;
		Stats GetStatsNoLock() const;
		size_t WriteAllocs() const;
		void SendStatsToMemPro(MemProSendFn send_fn, void* p_context);
#endif
		VMEM_FORCE_INLINE FSA* GetFSA(int size);

		void Lock() const;

		void Release() const;

	private:
		FSAHeap& operator=(const FSAHeap& other);

		//------------------------------------------------------------------------
		// data
	private:
		PageHeap* mp_PageHeap;

		static const int m_MaxFSASize = 16*1024;
		static const int m_MaxFSACount = m_MaxFSASize / VMEM_NATURAL_ALIGNMENT;

		FSA* m_FSAMap[m_MaxFSACount];

#ifdef VMEM_INC_INTEG_CHECK
		int m_FSAIntegCheckIndex;		// for incremental integrity checking
#endif
		InternalHeap& m_InternalHeap;

		Bitfield m_AllocatedFSAs;

		FSAUpdateList m_FSAUpdateList;

		int m_PageSize;
	};

	//------------------------------------------------------------------------
	void* FSAHeap::Alloc(size_t size)
	{
		VMEM_ASSERT(size >= 0, "invalid size");
		size_t index = (size + VMEM_NATURAL_ALIGNMENT - 1) / VMEM_NATURAL_ALIGNMENT;
		VMEM_ASSERT(index >= 0 && index < m_MaxFSASize/VMEM_NATURAL_ALIGNMENT, "FSA index out of range");
		VMEM_ASSUME(index >= 0 && index < m_MaxFSASize/VMEM_NATURAL_ALIGNMENT);
		VMEM_ASSERT(m_FSAMap[index], "FSA not setup for the specified size");

		return m_FSAMap[index]->Alloc();
	}

	//------------------------------------------------------------------------
	void FSAHeap::Free(void* p)
	{
		Free(p, mp_PageHeap->GetPageSize());
	}

	//------------------------------------------------------------------------
	// passing in page_size as an optimisaton so that it doesn't have to be looked up from the page heap
	void FSAHeap::Free(void* p, int page_size)
	{
		FSA* p_fsa = FSA::GetFSA(p, page_size);
		p_fsa->Free(p, page_size);
	}

	//------------------------------------------------------------------------
	FSA* FSAHeap::GetFSA(int size)
	{
		int index = (size + VMEM_NATURAL_ALIGNMENT - 1) / VMEM_NATURAL_ALIGNMENT;
		VMEM_ASSERT(index >= 0 && index < m_MaxFSASize/VMEM_NATURAL_ALIGNMENT, "FSA index out of range");
		VMEM_ASSUME(index >= 0 && index < m_MaxFSASize/VMEM_NATURAL_ALIGNMENT);
		VMEM_ASSERT(m_FSAMap[index], "FSA not setup for the specified size");

		return m_FSAMap[index];
	}
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_FSAHEAP_H_INCLUDED

