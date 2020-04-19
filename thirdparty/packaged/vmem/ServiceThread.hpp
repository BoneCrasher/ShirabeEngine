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
#ifndef VMEM_SERVICETHREAD_H_INCLUDED
#define VMEM_SERVICETHREAD_H_INCLUDED

//------------------------------------------------------------------------
#include "VMemCore.hpp"
#include "VMemStats.hpp"
#include "VMemThread.hpp"

#ifdef VMEM_SERVICE_THREAD
	#include <atomic>
#endif

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#ifdef VMEM_SERVICE_THREAD

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	class ServiceThread
	{
	public:
		typedef void (*UpdateFn)();

		//------------------------------------------------------------------------
		ServiceThread(UpdateFn update_fn)
		:	m_UpdateFn(update_fn),
			m_ServiceThread(ThreadHandle()),
			m_Running(true),
			m_WakeEvent(false, true),
			m_ThreadFinishedEvent(false, false)
		{
		}

		//------------------------------------------------------------------------
		bool Initialise()
		{
			m_ServiceThread = Thread::CreateThread(ThreadMainStatic, this);

			if(!m_ServiceThread)
			{
				m_Running = false;
				m_ThreadFinishedEvent.Set();
				return false;
			}

			return true;
		}

		//------------------------------------------------------------------------
		static int ThreadMainStatic(void* p_context)
		{
			ServiceThread* p_this = (ServiceThread*)p_context;
			p_this->ThreadMain();
			return 0;
		}

		//------------------------------------------------------------------------
		void ThreadMain()
		{
			while(m_Running)
			{
				m_UpdateFn();

				m_WakeEvent.Wait(VMEM_SERVICE_THREAD_PERIOD);
			}

			m_ThreadFinishedEvent.Set();
		}

		//------------------------------------------------------------------------
		void Stop()
		{
			m_Running = false;
			m_WakeEvent.Set();
			m_ThreadFinishedEvent.Wait();
		}

		//------------------------------------------------------------------------
		// data
	private:
		UpdateFn m_UpdateFn;

		ThreadHandle m_ServiceThread;
		std::atomic<bool> m_Running;
		Event m_WakeEvent;
		Event m_ThreadFinishedEvent;
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_SERVICE_THREAD

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_SERVICETHREAD_H_INCLUDED

