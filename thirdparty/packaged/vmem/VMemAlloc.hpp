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
/*
	Version:	3.6
	Date:		04/09/2019
*/

//------------------------------------------------------------------------
#ifndef VMEM_VMEMALLOC_H_INCLUDED
#define VMEM_VMEMALLOC_H_INCLUDED

//------------------------------------------------------------------------
#include "VMemDefs.hpp"
#include "VMemStats.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#ifdef VMEM_OS_WIN
	#include <tchar.h>
#endif

//-----------------------------------------------------------------------------
#if defined(VMEM_OS_WIN)
	#define VMEM_RESTRICT_RETURN __declspec(restrict)
#else
	#define VMEM_RESTRICT_RETURN
#endif

//------------------------------------------------------------------------
//
// Notes:
//
// VMem will always return a valid non-NULL pointer for allocations of zero bytes. This
// applies to both Alloc(0) and Realloc(p, 0). The C++ standard says that the return
// result of malloc(0) is implementation defined. This is not the same
// on all systems, some systems return NULL for allocations of zero bytes, so please ensure
// you check the size before calling Alloc if you require this behavour.
//
// If no alignment is specified: In 32 bit allocations are aligned to 4 bytes.
// In 64 bit all allocations are aligned to 16 bytes.
// Allocating with non-default alignment is less optimal, only use where absolutely necessary.
//
// You can pass in an alignment to the Alloc function, but there is a small overhead if the
// alignment is not the natural alignment. If you doing lots of aligned calls use the AllocAligned
// functions instead. These are more efficient but you need to make sure you free with FreeAligned.
// If 99% of your allocations use the default alignment then it is suggested that you simply
// pass in the alignment to Alloc instead of using the AllocAligned functions. This means that
// you don't have to match up the Free calls to be FreeAligned.
//
// VMem is thread safe except for the case of calling GetSize or Realloc
// on an allocation that is currently int he process of being freed on another thread.
//
// GetSize will always be guaranteed to return a size >= the size that was requested.
// The actual size allocated will sometimes be larger than the size requested.
//
// This is the main VMem Interface and all interaction with VMem should be done though this
// interface.
//
// The only functions to need to call are VMem::Alloc(size) and VMem::Free(p). All other
// functions are optional. There is no need to call Initailise or Destroy, these will be
// called automatically on first use and global teardown.
//
namespace VMem
{
	// initialise

	bool Initialise();

	void Destroy();

	void Flush();

	void CheckIntegrity();

	void SetDebugBreak(void(*DebugBreakFn)(const _TCHAR* p_message));

	typedef void(*LogFn)(const _TCHAR* p_message);
	void SetLogFunction(LogFn log_fn);

	// allocations

	VMEM_RESTRICT_RETURN void* Alloc(size_t size, size_t alignment=VMEM_NATURAL_ALIGNMENT);

	VMEM_RESTRICT_RETURN void* Realloc(void* p, size_t new_size, size_t alignment=VMEM_NATURAL_ALIGNMENT);

	void Free(void* p);

	size_t GetSize(void* p);

	bool Owns(void* p);

	// aligned allocations

	void* AllocAligned(size_t size, size_t alignment);

	void* ReallocAligned(void* p, size_t new_size, size_t alignment);

	void FreeAligned(void* p);

	size_t GetSizeAligned(void* p);

	// heaps

	typedef void* HeapHandle;

	HeapHandle CreateHeap(int page_size = VMEM_SYS_PAGE_SIZE, int reserve_flags = VMEM_DEFAULT_RESERVE_FLAGS, int commit_flags = VMEM_DEFAULT_COMMIT_FLAGS);

	void DestroyHeap(HeapHandle heap);

	void HeapCheckIntegrity(HeapHandle heap);

	// heap allocations

	VMEM_RESTRICT_RETURN void* HeapAlloc(HeapHandle heap, size_t size, size_t alignment = VMEM_NATURAL_ALIGNMENT);

	VMEM_RESTRICT_RETURN void* HeapRealloc(HeapHandle heap, void* p, size_t new_size, size_t alignment = VMEM_NATURAL_ALIGNMENT);

	void HeapFree(HeapHandle heap, void* p);

	size_t HeapGetSize(HeapHandle heap, void* p);

	bool HeapOwns(HeapHandle heap, void* p);

	// aligned heap allocations

	void* HeapAllocAligned(HeapHandle heap, size_t size, size_t alignment);

	void* HeapReallocAligned(HeapHandle heap, void* p, size_t new_size, size_t alignment);

	void HeapFreeAligned(HeapHandle heap, void* p);

	size_t HeapGetSizeAligned(HeapHandle heap, void* p);

	size_t HeapOwnsAligned(HeapHandle heap, void* p);

	// thread heaps

	void CreateThreadHeap();

	void DestroyThreadHeap();

	void ThreadHeapCheckIntegrity();

	// thread heap allocations

	VMEM_RESTRICT_RETURN void* ThreadHeapAlloc(size_t size, size_t alignment = VMEM_NATURAL_ALIGNMENT);

	VMEM_RESTRICT_RETURN void* ThreadHeapRealloc(void* p, size_t new_size, size_t alignment = VMEM_NATURAL_ALIGNMENT);

	void ThreadHeapFree(void* p);

	size_t ThreadHeapGetSize(void* p);

	// aligned thread heap allocations

	void* ThreadHeapAllocAligned(size_t size, size_t alignment);

	void* ThreadHeapReallocAligned(void* p, size_t new_size, size_t alignment);

	void ThreadHeapFreeAligned(void* p);

	size_t ThreadHeapGetSizeAligned(void* p);

	// physical heaps

#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
	HeapHandle CreatePhysicalHeap(size_t coalesce_heap_region_size, size_t coalesce_heap_max_size, int flags);

	void DestroyPhysicalHeap(HeapHandle heap);

	void* PhysicalHeapAlloc(HeapHandle heap, size_t size);

	bool PhysicalHeapFree(HeapHandle heap, void* p);

	size_t PhysicalHeapGetSize(HeapHandle heap, void* p);

	bool PhysicalHeapOwns(HeapHandle heap, void* p);

	void* PhysicalHeapAllocAligned(HeapHandle heap, size_t size, size_t alignment);

	bool PhysicalHeapFreeAligned(HeapHandle heap, void* p);

	size_t PhysicalHeapGetSizeAligned(HeapHandle heap, void* p);

	void PhysicalHeapCheckIntegrity(HeapHandle heap);

	void PhysicalHeapTrim(HeapHandle heap);

	size_t PhysicalHeapGetMaxAllocSize(HeapHandle heap);

	void PhysicalHeapWriteStats(HeapHandle heap);
#endif

	// debug & stats

#ifdef VMEM_COALESCE_HEAP_PER_THREAD
	void CreateCoalesceHeapForThisThread();
#endif

#ifdef VMEM_ENABLE_STATS
	VMemHeapStats GetStats();
	VMemHeapStats GetMainHeapStats();
	VMemHeapStats HeapGetStats(HeapHandle heap);
	VMemHeapStats ThreadHeapGetStats();
	void WriteStats();
	void HeapWriteStats(HeapHandle heap);
	void ThreadHeapWriteStats();
	size_t WriteAllocs();
	void SendStatsToMemPro(void (*send_fn)(void*, int, void*), void* p_context);
#endif

#ifdef VMEM_CUSTOM_ALLOC_INFO
	VMemCustomAllocInfo* GetCustomAllocInfo(void* p);
#endif

#ifdef VMEM_PROTECTED_HEAP
	typedef bool (*VMemSHouldProtectFn)(int i, size_t size);
	void SetVMemShouldProtectFn(VMemSHouldProtectFn should_protect_fn);
#endif
}

//------------------------------------------------------------------------
#if defined(VMEM_OVERRIDE_MALLOC)
	#ifdef VMEM_PLATFORM_PS4

		#define VMEM_PS4_MALLOC_ALIGNMENT 32

		#include <errno.h>
		#include <stdlib.h>
		#include <memory.h>

		namespace VMemUtils
		{
			bool IsPow2(size_t value)
			{
				return (value & (value - 1)) == 0;
			}

			size_t AlignUpToNextPow2(size_t value)
			{
				size_t result = 1;
				while (result < value)
					result <<= 1;
				return result;
			}
		}

		extern "C"
		{
			int user_malloc_init(void);
			int user_malloc_finalize(void);
			void *user_malloc(size_t size);
			void user_free(void *ptr);
			void *user_calloc(size_t nelem, size_t size);
			void *user_realloc(void *ptr, size_t size);
			void *user_memalign(size_t boundary, size_t size);
			int user_posix_memalign(void **ptr, size_t boundary, size_t size);
			void *user_reallocalign(void *ptr, size_t size, size_t boundary);
			int user_malloc_stats(SceLibcMallocManagedSize *mmsize);
			int user_malloc_stats_fast(SceLibcMallocManagedSize *mmsize);
			size_t user_malloc_usable_size(void *ptr);
		}

		int user_malloc_init(void)
		{
			// Do nothing. VMem initialises on first use
			return 0;
		}

		int user_malloc_finalize(void)
		{
			// Do nothing. VMem initialises on first use
			return 0;
		}

		void *user_malloc(size_t size)
		{
			return VMem::AllocAligned(size, VMEM_PS4_MALLOC_ALIGNMENT);
		}

		void user_free(void *ptr)
		{
			VMem::FreeAligned(ptr);
		}

		void *user_calloc(size_t nelem, size_t size)
		{
			size_t total_size = nelem * size;
			void* p = VMem::AllocAligned(total_size, VMEM_PS4_MALLOC_ALIGNMENT);
			memset(p, 0, total_size);
			return p;
		}

		void *user_realloc(void *ptr, size_t size)
		{
			return VMem::ReallocAligned(ptr, size, VMEM_PS4_MALLOC_ALIGNMENT);
		}

		void *user_memalign(size_t boundary, size_t size)
		{
			size_t alignment = boundary;

			if (alignment < VMEM_PS4_MALLOC_ALIGNMENT)
				alignment = VMEM_PS4_MALLOC_ALIGNMENT;
			else if (!VMemUtils::IsPow2(alignment))
				alignment = VMemUtils::AlignUpToNextPow2(alignment);

			return VMem::AllocAligned(size, alignment);
		}

		int user_posix_memalign(void **ptr, size_t boundary, size_t size)
		{
			size_t alignment = boundary;

			if (!VMemUtils::IsPow2(alignment))
				return EINVAL;
			
			if (alignment <= VMEM_PS4_MALLOC_ALIGNMENT)
				alignment = VMEM_PS4_MALLOC_ALIGNMENT;

			void* p = VMem::AllocAligned(size, alignment);

			if (!p)
				return ENOMEM;

			*ptr = p;

			return 0;
		}

		void *user_reallocalign(void *ptr, size_t size, size_t boundary)
		{
			size_t alignment = boundary;

			if (alignment < VMEM_PS4_MALLOC_ALIGNMENT)
				alignment = VMEM_PS4_MALLOC_ALIGNMENT;
			else if (!VMemUtils::IsPow2(alignment))
				alignment = VMemUtils::AlignUpToNextPow2(alignment);

			return VMem::ReallocAligned(ptr, size, alignment);
		}

		int user_malloc_stats(SceLibcMallocManagedSize *)
		{
			// not implemented. If needed can use VMem::GetStats
			return 0;
		}

		int user_malloc_stats_fast(SceLibcMallocManagedSize *)
		{
			// not implemented. If needed can use VMem::GetStats
			return 0;
		}

		size_t user_malloc_usable_size(void *ptr)
		{
			return VMem::GetSizeAligned(ptr);
		}

	#else

		#error // not supported, see comment below
		//
		// Overriding malloc and free is not supported by the C++ standard, and
		// is not officially supported by VMem.
		//
		// However, it is usually possible to replace the functions. 
		// If you do manage to override them VMem will continue to work. VMem
		// uses lower level memory allocation functions and doesn't call malloc itself.
		// If you do run into any problems with VMem after overriding malloc/free
		// please let me know.
		//
		// Linkers will usually look in the crt libs last for an implementation, so by adding
		// your own functions **that match the definitions exactly** you can override
		// them. However, be aware that it is very system dependent, and is not
		// guaranteed to work. The file that they are overridden in does matter.
		// (the code can't be in a header file with gcc for example).
		//
		// The best thing to do is to copy the following 4 functions and paste them
		// into your application main.cpp, ideally as one of the first things the
		// file does, before any standard headers.

		// *********************************************************
		// ******** Copy these functions into your main.cpp ********
		/*
		void* malloc(size_t	size)
		{
			return VMem::Alloc(size);
		}

		void* calloc(size_t size, size_t count)
		{
			int total_size = count * size;
			void* p = VMem::Alloc(total_size);
			if(p)
				memset(p, 0, total_size);
			return p;
		}

		void* realloc(void *p, size_t new_size)
		{
			return VMem::Realloc(p, new_size);
		}

		void free(void *p)
		{
			VMem::Free(p);
		}
		*/
		// *********************************************************

	#endif
#endif

//------------------------------------------------------------------------
#ifdef VMEM_OVERRIDE_NEW_DELETE

#ifdef MEMPRO
	#undef VMEM_OVERRIDE_NEW_DELETE
	#include "MemPro.cpp"
	#define VMEM_OVERRIDE_NEW_DELETE
	#define VMEM_WAIT_FOR_CONNECTION true
	#define VMEM_TRACK_ALLOC(p, size)	MemPro::TrackAlloc(p, size, VMEM_WAIT_FOR_CONNECTION)
	#define VMEM_TRACK_FREE(p)			MemPro::TrackFree(p, VMEM_WAIT_FOR_CONNECTION)
#else
	#define VMEM_TRACK_ALLOC(p, size)
	#define VMEM_TRACK_FREE(p)
#endif

	#include <new>

#if defined(VMEM_PLATFORM_APPLE)
	void* operator new(std::size_t size) throw(std::bad_alloc)
	{
		void* p = VMem::Alloc(size);
		VMEM_TRACK_ALLOC(p, size);
		return p;
	}

	void* operator new(std::size_t size, const std::nothrow_t&) throw()
	{
		void* p = VMem::Alloc(size);
		VMEM_TRACK_ALLOC(p, size);
		return p;
	}

	void  operator delete(void* p) throw()
	{
		VMEM_TRACK_FREE(p);
		VMem::Free(p);
	}

	void  operator delete(void* p, const std::nothrow_t&) throw()
	{
		VMEM_TRACK_FREE(p);
		VMem::Free(p);
	}

	void* operator new[](std::size_t size) throw(std::bad_alloc)
	{
		void* p = VMem::Alloc(size);
		VMEM_TRACK_ALLOC(p, size);
		return p;
	}

	void* operator new[](std::size_t size, const std::nothrow_t&) throw()
	{
		void* p = VMem::Alloc(size);
		VMEM_TRACK_ALLOC(p, size);
		return p;
	}

	void  operator delete[](void* p) throw()
	{
		VMEM_TRACK_FREE(p);
		VMem::Free(p);
	}

	void  operator delete[](void* p, const std::nothrow_t&) throw()
	{
		VMEM_TRACK_FREE(p);
		VMem::Free(p);
	}

#elif defined(VMEM_PLATFORM_PS4)

	void *user_new(std::size_t size) throw(std::bad_alloc)
	{
		return VMem::Alloc(size);
	}

	void *user_new(std::size_t size, const std::nothrow_t& x) throw()
	{
		return VMem::Alloc(size);
	}

	void *user_new_array(std::size_t size) throw(std::bad_alloc)
	{
		return user_new(size);
	}

	void *user_new_array(std::size_t size, const std::nothrow_t& x) throw()
	{
		return user_new(size, x);
	}

	void user_delete(void *ptr) throw()
	{
		VMem::Free(ptr);
	}

	void user_delete(void *ptr, const std::nothrow_t& x) throw()
	{
		(void)x;
		user_delete(ptr);
	}

	void user_delete_array(void *ptr) throw()
	{
		user_delete(ptr);
	}

	void user_delete_array(void *ptr, const std::nothrow_t& x) throw()
	{
		user_delete(ptr, x);
	}

#else
	void* operator new(size_t size)
	{
		void* p = VMem::Alloc(size);
		VMEM_TRACK_ALLOC(p, size);
		return p;
	}

	void operator delete(void* p)
	{
		VMEM_TRACK_FREE(p);
		VMem::Free(p);
	}

	void* operator new[](size_t size)
	{
		void* p = VMem::Alloc(size);
		VMEM_TRACK_ALLOC(p, size);
		return p;
	}

	void operator delete[](void* p)
	{
		VMEM_TRACK_FREE(p);
		VMem::Free(p);
	}
#endif

#endif

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_VMEMALLOC_H_INCLUDED

