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
#include "VMemSys.hpp"
#include "VMemCore.hpp"
#include "VMemCriticalSection.hpp"
#include "RelaxedAtomic.hpp"
#include "PhysicalPageMapper.hpp"
#include <stdio.h>
#include <stdarg.h>

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

#if defined(VMEM_PLATFORM_WIN) || defined(VMEM_PLATFORM_LINUX)
	#include <malloc.h>
#endif

#if defined(VMEM_PLATFORM_XBOX360) || defined(VMEM_PLATFORM_XBOXONE) || defined(VMEM_PLATFORM_SWITCH) || defined(VMEM_PLATFORM_ANDROID)
	#include <malloc.h>
#endif

#if defined(VMEM_PLATFORM_LINUX) || defined(VMEM_PLATFORM_APPLE) || defined(VMEM_PLATFORM_ANDROID) || defined(VMEM_PLATFORM_SWITCH)
	#include <sys/mman.h>
#endif

#ifdef VMEM_PLATFORM_XBOXONE
	#include <xmem.h>
#endif

#ifdef VMEM_PLATFORM_SWITCH
	#include <nn/os.h>
#endif

//-----------------------------------------------------------------------------
#ifdef VMEM_OS_WIN
	#pragma warning(disable:4100)
#endif

//-----------------------------------------------------------------------------
// Using malloc as the base allocator is not recommended. It is only provided here
// as a quick way to get VMem working on any platform. Malloc does not support
// committing and decommitting of memory which VMem needs to perform optimally.
// If you do use malloc please ensure you set the region sizes carefully in VMemHeap
// in order to reduce the memory wastage.
//#define VMEM_USE_MALLOC

//-----------------------------------------------------------------------------
#ifdef VMEM_SIMULATE_PS4
	#ifdef off_t
		#undef off_t
	#endif
	namespace PS4Simulator { typedef long long off_t; }
	#define off_t PS4Simulator::off_t
	int sceKernelReserveVirtualRange(void**, size_t, int, int);
	int sceKernelMunmap(void*, size_t);
	int sceKernelMapDirectMemory(void**, size_t, int, int, off_t, size_t);
	int sceKernelAllocateMainDirectMemory(size_t, size_t, int, off_t*);
	int sceKernelCheckedReleaseDirectMemory(off_t, size_t);
	struct SceKernelVirtualQueryInfo { void *start; void *end; off_t offset; };
	int sceKernelVirtualQuery(const void *, int, SceKernelVirtualQueryInfo*, size_t);
	#define SCE_OK						0
	#define SCE_KERNEL_WB_ONION			0
	#define SCE_KERNEL_WC_GARLIC		1
	#define SCE_KERNEL_MAP_FIXED		(1<<1)
	#define SCE_KERNEL_MAP_NO_COALESCE	(1<<2)
	#define SCE_KERNEL_MAP_NO_OVERWRITE	(1<<3)
	#define SCE_KERNEL_PROT_CPU_RW		(1<<4)
	#define SCE_KERNEL_PROT_GPU_RW		(1<<5)
#endif

//-----------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	size_t g_ReservedBytes = 0;
	size_t g_CommittedBytes = 0;

	void (*g_LogFn)(const _TCHAR* p_message) = NULL;

	//------------------------------------------------------------------------
	struct VMemSysData
	{
		CriticalSection m_CriticalSection;

#ifdef VMEM_USE_PAGE_MAPPER
		PhysicalPageMapper m_PhysicalPageMapper;
#endif
	};
	byte gp_VMemSysDataMem[sizeof(VMemSysData)] VMEM_ALIGN_8;
	VMemSysData* gp_VMemSysData = NULL;

	//------------------------------------------------------------------------
	void Log(const _TCHAR* p_message)
	{
		CriticalSectionScope lock(gp_VMemSysData->m_CriticalSection);

		if(g_LogFn)
		{
			g_LogFn(p_message);
		}
		else
		{
			#if defined(VMEM_OS_WIN)
				OutputDebugString(p_message);
			#else
				printf("%s", p_message);
			#endif
		}
	}

	//------------------------------------------------------------------------
	void PrintLastError()
	{
#ifdef VMEM_PLATFORM_WIN
		_TCHAR* p_buffer = NULL;
		va_list args = NULL;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			::GetLastError(),
			0,
			(_TCHAR*)&p_buffer,
			4 * 1024,
			&args);
		Log(p_buffer);
#endif
	}

	//------------------------------------------------------------------------
#if defined(VMEM_PLATFORM_PS4) || defined(VMEM_SIMULATE_PS4)
	void Decommit_PS4(void* p, size_t size, int page_size);

	bool Commit_PS4(void* p, size_t size, int page_size, int commit_flags)
	{
		VMEM_ASSERT((size % page_size) == 0, "commit size must be a multiple of SYS_PAGE_SIZE");

		int ps4_flags = commit_flags ? commit_flags : SCE_KERNEL_WB_ONION;

		void* p_map = p;
		size_t map_size = size;
		size_t physical_alloc_size = size;

		while(map_size)
		{
			size_t alignment = page_size;
			off_t physical = 0;
			int alloc_result = sceKernelAllocateMainDirectMemory(physical_alloc_size, alignment, ps4_flags, &physical);

			if(alloc_result == SCE_OK)
			{
				int protection = SCE_KERNEL_PROT_CPU_RW;
				if (ps4_flags != SCE_KERNEL_WB_ONION)
					protection |= SCE_KERNEL_PROT_GPU_RW;

				int flags = SCE_KERNEL_MAP_FIXED;

				int map_result = sceKernelMapDirectMemory(&p_map, physical_alloc_size, protection, flags, physical, 0);
				VMEM_ASSERT(map_result == SCE_OK, "VirtualCommit failed");
				VMEM_UNREFERENCED_PARAM(map_result);

				map_size -= physical_alloc_size;
				p_map = (byte*)p_map + physical_alloc_size;

				physical_alloc_size = VMin(physical_alloc_size, map_size);
			}
			else
			{
				if(physical_alloc_size == page_size)
				{
					// failed due to out of memory, so clean up whatever we did managed to allocate and break out
					Decommit_PS4(p, size - map_size, page_size);
					return false;
				}

				// try a smaller size
				physical_alloc_size = AlignSizeUpPow2(physical_alloc_size / 2, page_size);
			}
		}

		VMEM_ASSERT(map_size == 0, "error in Decommit_PS4");
		return true;
	}
#endif

	//------------------------------------------------------------------------
#if defined(VMEM_PLATFORM_PS4) || defined(VMEM_SIMULATE_PS4)
	void Decommit_PS4(void* p, size_t size, int page_size)
	{
        VMEM_ASSERT((size % page_size) == 0, "decommit size must be a multiple of SYS_PAGE_SIZE");

		size_t decommit_size = size;
		void* p_decommit = p;

		while (decommit_size > 0)
		{
			// find the physical address that this virtual address is mapped to
			SceKernelVirtualQueryInfo mem_info;
			memset(&mem_info, 0, sizeof(SceKernelVirtualQueryInfo));
			int query_result = sceKernelVirtualQuery(p_decommit, 0, &mem_info, sizeof(SceKernelVirtualQueryInfo));
			VMEM_ASSERT(query_result == SCE_OK, "Query Virtual memory failed");
			VMEM_UNREFERENCED_PARAM(query_result);

			size_t range_size = VMin((size_t)((byte*)mem_info.end - (byte*)p_decommit), decommit_size);
			size_t physical_offset = (byte*)p_decommit - (byte*)mem_info.start;

			// put the virtual address range back into the reserved state (decommit it)
			unsigned int flags = SCE_KERNEL_MAP_FIXED;
			int reserve_result = sceKernelReserveVirtualRange(&p_decommit, range_size, flags, page_size);
			VMEM_ASSERT(reserve_result == SCE_OK, "VirtualDecommit failed");
			VMEM_UNREFERENCED_PARAM(reserve_result);

			// release the physical memory that was mapped to the address. This must happen after the
			// reserve call because it will unmap the range and another thread/process could grab it.
			// Calling reserve first means that it doesn't go 
			int release_result = sceKernelCheckedReleaseDirectMemory(mem_info.offset + physical_offset, range_size);
			VMEM_ASSERT(release_result == SCE_OK, "VirtualRelease failed");
			VMEM_UNREFERENCED_PARAM(release_result);

			p_decommit = (byte*)p_decommit + range_size;
			decommit_size -= range_size;
		}
	}
#endif

	//------------------------------------------------------------------------
	void VMemSysCreate()
	{
		new(gp_VMemSysDataMem)VMemSysData();
		gp_VMemSysData = (VMemSysData*)gp_VMemSysDataMem;
	}

	//------------------------------------------------------------------------
	void VMemSysDestroy()
	{
		gp_VMemSysData->~VMemSysData();
		gp_VMemSysData = NULL;

		g_ReservedBytes = 0;
		g_CommittedBytes = 0;
	}

	//------------------------------------------------------------------------
	void* VirtualReserve(size_t size, int page_size, int reserve_flags)
	{
		VMEM_ASSERT(page_size >= VMEM_SYS_PAGE_SIZE && (page_size & (VMEM_SYS_PAGE_SIZE - 1)) == 0, "invalid page size");
		VMEM_ASSERT((size & (page_size - 1)) == 0, "reserve size not aligned to page size");

		CriticalSectionScope lock(gp_VMemSysData->m_CriticalSection);

#if defined(VMEM_USE_MALLOC)

		// ====================================================
		//					MALLOC reserve
		// ====================================================
		VMEM_UNREFERENCED_PARAM(reserve_flags);
		void* p = _aligned_malloc(size, page_size);

#elif defined(VMEM_OS_WIN) && !defined(VMEM_SIMULATE_PS4)

		// ====================================================
		//					WIN reserve
		// ====================================================
		unsigned int flags = MEM_RESERVE | reserve_flags;

		// only zero memory if memset defined
		#if !defined(VMEM_ENABLE_MEMSET) && defined(VMEM_PLATFORM_XBOX360)
			flags |= MEM_NOZERO;
		#endif

		// large pages more effecient on this platform
		#ifdef VMEM_PLATFORM_XBOX360
			flags |= MEM_LARGE_PAGES;
		#endif

		#ifdef VMEM_PLATFORM_XBOXONE
			flags |= MEM_LARGE_PAGES | MEM_TITLE;
		#endif

		void* p = ::VirtualAlloc(NULL, size, flags, PAGE_NOACCESS);

#elif defined(VMEM_PLATFORM_PS4) || defined(VMEM_SIMULATE_PS4)

		// ====================================================
		//					PS4 reserve
		// ====================================================
		void* p = NULL;
		if (sceKernelReserveVirtualRange(&p, size,
			SCE_KERNEL_MAP_NO_OVERWRITE | reserve_flags,
			page_size) != SCE_OK)
		{
			return NULL;
		}

#elif defined(VMEM_PLATFORM_SWITCH)

		// ====================================================
		//					Nintendo Switch reserve
		// ====================================================
		void* p = NULL;
		VMEM_UNREFERENCED_PARAM(reserve_flags);
		nn::Result result = nn::os::AllocateAddressRegion((uintptr_t*)&p, size);
		if (!result.IsSuccess())
			return NULL;

#else
		// ====================================================
		//					LINUX reserve
		// ====================================================
		void* p = mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANON | reserve_flags, -1, 0);
		if(p == MAP_FAILED)
			p = NULL;

		msync(p, size, (MS_SYNC | MS_INVALIDATE));

#endif

		if(p)
			g_ReservedBytes += size;

		#if !(defined(VMEM_SIMULATE_PS4) && defined(VMEM_OS_LINUX))
			VMEM_ASSERT(((size_t)p & (page_size-1)) == 0, "Memory not aligned!");
		#endif

		return p;
	}

	//------------------------------------------------------------------------
	void VirtualRelease(void* p, size_t size)
	{
		CriticalSectionScope lock(gp_VMemSysData->m_CriticalSection);

#if defined(VMEM_USE_MALLOC)

		// ====================================================
		//					MALLOC release
		// ====================================================
		_aligned_free(p);

#elif defined(VMEM_OS_WIN) && !defined(VMEM_SIMULATE_PS4)

		// ====================================================
		//					WIN release
		// ====================================================
		BOOL b = ::VirtualFree(p, 0, MEM_RELEASE);
		if(!b)
			PrintLastError();
		VMEM_ASSERT(b, "VirtualFree failed");

#elif defined(VMEM_PLATFORM_PS4) || defined(VMEM_SIMULATE_PS4)

		// ====================================================
		//					PS4 release
		// ====================================================
		int ret = sceKernelMunmap(p, size);
		VMEM_ASSERT(ret == SCE_OK, "sceKernelMunmap failed!?");
		VMEM_UNREFERENCED_PARAM(ret);

#elif defined(VMEM_PLATFORM_SWITCH)

		// ====================================================
		//			   Nintendo Switch release
		// ====================================================
		nn::Result result = nn::os::FreeAddressRegion(address);
		VMEM_ASSERT(result.IsSuccess());
		VMEM_UNREFERENCED_PARAM(result);

#else

		// ====================================================
		//					LINUX release
		// ====================================================
		msync(p, size, MS_SYNC);
		#ifdef VMEM_ASSERTS
			int ret = munmap(p, size);
			VMEM_ASSERT(ret == 0, "munmap failed");
		#else
			munmap(p, size);
		#endif

#endif

		g_ReservedBytes -= size;
	}

	//------------------------------------------------------------------------
	bool VirtualCommit(void* p, size_t size, int page_size, int commit_flags)
	{
		VMEM_ASSERT(page_size >= VMEM_SYS_PAGE_SIZE && (page_size & (VMEM_SYS_PAGE_SIZE - 1)) == 0, "invalid page size");

		CriticalSectionScope lock(gp_VMemSysData->m_CriticalSection);

#if VMEM_SIMULATE_OOM
		int simluate_oom_count = GetSimulateOOMCount();
		if(simluate_oom_count > 0)
		{
			if(simluate_oom_count == 1)
  				return false;
			else
				SetSimulateOOMCount(simluate_oom_count - 1);
		}
#endif
		bool success = false;

#if defined(VMEM_USE_MALLOC)

		// ====================================================
		//					MALLOC commit
		// ====================================================
		VMEM_UNREFERENCED_PARAM(p);
		VMEM_UNREFERENCED_PARAM(size);
		VMEM_UNREFERENCED_PARAM(page_size);
		VMEM_UNREFERENCED_PARAM(commit_flags);
		success = true;

#elif defined(VMEM_USE_PAGE_MAPPER)

		// ====================================================
		//			   PageMapper commit (PS4)
		// ====================================================

		success = gp_VMemSysData->m_PhysicalPageMapper.MapPhysicalPages(p, size, commit_flags);

#elif defined(VMEM_PLATFORM_PS4) || defined(VMEM_SIMULATE_PS4)

        // ====================================================
        //					PS4 commit
        // ====================================================

		success = Commit_PS4(p, size, page_size, commit_flags);

#elif defined(VMEM_PLATFORM_SWITCH)

		// ====================================================
		//			  Nintendo Switch commit
		// ====================================================

		VMEM_UNREFERENCED_PARAM(commit_flags);
		nn::Result result = nn::os::AllocateMemoryPages((uintptr_t&)p, size);
		success = result.IsSuccess();

#elif defined(VMEM_OS_WIN)

		// ====================================================
		//					WIN commit
		// ====================================================

		unsigned int va_flags = MEM_COMMIT | commit_flags;

		// only zero memory if memset defined
		#if !defined(VMEM_ENABLE_MEMSET) && defined(VMEM_PLATFORM_XBOX360)
			va_flags |= MEM_NOZERO;
		#endif

		// large pages more effecient on this platform
		#if defined(VMEM_PLATFORM_XBOX360) || defined(VMEM_PLATFORM_XBOXONE)
			va_flags |= MEM_LARGE_PAGES;
		#endif

		success = ::VirtualAlloc(p, size, va_flags, PAGE_READWRITE) != NULL;

#else

		// ====================================================
		//					LINUX commit
		// ====================================================
		
		mmap(p, size, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_PRIVATE | MAP_ANON | commit_flags, -1, 0);
		msync(p, size, MS_SYNC | MS_INVALIDATE);
		success = true;

#endif

		if(success)
		{
			g_CommittedBytes += size;
			VMEM_MEMSET(p, VMEM_COMMITTED_MEM, size);
		}

		return success;
	}

	//------------------------------------------------------------------------
	void VirtualDecommit(void* p, size_t size, int page_size, int commit_flags)
	{
		VMEM_ASSERT(page_size >= VMEM_SYS_PAGE_SIZE && (page_size & (VMEM_SYS_PAGE_SIZE - 1)) == 0, "invalid page size");

		CriticalSectionScope lock(gp_VMemSysData->m_CriticalSection);

#if defined(VMEM_USE_MALLOC)

		// ====================================================
		//					MALLOC decommit
		// ====================================================
		// do nothing
		VMEM_UNREFERENCED_PARAM(p);
		VMEM_UNREFERENCED_PARAM(size);
		VMEM_UNREFERENCED_PARAM(page_size);
		VMEM_UNREFERENCED_PARAM(commit_flags);

#elif defined(VMEM_USE_PAGE_MAPPER)

		// ====================================================
		//			   PageMapper decommit (PS4)
		// ====================================================

		gp_VMemSysData->m_PhysicalPageMapper.UnMapPhysicalPages(p, size, commit_flags);

#elif defined(VMEM_PLATFORM_PS4) || defined(VMEM_SIMULATE_PS4)

        // ====================================================
        //					PS4 decommit
        // ====================================================

		VMEM_UNREFERENCED_PARAM(commit_flags);
		Decommit_PS4(p, size, page_size);

#elif defined(VMEM_PLATFORM_SWITCH)

		// ====================================================
		//			  Nintendo Switch decommit
		// ====================================================

		VMEM_UNREFERENCED_PARAM(commit_flags);
		nn::Result result = nn::os::FreeMemoryPages((uintptr_t&)p, size);
		VMEM_ASSERT(!result.IsSuccess());
		VMEM_UNREFERENCED_PARAM(result);

#elif defined(VMEM_OS_WIN)

		// ====================================================
		//					WIN decommit
		// ====================================================

		#ifdef VMEM_OS_WIN
			#pragma warning(push)
			#pragma warning(disable : 6250)		// disable decommit but not release warning
		#endif

		VMEM_UNREFERENCED_PARAM(commit_flags);

		BOOL b = ::VirtualFree(p, size, MEM_DECOMMIT);

		#ifdef VMEM_OS_WIN
			#pragma warning(pop)
		#endif

		if(!b)
			PrintLastError();
		VMEM_ASSERT(b, "VirtualFree failed");

#else

		// ====================================================
		//					LINUX decommit
		// ====================================================

		VMEM_UNREFERENCED_PARAM(commit_flags);

		mmap(p, size, PROT_NONE, MAP_FIXED | MAP_PRIVATE | MAP_ANON, -1, 0);
		msync(p, size, MS_SYNC | MS_INVALIDATE);

#endif
		g_CommittedBytes -= size;
	}

	//------------------------------------------------------------------------
	size_t GetReservedBytes()
	{
		return g_ReservedBytes;
	}

	//------------------------------------------------------------------------
	size_t GetCommittedBytes()
	{
		return g_CommittedBytes;
	}

	//------------------------------------------------------------------------
	// this does not need to be implemented, it is only for debug purposes. You can just return true.
#ifdef VMEM_ASSERTS
	bool Committed(void* p, size_t size)
	{
#if defined(VMEM_OS_WIN)
		MEMORY_BASIC_INFORMATION mem_info;

		byte* p_check_p = (byte*)p;
		size_t check_size = size;
		while(check_size)
		{
			memset(&mem_info, 0, sizeof(mem_info));
			VirtualQuery(p_check_p, &mem_info, sizeof(mem_info));
			if(mem_info.State != MEM_COMMIT)
				return false;
			size_t reserve_size = VMin(check_size, (size_t)mem_info.RegionSize);
			check_size -= reserve_size;
			p_check_p += reserve_size;
		}

		return true;
#elif defined(VMEM_PLATFORM_PS4)
		SceKernelVirtualQueryInfo info;
		int ret = sceKernelVirtualQuery(p, 0, &info, sizeof(info));
		VMEM_ASSERT(ret == SCE_OK, "sceKernelVirtualQuery failed");
		if (ret == SCE_OK)
			return info.isDirectMemory == 1;
		return true;
#else
		VMEM_UNREFERENCED_PARAM(p);
		VMEM_UNREFERENCED_PARAM(size);
		return true;
#endif
	}
#endif

	//------------------------------------------------------------------------
	// this does not need to be implemented, it is only for debug purposes. You can just return true.
#ifdef VMEM_ASSERTS
	bool Reserved(void* p, size_t size)
	{
#ifdef VMEM_OS_WIN
		MEMORY_BASIC_INFORMATION mem_info;

		byte* p_check_p = (byte*)p;
		size_t check_size = size;
		while(check_size)
		{
			memset(&mem_info, 0, sizeof(mem_info));
			VirtualQuery(p_check_p, &mem_info, sizeof(mem_info));
			if(mem_info.State != MEM_RESERVE)
				return false;
			size_t reserve_size = VMin(check_size, (size_t)mem_info.RegionSize);
			check_size -= reserve_size;
			p_check_p += reserve_size;
		}

		return true;
#else
		VMEM_UNREFERENCED_PARAM(p);
		VMEM_UNREFERENCED_PARAM(size);
		return true;
#endif
	}
#endif

	//------------------------------------------------------------------------
	void DebugWrite(const _TCHAR* p_str, ...)
	{
#ifdef VMEM_ENABLE_STATS
		va_list args;
		va_start(args, p_str);

		const int max_string_length = 1024;
		static _TCHAR gString[max_string_length];
		_vstprintf_s(gString, max_string_length, p_str, args);
		Log(gString);

		va_end(args);
#endif
	}

	//------------------------------------------------------------------------
	void Break()
	{
#ifdef VMEM_OS_WIN
		DebugBreak();
#else
		assert(false);
#endif
	}

	//------------------------------------------------------------------------
	void DumpSystemMemory()
	{
#if defined(VMEM_PLATFORM_WIN)
		MEMORYSTATUSEX status;
		status.dwLength = sizeof(status);
		GlobalMemoryStatusEx(&status);

		DebugWrite(_T("There is  %7ld percent of memory in use.\n"), status.dwMemoryLoad);
		DebugWrite(_T("There are %7I64d total Kbytes of physical memory.\n"), status.ullTotalPhys/1024);
		DebugWrite(_T("There are %7I64d free Kbytes of physical memory.\n"), status.ullAvailPhys/1024);
		DebugWrite(_T("There are %7I64d total Kbytes of paging file.\n"), status.ullTotalPageFile/1024);
		DebugWrite(_T("There are %7I64d free Kbytes of paging file.\n"), status.ullAvailPageFile/1024);
		DebugWrite(_T("There are %7I64d total Kbytes of virtual memory.\n"), status.ullTotalVirtual/1024);
		DebugWrite(_T("There are %7I64d free Kbytes of virtual memory.\n"), status.ullAvailVirtual/1024);
		DebugWrite(_T("There are %7I64d free Kbytes of extended memory.\n"), status.ullAvailExtendedVirtual/1024);

#elif defined(VMEM_PLATFORM_XBOX360)

		MEMORYSTATUS status;
		status.dwLength = sizeof(status);
		GlobalMemoryStatus(&status);

		DebugWrite(_T("There is  %7ld percent of memory in use.\n"), status.dwMemoryLoad);
		DebugWrite(_T("There are %7I64d total Kbytes of physical memory.\n"), status.dwTotalPhys/1024);
		DebugWrite(_T("There are %7I64d free Kbytes of physical memory.\n"), status.dwAvailPhys/1024);
		DebugWrite(_T("There are %7I64d total Kbytes of paging file.\n"), status.dwTotalPageFile/1024);
		DebugWrite(_T("There are %7I64d free Kbytes of paging file.\n"), status.dwAvailPageFile/1024);
		DebugWrite(_T("There are %7I64d total Kbytes of virtual memory.\n"), status.dwTotalVirtual/1024);
		DebugWrite(_T("There are %7I64d free Kbytes of virtual memory.\n"), status.dwAvailVirtual/1024);

#elif defined(VMEM_PLATFORM_XBOXONE)

		TITLEMEMORYSTATUS status;
		status.dwLength = sizeof(status);
		TitleMemoryStatus(&status);

		DebugWrite(_T("%7I64dKB Total memory available to the current title\n"), status.ullTotalMem/1024);
		DebugWrite(_T("%7I64dKB Free memory currently available to the title\n"), status.ullAvailMem/1024);
		DebugWrite(_T("%7I64dKB Total memory currently allocated in the legacy working set\n"), status.ullLegacyUsed/1024);
		DebugWrite(_T("%7I64dKB Peak amount of memory allocated within the legacy working set\n"), status.ullLegacyPeak/1024);
		DebugWrite(_T("%7I64dKB The amount of memory currently available within the legacy working set\n"), status.ullLegacyAvail/1024);
		DebugWrite(_T("%7I64dKB The amount of free memory currently available within the title working set\n"), status.ullTitleAvail/1024);

#elif defined(VMEM_OS_LINUX)
		// not possible on LINUX

#else

		#error platform not defined

#endif
	}

	//------------------------------------------------------------------------
	void VMemSysSetLogFunction(void (*LogFn)(const _TCHAR* p_message))
	{
		g_LogFn = LogFn;
	}

	//------------------------------------------------------------------------
	void VMemSysCheckIntegrity()
	{
#ifdef VMEM_USE_PAGE_MAPPER
		CriticalSectionScope lock(gp_VMemSysData->m_CriticalSection);

		gp_VMemSysData->m_PhysicalPageMapper.CheckIntegrity();
#endif
	}

	//------------------------------------------------------------------------
	void VMemSysWriteStats()
	{
#if defined(VMEM_USE_PAGE_MAPPER) && defined(VMEM_ENABLE_STATS)
		Stats stats = gp_VMemSysData->m_PhysicalPageMapper.GetStats();
		DebugWrite(_T("\n"));
		DebugWrite(_T("                       Used                 Unused               Overhead                  Total               Reserved\n"));
		DebugWrite(_T("Phys Pages: "));		  VMem::WriteStats(stats);
#endif
	}
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

