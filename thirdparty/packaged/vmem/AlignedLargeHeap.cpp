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

	VMem is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with VMem. If not, see <http://www.gnu.org/licenses/>.

	VMem can only be used in a commercial product if a commercial license has been purchased.
	Please see http://www.puredevsoftware.com/vmem/License.htm.
*/
//------------------------------------------------------------------------
#include "VMem_PCH.hpp"
#include "AlignedLargeHeap.hpp"
#include "VirtualMem.hpp"
#include "VMemMemProStats.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	AlignedLargeHeap::AlignedLargeHeap(int reserve_flags, int commit_flags, VirtualMem& virtual_mem)
	:	m_Allocs(virtual_mem.GetPageSize()),
		m_ReserveFlags(reserve_flags),
		m_CommitFlags(commit_flags),
		m_VirtualMem(virtual_mem)
	{
	}

	//------------------------------------------------------------------------
	AlignedLargeHeap::~AlignedLargeHeap()
	{
		if(m_Allocs.Initialised())
		{
			HashMap<AddrKey, LargeAlignedAlloc>::Iterator iter = m_Allocs.GetIterator();
			while(iter.MoveNext())
			{
				void* p = (void*)iter.GetKey().GetAddr();
				Free(p);
			}
		}
	}

	//------------------------------------------------------------------------
	bool AlignedLargeHeap::Initialise()
	{
		return m_Allocs.Initialise();
	}

	//------------------------------------------------------------------------
	void* AlignedLargeHeap::Alloc(size_t size, size_t alignment)
	{
		int page_size = m_VirtualMem.GetPageSize();

		size_t aligned_size = AlignSizeUpPow2(size + sizeof(AlignedHeader) + alignment, page_size);

		WriteLockScope lock(m_ReadWriteLock);

		void* p = m_VirtualMem.Reserve(aligned_size, page_size, m_ReserveFlags);
		if(!p)
		{
			return NULL;
		}

		if(!m_VirtualMem.Commit(p, aligned_size, m_CommitFlags))
		{
			m_VirtualMem.Release(p);
			return NULL;
		}

#ifdef VMEM_CUSTOM_ALLOC_INFO
		void* aligned_p = (byte*)AlignUp((byte*)p + sizeof(AlignedHeader) + sizeof(VMemCustomAllocInfo), alignment) - sizeof(VMemCustomAllocInfo);
#else
		void* aligned_p = AlignUp((byte*)p + sizeof(AlignedHeader), alignment);
#endif
		LargeAlignedAlloc alloc;
		alloc.m_Size = size;
		alloc.m_AlignedSize = aligned_size;
		if(!m_Allocs.Add(aligned_p, alloc))
		{
			m_VirtualMem.Decommit(p, aligned_size, m_CommitFlags);
			m_VirtualMem.Release(p);
			return NULL;		// Out of memory
		}

		AlignedHeader* p_header = (AlignedHeader*)aligned_p - 1;
		p_header->p = p;

		VMEM_MEMSET(aligned_p, VMEM_ALLOCATED_MEM, size);

		VMEM_STATS(m_Stats.m_Used += size);
		VMEM_STATS(m_Stats.m_Unused += aligned_size - size);
		VMEM_STATS(m_Stats.m_Reserved += aligned_size);
		#ifdef VMEM_CUSTOM_ALLOC_INFO
			VMEM_STATS(m_Stats.m_Used -= sizeof(VMemCustomAllocInfo));
			VMEM_STATS(m_Stats.m_Overhead += sizeof(VMemCustomAllocInfo));
		#endif

		return aligned_p;
	}

	//------------------------------------------------------------------------
	bool AlignedLargeHeap::Free(void* p)
	{
		WriteLockScope lock(m_ReadWriteLock);

		LargeAlignedAlloc alloc;
		AddrKey key(p);
		if(!m_Allocs.TryGetValue(key, alloc))
			return false;

		AlignedHeader* p_header = (AlignedHeader*)p - 1;
		void* aligned_p = p_header->p;

		m_VirtualMem.Decommit(aligned_p, alloc.m_AlignedSize, m_CommitFlags);
		m_VirtualMem.Release(aligned_p);

		#ifdef VMEM_CUSTOM_ALLOC_INFO
			VMEM_STATS(m_Stats.m_Used += sizeof(VMemCustomAllocInfo));
			VMEM_STATS(m_Stats.m_Overhead -= sizeof(VMemCustomAllocInfo));
		#endif
		VMEM_STATS(m_Stats.m_Used -= alloc.m_Size);
		VMEM_STATS(m_Stats.m_Unused -= alloc.m_AlignedSize - alloc.m_Size);
		VMEM_STATS(m_Stats.m_Reserved -= alloc.m_AlignedSize);

		m_Allocs.Remove(key);

		return true;
	}

	//------------------------------------------------------------------------
	size_t AlignedLargeHeap::GetSize(void* p) const
	{
		ReadLockScope lock(m_ReadWriteLock);

		LargeAlignedAlloc alloc;
		if(m_Allocs.TryGetValue(p, alloc))
			return alloc.m_Size;

		return VMEM_INVALID_SIZE;
	}

	//------------------------------------------------------------------------
	bool AlignedLargeHeap::Owns(void* p) const
	{
		ReadLockScope lock(m_ReadWriteLock);

		LargeAlignedAlloc alloc;
		return m_Allocs.TryGetValue(p, alloc);
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	size_t AlignedLargeHeap::WriteAllocs()
	{
		ReadLockScope lock(m_ReadWriteLock);

		DebugWrite(_T("-------------------------\n"));
		DebugWrite(_T("AlignedLargeHeap\n"));

		int written_allocs = 0;
		int alloc_count = 0;
		size_t allocated_bytes = 0;

		HashMap<AddrKey, LargeAlignedAlloc>::Iterator iter = m_Allocs.GetIterator();
		while(iter.MoveNext())
		{
			void* p = (void*)iter.GetKey().GetAddr();
			LargeAlignedAlloc alloc = iter.GetValue();

			if(written_allocs != VMEM_WRITE_ALLOCS_MAX)
			{
				WriteAlloc(p, alloc.m_Size);
				++written_allocs;
			}
			++alloc_count;
			allocated_bytes += alloc.m_Size;
		}

		if(written_allocs == VMEM_WRITE_ALLOCS_MAX)
			DebugWrite(_T("Only showing first %d allocations\n"), VMEM_WRITE_ALLOCS_MAX);

		DebugWrite(_T("%d bytes allocated across %d allocations\n"), allocated_bytes, alloc_count);

		return allocated_bytes;
	}
#endif

	//------------------------------------------------------------------------
	void AlignedLargeHeap::AcquireLockShared() const
	{
		m_ReadWriteLock.EnterShared();
	}

	//------------------------------------------------------------------------
	void AlignedLargeHeap::ReleaseLockShared() const
	{
		m_ReadWriteLock.LeaveShared();
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	void AlignedLargeHeap::SendStatsToMemPro(MemProSendFn send_fn, void* p_context)
	{
		ReadLockScope lock(m_ReadWriteLock);

		MemProStats::LageHeapStats stats;

		stats.m_AllocCount = 0;
		HashMap<AddrKey, LargeAlignedAlloc>::Iterator iter = m_Allocs.GetIterator();
		while(iter.MoveNext())
			++stats.m_AllocCount;

		stats.m_Stats = m_Stats;

		SendEnumToMemPro(vmem_LargeHeap, send_fn, p_context);
		SendToMemPro(stats, send_fn, p_context);
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	Stats AlignedLargeHeap::GetStats() const
	{
		ReadLockScope lock(m_ReadWriteLock);
		return GetStatsNoLock();
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	Stats AlignedLargeHeap::GetStatsNoLock() const
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

