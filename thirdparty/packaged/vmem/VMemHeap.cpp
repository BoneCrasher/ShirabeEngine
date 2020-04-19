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
#include "VMemHeap.hpp"
#include "VMemCore.hpp"
#include "VirtualMem.hpp"
#include "VMemSys.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#ifdef VMEM_X64
	#define VMEM_COALESCE1_HEAP_REGION_SIZE (768*1024*1024)
	#define VMEM_COALESCE2_HEAP_REGION_SIZE (768*1024*1024)
#else
	#define VMEM_COALESCE1_HEAP_REGION_SIZE (128*1024*1024)
	#define VMEM_COALESCE2_HEAP_REGION_SIZE (256*1024*1024)
#endif

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
#ifdef VMEM_FSA_HEAP_PER_THREAD
	__declspec(thread) VMemHeapTLS* gp_ThreadFASAHeap = NULL;
#endif

	//------------------------------------------------------------------------
	namespace HeapSettings
	{
		enum Enum
		{
			// on windows we can reserve pages without committing them, this allows us
			// to have larger region sizes which improves performance
#if defined(VMEM_PLATFORM_XBOXONE) || defined(VMEM_PLATFORM_PS4) || defined(VMEM_SIMULATE_PS4)
			FSAHeap1_PageSize = 4 * 1024,
			FSAHeap1_RegionSize = 256 * 1024 * 1024,
			FSAHeap1_MaxSize = 64,

			FSAHeap2_PageSize = 64 * 1024,
			FSAHeap2_RegionSize = 256 * 1024 * 1024,
			FSAHeap2_MaxSize = 2048,

			CoalesceHeap1_RegionSize = VMEM_COALESCE1_HEAP_REGION_SIZE,
			CoalesceHeap1_MinSize = FSAHeap2_MaxSize,
			CoalesceHeap1_MaxSize = 128 * 1024,

			CoalesceHeap2_RegionSize = VMEM_COALESCE2_HEAP_REGION_SIZE,
			CoalesceHeap2_MinSize = CoalesceHeap1_MaxSize,
			CoalesceHeap2_MaxSize = 1024 * 1024,

			AlignedCoalesceHeap_RegionSize = 16 * 1024 * 1024,
			AlignedCoalesceHeap_MinSize = 32,
			AlignedCoalesceHeap_MaxSize = 2*1024,
#elif defined(VMEM_OS_WIN)
			FSAHeap1_PageSize = 4 * 1024,
			FSAHeap1_RegionSize = 256 * 1024 * 1024,
			FSAHeap1_MaxSize = 64,

			FSAHeap2_PageSize = 64 * 1024,
			FSAHeap2_RegionSize = 256 * 1024 * 1024,
			FSAHeap2_MaxSize = 2048,

			CoalesceHeap1_RegionSize = VMEM_COALESCE1_HEAP_REGION_SIZE,
			CoalesceHeap1_MinSize = FSAHeap2_MaxSize,
			CoalesceHeap1_MaxSize = 128 * 1024,

			CoalesceHeap2_RegionSize = VMEM_COALESCE2_HEAP_REGION_SIZE,
			CoalesceHeap2_MinSize = CoalesceHeap1_MaxSize,
			CoalesceHeap2_MaxSize = 1024 * 1024,

			AlignedCoalesceHeap_RegionSize = 16 * 1024 * 1024,
			AlignedCoalesceHeap_MinSize = 32,
			AlignedCoalesceHeap_MaxSize = 2*1024,
#else
			FSAHeap1_PageSize = 4 * 1024,
			FSAHeap1_RegionSize = 1024 * 1024,
			FSAHeap1_MaxSize = 64,

			FSAHeap2_PageSize = 32 * 1024,
			FSAHeap2_RegionSize = 4 * 1024 * 1024,
			FSAHeap2_MaxSize = 512,

			CoalesceHeap1_RegionSize = 4 * 1024 * 1024,
			CoalesceHeap1_MinSize = 512,
			CoalesceHeap1_MaxSize = 10 * 1024,

			CoalesceHeap2_RegionSize = 8 * 1024 * 1024,
			CoalesceHeap2_MinSize = CoalesceHeap1_MaxSize,
			CoalesceHeap2_MaxSize = 1024 * 1024,

			AlignedCoalesceHeap_RegionSize = 16 * 1024 * 1024,
			AlignedCoalesceHeap_MinSize = 32,
			AlignedCoalesceHeap_MaxSize = 2*1024,
#endif
		};
	}

	//------------------------------------------------------------------------
	VMemHeap::VMemHeap(int page_size, int reserve_flags, int commit_flags)
	:	m_VirtualMem(page_size),
		m_PageSize(page_size),
		mp_FSA1PageHeapRegion(NULL),
		mp_FSA2PageHeapRegion(NULL),
		mp_CoalesceHeap1Region(NULL),
		mp_CoalesceHeap2Region(NULL),
		m_InternalHeap(m_VirtualMem),
		m_HeapRegions(m_InternalHeap),
		m_FSA1PageHeap(ToInt(HeapSettings::FSAHeap1_PageSize), ToInt(HeapSettings::FSAHeap1_RegionSize), m_InternalHeap, m_HeapRegions, RegionType::PageHeap1, reserve_flags, commit_flags, m_VirtualMem),
		m_FSA2PageHeap(ToInt(HeapSettings::FSAHeap2_PageSize), ToInt(HeapSettings::FSAHeap2_RegionSize), m_InternalHeap, m_HeapRegions, RegionType::PageHeap2, reserve_flags, commit_flags, m_VirtualMem),
#ifdef VMEM_FSA_HEAP_PER_THREAD
		mp_VMemHeapTLSList(NULL),
#else
		m_FSAHeap1(&m_FSA1PageHeap, m_InternalHeap, page_size),
#endif
		m_FSAHeap2(&m_FSA2PageHeap, m_InternalHeap, page_size),
		m_CoalesceHeap1(ToInt(HeapSettings::CoalesceHeap1_RegionSize), ToInt(HeapSettings::CoalesceHeap1_MinSize), ToInt(HeapSettings::CoalesceHeap1_MaxSize), m_InternalHeap, m_HeapRegions, RegionType::CoalesceHeap1, reserve_flags, commit_flags, m_VirtualMem, VMEM_COALESCE_HEAP1_CACHE, false),
		m_CoalesceHeap2(ToInt(HeapSettings::CoalesceHeap2_RegionSize), ToInt(HeapSettings::CoalesceHeap2_MinSize), ToInt(HeapSettings::CoalesceHeap2_MaxSize), m_InternalHeap, m_HeapRegions, RegionType::CoalesceHeap2, reserve_flags, commit_flags, m_VirtualMem, VMEM_COALESCE_HEAP2_CACHE, false),
		m_LargeHeap(reserve_flags, commit_flags, m_VirtualMem),
		m_AlignedCoalesceHeap(ToInt(HeapSettings::AlignedCoalesceHeap_RegionSize), ToInt(HeapSettings::AlignedCoalesceHeap_MinSize), ToInt(HeapSettings::AlignedCoalesceHeap_MaxSize), m_InternalHeap, m_HeapRegions, RegionType::AlignedCoalesceHeap, m_VirtualMem, reserve_flags, commit_flags),
		m_AlignedLargeHeap(reserve_flags, commit_flags, m_VirtualMem)
#ifdef VMEM_PROTECTED_HEAP
		,m_ProtectedHeap(page_size, reserve_flags, commit_flags)
#endif
	{
	}

	//------------------------------------------------------------------------
	bool VMemHeap::Initialise()
	{
		if(!m_HeapRegions.Initialise())
			return false;

		if(!m_FSA1PageHeap.Initialise())
			return false;

		if(!m_FSA2PageHeap.Initialise())
			return false;

		if(!m_FSAHeap1.Initialise())
			return false;

		if(!m_FSAHeap2.Initialise())
			return false;

		if(!m_CoalesceHeap1.Initialise())
			return false;

		if(!m_CoalesceHeap2.Initialise())
			return false;

		if(!m_LargeHeap.Initialise())
			return false;

		if(!m_AlignedCoalesceHeap.Initialise())
			return false;

		if(!m_AlignedLargeHeap.Initialise())
			return false;

#ifndef VMEM_FSA_HEAP_PER_THREAD
		if(!InitialiseFSAHeap1(m_FSAHeap1))
			return false;
#endif
		if(!InitialiseFSAHeap2(m_FSAHeap2))
			return false;

		InitialiseTrailGuards();

#ifdef VMEM_COALESCE_HEAP_PER_THREAD
		CreateCoalesceHeapForThisThread();
#endif

		mp_FSA1PageHeapRegion = m_FSA1PageHeap.GetFirstRegion();
		mp_FSA2PageHeapRegion = m_FSA2PageHeap.GetFirstRegion();
		mp_CoalesceHeap1Region = m_CoalesceHeap1.GetFirstRegion();
		mp_CoalesceHeap2Region = m_CoalesceHeap2.GetFirstRegion();

		return true;
	}

	//------------------------------------------------------------------------
	VMemHeap::~VMemHeap()
	{
#ifdef VMEM_FSA_HEAP_PER_THREAD
		VMemHeapTLS* p_iter = mp_VMemHeapTLSList;
		while(p_iter)
		{
			VMemHeapTLS* p_next = p_iter->mp_Next;

			CoalesceHeap* p_coalesce_heap = p_iter->mp_CoalesceHeap;
			if(p_coalesce_heap)
				m_InternalHeap.Delete_WithFalseSharingBuffer(p_coalesce_heap);

			m_InternalHeap.Delete_WithFalseSharingBuffer(p_iter);

			p_iter = p_next;
		}
		gp_ThreadFASAHeap = NULL;
#endif
	}

	//------------------------------------------------------------------------
#ifdef VMEM_FSA_HEAP_PER_THREAD
	VMemHeapTLS* VMemHeap::CreateVMemHeapTLS()
	{
		VMemHeapTLS* p_fsa_heap = m_InternalHeap.New_WithFalseSharingBuffer<VMemHeapTLS>(&m_FSA1PageHeap, m_InternalHeap);

		InitialiseFSAHeap1(p_fsa_heap->m_FSAHeap1);

		{
			CriticalSectionScope lock(m_VMemHeapTLSListLock);
			p_fsa_heap->mp_Next = mp_VMemHeapTLSList;
			mp_VMemHeapTLSList = p_fsa_heap;
		}

		gp_ThreadFASAHeap = p_fsa_heap;

		return p_fsa_heap;
	}
#endif

	//------------------------------------------------------------------------
	bool VMemHeap::InitialiseFSAHeap1(FSAHeap& fsa_heap)
	{
#ifdef VMEM_X64
		const int alignment = VMEM_NATURAL_ALIGNMENT;
		const int step = VMEM_NATURAL_ALIGNMENT;

		for(int i=step; i<=HeapSettings::FSAHeap1_MaxSize; i+=step)
		{
			if(!fsa_heap.InitialiseFSA(i, alignment))
				return false;
		}
#else
		if(!fsa_heap.InitialiseFSA(4, 4)) return false;
		if(!fsa_heap.InitialiseFSA(8, 8)) return false;
		if(!fsa_heap.InitialiseFSA(12, 12)) return false;
		if(!fsa_heap.InitialiseFSA(16, 16)) return false;
		if(!fsa_heap.InitialiseFSA(20, 20)) return false;
		if(!fsa_heap.InitialiseFSA(24, 24)) return false;
		if(!fsa_heap.InitialiseFSA(28, 28)) return false;
		if(!fsa_heap.InitialiseFSA(32, 16)) return false;
		if(!fsa_heap.InitialiseFSA(36, 36)) return false;
		if(!fsa_heap.InitialiseFSA(40, 40)) return false;
		if(!fsa_heap.InitialiseFSA(44, 44)) return false;
		if(!fsa_heap.InitialiseFSA(48, 16)) return false;
		for(int i=64; i<=HeapSettings::FSAHeap1_MaxSize; i+=16)
		{
			if(!fsa_heap.InitialiseFSA(i, 16))
				return false;
		}
#endif
		return true;
	}

	//------------------------------------------------------------------------
	bool VMemHeap::InitialiseFSAHeap2(FSAHeap& fsa_heap)
	{
		const int alignment = VMEM_NATURAL_ALIGNMENT;
		const int step = VMax(16, VMEM_NATURAL_ALIGNMENT);

		size_t last_size = HeapSettings::FSAHeap1_MaxSize;

		for(size_t i=HeapSettings::FSAHeap1_MaxSize+step; i<=HeapSettings::FSAHeap2_MaxSize; i+=step)
		{
			if(i - last_size > (i * VMEM_FSA_WASTAGE) / 100 || i == HeapSettings::FSAHeap2_MaxSize)
			{
				if(!fsa_heap.InitialiseFSA(ToInt(i), alignment))
					return false;

				last_size = i;
			}
		}

		return true;
	}

	//------------------------------------------------------------------------
#ifdef VMEM_FSA_HEAP_PER_THREAD
	VMEM_FORCE_INLINE VMemHeapTLS* VMemHeap::GetVMemHeapTLS()
	{
		VMemHeapTLS* p_fsa_heap = gp_ThreadFASAHeap;
		if(!p_fsa_heap)
			p_fsa_heap = CreateVMemHeapTLS();

		return p_fsa_heap;
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_SERVICE_THREAD
	void VMemHeap::Update()
	{
		m_VirtualMem.Update();

#ifdef VMEM_FSA_HEAP_PER_THREAD
		{
			CriticalSectionScope lock(m_VMemHeapTLSListLock);
			for(VMemHeapTLS* p_iter=mp_VMemHeapTLSList; p_iter; p_iter=p_iter->mp_Next)
				p_iter->m_FSAHeap1.Update();
		}
#else
		#if defined(VMEM_FSA_PAGE_CACHE) || defined(VMEM_POPULATE_FSA_CACHE)
			m_FSAHeap1.Update();
		#endif
#endif
		#if defined(VMEM_FSA_PAGE_CACHE) || defined(VMEM_POPULATE_FSA_CACHE)
			m_FSAHeap2.Update();
		#endif

		m_CoalesceHeap1.Update();
		m_CoalesceHeap2.Update();

		m_AlignedCoalesceHeap.Update();
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_COALESCE_HEAP_PER_THREAD
	void VMemHeap::CreateCoalesceHeapForThisThread()
	{
		VMEM_ASSERT(GetVMemHeapTLS()->mp_CoalesceHeap.load() == NULL, "This thread already has its own CoalesceHeap");
		CoalesceHeap* p_coalesce_heap = m_InternalHeap.New_WithFalseSharingBuffer<CoalesceHeap>(ToInt(HeapSettings::CoalesceHeap1_RegionSize), ToInt(HeapSettings::CoalesceHeap1_MinSize), ToInt(HeapSettings::CoalesceHeap1_MaxSize), m_InternalHeap, m_HeapRegions, RegionType::TLSCoalesceHeap);
		GetVMemHeapTLS()->mp_CoalesceHeap.store(p_coalesce_heap, std::memory_order_release);
	}
#endif

	//------------------------------------------------------------------------
	void* VMemHeap::Alloc(size_t size, size_t alignment)
	{
		VMEM_GLOBAL_LOCK

		VMEM_ASSERT(size >= 0, "can't allocate negative size");

#ifdef VMEM_INC_INTEG_CHECK
		IncIntegrityCheck();
#endif

		if(alignment != VMEM_NATURAL_ALIGNMENT)
			return AlignedAlloc(size, alignment);

		void* p = NULL;

#ifdef VMEM_PROTECTED_HEAP
		p = m_ProtectedHeap.Alloc(size);
		if(p)
			return p;
#endif

#ifdef VMEM_FSA_HEAP_PER_THREAD
		if(size <= HeapSettings::FSAHeap1_MaxSize)
			p = GetVMemHeapTLS()->m_FSAHeap1.Alloc(size);
#else
		if(size <= HeapSettings::FSAHeap1_MaxSize)
			p = m_FSAHeap1.Alloc(size);
#endif
		else if(size <= HeapSettings::FSAHeap2_MaxSize)
			p = m_FSAHeap2.Alloc(size);

		else if(size <= HeapSettings::CoalesceHeap1_MaxSize)
		{
#ifdef VMEM_COALESCE_HEAP_PER_THREAD
			VMemHeapTLS* p_heap_tls = GetVMemHeapTLS();
			CoalesceHeap* p_coalesce_heap = p_heap_tls->mp_CoalesceHeap;
			if(p_coalesce_heap)
				p = p_coalesce_heap->Alloc(size);
			else
				p = m_CoalesceHeap1.Alloc(size);
#else
			p = m_CoalesceHeap1.Alloc(size);
#endif
		}

		else if(size <= HeapSettings::CoalesceHeap2_MaxSize)
			p = m_CoalesceHeap2.Alloc(size);

		else
			p = m_LargeHeap.Alloc(size);

		return p;
	}

	//------------------------------------------------------------------------
	void* VMemHeap::AlignedAlloc(size_t size, size_t alignment)
	{
		VMEM_ASSERT(alignment, "zero alignment not supported");

		void* p = NULL;

		size_t aligned_size = size + sizeof(AlignedHeader) + alignment;

#ifdef VMEM_CUSTOM_ALLOC_INFO
		bool can_use_non_aligned_heap = false;
#else
		bool can_use_non_aligned_heap = true;
#endif
		if(can_use_non_aligned_heap && alignment == (size_t)m_PageSize)
		{
			p = m_LargeHeap.Alloc(size ? size : m_PageSize);
		}
		else if(aligned_size <= HeapSettings::AlignedCoalesceHeap_MaxSize)
		{
			p = m_AlignedCoalesceHeap.Alloc(size, alignment);
		}
		else
		{
			p = m_AlignedLargeHeap.Alloc(size, alignment);
		}

#ifdef VMEM_CUSTOM_ALLOC_INFO
		VMEM_ASSERT(!p || (((uint64)p + sizeof(VMemCustomAllocInfo)) % (uint64)alignment) == 0, "alignment failed");
#else
		VMEM_ASSERT(!p || (((uint64)p) % (uint64)alignment) == 0, "alignment failed");
#endif

		return p;
	}

	//------------------------------------------------------------------------
	void VMemHeap::Free(void* p)
	{
		VMEM_GLOBAL_LOCK

#ifdef VMEM_PROTECTED_HEAP
			if(m_ProtectedHeap.Free(p))
				return;
#endif
		if(p >= mp_FSA1PageHeapRegion && p < (byte*)mp_FSA1PageHeapRegion + HeapSettings::FSAHeap1_RegionSize)
		{
			FSA::GetFSA(p, HeapSettings::FSAHeap1_PageSize)->Free(p, HeapSettings::FSAHeap1_PageSize);
			return;
		}

		if(p >= mp_FSA2PageHeapRegion && p < (byte*)mp_FSA2PageHeapRegion + HeapSettings::FSAHeap2_RegionSize)
		{
			FSA::GetFSA(p, HeapSettings::FSAHeap2_PageSize)->Free(p, HeapSettings::FSAHeap2_PageSize);
			return;
		}

		if(p >= mp_CoalesceHeap1Region && p < (byte*)mp_CoalesceHeap1Region + HeapSettings::CoalesceHeap1_RegionSize)
		{
			m_CoalesceHeap1.Free(p);
			return;
		}

		if(p >= mp_CoalesceHeap2Region && p < (byte*)mp_CoalesceHeap2Region + HeapSettings::CoalesceHeap2_RegionSize)
		{
			m_CoalesceHeap2.Free(p);
			return;
		}

		if(!p)
			return;

#ifdef VMEM_COALESCE_HEAP_PER_THREAD
		HeapRegions::RegionAndHeap region = m_HeapRegions.GetRegionAndHeap(p);
		RegionType::Enum region_type = region.m_RegionType;
#else
		RegionType::Enum region_type = m_HeapRegions.GetRegion(p);
#endif
		switch(region_type)
		{
			case RegionType::PageHeap1:
				FSA::GetFSA(p, HeapSettings::FSAHeap1_PageSize)->Free(p, HeapSettings::FSAHeap1_PageSize);
				return;

			case RegionType::PageHeap2:
				FSA::GetFSA(p, HeapSettings::FSAHeap2_PageSize)->Free(p, HeapSettings::FSAHeap2_PageSize);
				return;

			case RegionType::CoalesceHeap1:
				m_CoalesceHeap1.Free(p);
				return;

			case RegionType::CoalesceHeap2:
				m_CoalesceHeap2.Free(p);
				return;

			case RegionType::AlignedCoalesceHeap:
				m_AlignedCoalesceHeap.Free(p);
				return;

#ifdef VMEM_COALESCE_HEAP_PER_THREAD
			case RegionType::TLSCoalesceHeap:
				((CoalesceHeap*)region.mp_Heap)->Free(p);
				return;
#endif
			case RegionType::Invalid:
			{
				if(m_LargeHeap.Free(p))
					return;

				if(m_AlignedLargeHeap.Free(p))
					return;
			} break;
		}

		VMEM_BREAK("Trying to free allocation that is not owned by VMem");
	}

	//------------------------------------------------------------------------
	size_t VMemHeap::GetSize(void* p)
	{
		VMEM_GLOBAL_LOCK

		if(p >= mp_FSA1PageHeapRegion && p < (byte*)mp_FSA1PageHeapRegion + HeapSettings::FSAHeap1_RegionSize)
		{
			return FSA::GetFSA(p, HeapSettings::FSAHeap1_PageSize)->GetSize();
		}

		if (p >= mp_FSA2PageHeapRegion && p < (byte*)mp_FSA2PageHeapRegion + HeapSettings::FSAHeap2_RegionSize)
		{
			return FSA::GetFSA(p, HeapSettings::FSAHeap2_PageSize)->GetSize();
		}

		if ((p >= mp_CoalesceHeap1Region && p < (byte*)mp_CoalesceHeap1Region + HeapSettings::CoalesceHeap1_RegionSize) ||
			(p >= mp_CoalesceHeap2Region && p < (byte*)mp_CoalesceHeap2Region + HeapSettings::CoalesceHeap2_RegionSize))
		{
			return CoalesceHeap::GetSize(p);
		}

#ifdef VMEM_PROTECTED_HEAP
		size_t size = m_ProtectedHeap.GetSize(p);
		if(size != VMEM_INVALID_SIZE) return size;
#endif

#ifdef VMEM_COALESCE_HEAP_PER_THREAD
		HeapRegions::RegionAndHeap region = m_HeapRegions.GetRegionAndHeap(p);
		RegionType::Enum region_type = region.m_RegionType;
#else
		RegionType::Enum region_type = m_HeapRegions.GetRegion(p);
#endif
		switch(region_type)
		{
			case RegionType::PageHeap1:
				return FSA::GetFSA(p, HeapSettings::FSAHeap1_PageSize)->GetSize();

			case RegionType::PageHeap2:
				return FSA::GetFSA(p, HeapSettings::FSAHeap2_PageSize)->GetSize();

			case RegionType::CoalesceHeap1:
				return CoalesceHeap::GetSize(p);

			case RegionType::CoalesceHeap2:
				return CoalesceHeap::GetSize(p);

			case RegionType::AlignedCoalesceHeap:
				return AlignedCoalesceHeap::GetSize(p);

#ifdef VMEM_COALESCE_HEAP_PER_THREAD
			case RegionType::TLSCoalesceHeap:
				return CoalesceHeap::GetSize(p);
#endif
			case RegionType::Invalid:
			{
				size_t large_size = m_LargeHeap.GetSize(p);
				if(large_size != VMEM_INVALID_SIZE) return large_size;

				size_t large_aligned_size = m_AlignedLargeHeap.GetSize(p);
				if(large_aligned_size != VMEM_INVALID_SIZE) return large_aligned_size;
			} break;
		}

		return VMEM_INVALID_SIZE;
	}

	//------------------------------------------------------------------------
	bool VMemHeap::Owns(void* p)
	{
		VMEM_GLOBAL_LOCK

		if(p >= mp_FSA1PageHeapRegion && p < (byte*)mp_FSA1PageHeapRegion + HeapSettings::FSAHeap1_RegionSize)
			return true;

		if (p >= mp_FSA2PageHeapRegion && p < (byte*)mp_FSA2PageHeapRegion + HeapSettings::FSAHeap2_RegionSize)
			return true;

		if ((p >= mp_CoalesceHeap1Region && p < (byte*)mp_CoalesceHeap1Region + HeapSettings::CoalesceHeap1_RegionSize) ||
			(p >= mp_CoalesceHeap2Region && p < (byte*)mp_CoalesceHeap2Region + HeapSettings::CoalesceHeap2_RegionSize))
		{
			return true;
		}

#ifdef VMEM_PROTECTED_HEAP
		if(m_ProtectedHeap.GetSize(p) != VMEM_INVALID_SIZE)
			return true;
#endif

#ifdef VMEM_COALESCE_HEAP_PER_THREAD
		HeapRegions::RegionAndHeap region = m_HeapRegions.GetRegionAndHeap(p);
		RegionType::Enum region_type = region.m_RegionType;
#else
		RegionType::Enum region_type = m_HeapRegions.GetRegion(p);
#endif
		if (region_type != RegionType::Invalid)
			return true;

		if(m_LargeHeap.Owns(p))
			return true;

		if(m_AlignedLargeHeap.Owns(p))
			return true;

		return false;
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	void VMemHeap::SendStatsToMemPro(MemProSendFn send_fn, void* p_context)
	{
		VMEM_GLOBAL_LOCK
		m_InternalHeap.SendStatsToMemPro(send_fn, p_context);
		m_FSA1PageHeap.SendStatsToMemPro(send_fn, p_context);
		m_FSA2PageHeap.SendStatsToMemPro(send_fn, p_context);
#ifdef VMEM_FSA_HEAP_PER_THREAD
		{
			CriticalSectionScope lock(m_VMemHeapTLSListLock);
			for(VMemHeapTLS* p_iter=mp_VMemHeapTLSList; p_iter; p_iter=p_iter->mp_Next)
				p_iter->m_FSAHeap1.SendStatsToMemPro(send_fn, p_context);
		}
#else
		m_FSAHeap1.SendStatsToMemPro(send_fn, p_context);
#endif
		m_FSAHeap2.SendStatsToMemPro(send_fn, p_context);

		m_CoalesceHeap1.SendStatsToMemPro(send_fn, p_context);
		m_CoalesceHeap2.SendStatsToMemPro(send_fn, p_context);

		m_LargeHeap.SendStatsToMemPro(send_fn, p_context);
		m_AlignedCoalesceHeap.SendStatsToMemPro(send_fn, p_context);
		m_AlignedLargeHeap.SendStatsToMemPro(send_fn, p_context);

		SendEnumToMemPro(vmem_End, send_fn, p_context);
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	VMemHeapStats VMemHeap::GetStatsNoLock() const
	{
		VMemHeapStats stats;

		Stats page_heap1 = m_FSA1PageHeap.GetStatsNoLock();
		page_heap1.m_Used = 0;												// this is tracked by the FSAHeap
#ifdef VMEM_FSA_HEAP_PER_THREAD
		for(VMemHeapTLS* p_iter=mp_VMemHeapTLSList; p_iter; p_iter=p_iter->mp_Next)
		{
			stats.m_FSAHeap1 += p_iter->m_FSAHeap1.GetStatsNoLock();
			stats.m_Internal.m_Overhead += sizeof(VMemHeapTLS) + InternalHeap::GetFalseSharingBufferOverhead();

			#ifdef VMEM_COALESCE_HEAP_PER_THREAD
				CoalesceHeap* p_coalesce_heap = p_iter->mp_CoalesceHeap.load();
				if(p_coalesce_heap)
				{
					stats.m_CoalesceHeap1 += p_coalesce_heap->GetStatsNoLock();
					stats.m_Internal.m_Overhead += sizeof(CoalesceHeap) + InternalHeap::GetFalseSharingBufferOverhead();
				}
			#endif
		}
		stats.m_FSAHeap1 += page_heap1;
#else
		stats.m_FSAHeap1 = m_FSAHeap1.GetStatsNoLock() + page_heap1;
#endif

		Stats page_heap2 = m_FSA2PageHeap.GetStatsNoLock();
		page_heap2.m_Used = 0;												// this is tracked by the FSAHeap
		stats.m_FSAHeap2 = m_FSAHeap2.GetStatsNoLock() + page_heap2;

		stats.m_CoalesceHeap1 += m_CoalesceHeap1.GetStatsNoLock();
		stats.m_CoalesceHeap2 = m_CoalesceHeap2.GetStatsNoLock();

		stats.m_LargeHeap = m_LargeHeap.GetStatsNoLock();

		stats.m_AlignedCoalesceHeap = m_AlignedCoalesceHeap.GetStatsNoLock();
		stats.m_AlignedLargeHeap = m_AlignedLargeHeap.GetStatsNoLock();

		stats.m_Internal += m_InternalHeap.GetStatsNoLock();
		stats.m_Internal.m_Used = 0;										// this is tracked by the allocator overheads

		stats.m_Internal.m_Overhead += m_HeapRegions.GetMemoryUsage_NoLock();

		stats.m_Total =
			stats.m_FSAHeap1 +
			stats.m_FSAHeap2 +
			stats.m_CoalesceHeap1 +
			stats.m_CoalesceHeap2 +
			stats.m_LargeHeap +
			stats.m_AlignedCoalesceHeap +
			stats.m_AlignedLargeHeap +
			stats.m_Internal;

#ifdef VMEM_PROTECTED_HEAP
		stats.m_Total += m_ProtectedHeap.GetStatsNoLock();
#endif

		Stats virtual_mem_stats = m_VirtualMem.GetStatsNoLock();
		stats.m_Internal += virtual_mem_stats;
		stats.m_Total += virtual_mem_stats;

		return stats;
	}
#endif

	//------------------------------------------------------------------------
	void VMemHeap::Flush()
	{
#ifdef VMEM_FSA_HEAP_PER_THREAD
		{
			CriticalSectionScope lock(m_VMemHeapTLSListLock);
			for(VMemHeapTLS* p_iter=mp_VMemHeapTLSList; p_iter; p_iter=p_iter->mp_Next)
			{
				p_iter->m_FSAHeap1.Flush();

				#ifdef VMEM_COALESCE_HEAP_PER_THREAD
					CoalesceHeap* p_coalesce_heap = p_iter->mp_CoalesceHeap.load();
					if(p_coalesce_heap)
						p_coalesce_heap->Flush();
				#endif
			}
		}
#else
		m_FSAHeap1.Flush();
#endif
		m_FSAHeap2.Flush();

		m_CoalesceHeap1.Flush();
		m_CoalesceHeap2.Flush();

		m_VirtualMem.Flush();
	}

	//------------------------------------------------------------------------
	void VMemHeap::CheckIntegrity()
	{
		VMEM_GLOBAL_LOCK

#ifdef VMEM_FSA_HEAP_PER_THREAD
		{
			CriticalSectionScope lock(m_VMemHeapTLSListLock);
			for(VMemHeapTLS* p_iter=mp_VMemHeapTLSList; p_iter; p_iter=p_iter->mp_Next)
				p_iter->m_FSAHeap1.CheckIntegrity();
		}
#else
		m_FSAHeap1.CheckIntegrity();
#endif
		m_FSAHeap2.CheckIntegrity();

		m_CoalesceHeap1.CheckIntegrity();
		m_CoalesceHeap2.CheckIntegrity();

#ifdef VMEM_PROTECTED_HEAP
		m_ProtectedHeap.CheckIntegrity();
#endif

		m_VirtualMem.CheckIntegrity();
	}

	//------------------------------------------------------------------------
#ifdef VMEM_INC_INTEG_CHECK
	void VMemHeap::IncIntegrityCheck()
	{
		static volatile long i = 0;
		if(i == VMEM_INC_INTEG_CHECK)
		{
#ifdef VMEM_FSA_HEAP_PER_THREAD
			{
				CriticalSectionScope lock(m_VMemHeapTLSListLock);
				for(VMemHeapTLS* p_iter=mp_VMemHeapTLSList; p_iter; p_iter=p_iter->mp_Next)
					p_iter->m_FSAHeap1.IncIntegrityCheck();
			}
#else
			m_FSAHeap1.IncIntegrityCheck();
#endif
		}
		else if(i == 2*VMEM_INC_INTEG_CHECK)
		{
			m_FSAHeap2.IncIntegrityCheck();
		}
		else if(i == 3*VMEM_INC_INTEG_CHECK)
		{
			m_CoalesceHeap1.CheckIntegrity();
		}
		else if(i == 4*VMEM_INC_INTEG_CHECK)
		{
			m_CoalesceHeap2.CheckIntegrity();
		}
		else if(i == 6*VMEM_INC_INTEG_CHECK)
		{
			i = -1;
		}

		VMem_InterlockedIncrement(i);
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	size_t VMemHeap::WriteAllocs()
	{
		VMEM_GLOBAL_LOCK

		size_t allocated_bytes = 0;

#ifdef VMEM_FSA_HEAP_PER_THREAD
		{
			CriticalSectionScope lock(m_VMemHeapTLSListLock);
			for(VMemHeapTLS* p_iter=mp_VMemHeapTLSList; p_iter; p_iter=p_iter->mp_Next)
				allocated_bytes += p_iter->m_FSAHeap1.WriteAllocs();
		}
#else
		allocated_bytes += m_FSAHeap1.WriteAllocs();
#endif
		allocated_bytes += m_FSAHeap2.WriteAllocs();

		allocated_bytes += m_CoalesceHeap1.WriteAllocs();
		allocated_bytes += m_CoalesceHeap2.WriteAllocs();

		allocated_bytes += m_LargeHeap.WriteAllocs();

		allocated_bytes += m_AlignedCoalesceHeap.WriteAllocs();
		allocated_bytes += m_AlignedCoalesceHeap.WriteAllocs();

		return allocated_bytes;
	}
#endif

	//------------------------------------------------------------------------
	void VMemHeap::InitialiseTrailGuards()
	{
#ifdef VMEM_TRAIL_GUARDS
		int start_size = sizeof(void*);
		for(int i=start_size; i<=ToInt(HeapSettings::FSAHeap1_MaxSize); i+=4)
		{
#ifdef VMEM_FSA_HEAP_PER_THREAD
			{
				CriticalSectionScope lock(m_VMemHeapTLSListLock);
				for(VMemHeapTLS* p_iter=mp_VMemHeapTLSList; p_iter; p_iter=p_iter->mp_Next)
					p_iter->m_FSAHeap1.GetFSA(i)->InitialiseTrailGuard(100*1024, 1000);
			}
#else
			FSA* p_fsa = m_FSAHeap1.GetFSA(i);
			p_fsa->InitialiseTrailGuard(100*1024, 1000);
#endif
		}

		for(int i=ToInt(HeapSettings::FSAHeap1_MaxSize)+4; i<=ToInt(HeapSettings::FSAHeap2_MaxSize); i+=4)
		{
			FSA* p_fsa = m_FSAHeap2.GetFSA(i);
			p_fsa->InitialiseTrailGuard(100*1024, 1000);
		}

		m_CoalesceHeap1.InitialiseTrailGuard(1024*1024, 100);
		m_CoalesceHeap2.InitialiseTrailGuard(1024*1024, 100);
#endif
	}

	//------------------------------------------------------------------------
	void VMemHeap::AcquireLockShared() const
	{
#ifdef VMEM_FSA_HEAP_PER_THREAD
		m_VMemHeapTLSListLock.Enter();

		for(VMemHeapTLS* p_iter=mp_VMemHeapTLSList; p_iter; p_iter=p_iter->mp_Next)
		{
			p_iter->m_FSAHeap1.Lock();

			#ifdef VMEM_COALESCE_HEAP_PER_THREAD
				CoalesceHeap* p_coalesce_heap = p_iter->mp_CoalesceHeap.load();
				if(p_coalesce_heap)
					p_coalesce_heap->Lock();
			#endif
		}
#else
		m_FSAHeap1.Lock();
#endif
		m_FSAHeap2.Lock();

		m_CoalesceHeap1.Lock();
		m_CoalesceHeap2.Lock();

		m_LargeHeap.AcquireLockShared();
		m_AlignedCoalesceHeap.Lock();
		m_AlignedLargeHeap.AcquireLockShared();
#ifdef VMEM_PROTECTED_HEAP
		m_ProtectedHeap.Lock();
#endif
		m_VirtualMem.Lock();
	}

	//------------------------------------------------------------------------
	void VMemHeap::ReleaseLockShared() const
	{
		m_VirtualMem.Release();

		m_AlignedLargeHeap.ReleaseLockShared();
		m_AlignedCoalesceHeap.Release();
		m_LargeHeap.ReleaseLockShared();

		m_CoalesceHeap2.Release();
		m_CoalesceHeap1.Release();

		m_FSAHeap2.Release();
#ifdef VMEM_FSA_HEAP_PER_THREAD
		for(VMemHeapTLS* p_iter=mp_VMemHeapTLSList; p_iter; p_iter=p_iter->mp_Next)
		{
			p_iter->m_FSAHeap1.Release();

			#ifdef VMEM_COALESCE_HEAP_PER_THREAD
				CoalesceHeap* p_coalesce_heap = p_iter->mp_CoalesceHeap.load();
				if(p_coalesce_heap)
					p_coalesce_heap->Release();
			#endif
		}
#else
		m_FSAHeap1.Release();
#endif

#ifdef VMEM_PROTECTED_HEAP
		m_ProtectedHeap.Release();
#endif

	#ifdef VMEM_FSA_HEAP_PER_THREAD
		m_VMemHeapTLSListLock.Leave();
	#endif
	}
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

