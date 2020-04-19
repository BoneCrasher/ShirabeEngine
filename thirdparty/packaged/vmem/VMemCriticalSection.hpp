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
//-----------------------------------------------------------------------------
#ifndef VMEM_CRITICALSECTION_H_INCLUDED
#define VMEM_CRITICALSECTION_H_INCLUDED

//-----------------------------------------------------------------------------
#include "VMemCore.hpp"
#include "VMemSys.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//-----------------------------------------------------------------------------
#ifdef VMEM_OS_LINUX
	#include <pthread.h>
#endif

//-----------------------------------------------------------------------------
#if defined(VMEM_OS_WIN)
//	#define VMEM_USE_SLIM_READ_WRITE_LOCK
#endif

//-----------------------------------------------------------------------------
//#define VMEM_CHECK_FOR_RECURSIVE_LOCKS

//-----------------------------------------------------------------------------
namespace VMem
{
	//-----------------------------------------------------------------------------
	// This is NOT re-entrant. Avoids having to have an extra interlocked increment.
	// Yields after 40 checks.
	class CriticalSection
	{
	public:
		//-----------------------------------------------------------------------------
		CriticalSection()
#ifdef VMEM_CHECK_FOR_RECURSIVE_LOCKS
		:	m_Locked(false)
#endif
		{
#if defined(VMEM_OS_WIN) && defined(VMEM_USE_SLIM_READ_WRITE_LOCK)

			InitializeSRWLock(&m_CritSec);

#elif defined(VMEM_OS_WIN) && !defined(VMEM_USE_SLIM_READ_WRITE_LOCK)

			// disable debug info in release
			#if VMEM_DEBUG_LEVEL == 0
				int flags = CRITICAL_SECTION_NO_DEBUG_INFO;
			#else
				int flags = 0;
			#endif
			InitializeCriticalSectionEx(&m_CritSec, m_SpinCount, flags);

#elif defined(VMEM_PLATFORM_PS4)
			scePthreadMutexInit(&m_Mutex, NULL, NULL);

#elif defined(VMEM_OS_LINUX)

			VMEM_ASSERT((((int64)this) % 8) == 0, "CriticalSection not aligned to 8 bytes");

			pthread_mutexattr_t attr;
			pthread_mutexattr_init(&attr);
			pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
			pthread_mutex_init(&cs, &attr);

#else
			#error platform not defined
#endif
		}

		//-----------------------------------------------------------------------------
		~CriticalSection()
		{
#if defined(VMEM_OS_WIN) && !defined(VMEM_USE_SLIM_READ_WRITE_LOCK)
			DeleteCriticalSection(&m_CritSec);
#elif defined(VMEM_PLATFORM_PS4)
			scePthreadMutexDestroy(&m_Mutex);
#elif defined(VMEM_OS_LINUX)
			pthread_mutex_destroy(&cs);
#else
			#error platform not defined
#endif
		}

		//-----------------------------------------------------------------------------
#if defined(VMEM_OS_WIN) && !defined(VMEM_USE_SLIM_READ_WRITE_LOCK)
		VMEM_ACQUIRES_LOCK(this->m_CritSec) VMEM_FORCE_INLINE void Enter()
		{
			EnterCriticalSection(&m_CritSec);

			#ifdef VMEM_CHECK_FOR_RECURSIVE_LOCKS
				VMEM_ASSERT(!m_Locked, "critical section locked recursively! Please contact slynch@puredevsoftware.com");
				m_Locked = true;
			#endif
		}
#endif

		//-----------------------------------------------------------------------------
#if defined(VMEM_OS_WIN) && defined(VMEM_USE_SLIM_READ_WRITE_LOCK)
		VMEM_FORCE_INLINE VMEM_ACQUIRES_LOCK(this->m_CritSec) void Enter()
		{
			AcquireSRWLockExclusive(&m_CritSec);
		}
#endif

		//-----------------------------------------------------------------------------
#if defined(VMEM_OS_LINUX)
		VMEM_FORCE_INLINE void Enter()
		{
			#if defined(VMEM_PLATFORM_PS4)
				scePthreadMutexLock(&m_Mutex);
			#else
				pthread_mutex_lock(&cs);
			#endif

			#ifdef VMEM_CHECK_FOR_RECURSIVE_LOCKS
				VMEM_ASSERT(!m_Locked, "critical section locked recursively! Please contact slynch@puredevsoftware.com");
				m_Locked = true;
			#endif
		}
#endif

		//-----------------------------------------------------------------------------
#if defined(VMEM_OS_WIN) && defined(VMEM_USE_SLIM_READ_WRITE_LOCK)
		VMEM_FORCE_INLINE VMEM_RELEASES_LOCK(this->m_CritSec) void Leave()
		{
			#ifdef VMEM_CHECK_FOR_RECURSIVE_LOCKS
				m_Locked = false;
			#endif

			ReleaseSRWLockExclusive(&m_CritSec);
		}
#endif

		//-----------------------------------------------------------------------------
#if defined(VMEM_OS_WIN) && !defined(VMEM_USE_SLIM_READ_WRITE_LOCK)
		VMEM_FORCE_INLINE VMEM_RELEASES_LOCK(this->m_CritSec) void Leave()
		{
			#ifdef VMEM_CHECK_FOR_RECURSIVE_LOCKS
				m_Locked = false;
			#endif

			LeaveCriticalSection(&m_CritSec);
		}
#endif

		//-----------------------------------------------------------------------------
#if defined(VMEM_OS_LINUX)
		VMEM_FORCE_INLINE VMEM_RELEASES_LOCK(this->m_CritSec) void Leave()
		{
			#if defined(VMEM_PLATFORM_PS4)
				scePthreadMutexUnlock(&m_Mutex);
			#else
				pthread_mutex_unlock(&cs);
			#endif
		}
#endif

		//-----------------------------------------------------------------------------
	private:
		CriticalSection(const CriticalSection&);
		CriticalSection& operator=(const CriticalSection&);

		//-----------------------------------------------------------------------------
		// data
	private:
#if defined(VMEM_OS_WIN) && defined(VMEM_USE_SLIM_READ_WRITE_LOCK)
		SRWLOCK m_CritSec;
#elif defined(VMEM_OS_WIN) && !defined(VMEM_USE_SLIM_READ_WRITE_LOCK)
		static const int m_SpinCount = 4000;
		CRITICAL_SECTION m_CritSec;
#elif defined(VMEM_PLATFORM_PS4)
		ScePthreadMutex m_Mutex;
#elif defined(VMEM_OS_LINUX)
		pthread_mutex_t cs;
#else
		#error platform not defined
#endif

#ifdef VMEM_CHECK_FOR_RECURSIVE_LOCKS
		bool m_Locked;
#endif
	};

	//-----------------------------------------------------------------------------
	class CriticalSectionScope
	{
	public:
		VMEM_FORCE_INLINE CriticalSectionScope(CriticalSection& critical_section) : m_CriticalSection(critical_section) { critical_section.Enter(); }
		VMEM_FORCE_INLINE ~CriticalSectionScope() { m_CriticalSection.Leave(); }

	private:
		CriticalSectionScope& operator=(const CriticalSectionScope& other);

		CriticalSection& m_CriticalSection;
	};

	//-----------------------------------------------------------------------------
	class ReadWriteLock
	{
	public:
		//-----------------------------------------------------------------------------
		ReadWriteLock()
		{
#if defined(VMEM_OS_WIN)
			InitializeSRWLock(&m_CritSec);
#endif
		}

		//-----------------------------------------------------------------------------
		VMEM_ACQUIRES_SHARED_LOCK(this->m_CritSec) void EnterShared()
		{
#if defined(VMEM_OS_WIN)
			AcquireSRWLockShared(&m_CritSec);
#else
			m_CritSec.Enter();
#endif
		}

		//-----------------------------------------------------------------------------
		VMEM_RELEASES_SHARED_LOCK(this->m_CritSec) void LeaveShared()
		{
#if defined(VMEM_OS_WIN)
			ReleaseSRWLockShared(&m_CritSec);
#else
			m_CritSec.Leave();
#endif
		}

		//-----------------------------------------------------------------------------
		VMEM_ACQUIRES_EXCLUSIVE_LOCK(this->m_CritSec) void EnterExclusive()
		{
#if defined(VMEM_OS_WIN)
			AcquireSRWLockExclusive(&m_CritSec);
#else
			m_CritSec.Enter();
#endif
		}

		//-----------------------------------------------------------------------------
		VMEM_RELEASES_EXCLUSIVE_LOCK(this->m_CritSec) void LeaveExclusive()
		{
#if defined(VMEM_OS_WIN)

			#ifdef VMEM_OS_WIN
				#pragma warning(push)
				#pragma warning(disable : 26110)	// disable release but not enter warning
			#endif

			ReleaseSRWLockExclusive(&m_CritSec);

			#ifdef VMEM_OS_WIN
				#pragma warning(pop)
			#endif
#else
			m_CritSec.Leave();
#endif
		}

		//-----------------------------------------------------------------------------
		// data
	private:
#if defined(VMEM_OS_WIN)
		SRWLOCK m_CritSec;
#else
		CriticalSection m_CritSec;
#endif
	};

	//-----------------------------------------------------------------------------
	class ReadLockScope
	{
	public:
		ReadLockScope(ReadWriteLock& read_write_lock)
		:	m_ReadWriteLock(read_write_lock)
		{
			read_write_lock.EnterShared();
		}

		~ReadLockScope()
		{
			m_ReadWriteLock.LeaveShared();
		}

	private:
		ReadLockScope(const ReadLockScope&);
		ReadLockScope& operator=(const ReadLockScope&);

	private:
		//-----------------------------------------------------------------------------
		// data
		ReadWriteLock& m_ReadWriteLock;
	};

	//-----------------------------------------------------------------------------
	class WriteLockScope
	{
	public:
		WriteLockScope(ReadWriteLock& read_write_lock)
		:	m_ReadWriteLock(read_write_lock)
		{
			read_write_lock.EnterExclusive();
		}

		~WriteLockScope()
		{
			m_ReadWriteLock.LeaveExclusive();
		}

	private:
		WriteLockScope(const WriteLockScope&);
		WriteLockScope& operator=(const WriteLockScope&);

	private:
		//-----------------------------------------------------------------------------
		// data
		ReadWriteLock& m_ReadWriteLock;
	};
}

//-----------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//-----------------------------------------------------------------------------
#endif		// #ifndef VMEM_CRITICALSECTION_H_INCLUDED

