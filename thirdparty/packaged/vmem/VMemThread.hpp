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
#ifndef VMEM_VMEMTHREAD_H_INCLUDED
#define VMEM_VMEMTHREAD_H_INCLUDED

//------------------------------------------------------------------------
#include "VMemCore.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#ifdef VMEM_SERVICE_THREAD

#ifdef VMEM_OS_LINUX
	#include <pthread.h>
	#include <sys/time.h>
	#include <errno.h> 
#endif

//------------------------------------------------------------------------
#ifdef VMEM_OS_WIN
	#pragma warning(push)
	#pragma warning(disable : 4100)
#endif

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	class InternalHeap;

	//------------------------------------------------------------------------
	typedef int (*ThreadMain)(void*);

#if defined(VMEM_OS_WIN)
	typedef HANDLE ThreadHandle;
#elif defined(VMEM_PLATFORM_PS4)
	typedef ScePthread ThreadHandle;
#elif defined(VMEM_OS_LINUX)
	typedef pthread_t ThreadHandle;
#else
	#error	// OS not defined
#endif

	//------------------------------------------------------------------------
	namespace Thread
	{
		void Initialise();

		void Destroy();

		ThreadHandle CreateThread(ThreadMain p_thread_main, void* p_context);

		void TerminateThread(ThreadHandle thread);

		void WaitForThreadToTerminate(ThreadHandle thread);
	}

	//------------------------------------------------------------------------
	class Event
	{
	public:
		//------------------------------------------------------------------------
		Event(bool initial_state, bool auto_reset)
		{
#ifdef VMEM_OS_WIN
			m_Handle = CreateEvent(NULL, !auto_reset, initial_state, NULL);
#elif defined(VMEM_PLATFORM_PS4)
			scePthreadCondInit(&m_Cond, NULL, NULL);
			scePthreadMutexInit(&m_Mutex, NULL, NULL);
			m_Signalled = false;
			m_AutoReset = auto_reset;
	
			if(initial_state)
				Set();
#else
			pthread_cond_init(&m_Cond, NULL);
			pthread_mutex_init(&m_Mutex, NULL);
			m_Signalled = false;
			m_AutoReset = auto_reset;
	
			if(initial_state)
				Set();
#endif
		}

		//------------------------------------------------------------------------
		~Event()
		{
#ifdef VMEM_OS_WIN
			CloseHandle(m_Handle);
#elif defined(VMEM_PLATFORM_PS4)
			scePthreadMutexDestroy(&m_Mutex);
			scePthreadCondDestroy(&m_Cond);
#else
			pthread_mutex_destroy(&m_Mutex);
			pthread_cond_destroy(&m_Cond);
#endif
		}

		//------------------------------------------------------------------------
		void Set() const
		{
#ifdef VMEM_OS_WIN
			SetEvent(m_Handle);
#elif defined(VMEM_PLATFORM_PS4)
			scePthreadMutexLock(&m_Mutex);
			m_Signalled = true;
			scePthreadMutexUnlock(&m_Mutex);
			scePthreadCondSignal(&m_Cond);
#else
			pthread_mutex_lock(&m_Mutex);
			m_Signalled = true;
			pthread_mutex_unlock(&m_Mutex);
			pthread_cond_signal(&m_Cond);
#endif
		}

		//------------------------------------------------------------------------
		void Reset()
		{
#ifdef VMEM_OS_WIN
			ResetEvent(m_Handle);
#elif defined(VMEM_PLATFORM_PS4)
			scePthreadMutexLock(&m_Mutex);
			m_Signalled = false;
			scePthreadMutexUnlock(&m_Mutex);
#else
			pthread_mutex_lock(&m_Mutex);
			m_Signalled = false;
			pthread_mutex_unlock(&m_Mutex);
#endif
		}

		//------------------------------------------------------------------------
		int Wait(int timeout=-1) const
		{
#ifdef VMEM_OS_WIN
			VMEM_STATIC_ASSERT(INFINITE == -1, "INFINITE == -1");
			return WaitForSingleObject(m_Handle, timeout) == 0/*WAIT_OBJECT_0*/;
#elif defined(VMEM_PLATFORM_PS4)
			scePthreadMutexLock(&m_Mutex);
	
			if(m_Signalled)
			{
				m_Signalled = false;
				scePthreadMutexUnlock(&m_Mutex);
				return true;
			}
	
			if(timeout == -1)
			{
				while(!m_Signalled)
					scePthreadCondWait(&m_Cond, &m_Mutex);
		
				if(!m_AutoReset)
					m_Signalled = false;

				scePthreadMutexUnlock(&m_Mutex);
				return true;
			}
			else
			{		
				int ret = 0;
				do
				{
					ret = scePthreadCondTimedwait(&m_Cond, &m_Mutex, timeout * 1000);
				} while (!m_Signalled && ret != SCE_KERNEL_ERROR_ETIMEDOUT);

				if(ret != SCE_KERNEL_ERROR_ETIMEDOUT)
				{
					if(!m_AutoReset)
						m_Signalled = false;

					scePthreadMutexUnlock(&m_Mutex);
					return true;
				}
		
				scePthreadMutexUnlock(&m_Mutex);
				return false;
			}
#else
			pthread_mutex_lock(&m_Mutex);
	
			if(m_Signalled)
			{
				m_Signalled = false;
				pthread_mutex_unlock(&m_Mutex);
				return true;
			}
	
			if(timeout == -1)
			{
				while(!m_Signalled)
					pthread_cond_wait(&m_Cond, &m_Mutex);
		
				if(!m_AutoReset)
					m_Signalled = false;

				pthread_mutex_unlock(&m_Mutex);
		
				return true;
			}
			else
			{		
				timeval curr;
				gettimeofday(&curr, NULL);
		
				timespec time;
				time.tv_sec  = curr.tv_sec + timeout / 1000;
				time.tv_nsec = (curr.tv_usec * 1000) + ((timeout % 1000) * 1000000);
		
				time.tv_sec += time.tv_nsec / 1000000000L;
				time.tv_nsec = time.tv_nsec % 1000000000L;

				int ret = 0;
				do
				{
					ret = pthread_cond_timedwait(&m_Cond, &m_Mutex, &time);

				} while (!m_Signalled && ret != ETIMEDOUT);

				if(m_Signalled)
				{
					if(!m_AutoReset)
						m_Signalled = false;

					pthread_mutex_unlock(&m_Mutex);
					return true;
				}
		
				pthread_mutex_unlock(&m_Mutex);
				return false;
			}
#endif
		}

		//------------------------------------------------------------------------
		// data
	private:
#ifdef VMEM_OS_WIN
		HANDLE m_Handle;
#elif defined(VMEM_PLATFORM_PS4)
		mutable ScePthreadCond m_Cond;
		mutable ScePthreadMutex m_Mutex;
		mutable volatile bool m_Signalled;
		bool m_AutoReset;
#else
		mutable pthread_cond_t  m_Cond;
		mutable pthread_mutex_t m_Mutex;
		mutable volatile bool m_Signalled;
		bool m_AutoReset;
#endif
	};
}

//------------------------------------------------------------------------
#ifdef VMEM_OS_WIN
	#pragma warning(pop)
#endif

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_SERVICE_THREAD

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_VMEMTHREAD_H_INCLUDED

