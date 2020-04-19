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
#ifndef VMEM_LARGEHEAP_H_INCLUDED
#define VMEM_LARGEHEAP_H_INCLUDED

//------------------------------------------------------------------------
#include "VMemCore.hpp"
#include "VMemStats.hpp"
#include "BasicFSA.hpp"
#include "VMemCriticalSection.hpp"
#include "VMemHashMap.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	class VirtualMem;

	//------------------------------------------------------------------------
	class LargeHeap
	{
	public:
		LargeHeap(int reserve_flags, int commit_flags, VirtualMem& virtual_mem);

		~LargeHeap();

		bool Initialise();

		void* Alloc(size_t size);

		bool Free(void* p);

		size_t GetSize(void* p) const;

		bool Owns(void* p) const;

#ifdef VMEM_ENABLE_STATS
		Stats GetStats() const;
		Stats GetStatsNoLock() const;
		size_t WriteAllocs();
		void SendStatsToMemPro(MemProSendFn send_fn, void* p_context);
#endif

		void AcquireLockShared() const;

		void ReleaseLockShared() const;

	private:
		LargeHeap(const LargeHeap&);
		void operator=(const LargeHeap&);

		//------------------------------------------------------------------------
		// data
	private:
		mutable VMem::ReadWriteLock m_ReadWriteLock;

		HashMap<AddrKey, size_t> m_Allocs;

		int m_ReserveFlags;
		int m_CommitFlags;

		VirtualMem& m_VirtualMem;

#ifdef VMEM_ENABLE_STATS
		Stats m_Stats;
#endif
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_LARGEHEAP_H_INCLUDED

