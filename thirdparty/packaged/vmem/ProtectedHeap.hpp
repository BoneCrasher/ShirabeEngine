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
#ifndef VMEM_PROTECTED_HEAP_H_INCLUDED
#define VMEM_PROTECTED_HEAP_H_INCLUDED

//------------------------------------------------------------------------
#include "VMemCore.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#ifdef VMEM_PROTECTED_HEAP

#include "VMemCriticalSection.hpp"
#include "VMemHashMap.hpp"
#include "VMemStats.hpp"

#ifdef VMEM_OS_LINUX
	#include <pthread.h>
	#include <sys/time.h>
#endif

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	class ProtectedHeap
	{
	public:
		ProtectedHeap(int page_size, int reserve_flags, int commit_flags);
		
		~ProtectedHeap();

		void* Alloc(size_t size);

		bool Free(void* p);

		size_t GetSize(void* p) const;

		void CheckIntegrity();

		void Lock() const { m_CriticalSection.Enter(); }

		void Release() const { m_CriticalSection.Leave(); }

#ifdef VMEM_ENABLE_STATS
		Stats GetStats() const;
		Stats GetStatsNoLock() const;
#endif
	private:
		bool ReleaseOldestProtectedAlloc();

		bool AddUnreleasedPage(void* p_page, size_t size);

		void* AllocInternal(size_t size);

		ProtectedHeap& operator=(const ProtectedHeap&);

		//------------------------------------------------------------------------
		// data
	private:
		HashMap<AddrKey, size_t> m_Allocs;

		int m_PageSize;

		int m_ReserveFlags;
		int m_CommitFlags;

		mutable CriticalSection m_CriticalSection;

		int m_AllocIndex;

		size_t m_Overhead;

		struct UnreleasedAllocsPage
		{
			UnreleasedAllocsPage* mp_Prev;
			UnreleasedAllocsPage* mp_Next;
			int m_UsedCount;
			int m_ReleasedCount;
		};
		struct UnreleasedAlloc
		{
			void* mp_Page;
			size_t m_Size;
		};
		UnreleasedAllocsPage m_UnreleasedAllocsPageList;		// list of linked pages contains lists of unreleased memory allocs

#ifdef VMEM_ENABLE_STATS
		Stats m_Stats;
#endif
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_PROTECTED_HEAP

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_PROTECTED_HEAP_H_INCLUDED

