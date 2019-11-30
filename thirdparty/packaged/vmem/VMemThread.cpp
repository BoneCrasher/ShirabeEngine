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
#include "VMemThread.hpp"
#include "VMemCriticalSection.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#ifdef VMEM_SERVICE_THREAD

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	struct ThreadGlobalData
	{
		CriticalSection m_CriticalSection;
		ThreadMain mp_ThreadMain;
		void* mp_Context;
	};
	char gp_ThreadGlobalDataMem[sizeof(ThreadGlobalData)] VMEM_ALIGN_8 = {0};
	ThreadGlobalData* gp_ThreadGlobalData = NULL;

	//------------------------------------------------------------------------
	void Thread::Initialise()
	{
		VMEM_ASSERT(!gp_ThreadGlobalData, "VMem not shut down correctly or gp_ThreadGlobalData has been corrupted");
		gp_ThreadGlobalData = (ThreadGlobalData*)gp_ThreadGlobalDataMem;
		new (gp_ThreadGlobalData)ThreadGlobalData();
	}

	//------------------------------------------------------------------------
	void Thread::Destroy()
	{
		gp_ThreadGlobalData->~ThreadGlobalData();
		gp_ThreadGlobalData = NULL;
	}

	//------------------------------------------------------------------------
#if defined(VMEM_OS_WIN)
	unsigned long WINAPI PlatformThreadMain(void*)
	{
		ThreadMain p_thread_main = gp_ThreadGlobalData->mp_ThreadMain;
		gp_ThreadGlobalData->mp_ThreadMain = NULL;

		void* p_context = gp_ThreadGlobalData->mp_Context;
		gp_ThreadGlobalData->mp_Context = NULL;

		gp_ThreadGlobalData->m_CriticalSection.Leave();

		return p_thread_main(p_context);
	}
#elif defined(VMEM_OS_LINUX)
	void* PlatformThreadMain(void*)
	{
		ThreadMain p_thread_main = gp_ThreadGlobalData->mp_ThreadMain;
		gp_ThreadGlobalData->mp_ThreadMain = NULL;

		void* p_context = gp_ThreadGlobalData->mp_Context;
		gp_ThreadGlobalData->mp_Context = NULL;

		gp_ThreadGlobalData->m_CriticalSection.Leave();

		p_thread_main(p_context);

		return NULL;
	}
#else
	#error	// OS not defined
#endif

	//------------------------------------------------------------------------
	ThreadHandle Thread::CreateThread(ThreadMain p_thread_main, void* p_context)
	{
		gp_ThreadGlobalData->m_CriticalSection.Enter();
		gp_ThreadGlobalData->mp_ThreadMain = p_thread_main;
		gp_ThreadGlobalData->mp_Context = p_context;

	#if defined(VMEM_OS_WIN)
		return ::CreateThread(NULL, 0, PlatformThreadMain, NULL, 0, NULL);
	#elif defined(VMEM_PLATFORM_PS4)
		ScePthread thread;
		scePthreadCreate(&thread, NULL, PlatformThreadMain, NULL, "VMem");
		return thread;
	#elif defined(VMEM_OS_LINUX)
		pthread_t thread;
		pthread_create(&thread, NULL, PlatformThreadMain, NULL);
		return thread;
	#else
		#error	// OS not defined
	#endif
	}

	//------------------------------------------------------------------------
	// only used in dire circumstances when VMem is trying to shut down after a crash (in the VMem tests)
	void Thread::TerminateThread(ThreadHandle thread)
	{
	#if defined(VMEM_PLATFORM_WIN)
		::TerminateThread(thread, 0);
	#elif defined(VMEM_PLATFORM_PS4)
		scePthreadCancel(thread);
	#elif defined(VMEM_OS_LINUX) && !defined(VMEM_PLATFORM_ANDROID)
		pthread_cancel(thread);
	#else
		Break(_T("pthread_cancel not implemented on this platform"));
	#endif

		WaitForThreadToTerminate(thread);
	}

	//------------------------------------------------------------------------
	void Thread::WaitForThreadToTerminate(ThreadHandle thread)
	{
	#if defined(VMEM_OS_WIN)
		WaitForSingleObject(thread, INFINITE);
	#elif defined(VMEM_PLATFORM_PS4)
		scePthreadJoin(thread, NULL);
	#elif defined(VMEM_OS_LINUX)
		pthread_join(thread, NULL);
	#else
		#error	// OS not defined
	#endif
	}
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_SERVICE_THREAD

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

