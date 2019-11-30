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
#include "VMemAlloc.hpp"
#include "VMemSys.hpp"
#include "VMemHeap.hpp"
#include "VirtualMem.hpp"
#include "ServiceThread.hpp"
#include "PhysicalHeap.hpp"
#include "VMemDefs.hpp"
#include <stdio.h>

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
// write each allocation and free out to a binary file. Each operation is of
// the form <pointer (4 bytes), size (4 bytes)>
// Free operations are identified by a size of VMEM_INVALID_SIZE
// Playing back the exact allocations and frees can be useful for tracking down bugs.
//#define VMEM_RECORD_ALLOCS

//------------------------------------------------------------------------
#if defined(VMEM_RECORD_ALLOCS)
	#if defined(VMEM_PLATFORM_LINUX) || defined(VMEM_PLATFORM_APPLE) || defined(VMEM_PLATFORM_ANDROID)
		#ifdef __GLIBC__
			#include <sched.h>
		#else
			#include <linux/getcpu.h>
		#endif
	#endif

	#if defined(VMEM_PLATFORM_SWITCH)
		#ifdef __GLIBC__
			#include <sched.h>
		#else
			#include <linux/getcpu.h>
		#endif
	#endif
#endif

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	#define VMEM_CUSTOM_HEAP_MARKER 0xc5084ea6c5084ea6ULL
	#define VMEM_PHYSICAL_CUSTOM_HEAP_MARKER 0x64ea440064ea4400ULL

	//------------------------------------------------------------------------
	struct VMemMain
	{
		VMemMain()
		:	m_MainHeap(VMEM_SYS_PAGE_SIZE, VMEM_DEFAULT_RESERVE_FLAGS, VMEM_DEFAULT_COMMIT_FLAGS),
			mp_CustomHeaps(NULL),
			mp_PhysicalCustomHeaps(NULL)
		{
		}

		VMemHeap m_MainHeap;

		CriticalSection m_CriticalSection;

		struct CustomHeap* mp_CustomHeaps;

		struct PhysicalCustomHeap* mp_PhysicalCustomHeaps;
	};

	//------------------------------------------------------------------------
	VMEM_THREAD_LOCAL HeapHandle g_ThreadHeapHandle;

	//------------------------------------------------------------------------
	VMemHeapStats GetStats_NoLock(VMemMain* p_vmem);

	//------------------------------------------------------------------------
	// we do it this way to avoid order of static initialisation problems
	VMem::byte g_VMemMem[sizeof(VMemMain)] VMEM_ALIGN_8;
	VMemMain* gp_VMem = NULL;

#ifdef VMEM_SERVICE_THREAD
	VMem::byte g_ServiceThreadMem[sizeof(ServiceThread)] VMEM_ALIGN_8;
	ServiceThread* gp_ServiceThread = NULL;
#endif

	//------------------------------------------------------------------------
	VMEM_FORCE_INLINE VMemMain* GetVMem()
	{
		VMemMain* p_vmem = gp_VMem;
		VMEM_ASSERT(p_vmem, "VMem has not been initialised!");
		return p_vmem;
	}

	//------------------------------------------------------------------------
	struct CustomHeap
	{
		CustomHeap(
			int page_size,
			int reserve_flags,
			int commit_flags)
		:
			m_Heap(page_size, reserve_flags, commit_flags)
#ifdef VMEM_ASSERTS
			,m_Marker(VMEM_CUSTOM_HEAP_MARKER)
#endif
		{
		}
		VMemHeap m_Heap;
		CustomHeap* mp_Next;

		#ifdef VMEM_ASSERTS
			uint64 m_Marker;
		#endif
	};

	//------------------------------------------------------------------------
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
	struct PhysicalCustomHeap
	{
		PhysicalCustomHeap(size_t coalesce_heap_region_size, size_t coalesce_heap_max_size, int flags)
		:	m_Heap(coalesce_heap_region_size, coalesce_heap_max_size, flags)
			#ifdef VMEM_ASSERTS
			,m_Marker(VMEM_PHYSICAL_CUSTOM_HEAP_MARKER)
			#endif
		{
		}

		PhysicalHeap m_Heap;
		PhysicalCustomHeap* mp_Next;

		#ifdef VMEM_ASSERTS
			uint64 m_Marker;
		#endif
	};
#endif

	//------------------------------------------------------------------------
	VMEM_NO_INLINE void OutOfMemoryHandler()
	{
		DebugWrite(_T("Out of memory!\n"));

#ifdef VMEM_ENABLE_STATS
		WriteStats();
#endif
		DumpSystemMemory();
	}

	//------------------------------------------------------------------------
	class TeardownIntegrityCheck
	{
	public:
		~TeardownIntegrityCheck() { CheckIntegrity(); }
	};

	TeardownIntegrityCheck g_TeardownIntegrityCheck;

	//------------------------------------------------------------------------
#ifdef VMEM_PROTECTED_HEAP
	VMemSHouldProtectFn g_VMemSHouldProtectFn = NULL;
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_RECORD_ALLOCS
	VMem::byte g_RecordCritSecMem[sizeof(CriticalSection)] VMEM_ALIGN_8;
	CriticalSection& g_RecordCritSec = (CriticalSection&)g_RecordCritSecMem;
	FILE* gp_RecordFile = NULL;
	void InitialiseRecording()
	{
		new (&g_RecordCritSec)CriticalSection();

		#ifdef VMEM_PLATFORM_PS4
			const char* p_filename = "/hostapp/allocs.bin";
		#else
			const char* p_filename = "allocs.bin";
		#endif

		#ifdef VMEM_PLATFORM_PS4
			gp_RecordFile = (FILE*)(unsigned long long)sceKernelOpen(p_filename, SCE_KERNEL_O_WRONLY | SCE_KERNEL_O_CREAT, SCE_KERNEL_S_IRWU);
		#elif defined(VMEM_OS_WIN)
			fopen_s(&gp_RecordFile, p_filename, "wb");
		#else
			gp_RecordFile = fopen(p_filename, "wb");
		#endif
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_RECORD_ALLOCS
	void UninitialiseRecording()
	{
		#ifdef VMEM_PLATFORM_PS4
			sceKernelClose((int)(unsigned long long)gp_RecordFile);
		#else
			fclose(gp_RecordFile);
		#endif
		g_RecordCritSec.~CriticalSection();
	}
#endif


	//------------------------------------------------------------------------
#ifdef VMEM_SERVICE_THREAD
	void ServiceThreadUpdate()
	{
		VMemMain* p_vmem = gp_VMem;

		if(p_vmem)
		{
			CriticalSectionScope lock(p_vmem->m_CriticalSection);

			p_vmem->m_MainHeap.Update();

			for (CustomHeap* p_heap = p_vmem->mp_CustomHeaps; p_heap; p_heap = p_heap->mp_Next)
				p_heap->m_Heap.Update();
		}
	}
#endif

	//------------------------------------------------------------------------
	bool InitialiseVMem()
	{
#ifdef VMEM_RECORD_ALLOCS
		InitialiseRecording();
#endif
		// only one thhread will make the lock, so when another thread gets
		// the lock it must check that it hasn't already been initialised
		if(!gp_VMem)
		{
			InitialiseCore();

			#ifdef VMEM_SERVICE_THREAD
				Thread::Initialise();
			#endif

			VMemMain* p_vmem = (VMemMain*)g_VMemMem;
			VMEM_MEMSET((void*)p_vmem, VMEM_ALLOCATED_MEM, sizeof(g_VMemMem));

			new (p_vmem) VMemMain();

			gp_VMem = p_vmem;

			if(!p_vmem->m_MainHeap.Initialise())
			{
				p_vmem->~VMemMain();
				UninitialiseCore();
				#ifdef VMEM_SERVICE_THREAD
					Thread::Destroy();
				#endif
				gp_VMem = NULL;
				return false;
			}

			// must do this LAST because creating a thread can call malloc, and if malloc
			// is overridden we can get into an inifnite loop!
			// as long as gp_VMem has been set we should avoid infinity.
#ifdef VMEM_SERVICE_THREAD
			gp_ServiceThread = (ServiceThread*)g_ServiceThreadMem;
			new (gp_ServiceThread) ServiceThread(ServiceThreadUpdate);

			if(!gp_ServiceThread->Initialise())
				return false;
#endif
		}

		return true;
	}

	//------------------------------------------------------------------------
	// we need to initialise VMem in global startup because the initialise is
	// not thread safe. Global startup should happen before any threads are
	// created. If you are creating threads in global startup then the results
	// are undefined.
	class ForceInitialisationDuringGlobalStartup
	{
	public:
		ForceInitialisationDuringGlobalStartup()
		{
			if(!gp_VMem)
			{
				if(!InitialiseVMem())
					VMem::Break();		// failed to initialise VMem
			}
		}
	} g_ForceInitialisationDuringGlobalStartup;

	//------------------------------------------------------------------------
#ifdef VMEM_RECORD_ALLOCS
	void WriteToRecordingFile(void* p_data, size_t size)
	{
		#if defined(VMEM_PLATFORM_PS4)
			size_t result = sceKernelWrite((int)(unsigned long long)gp_RecordFile, p_data, size);
			VMEM_ASSERT(result == size, "error writing to recording file");
			VMEM_UNREFERENCED_PARAM(result);
		#else
			fwrite(p_data, size, 1, gp_RecordFile);
		#endif
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_RECORD_ALLOCS
	void Record(void* p, size_t size, size_t alignment, void* p_new)
	{
		uint64 time = 0;
		int cpuid = 0;

		#ifdef VMEM_OS_WIN
			QueryPerformanceCounter((LARGE_INTEGER*)&time);
			cpuid = GetCurrentProcessorNumber();
		#elif defined(__GLIBC__)
			cpuid = sched_getcpu();
		#elif defined(VMEM_PLATFORM_PS4)
			cpuid = sceKernelGetCurrentCpu();
		#else
			unsigned int node = 0;
			getcpu((unsigned int*)&cpuid, &node, NULL);
		#endif

		g_RecordCritSec.Enter();
		WriteToRecordingFile(&p, sizeof(p));
		WriteToRecordingFile(&size, sizeof(size));
		WriteToRecordingFile(&alignment, sizeof(size));
		WriteToRecordingFile(&p_new, sizeof(p_new));
		WriteToRecordingFile(&time, sizeof(time));
		WriteToRecordingFile(&cpuid, sizeof(cpuid));
		g_RecordCritSec.Leave();
	}
	#define VMEM_RECORD(addr, byte_count, alignment, new_addr) Record(addr, byte_count, alignment, new_addr);
#else
	#define VMEM_RECORD(addr, byte_count, alignment, new_addr)
#endif

	//------------------------------------------------------------------------
	VMEM_RESTRICT_RETURN void* Alloc(size_t size, size_t alignment)
	{
		VMemMain* p_vmem = gp_VMem;
		if(!p_vmem)
		{
			if(!InitialiseVMem())
				return NULL;
			p_vmem = gp_VMem;
		}

		size_t size_with_info;
#ifdef VMEM_CUSTOM_ALLOC_INFO
		VMEM_STATIC_ASSERT((sizeof(VMemCustomAllocInfo) % VMEM_NATURAL_ALIGNMENT) == 0, "invalid size for VMemCustomAllocInfo");
		size_with_info = size + sizeof(VMemCustomAllocInfo);
#else
		size_with_info = size;
#endif
		void* p = p_vmem->m_MainHeap.Alloc(size_with_info, alignment);

		if(!p)
		{
			Flush();
			p = p_vmem->m_MainHeap.Alloc(size_with_info, alignment);
			if(!p)
			{
				OutOfMemoryHandler();
				return NULL;
			}
		}

#ifdef VMEM_CUSTOM_ALLOC_INFO
		memset(p, 0, sizeof(VMemCustomAllocInfo));
		p = (VMem::byte*)p + sizeof(VMemCustomAllocInfo);
#endif
		VMEM_RECORD(p, size, alignment, NULL);

		return p;
	}

	//------------------------------------------------------------------------
	void Free(void* p)
	{
#ifdef VMEM_CUSTOM_ALLOC_INFO
		void* p_alloc = (VMem::byte*)p - sizeof(VMemCustomAllocInfo);
#else
		void* p_alloc = p;
#endif
		GetVMem()->m_MainHeap.Free(p_alloc);

		VMEM_RECORD(p, VMEM_INVALID_SIZE, VMEM_INVALID_SIZE, NULL);
	}

	//------------------------------------------------------------------------
	size_t GetSize(void* p)
	{
#ifdef VMEM_CUSTOM_ALLOC_INFO
		void* p_alloc = (VMem::byte*)p - sizeof(VMemCustomAllocInfo);
#else
		void* p_alloc = p;
#endif
		return GetVMem()->m_MainHeap.GetSize(p_alloc);
	}

	//------------------------------------------------------------------------
	bool Owns(void* p)
	{
#ifdef VMEM_CUSTOM_ALLOC_INFO
		void* p_alloc = (VMem::byte*)p - sizeof(VMemCustomAllocInfo);
#else
		void* p_alloc = p;
#endif
		return GetVMem()->m_MainHeap.Owns(p_alloc);
	}

	//------------------------------------------------------------------------
	VMEM_RESTRICT_RETURN void* Realloc(void* p, size_t new_size, size_t alignment)
	{
#ifdef VMEM_CUSTOM_ALLOC_INFO
		size_t new_alloc_size = new_size + sizeof(VMemCustomAllocInfo);
#else
		size_t new_alloc_size = new_size;
#endif
		VMemMain* p_vmem = gp_VMem;
		if(!p_vmem)
		{
			if(!InitialiseVMem())
				return NULL;
			p_vmem = gp_VMem;
		}

		void* new_p = NULL;
		if(p)
		{
#ifdef VMEM_CUSTOM_ALLOC_INFO
			void* p_alloc = (VMem::byte*)p - sizeof(VMemCustomAllocInfo);
#else
			void* p_alloc = p;
#endif
			size_t old_size = p_vmem->m_MainHeap.GetSize(p_alloc);

			bool new_alloc = true;
			if(new_alloc_size <= old_size)
			{
				if(new_alloc_size <= 512)
					new_alloc = new_alloc_size > old_size || new_alloc_size < old_size/2;
				else
					new_alloc = new_alloc_size > old_size || new_alloc_size < 6*old_size/8;
			}

			if(new_alloc)
			{
				new_p = p_vmem->m_MainHeap.Alloc(new_alloc_size, alignment);
				if(!new_p)
				{
					p_vmem->m_MainHeap.Flush();
					new_p = p_vmem->m_MainHeap.Alloc(new_alloc_size, alignment);
					if(!new_p)
					{
						OutOfMemoryHandler();
						return NULL;
					}
				}

				VMEM_ASSERT(old_size != VMEM_INVALID_SIZE, "trying to realloc invalid pointer");
				size_t copy_size = VMin(old_size, new_alloc_size);
				memcpy(new_p, p_alloc, copy_size);
				p_vmem->m_MainHeap.Free(p_alloc);
			}
			else
			{
				new_p = p_alloc;
			}
		}
		else
		{
			new_p = p_vmem->m_MainHeap.Alloc(new_alloc_size, alignment);
			if(!new_p)
			{
				Flush();
				new_p = p_vmem->m_MainHeap.Alloc(new_alloc_size, alignment);
				if(!new_p)
				{
					OutOfMemoryHandler();
					return NULL;
				}
			}

			#ifdef VMEM_CUSTOM_ALLOC_INFO
				memset(new_p, 0, sizeof(VMemCustomAllocInfo));
			#endif
		}

#ifdef VMEM_CUSTOM_ALLOC_INFO
		new_p = (VMem::byte*)new_p + sizeof(VMemCustomAllocInfo);
#endif
		VMEM_RECORD(p, new_size, alignment, new_p);

		return new_p;
	}

	//------------------------------------------------------------------------
	void* AllocAligned(size_t size, size_t alignment)
	{
		if (alignment == 0)
			alignment = VMem::VMax<size_t>(VMem::VMin<size_t>(size, VMEM_NATURAL_ALIGNMENT), 4);

		size_t aligned_size = size + sizeof(AlignedHeader) + alignment;

		void* p = Alloc(aligned_size);
		if(!p)
		{
			Flush();
			p = Alloc(aligned_size);
			if(!p)
			{
				OutOfMemoryHandler();
				return NULL;
			}
		}

		void* aligned_p = AlignUp((byte*)p + sizeof(AlignedHeader), alignment);

		AlignedHeader* p_header = (AlignedHeader*)aligned_p - 1;
		p_header->p = p;

		return aligned_p;
	}

	//------------------------------------------------------------------------
	void* ReallocAligned(void* p, size_t new_size, size_t alignment)
	{
		void* new_p = AllocAligned(new_size, alignment);
		if(p)
		{
			size_t old_size = GetSizeAligned(p);
			VMEM_ASSERT(old_size != VMEM_INVALID_SIZE, "trying to realloc invalid pointer");
			size_t copy_size = VMem::VMin(old_size, new_size);
			if(new_p)
				memcpy_s(new_p, new_size, p, copy_size);
			FreeAligned(p);
		}
		return new_p;
	}

	//------------------------------------------------------------------------
	void FreeAligned(void* p)
	{
		if (p)
		{
			AlignedHeader* p_header = (AlignedHeader*)p - 1;
			Free(p_header->p);
		}
	}

	//------------------------------------------------------------------------
	size_t GetSizeAligned(void* p)
	{
		if(!p)
			return VMEM_INVALID_SIZE;

		AlignedHeader* p_header = (AlignedHeader*)p - 1;
		void* p_alloc = p_header->p;
		size_t aligned_size = GetSize(p_alloc);
		return aligned_size - ((byte*)p - (byte*)p_alloc);
	}

	//------------------------------------------------------------------------
	bool Initialise()
	{
		return gp_VMem ? true : InitialiseVMem();
	}

	//------------------------------------------------------------------------
	void DestroyAllCustomHeaps()
	{
		VMemMain* p_vmem = GetVMem();

		CustomHeap* p_heap = p_vmem->mp_CustomHeaps;
		while (p_heap)
		{
			CustomHeap* p_next = p_heap->mp_Next;
			p_heap->~CustomHeap();
			Free(p_heap);
			p_heap = p_next;
		}
		p_vmem->mp_CustomHeaps = NULL;
	}

	//------------------------------------------------------------------------
	void DestroyAllPhysicalCustomHeaps()
	{
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
		VMemMain* p_vmem = GetVMem();

		PhysicalCustomHeap* p_heap = p_vmem->mp_PhysicalCustomHeaps;
		while (p_heap)
		{
			PhysicalCustomHeap* p_next = p_heap->mp_Next;
			p_heap->~PhysicalCustomHeap();
			Free(p_heap);
			p_heap = p_next;
		}
		p_vmem->mp_PhysicalCustomHeaps = NULL;
#endif
	}

	//------------------------------------------------------------------------
	void Destroy()
	{
#ifdef VMEM_SERVICE_THREAD
		if(gp_ServiceThread)
		{
			gp_ServiceThread->Stop();
			gp_ServiceThread->~ServiceThread();
			gp_ServiceThread = NULL;
		}
#endif
		if(gp_VMem)
		{
			DestroyAllCustomHeaps();
			DestroyAllPhysicalCustomHeaps();

			gp_VMem->~VMemMain();
			gp_VMem = NULL;

			UninitialiseCore();

			#ifdef VMEM_SERVICE_THREAD
				Thread::Destroy();
			#endif
		}

#ifdef VMEM_RECORD_ALLOCS
		UninitialiseRecording();
#endif
	}

	//------------------------------------------------------------------------
	void Flush()
	{
		VMemMain* p_vmem = GetVMem();

		CriticalSectionScope lock(p_vmem->m_CriticalSection);

		p_vmem->m_MainHeap.Flush();

		for (CustomHeap* p_heap = p_vmem->mp_CustomHeaps; p_heap; p_heap = p_heap->mp_Next)
			p_heap->m_Heap.Flush();
	}

	//------------------------------------------------------------------------
	void LockAllHeaps()
	{
		VMemMain* p_vmem = GetVMem();

		p_vmem->m_MainHeap.AcquireLockShared();
		for (CustomHeap* p_heap = p_vmem->mp_CustomHeaps; p_heap; p_heap = p_heap->mp_Next)
			p_heap->m_Heap.AcquireLockShared();
	}

	//------------------------------------------------------------------------
	void UnlockAllHeaps()
	{
		VMemMain* p_vmem = GetVMem();

		for (CustomHeap* p_heap = p_vmem->mp_CustomHeaps; p_heap; p_heap = p_heap->mp_Next)
			p_heap->m_Heap.ReleaseLockShared();
		p_vmem->m_MainHeap.ReleaseLockShared();
	}

	//------------------------------------------------------------------------
	void CheckStats_NoLock(VMemMain* p_vmem)
	{
#ifdef VMEM_ENABLE_STATS
		LockAllHeaps();

		VMemHeapStats stats = p_vmem->m_MainHeap.GetStatsNoLock();

		for (CustomHeap* p_heap = p_vmem->mp_CustomHeaps; p_heap; p_heap = p_heap->mp_Next)
			stats += p_heap->m_Heap.GetStatsNoLock();

		// check
		VMEM_ASSERT(stats.m_Total.m_Reserved == VMem::GetReservedBytes(), "total reserved bytes doesn't match");
		VMEM_ASSERT(stats.m_Total.GetCommittedBytes() == VMem::GetCommittedBytes(), "total committed bytes doesn't match");

		UnlockAllHeaps();
#else
		VMEM_UNREFERENCED_PARAM(p_vmem);
#endif
	}

	//------------------------------------------------------------------------
	void CheckStats()
	{
		VMemMain* p_vmem = GetVMem();

		CriticalSectionScope lock(p_vmem->m_CriticalSection);

		CheckStats_NoLock(p_vmem);
	}

	//------------------------------------------------------------------------
	void CheckIntegrity()
	{
		VMemMain* p_vmem = gp_VMem;

		if(p_vmem)
		{
			CriticalSectionScope lock(p_vmem->m_CriticalSection);

			p_vmem->m_MainHeap.CheckIntegrity();

			for (CustomHeap* p_heap = p_vmem->mp_CustomHeaps; p_heap; p_heap = p_heap->mp_Next)
				p_heap->m_Heap.CheckIntegrity();

			#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
				for (PhysicalCustomHeap* p_heap = p_vmem->mp_PhysicalCustomHeaps; p_heap; p_heap = p_heap->mp_Next)
					p_heap->m_Heap.CheckIntegrity();
			#endif

			CheckStats_NoLock(p_vmem);
		}
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	VMemHeapStats GetStats()
	{
		VMemMain* p_vmem = GetVMem();

		CriticalSectionScope lock(p_vmem->m_CriticalSection);

		return GetStats_NoLock(p_vmem);
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	VMemHeapStats GetStats_NoLock(VMemMain* p_vmem)
	{
		LockAllHeaps();

		VMemHeapStats stats = p_vmem->m_MainHeap.GetStatsNoLock();

		for (CustomHeap* p_heap = p_vmem->mp_CustomHeaps; p_heap; p_heap = p_heap->mp_Next)
			stats += p_heap->m_Heap.GetStatsNoLock();

		UnlockAllHeaps();

		return stats;
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	VMemHeapStats GetMainHeapStats()
	{
		VMemMain* p_vmem = GetVMem();

		CriticalSectionScope lock(p_vmem->m_CriticalSection);

		LockAllHeaps();

		VMemHeapStats stats = p_vmem->m_MainHeap.GetStatsNoLock();

		UnlockAllHeaps();

		return stats;
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	void WriteStats()
	{
		VMemMain* p_vmem = GetVMem();

		CriticalSectionScope lock(p_vmem->m_CriticalSection);

		LockAllHeaps();

		VMemHeapStats stats = p_vmem->m_MainHeap.GetStatsNoLock();

		if(p_vmem->mp_CustomHeaps)
		{
			DebugWrite(_T("MainHeap ---------------------------------------------------------------------------------------------------------\n"));
			WriteStats(stats);

			VMemHeapStats total_stats = stats;

			int index = 0;
			for (CustomHeap* p_heap = p_vmem->mp_CustomHeaps; p_heap; p_heap = p_heap->mp_Next, ++index)
			{
				VMemHeapStats heap_stats = p_heap->m_Heap.GetStatsNoLock();
				DebugWrite(_T("\nHeap %d 0x%p --------------------------------------------------------------------------------------\n"), index, p_heap);
				WriteStats(heap_stats);
				total_stats += heap_stats;
			}

			// check
			VMEM_ASSERT(total_stats.m_Total.m_Reserved == VMem::GetReservedBytes(), "total reserved bytes doesn't match");
			VMEM_ASSERT(total_stats.m_Total.GetCommittedBytes() == VMem::GetCommittedBytes(), "total committed bytes doesn't match");

			DebugWrite(_T("\nTotal ------------------------------------------------------------------------------------------------------------\n"));
			WriteStats(total_stats);
		}
		else
		{
			VMEM_ASSERT(stats.m_Total.m_Reserved == VMem::GetReservedBytes(), "total reserved bytes doesn't match");
			VMEM_ASSERT(stats.m_Total.GetCommittedBytes() == VMem::GetCommittedBytes(), "total committed bytes doesn't match");

			WriteStats(stats);
		}

		VMemSysWriteStats();

		UnlockAllHeaps();
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	size_t WriteAllocs()
	{
		VMemMain* p_vmem = GetVMem();

		CriticalSectionScope lock(p_vmem->m_CriticalSection);

		size_t size = p_vmem->m_MainHeap.WriteAllocs();

		for (CustomHeap* p_heap = p_vmem->mp_CustomHeaps; p_heap; p_heap = p_heap->mp_Next)
			size += p_heap->m_Heap.WriteAllocs();

		return size;
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	void SendStatsToMemPro(void (*send_fn)(void*, int, void*), void* p_context)
	{
		VMemMain* p_vmem = GetVMem();

		CriticalSectionScope lock(p_vmem->m_CriticalSection);

		return p_vmem->m_MainHeap.SendStatsToMemPro(send_fn, p_context);
	}
#endif

	//------------------------------------------------------------------------
	void SetDebugBreak(void (*DebugBreakFn)(const _TCHAR* p_message))
	{
		SetDebugBreakFunction(DebugBreakFn);
	}

	//------------------------------------------------------------------------
	void SetLogFunction(LogFn log_fn)
	{
		VMemSysSetLogFunction(log_fn);
	}

	//------------------------------------------------------------------------
#ifdef VMEM_PROTECTED_HEAP
	void SetVMemShouldProtectFn(bool (*should_protect_fn)(int i, size_t size))
	{
		g_VMemSHouldProtectFn = should_protect_fn;
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_COALESCE_HEAP_PER_THREAD
	void CreateCoalesceHeapForThisThread()
	{
		return GetVMem()->m_MainHeap.CreateCoalesceHeapForThisThread();
	}
#endif

	//------------------------------------------------------------------------
	VMEM_FORCE_INLINE VMemHeap* GetHeap(HeapHandle heap)
	{
		CustomHeap* p_heap = (CustomHeap*)heap;
		VMEM_ASSERT(p_heap->m_Marker == VMEM_CUSTOM_HEAP_MARKER, "Not a VMem heap!");
		return &p_heap->m_Heap;
	}

	//------------------------------------------------------------------------
	HeapHandle CreateHeap(int page_size, int reserve_flags, int commit_flags)
	{
		VMemMain* p_vmem = gp_VMem;
		if (!p_vmem)
		{
			if (!InitialiseVMem())
				return NULL;
			p_vmem = gp_VMem;
		}

		CriticalSectionScope lock(p_vmem->m_CriticalSection);

		CustomHeap* p_heap = (CustomHeap*)Alloc(sizeof(CustomHeap));

		VMEM_MEMSET((void*)p_heap, VMEM_ALLOCATED_MEM, sizeof(CustomHeap));

		new (p_heap)CustomHeap(page_size, reserve_flags, commit_flags);

		if(!p_heap->m_Heap.Initialise())
		{
			p_heap->~CustomHeap();
			Free(p_heap);
			return NULL;
		}

		p_heap->mp_Next = p_vmem->mp_CustomHeaps;
		p_vmem->mp_CustomHeaps = p_heap;

		return p_heap;
	}

	//------------------------------------------------------------------------
	void DestroyHeap(HeapHandle heap)
	{
		VMemMain* p_vmem = GetVMem();

		CriticalSectionScope lock(p_vmem->m_CriticalSection);

		CustomHeap* p_heap = (CustomHeap*)heap;
		VMEM_ASSERT(p_heap->m_Marker == VMEM_CUSTOM_HEAP_MARKER, "Not a VMem heap!");

		if(p_heap == p_vmem->mp_CustomHeaps)
		{
			p_vmem->mp_CustomHeaps = p_heap->mp_Next;
		}
		else
		{
			bool found = false;
			for(CustomHeap* p_node = p_vmem->mp_CustomHeaps; p_node; p_node = p_node->mp_Next)
			{
				if(p_node->mp_Next == p_heap)
				{
					found = true;
					p_node->mp_Next = p_heap->mp_Next;
					break;
				}
			}
			VMEM_ASSERT(found, "bad heap handle passed in to DestroyHeap");
			VMEM_UNREFERENCED_PARAM(found);
		}

		p_heap->~CustomHeap();

		Free(p_heap);
	}

	//------------------------------------------------------------------------
	VMEM_RESTRICT_RETURN void* HeapAlloc(HeapHandle heap, size_t size, size_t alignment)
	{
		VMemHeap* p_heap = GetHeap(heap);

		size_t size_with_info;
#ifdef VMEM_CUSTOM_ALLOC_INFO
		VMEM_STATIC_ASSERT((sizeof(VMemCustomAllocInfo) % VMEM_NATURAL_ALIGNMENT) == 0, "invalid size for VMemCustomAllocInfo");
		size_with_info = size + sizeof(VMemCustomAllocInfo);
#else
		size_with_info = size;
#endif
		void* p = p_heap->Alloc(size_with_info, alignment);

		if (!p)
		{
			Flush();
			p = p_heap->Alloc(size, alignment);
			if (!p)
			{
				OutOfMemoryHandler();
				return NULL;
			}
		}

#ifdef VMEM_CUSTOM_ALLOC_INFO
		memset(p, 0, sizeof(VMemCustomAllocInfo));
		p = (VMem::byte*)p + sizeof(VMemCustomAllocInfo);
#endif
		return p;
	}

	//------------------------------------------------------------------------
	VMEM_RESTRICT_RETURN void* HeapRealloc(HeapHandle heap, void* p, size_t new_size, size_t alignment)
	{
		VMemHeap* p_heap = GetHeap(heap);

#ifdef VMEM_CUSTOM_ALLOC_INFO
		size_t new_alloc_size = new_size + sizeof(VMemCustomAllocInfo);
#else
		size_t new_alloc_size = new_size;
#endif
		void* new_p = NULL;
		if (p)
		{
#ifdef VMEM_CUSTOM_ALLOC_INFO
			void* p_alloc = (VMem::byte*)p - sizeof(VMemCustomAllocInfo);
#else
			void* p_alloc = p;
#endif
			size_t old_size = p_heap->GetSize(p_alloc);

			bool new_alloc = true;
			if (new_alloc_size <= old_size)
			{
				if (new_alloc_size <= 512)
					new_alloc = new_alloc_size > old_size || new_alloc_size < old_size / 2;
				else
					new_alloc = new_alloc_size > old_size || new_alloc_size < 6 * old_size / 8;
			}

			if (new_alloc)
			{
				new_p = p_heap->Alloc(new_alloc_size, alignment);
				if (!new_p)
				{
					Flush();
					new_p = p_heap->Alloc(new_alloc_size, alignment);
					if (!new_p)
					{
						OutOfMemoryHandler();
						return NULL;
					}
				}

				VMEM_ASSERT(old_size != VMEM_INVALID_SIZE, "trying to realloc invalid pointer");
				size_t copy_size = VMin(old_size, new_alloc_size);
				memcpy(new_p, p_alloc, copy_size);
				p_heap->Free(p_alloc);
			}
			else
			{
				new_p = p_alloc;
			}
		}
		else
		{
			new_p = p_heap->Alloc(new_alloc_size, alignment);
			if (!new_p)
			{
				Flush();
				new_p = p_heap->Alloc(new_alloc_size, alignment);
				if (!new_p)
				{
					OutOfMemoryHandler();
					return NULL;
				}
			}

#ifdef VMEM_CUSTOM_ALLOC_INFO
			memset(new_p, 0, sizeof(VMemCustomAllocInfo));
#endif
		}

#ifdef VMEM_CUSTOM_ALLOC_INFO
		new_p = (VMem::byte*)new_p + sizeof(VMemCustomAllocInfo);
#endif
		VMEM_RECORD(p, new_size, alignment, new_p);

		return new_p;
	}

	//------------------------------------------------------------------------
	void HeapFree(HeapHandle heap, void* p)
	{
#ifdef VMEM_CUSTOM_ALLOC_INFO
		void* p_alloc = (VMem::byte*)p - sizeof(VMemCustomAllocInfo);
#else
		void* p_alloc = p;
#endif
		GetHeap(heap)->Free(p_alloc);
	}

	//------------------------------------------------------------------------
	size_t HeapGetSize(HeapHandle heap, void* p)
	{
#ifdef VMEM_CUSTOM_ALLOC_INFO
		void* p_alloc = (VMem::byte*)p - sizeof(VMemCustomAllocInfo);
#else
		void* p_alloc = p;
#endif
		return GetHeap(heap)->GetSize(p_alloc);
	}

	//------------------------------------------------------------------------
	bool HeapOwns(HeapHandle heap, void* p)
	{
#ifdef VMEM_CUSTOM_ALLOC_INFO
		void* p_alloc = (VMem::byte*)p - sizeof(VMemCustomAllocInfo);
#else
		void* p_alloc = p;
#endif
		return GetHeap(heap)->Owns(p_alloc);
	}

	//------------------------------------------------------------------------
	void* HeapAllocAligned(HeapHandle heap, size_t size, size_t alignment)
	{
		if (alignment == 0)
			alignment = VMem::VMax<size_t>(VMem::VMin<size_t>(size, 16), 4);

		size_t aligned_size = size + sizeof(AlignedHeader) + alignment;

		void* p = HeapAlloc(heap, aligned_size);
		if (!p)
		{
			Flush();
			p = HeapAlloc(heap, aligned_size);
			if (!p)
			{
				OutOfMemoryHandler();
				return NULL;
			}
		}

		void* aligned_p = AlignUp((byte*)p + sizeof(AlignedHeader), alignment);

		AlignedHeader* p_header = (AlignedHeader*)aligned_p - 1;
		p_header->p = p;

		return aligned_p;
	}

	//------------------------------------------------------------------------
	void* HeapReallocAligned(HeapHandle heap, void* p, size_t new_size, size_t alignment)
	{
		void* new_p = HeapAllocAligned(heap, new_size, alignment);
		if (p)
		{
			size_t old_size = HeapGetSizeAligned(heap, p);
			VMEM_ASSERT(old_size != VMEM_INVALID_SIZE, "trying to realloc invalid pointer");
			size_t copy_size = VMem::VMin(old_size, new_size);
			if (new_p)
				memcpy_s(new_p, new_size, p, copy_size);
			HeapFreeAligned(heap, p);
		}
		return new_p;
	}

	//------------------------------------------------------------------------
	void HeapFreeAligned(HeapHandle heap, void* p)
	{
		if (p)
		{
			AlignedHeader* p_header = (AlignedHeader*)p - 1;
			HeapFree(heap, p_header->p);
		}
	}

	//------------------------------------------------------------------------
	size_t HeapGetSizeAligned(HeapHandle heap, void* p)
	{
		if (!p)
			return VMEM_INVALID_SIZE;

		AlignedHeader* p_header = (AlignedHeader*)p - 1;
		void* p_alloc = p_header->p;
		size_t aligned_size = HeapGetSize(heap, p_alloc);
		return aligned_size - ((byte*)p - (byte*)p_alloc);
	}

	//------------------------------------------------------------------------
	size_t HeapOwnsAligned(HeapHandle heap, void* p)
	{
		if (!p)
			return false;

		AlignedHeader* p_header = (AlignedHeader*)p - 1;
		void* p_alloc = p_header->p;

		return HeapOwns(heap, p_alloc);
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	VMemHeapStats HeapGetStats(HeapHandle heap)
	{
		CriticalSectionScope lock(GetVMem()->m_CriticalSection);

		VMemHeap* p_heap = GetHeap(heap);
		p_heap->AcquireLockShared();
		VMemHeapStats stats = p_heap->GetStatsNoLock();
		p_heap->ReleaseLockShared();
		return stats;
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	void HeapWriteStats(HeapHandle heap)
	{
		VMemHeapStats stats = HeapGetStats(heap);
		WriteStats(stats);
	}
#endif

	//------------------------------------------------------------------------
	void HeapCheckIntegrity(HeapHandle heap)
	{
		GetHeap(heap)->CheckIntegrity();
	}

	//------------------------------------------------------------------------
	VMEM_FORCE_INLINE HeapHandle GetThreadHeap()
	{
		HeapHandle heap = g_ThreadHeapHandle;
		VMEM_ASSERT(heap, "Heap not created for this thread");
		return heap;
	}

	//------------------------------------------------------------------------
	void CreateThreadHeap()
	{
		VMEM_ASSERT(!g_ThreadHeapHandle, "Heap already created for this thread");
		g_ThreadHeapHandle = CreateHeap();
	}

	//------------------------------------------------------------------------
	void DestroyThreadHeap()
	{
		DestroyHeap(GetThreadHeap());
		g_ThreadHeapHandle = NULL;
	}

	//------------------------------------------------------------------------
	void ThreadHeapCheckIntegrity()
	{
		HeapCheckIntegrity(GetThreadHeap());
	}

	//------------------------------------------------------------------------
	VMEM_RESTRICT_RETURN void* ThreadHeapAlloc(size_t size, size_t alignment)
	{
		return HeapAlloc(GetThreadHeap(), size, alignment);
	}

	//------------------------------------------------------------------------
	VMEM_RESTRICT_RETURN void* ThreadHeapRealloc(void* p, size_t new_size, size_t alignment)
	{
		return HeapRealloc(GetThreadHeap(), p, new_size, alignment);
	}

	//------------------------------------------------------------------------
	void ThreadHeapFree(void* p)
	{
		HeapFree(GetThreadHeap(), p);
	}

	//------------------------------------------------------------------------
	size_t ThreadHeapGetSize(void* p)
	{
		return HeapGetSize(GetThreadHeap(), p);
	}

	//------------------------------------------------------------------------
	void* ThreadHeapAllocAligned(size_t size, size_t alignment)
	{
		return HeapAllocAligned(GetThreadHeap(), size, alignment);
	}

	//------------------------------------------------------------------------
	void* ThreadHeapReallocAligned(void* p, size_t new_size, size_t alignment)
	{
		return HeapReallocAligned(GetThreadHeap(), p, new_size, alignment);
	}

	//------------------------------------------------------------------------
	void ThreadHeapFreeAligned(void* p)
	{
		HeapFreeAligned(GetThreadHeap(), p);
	}

	//------------------------------------------------------------------------
	size_t ThreadHeapGetSizeAligned(void* p)
	{
		return HeapGetSizeAligned(GetThreadHeap(), p);
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	VMemHeapStats ThreadHeapGetStats()
	{
		return HeapGetStats(GetThreadHeap());
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	void ThreadHeapWriteStats()
	{
		HeapWriteStats(GetThreadHeap());
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
	VMEM_FORCE_INLINE PhysicalHeap* GetPhysicalHeap(HeapHandle heap)
	{
		PhysicalCustomHeap* p_heap = (PhysicalCustomHeap*)heap;
		VMEM_ASSERT(p_heap->m_Marker == VMEM_PHYSICAL_CUSTOM_HEAP_MARKER, "Not a Physical heap!");
		return &p_heap->m_Heap;
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
	HeapHandle CreatePhysicalHeap(size_t coalesce_heap_region_size, size_t coalesce_heap_max_size, int flags)
	{
		VMemMain* p_vmem = gp_VMem;
		if (!p_vmem)
		{
			if (!InitialiseVMem())
				return NULL;
			p_vmem = gp_VMem;
		}

		CriticalSectionScope lock(p_vmem->m_CriticalSection);

		PhysicalCustomHeap* p_heap = (PhysicalCustomHeap*)Alloc(sizeof(PhysicalCustomHeap));

		VMEM_MEMSET(p_heap, VMEM_ALLOCATED_MEM, sizeof(PhysicalCustomHeap));

		new (p_heap)PhysicalCustomHeap(coalesce_heap_region_size, coalesce_heap_max_size, flags);

		p_heap->mp_Next = p_vmem->mp_PhysicalCustomHeaps;
		p_vmem->mp_PhysicalCustomHeaps = p_heap;

		return p_heap;
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
	void DestroyPhysicalHeap(HeapHandle heap)
	{
		VMemMain* p_vmem = GetVMem();

		CriticalSectionScope lock(p_vmem->m_CriticalSection);

		PhysicalCustomHeap* p_heap = (PhysicalCustomHeap*)heap;
		VMEM_ASSERT(p_heap->m_Marker == VMEM_PHYSICAL_CUSTOM_HEAP_MARKER, "Not a physical heap!");

		if (p_heap == p_vmem->mp_PhysicalCustomHeaps)
		{
			p_vmem->mp_PhysicalCustomHeaps = p_heap->mp_Next;
		}
		else
		{
			bool found = false;
			for (PhysicalCustomHeap* p_node = p_vmem->mp_PhysicalCustomHeaps; p_node; p_node = p_node->mp_Next)
			{
				if (p_node->mp_Next == p_heap)
				{
					found = true;
					p_node->mp_Next = p_heap->mp_Next;
					break;
				}
			}
			VMEM_ASSERT(found, "bad heap handle passed in to DestroyPhysicalHeap");
			VMEM_UNREFERENCED_PARAM(found);
		}

		p_heap->~PhysicalCustomHeap();

		Free(p_heap);
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
	void* PhysicalHeapAlloc(HeapHandle heap, size_t size)
	{
		return GetPhysicalHeap(heap)->Alloc(size);
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
	bool PhysicalHeapFree(HeapHandle heap, void* p)
	{
		return GetPhysicalHeap(heap)->Free(p);
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
	size_t PhysicalHeapGetSize(HeapHandle heap, void* p)
	{
		return GetPhysicalHeap(heap)->GetSize(p);
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
	bool PhysicalHeapOwns(HeapHandle heap, void* p)
	{
		return GetPhysicalHeap(heap)->Owns(p);
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
	void* PhysicalHeapAllocAligned(HeapHandle heap, size_t size, size_t alignment)
	{
		return GetPhysicalHeap(heap)->AllocAligned(size, alignment);
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
	bool PhysicalHeapFreeAligned(HeapHandle heap, void* p)
	{
		return GetPhysicalHeap(heap)->FreeAligned(p);
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
	size_t PhysicalHeapGetSizeAligned(HeapHandle heap, void* p)
	{
		return GetPhysicalHeap(heap)->GetSizeAligned(p);
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
	void PhysicalHeapCheckIntegrity(HeapHandle heap)
	{
		return GetPhysicalHeap(heap)->CheckIntegrity();
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
	void PhysicalHeapTrim(HeapHandle heap)
	{
		return GetPhysicalHeap(heap)->Trim();
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
	size_t PhysicalHeapGetMaxAllocSize(HeapHandle heap)
	{
		return GetPhysicalHeap(heap)->GetMaxAllocSize();
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
	void PhysicalHeapWriteStats(HeapHandle heap)
	{
		return GetPhysicalHeap(heap)->WriteStats();
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_CUSTOM_ALLOC_INFO
	VMemCustomAllocInfo* GetCustomAllocInfo(void* p)
	{
		return (VMemCustomAllocInfo*)((VMem::byte*)p - sizeof(VMemCustomAllocInfo));
	}
#endif
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

