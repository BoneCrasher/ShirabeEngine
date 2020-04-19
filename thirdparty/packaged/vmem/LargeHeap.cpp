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
#include "LargeHeap.hpp"
#include "VirtualMem.hpp"
#include "VMemMemProStats.hpp"
#include "VMemDefs.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	bool LargeHeap::Initialise()
	{
		return m_Allocs.Initialise();
	}

	//------------------------------------------------------------------------
	LargeHeap::LargeHeap(int reserve_flags, int commit_flags, VirtualMem& virtual_mem)
	:	m_Allocs(virtual_mem.GetPageSize()),
		m_ReserveFlags(reserve_flags),
		m_CommitFlags(commit_flags),
		m_VirtualMem(virtual_mem)
	{
	}

	//------------------------------------------------------------------------
	LargeHeap::~LargeHeap()
	{
		if(m_Allocs.Initialised())
		{
			HashMap<AddrKey, size_t>::Iterator iter = m_Allocs.GetIterator();
			while(iter.MoveNext())
			{
				void* p = (void*)iter.GetKey().GetAddr();
				Free(p);
			}
		}
	}

	//------------------------------------------------------------------------
	void* LargeHeap::Alloc(size_t size)
	{
		int page_size = m_VirtualMem.GetPageSize();
		size_t aligned_size = AlignSizeUpPow2(size, page_size);

		WriteLockScope lock(m_ReadWriteLock);

		void* p = m_VirtualMem.Reserve(aligned_size, page_size, m_ReserveFlags);
		if(!p)
			return NULL;

		if(!m_VirtualMem.Commit(p, aligned_size, m_CommitFlags))
		{
			m_VirtualMem.Release(p);
			return NULL;
		}

		if(!m_Allocs.Add(p, size))
		{
			m_VirtualMem.Decommit(p, aligned_size, m_CommitFlags);
			m_VirtualMem.Release(p);
			return NULL;		// Out of memory
		}

		VMEM_MEMSET(p, VMEM_ALLOCATED_MEM, aligned_size);

		VMEM_STATS(m_Stats.m_Used += size);
		VMEM_STATS(m_Stats.m_Unused += aligned_size - size);
		VMEM_STATS(m_Stats.m_Reserved += aligned_size);
		#ifdef VMEM_CUSTOM_ALLOC_INFO
			VMEM_STATS(m_Stats.m_Used -= sizeof(VMemCustomAllocInfo));
			VMEM_STATS(m_Stats.m_Overhead += sizeof(VMemCustomAllocInfo));
		#endif

		VMEM_ASSERT((((size_t)p) & (page_size-1)) == 0, "this allocator should align to page size");

		return p;
	}

	//------------------------------------------------------------------------
	bool LargeHeap::Free(void* p)
	{
		WriteLockScope lock(m_ReadWriteLock);

		size_t alloc_size = 0;
		AddrKey key(p);
		if(!m_Allocs.TryGetValue(key, alloc_size))
			return false;

		int page_size = m_VirtualMem.GetPageSize();
		size_t aligned_size = AlignSizeUpPow2(alloc_size, page_size);

		m_VirtualMem.Decommit(p, aligned_size, m_CommitFlags);
		m_VirtualMem.Release(p);

		#ifdef VMEM_CUSTOM_ALLOC_INFO
			VMEM_STATS(m_Stats.m_Used += sizeof(VMemCustomAllocInfo));
			VMEM_STATS(m_Stats.m_Overhead -= sizeof(VMemCustomAllocInfo));
		#endif
		VMEM_STATS(m_Stats.m_Used -= alloc_size);
		VMEM_STATS(m_Stats.m_Unused -= aligned_size - alloc_size);
		VMEM_STATS(m_Stats.m_Reserved -= aligned_size);

		m_Allocs.Remove(key);

		return true;
	}

	//------------------------------------------------------------------------
	size_t LargeHeap::GetSize(void* p) const
	{
		ReadLockScope lock(m_ReadWriteLock);

		size_t size = 0;
		if(m_Allocs.TryGetValue(p, size))
			return size;

		return VMEM_INVALID_SIZE;
	}

	//------------------------------------------------------------------------
	bool LargeHeap::Owns(void* p) const
	{
		ReadLockScope lock(m_ReadWriteLock);

		size_t size = 0;
		return m_Allocs.TryGetValue(p, size);
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	size_t LargeHeap::WriteAllocs()
	{
		ReadLockScope lock(m_ReadWriteLock);

		DebugWrite(_T("-------------------------\n"));
		DebugWrite(_T("LargeHeap\n"));

		int written_allocs = 0;
		int alloc_count = 0;
		size_t allocated_bytes = 0;

		HashMap<AddrKey, size_t>::Iterator iter = m_Allocs.GetIterator();
		while(iter.MoveNext())
		{
			void* p = (void*)iter.GetKey().GetAddr();
			size_t size = iter.GetValue();

			if(written_allocs != VMEM_WRITE_ALLOCS_MAX)
			{
				WriteAlloc(p, size);
				++written_allocs;
			}
			++alloc_count;
			allocated_bytes += size;
		}

		if(written_allocs == VMEM_WRITE_ALLOCS_MAX)
			DebugWrite(_T("Only showing first %d allocations\n"), VMEM_WRITE_ALLOCS_MAX);

		DebugWrite(_T("%d bytes allocated across %d allocations\n"), allocated_bytes, alloc_count);

		return allocated_bytes;
	}
#endif

	//------------------------------------------------------------------------
	void LargeHeap::AcquireLockShared() const
	{
		m_ReadWriteLock.EnterShared();
	}

	//------------------------------------------------------------------------
	void LargeHeap::ReleaseLockShared() const
	{
		m_ReadWriteLock.LeaveShared();
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	void LargeHeap::SendStatsToMemPro(MemProSendFn send_fn, void* p_context)
	{
		ReadLockScope lock(m_ReadWriteLock);

		MemProStats::LageHeapStats stats;

		stats.m_AllocCount = 0;
		HashMap<AddrKey, size_t>::Iterator iter = m_Allocs.GetIterator();
		while(iter.MoveNext())
			++stats.m_AllocCount;

		stats.m_Stats = m_Stats;

		SendEnumToMemPro(vmem_LargeHeap, send_fn, p_context);
		SendToMemPro(stats, send_fn, p_context);
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	Stats LargeHeap::GetStats() const
	{
		ReadLockScope lock(m_ReadWriteLock);
		return GetStatsNoLock();
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	Stats LargeHeap::GetStatsNoLock() const
	{
		int map_size = m_Allocs.GetAllocedMemory();

		Stats internal_stats;
		internal_stats.m_Reserved += map_size;
		internal_stats.m_Overhead += map_size;

		return m_Stats + internal_stats;
	}
#endif
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

