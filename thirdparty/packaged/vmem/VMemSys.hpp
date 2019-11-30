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
#ifndef VMEM_VMEMSYS_H_INCLUDED
#define VMEM_VMEMSYS_H_INCLUDED

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#include "VMemCore.hpp"

#if defined(VMEM_INC_INTEG_CHECK)
	#if defined(VMEM_PLATFORM_WIN)
		#include <intrin.h>
	#elif defined(VMEM_PLATFORM_PS4)
		#include <sce_atomic.h>
	#endif
#endif

//------------------------------------------------------------------------
namespace VMem
{
	void VMemSysCreate();

	void VMemSysDestroy();

	void* VirtualReserve(size_t size, int page_size, int reserve_flags);

	void VirtualRelease(void* p, size_t size);

	bool VirtualCommit(void* p, size_t size, int page_size, int commit_flags);

	void VirtualDecommit(void* p, size_t size, int page_size, int commit_flags);

	size_t GetReservedBytes();

	size_t GetCommittedBytes();

	void Break();

	void DebugWrite(const _TCHAR* p_str, ...);

	void DumpSystemMemory();

	void VMemSysSetLogFunction(void (*LogFn)(const _TCHAR* p_message));

	void VMemSysCheckIntegrity();

	void VMemSysWriteStats();

	//-----------------------------------------------------------------------------
#ifdef VMEM_ASSERTS
	bool Committed(void* p, size_t size);
	bool Reserved(void* p, size_t size);
#endif

	//-----------------------------------------------------------------------------
#if defined(VMEM_PLATFORM_APPLE)
	#define VMEM_ALIGN_8 __attribute__((aligned(8)))
#elif defined(VMEM_PLATFORM_PS4)
	#define VMEM_ALIGN_8  __attribute__((aligned(8)))
#else
	#define VMEM_ALIGN_8
#endif
	
	//-----------------------------------------------------------------------------
#ifdef VMEM_INC_INTEG_CHECK
	#if defined(VMEM_OS_WIN)
		VMEM_FORCE_INLINE void VMem_InterlockedIncrement(volatile long& value)
		{
			_InterlockedIncrement(&value);
		}
	#elif defined(VMEM_PLATFORM_PS4)
		VMEM_FORCE_INLINE void VMem_InterlockedIncrement(volatile long& value)
		{
			sceAtomicIncrement32((volatile int32_t*)&value);
		}
	#elif defined(VMEM_OS_LINUX)
		// can't gurantee platform has interlocked instructions, so use critical section.
		// Doesn't have to be particularly fast because it's only used in debug code
		class IncCritSec
		{
		public:
			IncCritSec()
			{
				pthread_mutexattr_t attr;
				pthread_mutexattr_init(&attr);
				pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
				pthread_mutex_init(&cs, &attr);
			}

			~IncCritSec() { pthread_mutex_destroy(&cs); }

			void Enter() { pthread_mutex_lock(&cs); }

			void Leave() { pthread_mutex_unlock(&cs); }

		private:
			pthread_mutex_t cs;
		};
		IncCritSec g_InterlockedAddCritSec;

		VMEM_FORCE_INLINE void VMem_InterlockedIncrement(volatile long& value)
		{
			g_InterlockedAddCritSec.Enter();
			++value;
			g_InterlockedAddCritSec.Leave();
		}
	#else
		#error platform not defined
	#endif
#endif
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_VMEMSYS_H_INCLUDED

