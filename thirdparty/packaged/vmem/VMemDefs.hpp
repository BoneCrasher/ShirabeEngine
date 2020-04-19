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
#ifndef VMEM_VMEMDEFS_H_INCLUDED
#define VMEM_VMEMDEFS_H_INCLUDED

//------------------------------------------------------------------------
#define VMEM_ENABLE		// *** enable\disable VMem here ***

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#include <stddef.h>

//------------------------------------------------------------------------
//
// *** If you are having memory corruption problems enable this define! ***
//
// Debug levels 3 and 4 are designed to be used when tracking down problems,
// such as memory corruptions. It includes periodic integrity checking, memory
// guards, guard pages, guard allocs and trail guards. Full checking does
// have a small memory overhead and can degrade performance. If performance
// is degraded too much you can define the only the checks that you want,
// see the DEBUG DEFINES section.

//#define VMEM_DEBUG_LEVEL 3							// enable full debug here

//------------------------------------------------------------------------
//
// Debug Levels
//
//		0	- All debug code defined out. Use for production biulds.
//		1	- Minimal debugging, minimal slowdown. Use for internal test builds.
//		2	- Standard debug. All but the really slow debug features enabled.
//		3	- Full debugging - very slow. Only enable to help track down bugs
//		4	- Corruption finder and page protection - very expensive.
//
#ifndef VMEM_DEBUG_LEVEL
	#if defined(_DEBUG) || defined(DEBUG) || defined(UE_BUILD_DEVELOPMENT) || defined(UE_BUILD_DEBUG)
		#define VMEM_DEBUG_LEVEL 2
	#else
		#define VMEM_DEBUG_LEVEL 0
	#endif
#endif

//------------------------------------------------------------------------
// Define the platform here.
//
//		VMEM_PLATFORM_WIN
//		VMEM_PLATFORM_LINUX
//		VMEM_PLATFORM_APPLE
//		VMEM_PLATFORM_XBOX360
//		VMEM_PLATFORM_XBOXONE
//		VMEM_PLATFORM_PS4
//		VMEM_PLATFORM_SWITCH
//		VMEM_PLATFORM_ANDROID
//
#if defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__WINDOWS__) || defined(_XBOX)
	#ifdef _XBOX
		#define VMEM_PLATFORM_XBOX360
	#elif defined(_XBOXONE) || defined(_DURANGO)
		#define VMEM_PLATFORM_XBOXONE
	#else
		#define VMEM_PLATFORM_WIN
	#endif
#elif defined(_PS4)
	#define VMEM_PLATFORM_PS4
#elif defined(_NINTENDO_SWITCH)
	#define VMEM_PLATFORM_SWITCH
#elif defined(__APPLE__)
	#define VMEM_PLATFORM_APPLE
#elif defined(PLATFORM_ANDROID)
	#define VMEM_PLATFORM_ANDROID
#else
	#define VMEM_PLATFORM_LINUX
#endif

//------------------------------------------------------------------------
// define the OS
#if defined(VMEM_PLATFORM_WIN)
	#define VMEM_OS_WIN
#elif defined(VMEM_PLATFORM_XBOX360) || defined(VMEM_PLATFORM_XBOXONE)
	#define VMEM_OS_WIN
#elif defined(VMEM_PLATFORM_LINUX) || defined(VMEM_PLATFORM_APPLE) || defined(VMEM_PLATFORM_ANDROID)
	#define VMEM_OS_LINUX
#elif defined(VMEM_PLATFORM_PS4)
	#define VMEM_OS_LINUX
#elif defined(VMEM_PLATFORM_SWITCH)
	#define VMEM_OS_LINUX
#else
	#error platform not defined
#endif

//------------------------------------------------------------------------
#if !defined(_MSC_VER) || _MSC_VER > 1600
	#define VMEM_ATOMICS_SUPPORTED 1
#else
	#define VMEM_ATOMICS_SUPPORTED 0
#endif

//------------------------------------------------------------------------
#if VMEM_DEBUG_LEVEL == 0 && VMEM_ATOMICS_SUPPORTED
	#define VMEM_DECOMMIT_CACHE
#endif

//------------------------------------------------------------------------
// when an FSA cache becomes empty don't release it immediately
#if VMEM_DEBUG_LEVEL == 0 && VMEM_ATOMICS_SUPPORTED
	#define VMEM_FSA_PAGE_CACHE
#endif

//------------------------------------------------------------------------
// when freeing allocations put them on to a free list so that they can be
// quickly re-used
#if VMEM_DEBUG_LEVEL == 0 && VMEM_ATOMICS_SUPPORTED
	#define VMEM_COALESCE_HEAP_CACHE
#endif

//------------------------------------------------------------------------
//#define VMEM_PRE_ALLOCATE_PHYSICAL_PAGES (512*1024*1024)

//------------------------------------------------------------------------
// if VMEM_CUSTOM_PHYSICAL_ALLOCATOR is defined then you must implement these 2 functions:
//
//	void* VMem_AllocatePhysical(size_t& size, size_t alignment, int commit_flags, void*& p_mapped_addr);
//	void VMem_FreePhysical(void* p_physical, void* p_mapped_addr, size_t size);
//
// VMem_AllocatePhysical must return a pointer to physical memory (for example, using 
// sceKernelAllocateMainDirectMemory on PS4). The memory can be mapped or unmapped.
//
// If the memory is mapped p_virtual must be set to the mapped virtual address. VMem
// will unmap the memory before it starts using it, and re-map it back to this address
// when the memory is given back. If the memory is not mapped p_virtual must be set to NULL.
// 
// if VMEM_CUSTOM_PHYSICAL_ALLOCATOR is not defined then VMem will allocate the
// physical memory itself from the OS (using sceKernelAllocateMainDirectMemory on PS4).
//
// You can return more memory than VMem asked for and VMem will use this memory. Make
// sure that you modify the size reference passed in.
//
// VMEM_CUSTOM_PHYSICAL_ALLOCATOR can be useful when you already allocate memory up-front
// for your application, and you want VMem to use that memory.
//
//#define VMEM_CUSTOM_PHYSICAL_ALLOCATOR

//------------------------------------------------------------------------
#if defined(VMEM_SIMULATE_PS4) && !defined(VMEM_CUSTOM_PHYSICAL_ALLOCATOR)
	#define VMEM_CUSTOM_PHYSICAL_ALLOCATOR
#endif

//------------------------------------------------------------------------
// define VMEM_USE_PAGE_MAPPER if you want to provide your own physical memory to VMem
#if defined(VMEM_PLATFORM_PS4) || defined(VMEM_SIMULATE_PS4)
//	#define VMEM_USE_PAGE_MAPPER
#endif

//------------------------------------------------------------------------
// experimental - do not use
// faster implementation of coalesce node lookup
//#define VMEM_COALESCE_HEAP_BUCKET_TREE

//------------------------------------------------------------------------
// experimental - do not use
// sort the node lists on the service thread instead of on free
#if VMEM_DEBUG_LEVEL == 0
//	#define VMEM_SORT_COALESCE_HEAP_ON_SERVICE_THREAD
#endif

//------------------------------------------------------------------------
// experimental - do not use
// each thread has its own FSA heap
#if VMEM_DEBUG_LEVEL == 0
//	#define VMEM_FSA_HEAP_PER_THREAD
#endif

//------------------------------------------------------------------------
// experimental - do not use
// populate the FSA cache on the service thread
#if VMEM_DEBUG_LEVEL == 0
//	#define VMEM_POPULATE_FSA_CACHE
#endif

//------------------------------------------------------------------------
// experimental - do not use
// can set certain thread to have their own coalesce heap to reduce contention
#if VMEM_DEBUG_LEVEL == 0
//	#define VMEM_COALESCE_HEAP_PER_THREAD
#endif

//------------------------------------------------------------------------
// create a service thread that runs in the background performing housekeeping
#if defined(VMEM_DECOMMIT_CACHE) || defined(VMEM_FSA_PAGE_CACHE) || defined(VMEM_SORT_COALESCE_HEAP_ON_SERVICE_THREAD) || defined(VMEM_COALESCE_HEAP_CACHE)
	#define VMEM_SERVICE_THREAD
	#define VMEM_SERVICE_THREAD_PERIOD 30			// (ms) how often to run the service thread update
#endif

//------------------------------------------------------------------------
// experimental - do not use
// fast non-locking cache for FSAs
#if VMEM_DEBUG_LEVEL == 0
//	#define VMEM_FSA_CACHE
//	#define VMEM_FSA_CACHE_SIZE 4096		// size of cache for each FSA
#endif

//------------------------------------------------------------------------
// amount of wastage allowed for FSA sizes (don't want an FSA for every possible size)
#define VMEM_FSA_WASTAGE 5	// (percent)

//------------------------------------------------------------------------
#if (defined(VMEM_OS_WIN) && defined(_WIN64)) || (defined(VMEM_PLATFORM_APPLE) && defined(__LP64__)) || defined(__x86_64)
	#define VMEM_X64
#endif

#if defined(VMEM_PLATFORM_PS4)
	#define VMEM_X64
#endif

#if defined(VMEM_PLATFORM_SWITCH)
	#define VMEM_X64
#endif

//------------------------------------------------------------------------
// DEBUG DEFINES - these are enabled based on the debug level, but you can
// enable them individually here.

//#define VMEM_ASSERTS					// small overhead
//#define VMEM_ENABLE_MEMSET			// memset all memory when it is committed, allocated and freed - small overhead
//#define VMEM_MEMSET_ONLY_SMALL 256	// memset only the first n bytes of every allocation - minimal overhead
//#define VMEM_ENABLE_STATS				// record memory usage in all heaps - minimal overhead
//#define VMEM_COALESCE_GUARDS			// guard blocks at the end of each alloc - small overhead
//#define VMEM_FSA_GUARDS				// guard blocks at the end of each alloc - medium overhead
//#define VMEM_INC_INTEG_CHECK 1000		// the frequency to do an incremental integrity check
//#define VMEM_TRAIL_GUARDS				// don't re-allocate the last n allocs to check for overwrite corruptions
//#define VMEM_ALTERNATE_PAGES			// only commit alternate pages in the page heaps
//#define VMEM_COALESCE_GUARD_PAGES 4	// always keep a page decommitted every n times the max alloc size. Can trap memory corruptions.
//#define VMEM_COALESCE_HEAP_MARKER		// put a marker word at the start of each allocation
//#define VMEM_MEMORY_CORRUPTION_FINDER	// don't re-use freed pages, only decommit them. This can help catch memory corruptions.
//#define VMEM_PROTECTED_HEAP			// add specific allocations to the protected debug heap
//#define VMEM_CHECK_ALLOCATED_MARKERS	// keep track of all allocations and check markers

//------------------------------------------------------------------------
#if VMEM_DEBUG_LEVEL >= 1
	#define VMEM_ASSERTS
	#define VMEM_ENABLE_STATS
	#define VMEM_COALESCE_GUARDS
	#define VMEM_COALESCE_HEAP_MARKER
#endif

//------------------------------------------------------------------------
#if VMEM_DEBUG_LEVEL >= 2
	#define VMEM_FSA_GUARDS
	#define VMEM_ENABLE_MEMSET
#elif VMEM_DEBUG_LEVEL == 1
	#define VMEM_MEMSET_ONLY_SMALL 256
#endif

//------------------------------------------------------------------------
#if VMEM_DEBUG_LEVEL >= 3
	#define VMEM_INC_INTEG_CHECK 1000
	#define VMEM_TRAIL_GUARDS
	#define VMEM_ALTERNATE_PAGES
	#define VMEM_COALESCE_GUARD_PAGES 4
	#define VMEM_CHECK_ALLOCATED_MARKERS
#endif

//------------------------------------------------------------------------
#if VMEM_DEBUG_LEVEL >= 4
	#define VMEM_MEMORY_CORRUPTION_FINDER
	#define VMEM_PROTECTED_HEAP
#endif

//------------------------------------------------------------------------
#if VMEM_DEBUG_LEVEL >= 1
//	#define VMEM_CUSTOM_ALLOC_INFO		// to enable VMem::GetCustomAllocInfo(p)
#endif

//------------------------------------------------------------------------
// define this for a _slight_ speed increase but more memory fragmentation
//#define VMEM_DISABLE_BIASING

//------------------------------------------------------------------------
// enable marker in all configs for x86 because it doesn't take up any more space
#if !defined(VMEM_X64) && !defined(VMEM_COALESCE_HEAP_MARKER)
	#define VMEM_COALESCE_HEAP_MARKER
#endif

//------------------------------------------------------------------------
#if defined(VMEM_MEMORY_CORRUPTION_FINDER) && (defined(VMEM_PLATFORM_XBOX360) || defined(VMEM_PLATFORM_XBOXONE))
	#undef VMEM_MEMORY_CORRUPTION_FINDER		// this feature will probably use up too much memory on consoles with fixed memory
#endif

//------------------------------------------------------------------------
#if defined(VMEM_MEMORY_CORRUPTION_FINDER) && defined(VMEM_PLATFORM_PS4)
	#undef VMEM_MEMORY_CORRUPTION_FINDER		// this feature will probably use up too much memory on consoles with fixed memory
#endif

//------------------------------------------------------------------------
#ifdef VMEM_MEMORY_CORRUPTION_FINDER
	#ifdef VMEM_TRAIL_GUARDS
		#undef VMEM_TRAIL_GUARDS
	#endif
	#ifndef VMEM_DISABLE_BIASING
		#define VMEM_DISABLE_BIASING
	#endif
	#ifdef VMEM_DECOMMIT_CACHE
		#undef VMEM_DECOMMIT_CACHE
	#endif
#endif

//------------------------------------------------------------------------
// the system page size
#if defined(VMEM_PLATFORM_XBOX360) || defined(VMEM_PLATFORM_XBOXONE)
	#define VMEM_SYS_PAGE_SIZE (64*1024)
#elif defined(VMEM_PLATFORM_PS4) || (defined(VMEM_SIMULATE_PS4) && defined(VMEM_OS_WIN))
	#define VMEM_SYS_PAGE_SIZE (16*1024)
#elif defined(VMEM_PLATFORM_SWITCH)
	#error // please define VMEM_SYS_PAGE_SIZE to the default page size for Nintendo Switch (nn::os::MemoryPageSize)
#elif defined(VMEM_PLATFORM_WIN)
	#define VMEM_SYS_PAGE_SIZE 4096
#elif defined(VMEM_OS_LINUX)
	#define VMEM_SYS_PAGE_SIZE 4096
#else
	#error platform not defined
#endif

//------------------------------------------------------------------------
// the flags that VMem uses to allocate internal (CPU) memory
#define VMEM_DEFAULT_RESERVE_FLAGS 0
#define VMEM_DEFAULT_COMMIT_FLAGS 0

//------------------------------------------------------------------------
#ifdef VMEM_OS_WIN
	#define VMEM_PHYSICAL_HEAP_SUPPORTED
#endif

//------------------------------------------------------------------------
#ifdef VMEM_PROTECTED_HEAP

#ifdef VMEM_PLATFORM_XBOXONE
	#define VMEM_PROTECTED_HEAP_MAX_OVERHEAD (400*1024*1024)
#elif defined(VMEM_X64)
	#define VMEM_PROTECTED_HEAP_MAX_OVERHEAD (10LL*1024LL*1024LL*1024LL)
#else
	#define VMEM_PROTECTED_HEAP_MAX_OVERHEAD (100*1024*1024)
#endif

// use this function to decide whether an allocation should go via the protected debug heap.
// i is incremented every allocation, size is the size of the allocation.
// you can also use VMem::SetVMemShouldProtectFn to set your own function.
namespace VMem
{
	inline bool VMemShouldProtect(int i, size_t size)
	{
		return size > VMEM_SYS_PAGE_SIZE/2 || (i % 100) == 0;
	}
}
#endif

//------------------------------------------------------------------------
#if defined(VMEM_PLATFORM_PS4) || defined(VMEM_SIMULATE_PS4)
	#define VMEM_NATURAL_ALIGNMENT 16
#elif defined(VMEM_X64)
	#define VMEM_NATURAL_ALIGNMENT 16
#else
	#define VMEM_NATURAL_ALIGNMENT 4
#endif

//------------------------------------------------------------------------
// guard sizes
#define VMEM_FSA_PRE_GUARD_SIZE		0
#define VMEM_FSA_POST_GUARD_SIZE	4
#define VMEM_COALESCE_GUARD_SIZE	VMEM_NATURAL_ALIGNMENT

//------------------------------------------------------------------------
// markers
#define VMEM_FSA_PAGE_HEADER_MARKER		0xf5a0f5a0
#define VMEM_FSA_MARKER					0xff55aa00
#define VMEM_COALESCE_ALLOC_MARKER		0xabcdef12

//------------------------------------------------------------------------
// memset codes
#define VMEM_COMMITTED_MEM		0xcfcfcfcf		// committed but not yet allocated
#define VMEM_DECOMMITTED_MEM	0xdededede		// decommitted memory (used in delayed decommit)
#define VMEM_UNUSED_PAGE		0xaeaeaeae
#define VMEM_ALLOCATED_MEM		0xcdcdcdcd		// allocated but not yet written to
#define VMEM_FREED_MEM			0xdddddddd		// freed memory
#define VMEM_GUARD_MEM			0xfdfdfdfd		// guards at ends of allocs
#define VMEM_TRAIL_GUARD_MEM	0xbdbdbdbd
#define VMEM_PROTECTED_PAGE		0xafafafaf

//------------------------------------------------------------------------
#define VMEM_DECOMMIT_CACHE_RELEASE_FRAME_DELAY 20		// number of frames to keep the released or decommitted memory the cache
#define VMEM_DECOMMIT_CACHE_SIZE (50*1024*1024)			// maximum size of the decommit cache
#define VMEM_DECOMMIT_CACHE_MAX_SIZE (10*1024*1024)		// maximum allocation size that can go into the decommit cache

//------------------------------------------------------------------------
#define VMEM_COALESCE_HEAP_CACHE_SIZE (10*1024*1024)
#define VMEM_COALESCE_HEAP_CACHE_MAX_AGE 5						// (frames) number of frames before removing from the cache
#define VMEM_COALESCE_HEAP_CACHE_BUCKET_STEP 128				// bytes between each bucket size
#define VMEM_COALESCE_HEAP_CACHE_MAX_BUCKET_SIZE (512*1024)		// don't cache allocations above this size
#define VMEM_COALESCE_HEAP1_CACHE true							// enable the coalesce cache on coalesce heap 1
#define VMEM_COALESCE_HEAP2_CACHE true							// enable the coalesce cache on coalesce heap 2

//------------------------------------------------------------------------
#define VMEM_FSA_PAGE_CACHE_COUNT_MIN 5		// keep at least this many empty pages around
#define VMEM_FSA_PAGE_CACHE_COUNT_MAX 20	// never have more than this many empty pages

//------------------------------------------------------------------------
// max number of allocs WriteAllocs will output for each heap
#define VMEM_WRITE_ALLOCS_MAX 16

//------------------------------------------------------------------------
// size returned by GetSize if the allocator doesn't own the alloc
#ifdef VMEM_X64
	#define VMEM_INVALID_SIZE ULLONG_MAX
#else
	#define VMEM_INVALID_SIZE UINT_MAX
#endif

//------------------------------------------------------------------------
// Windows defines for LINUX
#ifdef VMEM_OS_LINUX
	#ifndef _T
		#define _T(s) s
	#endif
	#define _TCHAR char
#endif

//------------------------------------------------------------------------
// Add debug info that you want to be associated with each allocation to
// this struct. Keep it as small as possible as it can add significant overhead.
// Size of the struct must be aligned to VMEM_NATURAL_ALIGNMENT.
// To use define VMEM_CUSTOM_ALLOC_INFO in VMemDefs.hpp
// To get the struct for an allocation use VMem::GetCustomAllocInfo(p)
// Must be a POD type. Must not allocate.
#ifdef VMEM_CUSTOM_ALLOC_INFO
struct VMemCustomAllocInfo
{
	int m_Type;		// Example - type of the allocation

	// align this struct to VMEM_NATURAL_ALIGNMENT
	#if VMEM_NATURAL_ALIGNMENT != 4
		int m_Padding[(VMEM_NATURAL_ALIGNMENT - sizeof(int)) / sizeof(int)];
	#endif
};
#endif

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_VMEMDEFS_H_INCLUDED

