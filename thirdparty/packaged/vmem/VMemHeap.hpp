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
#ifndef VMEM_VMEMHEAP_H_INCLUDED
#define VMEM_VMEMHEAP_H_INCLUDED

//------------------------------------------------------------------------
#include "VMemCore.hpp"
#include "VMemStats.hpp"
#include "InternalHeap.hpp"
#include "PageHeap.hpp"
#include "FSAHeap.hpp"
#include "CoalesceHeap.hpp"
#include "LargeHeap.hpp"
#include "ProtectedHeap.hpp"
#include "AlignedCoalesceHeap.hpp"
#include "AlignedLargeHeap.hpp"
#include "VMemThread.hpp"
#include "HeapRegions.hpp"
#include "VirtualMem.hpp"

#ifdef VMEM_FSA_HEAP_PER_THREAD
	#include <atomic>
#endif

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//#define VMEM_HEAP_GLOBAL_LOCK

//------------------------------------------------------------------------
#ifdef VMEM_HEAP_GLOBAL_LOCK
	#define VMEM_GLOBAL_LOCK CriticalSectionScope lock(m_CriticalSection);
#else
	#define VMEM_GLOBAL_LOCK
#endif

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
#if defined(VMEM_FSA_HEAP_PER_THREAD) || defined(VMEM_COALESCE_HEAP_PER_THREAD)
	class VMemHeapTLS
	{
	public:
		VMemHeapTLS(PageHeap* p_fsa1_page_heap, InternalHeap& internal_heap)
		:	m_FSAHeap1(p_fsa1_page_heap, internal_heap),
			mp_CoalesceHeap(NULL),
			mp_Next(NULL)
		{
		}

		FSAHeap m_FSAHeap1;

		std::atomic<CoalesceHeap*> mp_CoalesceHeap;

		VMemHeapTLS* mp_Next;
	};
#endif

	//------------------------------------------------------------------------
	class VMemHeap
	{
	public:
		//------------------------------------------------------------------------
		VMemHeap(int page_size, int reserve_flags, int commit_flags);

		~VMemHeap();

		bool Initialise();

#ifdef VMEM_COALESCE_HEAP_PER_THREAD
		void CreateCoalesceHeapForThisThread();
#endif
		void* Alloc(size_t size, size_t alignment);

		void Free(void* p);

		size_t GetSize(void* p);

		bool Owns(void* p);

		void Flush();

		void CheckIntegrity();

#ifdef VMEM_ENABLE_STATS
		VMemHeapStats GetStatsNoLock() const;
		size_t WriteAllocs();
		void SendStatsToMemPro(MemProSendFn send_fn, void* p_context);
#endif
		InternalHeap& GetInternalHeap() { return m_InternalHeap; }

#ifdef VMEM_SERVICE_THREAD
		void Update();
#endif

		void AcquireLockShared() const;
		void ReleaseLockShared() const;

	private:
		bool InitialiseFSAHeap1(FSAHeap& fsa_heap);
		bool InitialiseFSAHeap2(FSAHeap& fsa_heap);

		void InitialiseTrailGuards();

		VMEM_NO_INLINE void* AlignedAlloc(size_t size, size_t alignment);

#ifdef VMEM_INC_INTEG_CHECK
		void IncIntegrityCheck();
#endif

#ifdef VMEM_FSA_HEAP_PER_THREAD
		VMEM_NO_INLINE VMemHeapTLS* CreateVMemHeapTLS();

		VMEM_FORCE_INLINE VMemHeapTLS* GetVMemHeapTLS();
#endif

		//------------------------------------------------------------------------
		// data
	private:
		VirtualMem m_VirtualMem;

		int m_PageSize;

		void* mp_FSA1PageHeapRegion;
		void* mp_FSA2PageHeapRegion;
		void* mp_CoalesceHeap1Region;
		void* mp_CoalesceHeap2Region;

		InternalHeap m_InternalHeap;

		HeapRegions m_HeapRegions;

		PageHeap m_FSA1PageHeap;
		PageHeap m_FSA2PageHeap;

#ifdef VMEM_FSA_HEAP_PER_THREAD
		mutable CriticalSection m_VMemHeapTLSListLock;
		VMemHeapTLS* mp_VMemHeapTLSList;
#else
		FSAHeap m_FSAHeap1;
#endif
		FSAHeap m_FSAHeap2;

		CoalesceHeap m_CoalesceHeap1;
		CoalesceHeap m_CoalesceHeap2;

		LargeHeap m_LargeHeap;

		AlignedCoalesceHeap m_AlignedCoalesceHeap;
		AlignedLargeHeap m_AlignedLargeHeap;

		Stats m_Stats;

#ifdef VMEM_HEAP_GLOBAL_LOCK
		CriticalSection m_CriticalSection;
#endif

#ifdef VMEM_PROTECTED_HEAP
		ProtectedHeap m_ProtectedHeap;
#endif
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_VMEMHEAP_H_INCLUDED

