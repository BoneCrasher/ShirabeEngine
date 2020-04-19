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
#ifndef VMEM_VMEMCORE_H_INCLUDED
#define VMEM_VMEMCORE_H_INCLUDED

//------------------------------------------------------------------------
#include "VMemDefs.hpp"
#include "VMemStats.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#include <limits.h>

#ifdef VMEM_OS_WIN
	#pragma warning(push)
	#pragma warning(disable : 4668)
#endif
#include <stdint.h>
#ifdef VMEM_OS_WIN
	#pragma warning(pop)
#endif

#include <memory.h>

#ifdef VMEM_OS_WIN
	#include <tchar.h>
#else
	#include <stdio.h>
#endif

#ifdef VMEM_PLATFORM_ANDROID
	#include <stdarg.h>
#endif

#ifdef VMEM_PLATFORM_PS4
	#include <kernel.h>
#endif

//-----------------------------------------------------------------------------
#ifdef VMEM_OS_WIN
	#pragma warning(disable:4127)	// conditional expression is constant
#endif

//------------------------------------------------------------------------
#if defined(VMEM_OS_WIN)
	#define VMEM_POINTER_PREFIX "0x"
#elif defined(VMEM_PLATFORM_PS4)
	#define VMEM_POINTER_PREFIX "0x"
#else
	#define VMEM_POINTER_PREFIX ""
#endif

//------------------------------------------------------------------------
#ifdef VMEM_OS_WIN
	#define VMEM_DEBUG_BREAK __debugbreak()
#else
	#define VMEM_DEBUG_BREAK __builtin_trap()
#endif

//------------------------------------------------------------------------
#define VMEM_START_MULTI_LINE_MACRO	do {
#define VMEM_END_MULTI_LINE_MACRO } while (false)

//------------------------------------------------------------------------
#define VMEM_UNREFERENCED_PARAM(p) (void)(p)

//------------------------------------------------------------------------
#ifdef VMEM_ASSERTS
	#define VMEM_ASSERT(b, m)					\
		VMEM_START_MULTI_LINE_MACRO				\
		if(!(b))								\
		{										\
			if(!VMem::Break(_T(m) _T("\n")))	\
				VMEM_DEBUG_BREAK;				\
		}										\
		VMEM_END_MULTI_LINE_MACRO
#else
	#define VMEM_ASSERT(b, m) ((void)0)
#endif

//------------------------------------------------------------------------
#ifdef VMEM_ASSERTS
	#define VMEM_ASSERT2(b, m, a1, a2)					\
		VMEM_START_MULTI_LINE_MACRO						\
		if(!(b))										\
		{												\
			VMem::DebugWrite(_T(m) _T("\n"), a1, a2);	\
			if(!VMem::Break(_T("VMem Assert\n")))		\
				VMEM_DEBUG_BREAK;						\
		}												\
		VMEM_END_MULTI_LINE_MACRO
#else
	#define VMEM_ASSERT1(b, m, a) ((void)0)
#endif

//------------------------------------------------------------------------
#ifdef VMEM_ASSERTS
	#define VMEM_MEM_CHECK(p_value, expected_value)	\
		VMEM_START_MULTI_LINE_MACRO					\
		if(*(p_value) != (expected_value))			\
		{											\
			VMem::DebugWrite(sizeof(expected_value) == 8 ? _T("Memory corruption at ") _T(VMEM_POINTER_PREFIX) _T("%p : value ") _T(VMEM_POINTER_PREFIX) _T("%p : expected value ") _T(VMEM_POINTER_PREFIX) _T("%p\n") : _T("Memory corruption at ") _T(VMEM_POINTER_PREFIX) _T("%p : value 0x%08x : expected value 0x%08x\n"), (p_value), *(p_value), (expected_value));	\
			if(!VMem::Break(_T("VMem Assert\n")))	\
				VMEM_DEBUG_BREAK;					\
		}											\
		VMEM_END_MULTI_LINE_MACRO
#else
	#define VMEM_MEM_CHECK(p_value, expected_value) ((void)0)
#endif

//------------------------------------------------------------------------
#ifdef VMEM_ASSERTS
	#define VMEM_ASSERT_MEM(b, p_value)				\
		VMEM_START_MULTI_LINE_MACRO					\
		if(!(b))									\
		{											\
			VMem::DebugWrite(_T("Memory corruption at ") _T(VMEM_POINTER_PREFIX) _T("%p : value ") _T(VMEM_POINTER_PREFIX) _T("%p\n"), (p_value), *((void**)(p_value)));	\
			if(!VMem::Break(_T("VMem Assert\n")))	\
				VMEM_DEBUG_BREAK;					\
		}											\
		VMEM_END_MULTI_LINE_MACRO
#else
	#define VMEM_ASSERT_MEM(b, p_value) ((void)0)
#endif

//------------------------------------------------------------------------
#ifdef VMEM_ASSERTS
	#define VMEM_BREAK(m)				\
		VMEM_START_MULTI_LINE_MACRO		\
		if(!VMem::Break(_T(m)))			\
			VMEM_DEBUG_BREAK;			\
		VMEM_END_MULTI_LINE_MACRO
#else
	#define VMEM_BREAK(m) ((void)0)
#endif

//------------------------------------------------------------------------
// compile time assert
#define VMEM_STATIC_ASSERT(expr, message) static_assert(expr, message)

//------------------------------------------------------------------------
// for code that is only needed when asserts are turned on
#ifdef VMEM_ASSERTS
	#define VMEM_ASSERT_CODE(e) e
#else
	#define VMEM_ASSERT_CODE(e) ((void)0)
#endif

//------------------------------------------------------------------------
// for code that is only needed when memsetting is turned on
#if defined(VMEM_ENABLE_MEMSET)
	#define VMEM_MEMSET(p, value, size) memset(p, value, size)
#elif defined(VMEM_MEMSET_ONLY_SMALL)
	#define VMEM_MEMSET(p, value, size) memset(p, value, VMem::VMin((size_t)size, (size_t)VMEM_MEMSET_ONLY_SMALL))
#else
	#define VMEM_MEMSET(p, value, size) VMEM_UNREFERENCED_PARAM(p)
#endif

//------------------------------------------------------------------------
// for code that is only needed when stats are turned on
#ifdef VMEM_ENABLE_STATS
	#define VMEM_STATS(e) e
#else
	#define VMEM_STATS(e) ((void)0)
#endif

//------------------------------------------------------------------------
#ifdef VMEM_X64
	#define VMEM_X64_ONLY(s) s
#else
	#define VMEM_X64_ONLY(s)
#endif

//------------------------------------------------------------------------
#ifdef VMEM_X64
	VMEM_STATIC_ASSERT(sizeof(void*) == sizeof(long long), "please undefine VMEM_X64 for x86 builds");
#else
	VMEM_STATIC_ASSERT(sizeof(void*) == sizeof(unsigned int), "please define VMEM_X64 for 64bit builds");
#endif

//-----------------------------------------------------------------------------
#if defined(VMEM_OS_WIN)
	#if VMEM_DEBUG_LEVEL == 0
		#define VMEM_FORCE_INLINE __forceinline
	#else
		#define VMEM_FORCE_INLINE inline
	#endif
	#define VMEM_NO_INLINE __declspec(noinline)
#else
	#define VMEM_FORCE_INLINE inline
	#define VMEM_NO_INLINE
#endif

//------------------------------------------------------------------------
// analysis macros
#if defined(VMEM_OS_WIN) &&  _MSC_FULL_VER >= 170060315
	#define VMEM_ASSUME(e) __analysis_assume(e)
	#define VMEM_ACQUIRES_LOCK(e) _Acquires_lock_(e)
	#define VMEM_RELEASES_LOCK(e) _Releases_lock_(e)
	#define VMEM_ACQUIRES_SHARED_LOCK(e) _Acquires_shared_lock_(e)
	#define VMEM_RELEASES_SHARED_LOCK(e) _Releases_shared_lock_(e)
	#define VMEM_ACQUIRES_EXCLUSIVE_LOCK(e) _Acquires_exclusive_lock_(e)
	#define VMEM_RELEASES_EXCLUSIVE_LOCK(e) _Releases_exclusive_lock_(e)
#else
	#define VMEM_ASSUME(e)
	#define VMEM_ACQUIRES_LOCK(e)
	#define VMEM_RELEASES_LOCK(e)
	#define VMEM_ACQUIRES_SHARED_LOCK(e)
	#define VMEM_RELEASES_SHARED_LOCK(e)
	#define VMEM_ACQUIRES_EXCLUSIVE_LOCK(e)
	#define VMEM_RELEASES_EXCLUSIVE_LOCK(e)
#endif

//------------------------------------------------------------------------
VMEM_STATIC_ASSERT(sizeof(size_t) == sizeof(void*), "VMEM_X64 define not set correctly");

//------------------------------------------------------------------------
#define VMEM_INTERNAL_ALIGNMENT sizeof(void*)

//------------------------------------------------------------------------
#if VMEM_DEBUG_LEVEL != 0 || defined(VMEM_ASSERTS)
	#define VMEM_SIMULATE_OOM 1
#else
	#define VMEM_SIMULATE_OOM 0
#endif

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	// used for MemPro stats
	enum VmemAllocatorType
	{
		vmem_BasicCoalesceHeap = 0,
		vmem_PageHeap,
		vmem_FSAHeap,
		vmem_CoalesceHeap,
		vmem_LargeHeap,
		vmem_End
	};

	//------------------------------------------------------------------------
	struct AlignedHeader
	{
		void* p;
	};

	//------------------------------------------------------------------------
	struct PhysicalAlloc
	{
		PhysicalAlloc()
		:	mp_Physical(0),
			m_Size(0),
			mp_Virtual(NULL),
			mp_OriginalMappedAddr(NULL)
		{
		}

		PhysicalAlloc(void* p_physical, size_t size, void* p_mapped_addr, void* p_original_mapped_addr)
		:	mp_Physical(p_physical),
			m_Size(size),
			mp_Virtual(p_mapped_addr),
			mp_OriginalMappedAddr(p_original_mapped_addr)
		{
		}

		void* mp_Physical;
		size_t m_Size;
		void* mp_Virtual;				// the virtual address that the physical address is mapped to  (can be NULL)
		void* mp_OriginalMappedAddr;	// the original virtual address that the physical address was mapped to before given to VMem
	};

	//------------------------------------------------------------------------
	class InternalAllocator
	{
	public:
		virtual void* Alloc(size_t size) = 0;
		virtual void Free(void* p, size_t size) = 0;
	};

	//------------------------------------------------------------------------
	void InitialiseCore();

	void UninitialiseCore();

	bool Break(const _TCHAR* p_message);

	void DebugWrite(const _TCHAR* p_message, ...);

	void SetDebugBreakFunction(void (*DebugBreakFn)(const _TCHAR* p_message));

	#if VMEM_SIMULATE_OOM
		int GetSimulateOOMCount();
		void SetSimulateOOMCount(int value);
		int GetInternalHeapSimulateOOMCount();
		void SetInternalHeapSimulateOOMCount(int value);
		int GetPhysicalOOMCount();
		void SetPhysicalOOMCount(int value);
	#endif

	#ifdef VMEM_ENABLE_STATS
		void WriteStats(const VMem::Stats& stats);
		void WriteStats(const VMemHeapStats& stats);
	#endif

	//------------------------------------------------------------------------
	typedef unsigned char byte;

	//------------------------------------------------------------------------
	typedef long long int64;
	typedef unsigned long long uint64;

	//------------------------------------------------------------------------
	typedef void (*MemProSendFn)(void*, int, void*);

	//------------------------------------------------------------------------
	#ifdef VMEM_OS_WIN
		#define VMEM_THREAD_LOCAL __declspec(thread)
	#else
		#define VMEM_THREAD_LOCAL __thread
	#endif

	//------------------------------------------------------------------------
	template<typename T>
	VMEM_FORCE_INLINE T VMin(T a, T b)
	{
		return a < b ? a : b;
	}

	//------------------------------------------------------------------------
	template<typename T>
	VMEM_FORCE_INLINE T VMax(T a, T b)
	{
		return a > b ? a : b;
	}

	//-----------------------------------------------------------------------------
	template<typename T>
	VMEM_FORCE_INLINE void SendToMemPro(T& value, MemProSendFn send_fn, void* p_context)
	{
		send_fn(&value, sizeof(value), p_context);
	}

	//-----------------------------------------------------------------------------
	template<typename T>
	VMEM_FORCE_INLINE void SendEnumToMemPro(T value, MemProSendFn send_fn, void* p_context)
	{
		send_fn(&value, sizeof(value), p_context);
	}

	//-----------------------------------------------------------------------------
	VMEM_FORCE_INLINE bool IsPow2(unsigned int value)
	{
		return (value & (value-1)) == 0;
	}

	//------------------------------------------------------------------------
	VMEM_FORCE_INLINE int AlignUp(int i, int alignment)
	{
		return (((i-1) / alignment) + 1) * alignment;
	}

	//------------------------------------------------------------------------
	VMEM_FORCE_INLINE size_t AlignUp(size_t i, size_t alignment)
	{
		return (((i-1) / alignment) + 1) * alignment;
	}

	//------------------------------------------------------------------------
	VMEM_FORCE_INLINE size_t AlignUp(size_t i, int alignment)
	{
		return (((i-1) / alignment) + 1) * alignment;
	}

	//------------------------------------------------------------------------
	VMEM_FORCE_INLINE void* AlignUp(void* p, size_t alignment)
	{
		size_t i = (size_t)p;
		return (void*)((((i-1) / alignment) + 1) * alignment);
	}

	//------------------------------------------------------------------------
	VMEM_FORCE_INLINE int AlignUpPow2(int i, int alignment)
	{
		VMEM_ASSERT(IsPow2(alignment), "non-pow2 value passed to align function");
		int mask = alignment - 1;
		return (i + mask) & ~mask;
	}

	//------------------------------------------------------------------------
	VMEM_FORCE_INLINE size_t AlignSizeUpPow2(size_t i, int alignment)
	{
		VMEM_ASSERT(IsPow2(alignment), "non-pow2 value passed to align function");
		int mask = alignment - 1;
		return (i + mask) & ~mask;
	}

	//------------------------------------------------------------------------
	VMEM_FORCE_INLINE void* AlignUpPow2(void* p, int alignment)
	{
		VMEM_ASSERT(IsPow2(alignment), "non-pow2 value passed to align function");
		size_t i = (size_t)p;
		int mask = alignment - 1;
		return (void*)((i + mask) & ~mask);
	}

	//------------------------------------------------------------------------
	VMEM_FORCE_INLINE int AlignDownPow2(int i, int alignment)
	{
		VMEM_ASSERT(IsPow2(alignment), "non-pow2 value passed to align function");
		return i & ~(alignment-1);
	}

	//------------------------------------------------------------------------
	VMEM_FORCE_INLINE void* AlignDownPow2(void* p, int alignment)
	{
		VMEM_ASSERT(IsPow2(alignment), "non-pow2 value passed to align function");
		size_t i = (size_t)p;
		return (void*)(i & ~(alignment-1));
	}

	//------------------------------------------------------------------------
	VMEM_FORCE_INLINE int AlignUpToNextPow2(int size)
	{
		VMEM_ASSERT(size > 1, "size must be > 0 in AlignUpToNextPow2");

		int s = 1;
		while(s < size)
		{
			VMEM_ASSERT(s != (1<<31), "size out of range in AlignUpToNextPow2");
			s <<= 1;
		}
		return s;
	}

	//------------------------------------------------------------------------
	void WriteAlloc(const void* p_alloc, size_t size);

	//------------------------------------------------------------------------
	VMEM_FORCE_INLINE void SetGuards(void* p, int size)
	{
		VMEM_ASSERT((((size_t)p) & 3) == 0, "guard pointer misaligned");
		VMEM_ASSERT((size & 3) == 0, "guard pointer misaligned");

		unsigned int* p_guard = (unsigned int*)p;
		unsigned int* p_end_guard = (unsigned int*)((byte*)p + size);
		while(p_guard != p_end_guard)
			*p_guard++ = VMEM_GUARD_MEM;
	}

	//------------------------------------------------------------------------
	VMEM_FORCE_INLINE void CheckMemory(void* p, size_t size, unsigned int value)
	{
#ifdef VMEM_ASSERTS
		VMEM_ASSERT((((size_t)p) & 3) == 0, "CheckMemory pointer misaligned");
		VMEM_ASSERT((size & 3) == 0, "CheckMemory size misaligned");

		#ifdef VMEM_MEMSET_ONLY_SMALL
			size = VMem::VMin(size, (size_t)VMEM_MEMSET_ONLY_SMALL);
		#endif
		VMEM_ASSERT((size % sizeof(unsigned int)) == 0, "bad size");
		unsigned int* p_uint = (unsigned int*)p;
		unsigned int* p_end = (unsigned int*)((byte*)p + size);
		while(p_uint != p_end)
		{
			VMEM_MEM_CHECK(p_uint, value);
			++p_uint;
		}
#else
		VMEM_UNREFERENCED_PARAM(p);
		VMEM_UNREFERENCED_PARAM(size);
		VMEM_UNREFERENCED_PARAM(value);
#endif
	}

	//------------------------------------------------------------------------
	VMEM_FORCE_INLINE int ToInt(size_t size)
	{
		VMEM_ASSERT(size <= INT_MAX, "size out of range");
		return (int)size;
	}

	//------------------------------------------------------------------------
	VMEM_FORCE_INLINE size_t ToSizeT(int value)
	{
		VMEM_ASSERT(value >= 0, "value out of range while converting to size_t");
		return (size_t)value;
	}

	//------------------------------------------------------------------------
	#ifndef VMEM_OS_WIN
		VMEM_FORCE_INLINE int _vstprintf_s(char *buffer, size_t numberOfElements, const char *format, va_list argptr) { return vsprintf(buffer, format, argptr); }
	#endif

	//------------------------------------------------------------------------
	#ifndef VMEM_OS_WIN
		#define memcpy_s(p_dst, dst_size, p_src, src_size) memcpy(p_dst, p_src, src_size)
	#endif

	//------------------------------------------------------------------------
	#ifdef VMEM_OS_WIN
		#define VMEM_RESTRICT __restrict
	#else
		#define VMEM_RESTRICT
	#endif
}

//------------------------------------------------------------------------
VMEM_STATIC_ASSERT(sizeof(VMem::byte) == 1, "sizeof(VMem::byte) == 1");
VMEM_STATIC_ASSERT(sizeof(int) == 4, "sizeof(int) == 4");
VMEM_STATIC_ASSERT(sizeof(unsigned int) == 4, "sizeof(unsigned int) == 4");
VMEM_STATIC_ASSERT(sizeof(long long) == 8, "sizeof(long long) == 8");
VMEM_STATIC_ASSERT(sizeof(VMem::uint64) == 8, "sizeof(VMem::uint64) == 8");

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_VMEMCORE_H_INCLUDED

