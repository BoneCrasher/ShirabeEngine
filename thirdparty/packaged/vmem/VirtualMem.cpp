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
#include "VirtualMem.hpp"
#include "VMemCore.hpp"
#include "VMemThread.hpp"
#include "VMemCriticalSection.hpp"
#include "VMemHashMap.hpp"
#include "BasicFSA.hpp"
#include "RangeMap.hpp"
#include "List.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	VirtualMem::VirtualMem(int page_size)
	:	m_PageSize(page_size),
		m_Reservations(page_size)
#ifdef VMEM_ENABLE_STATS
		,m_ReservedOverhead(0)
#endif
#ifdef VMEM_DECOMMIT_CACHE
		,m_PendingDecommitBytes(0)
		,m_PendingReleaseBytes(0)
		,m_ReleaseNodePool(page_size)
		,m_DecommitNodePool(page_size)
		,m_RangeMapAllocator(page_size)
		,m_DecommitNodeMap(&m_RangeMapAllocator, page_size)
		,m_CurrentFrame(0)
#endif
	{
	}

	//------------------------------------------------------------------------
	VirtualMem::~VirtualMem()
	{
#ifdef VMEM_DECOMMIT_CACHE
		CriticalSectionScope lock(m_CriticalSection);
		FlushMemory(true);
#endif
	}

	//------------------------------------------------------------------------
#ifdef VMEM_DECOMMIT_CACHE
	int VirtualMem::GetReleaseNodeBucketIndex(size_t size)
	{
		return VMin((int)(size / (1024*1024)), m_ReleaseNodeBucketCount-1);
	}
#endif

	//------------------------------------------------------------------------
	void* VirtualMem::Reserve(size_t size, int align, int reserve_flags)
	{
		VMEM_ASSERT(size > 0 && (size & (m_PageSize-1)) == 0, "Invalid size passed to VirtualReserve");

		void* p = NULL;

		size_t aligned_size = size;
		bool needs_aligning = align != m_PageSize;

#ifdef VMEM_DECOMMIT_CACHE
		int bucket_index = GetReleaseNodeBucketIndex(size);
#endif
		CriticalSectionScope lock(m_CriticalSection);

#ifdef VMEM_DECOMMIT_CACHE
		// try ad find an existing released node in the release node buckets
		List<ReleaseNode>& node_list = m_ReleaseNodeBuckets[bucket_index];
		ReleaseNode* p_node_end = node_list.GetIterEnd();

		for(ReleaseNode* p_node=node_list.GetHead(); p_node!=p_node_end; p_node=p_node->mp_Next)
		{
			if(p_node->m_Size == size && p_node->m_Align == align && p_node->m_ReserveFlags == reserve_flags)
			{
				p = p_node->mp_Mem;
				node_list.Remove(p_node);
				aligned_size = p_node->m_AlignedSize;
				m_ReleaseNodePool.Free(p_node);
				m_PendingReleaseBytes -= size;
				break;
			}
		}
#endif
		// if we didn't find a pre-existing released node reserve from the OS
		if(!p)
		{
			if(needs_aligning)
			{
				VMEM_ASSERT((align & (m_PageSize-1)) == 0, "bad alignment");
				aligned_size = AlignSizeUpPow2(aligned_size + align-1, m_PageSize);
			}

			p = VMem::VirtualReserve(aligned_size, m_PageSize, reserve_flags);

			if(!p)
			{
#ifdef VMEM_DECOMMIT_CACHE
				FlushMemory(true);
				p = VMem::VirtualReserve(aligned_size, m_PageSize, reserve_flags);
				if(!p)
#endif
					return NULL;
			}

			VMEM_STATS(m_ReservedOverhead += aligned_size - size);
		}

		// record the virtual alloc and hash it based on the ALIGNED address
		Reservation reservation;
		reservation.mp_BaseAddr = p;
		reservation.m_Align = align;
		reservation.m_ReserveFlags = reserve_flags;
		reservation.m_Size = size;
		reservation.m_AlignedSize = aligned_size;

		if(needs_aligning)
			p = AlignUp((byte*)p, align);

		if(!m_Reservations.Add(p, reservation))
		{
			VMem::VirtualRelease(reservation.mp_BaseAddr, reservation.m_AlignedSize);
			VMEM_STATS(m_ReservedOverhead -= reservation.m_AlignedSize - reservation.m_Size);
			return NULL;		// out of memory
		}

		return p;
	}

	//------------------------------------------------------------------------
	void VirtualMem::Release(void* p)
	{
		CriticalSectionScope lock(m_CriticalSection);

		VMEM_ASSERT(p != NULL && ((size_t)p & (m_PageSize-1)) == 0, "Invalid address passed to VirtualRelease");

		Reservation reservation = m_Reservations.Remove(p);

#ifdef VMEM_DECOMMIT_CACHE
		p = reservation.mp_BaseAddr;

		size_t size = reservation.m_AlignedSize;
		VMEM_ASSERT(size > 0 && (size & (m_PageSize-1)) == 0, "Invalid size passed to VirtualRelease");

		int bucket_index = GetReleaseNodeBucketIndex(size);

		ReleaseNode* p_node = m_ReleaseNodePool.Alloc();
		if(!p_node)
		{
			// out of memory so release immediately
			DecommitNodesInRange(reservation.mp_BaseAddr, reservation.m_AlignedSize);
			VMem::VirtualRelease(reservation.mp_BaseAddr, reservation.m_AlignedSize);
			VMEM_STATS(m_ReservedOverhead -= reservation.m_AlignedSize - reservation.m_Size);
			return;
		}

		p_node->mp_Mem = p;
		p_node->m_Size = reservation.m_Size;
		p_node->m_AlignedSize = reservation.m_AlignedSize;
		p_node->m_Align = reservation.m_Align;
		p_node->m_ReserveFlags = reservation.m_ReserveFlags;
		p_node->mp_Next = NULL;
		p_node->mp_Prev = NULL;
		p_node->m_FlushFrame = m_CurrentFrame + VMEM_DECOMMIT_CACHE_RELEASE_FRAME_DELAY;

		m_ReleaseNodeBuckets[bucket_index].AddHead(p_node);

		m_PendingReleaseBytes += reservation.m_Size;
#else
		VMem::VirtualRelease(reservation.mp_BaseAddr, reservation.m_AlignedSize);
		VMEM_STATS(m_ReservedOverhead -= reservation.m_AlignedSize - reservation.m_Size);
#endif
	}

	//------------------------------------------------------------------------
	bool VirtualMem::Commit(void* p, size_t size, int commit_flags)
	{
#ifdef VMEM_DECOMMIT_CACHE
		CriticalSectionScope lock(m_CriticalSection);

		VMEM_ASSERT(p != NULL && ((size_t)p & (m_PageSize-1)) == 0, "Invalid address passed to VirtualCommit");
		VMEM_ASSERT(size >= 0 && (size & (m_PageSize-1)) == 0, "Invalid size passed to VirtualCommit");

		bool succeeded = TryCommit(p, size, commit_flags);
		if(!succeeded)
		{
			FlushMemory(true);
			succeeded = TryCommit(p, size, commit_flags);
		}

		if(succeeded)
		{
			VMEM_MEMSET(p, VMEM_COMMITTED_MEM, size);
			VMEM_ASSERT(VMem::Committed(p, size), "Commit succeeded but memory is not committed!");
		}

		return succeeded;
#else
		return VMem::VirtualCommit(p, size, m_PageSize, commit_flags);
#endif
	}

	//------------------------------------------------------------------------
#ifdef VMEM_DECOMMIT_CACHE
	void VirtualMem::DecommitNodesInRange(void* p, size_t size)
	{
		DecommitNode* p_node = m_DecommitNodeList.GetHead();
		DecommitNode* p_node_end = m_DecommitNodeList.GetIterEnd();
		while(p_node != p_node_end)
		{
			DecommitNode* p_next = p_node->mp_Next;

			// if there is any overlap decommit it
			if((byte*)p_node->mp_Mem + p_node->m_Size > p && p_node->mp_Mem < (byte*)p + size)
			{
				DecommitDecommitNode(p_node);
			}

			p_node = p_next;
		}
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_DECOMMIT_CACHE
	bool VirtualMem::TryCommit(void* p_commit, size_t commit_size, int commit_flags)
	{
		DecommitNode* p_decommit_node = NULL;

		// try and find a node that overlaps the commit range
		if (!m_DecommitNodeMap.TryRemoveRange(p_commit, commit_size, p_decommit_node))
		{
			// if we didn't find a node simply allocate from the OS and we are done
			return VMem::VirtualCommit(p_commit, commit_size, m_PageSize, commit_flags);
		}

		VMEM_ASSERT(p_decommit_node->m_CommitFlags == commit_flags, "can't re-commit with different physical flags");

		// handle the common case of re-committing exactly the same range
		if(p_decommit_node->mp_Mem == p_commit && p_decommit_node->m_Size == commit_size)
		{
			#ifdef VMEM_ENABLE_MEMSET
				CheckMemory(p_commit, commit_size, VMEM_DECOMMITTED_MEM);
			#endif

			m_DecommitNodeList.Remove(p_decommit_node);
			m_DecommitNodePool.Free(p_decommit_node);

			m_PendingDecommitBytes -= commit_size;

			return true;
		}
		else
		{
			// the decommit node is not exactly the same but overlaps, so we must take just the ranges that we need

			size_t committed_bytes = 0;
			void* p = p_commit;
			size_t total_bytes_to_commit = commit_size;

			// handle the case where p_commit is part way into the node
			if(p_decommit_node->mp_Mem < p)
			{
				#ifdef VMEM_ENABLE_MEMSET
					CheckMemory(p_decommit_node->mp_Mem, p_decommit_node->m_Size, VMEM_DECOMMITTED_MEM);
				#endif

				// only commit as much of the node as we need
				void* p_node_end = (byte*)p_decommit_node->mp_Mem + p_decommit_node->m_Size;
				size_t size_to_commit = VMem::VMin((size_t)((byte*)p_node_end - (byte*)p), total_bytes_to_commit);

				// reduce the size of the commit node
				p_decommit_node->m_Size = (byte*)p - (byte*)p_decommit_node->mp_Mem;
				p_decommit_node->m_CommitFlags = commit_flags;

				// re-add range to map
				DecommitNodeMap::Range range(p_decommit_node->mp_Mem, p, p_decommit_node);
				bool add_result = m_DecommitNodeMap.Add(range);
				VMEM_ASSERT(add_result, "add shouldn't fail because we've just removed an item");
				VMEM_UNREFERENCED_PARAM(add_result);

				VMEM_ASSERT(p_decommit_node->m_CommitFlags == commit_flags, "can't re-commit with different physical flags");

				p = (byte*)p + size_to_commit;
				total_bytes_to_commit -= size_to_commit;
				committed_bytes += size_to_commit;

				m_PendingDecommitBytes -= size_to_commit;

				// if we have some left over, create a new node
				if(p < p_node_end)
				{
					// allocate new node
					DecommitNode* p_new_node = m_DecommitNodePool.Alloc();
					
					size_t new_node_size = (byte*)p_node_end - (byte*)p;

					if(p_new_node)
					{
						// setup new node
						p_new_node->mp_Mem = p;
						p_new_node->m_Size = new_node_size;
						p_new_node->m_CommitFlags = commit_flags;
						p_new_node->m_FlushFrame = p_decommit_node->m_FlushFrame;
						VMEM_ASSERT_CODE(p_new_node->mp_Prev = p_new_node->mp_Next = NULL);
					
						// insert new node into list
						m_DecommitNodeList.Insert(p_decommit_node, p_new_node);

						// add new range to map
						DecommitNodeMap::Range offcut_range(p, (byte*)p + new_node_size, p_new_node);
						if(!m_DecommitNodeMap.Add(offcut_range))
						{
							m_DecommitNodeList.Remove(p_new_node);
							m_DecommitNodePool.Free(p_new_node);
							if(committed_bytes)
								VirtualMem::Decommit(p_commit, committed_bytes, commit_flags);
							return false;
						}

						VMEM_MEMSET(p_new_node->mp_Mem, VMEM_DECOMMITTED_MEM, p_new_node->m_Size);
					}
					else
					{
						// OOM so decommit the offcut range instead of adding it to the map
						VMem::VirtualDecommit(p, new_node_size, m_PageSize, commit_flags);
					}

					return true;
				}

				if(!total_bytes_to_commit)
					return true;

				// try and get the next range
				p_decommit_node = NULL;
				m_DecommitNodeMap.TryRemoveRange(p, total_bytes_to_commit, p_decommit_node);
			}

			// keep committing nodes and the gaps until we have committed enough
			while(total_bytes_to_commit && p_decommit_node)
			{
				#ifdef VMEM_ENABLE_MEMSET
					CheckMemory(p_decommit_node->mp_Mem, p_decommit_node->m_Size, VMEM_DECOMMITTED_MEM);
				#endif

				// if the node starts part way through the block commit the range before the node
				void* p_node_start = p_decommit_node->mp_Mem;
				if(p < p_node_start)
				{
					size_t size_to_commit = (byte*)p_node_start - (byte*)p;
					VMEM_ASSERT(size_to_commit < total_bytes_to_commit, "m_DecommitNodeMap has been corrupted");

					if(!VMem::VirtualCommit(p, size_to_commit, m_PageSize, commit_flags))
					{
						if(committed_bytes)
							VMem::VirtualDecommit(p_commit, committed_bytes, m_PageSize, commit_flags);
						bool add_result = m_DecommitNodeMap.Add(DecommitNodeMap::Range(p_decommit_node->mp_Mem, (byte*)p_decommit_node->mp_Mem + p_decommit_node->m_Size, p_decommit_node));
						VMEM_ASSERT(add_result, "add should never fail due to oom because we just removed an item");
						VMEM_UNREFERENCED_PARAM(add_result);
						return false;
					}

					p = (byte*)p + size_to_commit;
					total_bytes_to_commit -= size_to_commit;
					committed_bytes += size_to_commit;
				}

				VMEM_ASSERT(total_bytes_to_commit, "node doesn't overlap?");
				VMEM_ASSERT(p_decommit_node->mp_Mem == p, "Decommit node has been corrupted.");

				// commit as much of the node as we need
				size_t size_to_commit = VMem::VMin(p_decommit_node->m_Size, total_bytes_to_commit);

				p_decommit_node->mp_Mem = (byte*)p_decommit_node->mp_Mem + size_to_commit;
				p_decommit_node->m_Size = p_decommit_node->m_Size - size_to_commit;
				p_decommit_node->m_CommitFlags = commit_flags;

				VMEM_ASSERT(p_decommit_node->m_CommitFlags == commit_flags, "can't re-commit with different physical flags");

				p = (byte*)p + size_to_commit;
				total_bytes_to_commit -= size_to_commit;
				committed_bytes += size_to_commit;
				m_PendingDecommitBytes -= size_to_commit;

				if(p_decommit_node->m_Size)
				{
					DecommitNodeMap::Range range(p_decommit_node->mp_Mem, (byte*)p_decommit_node->mp_Mem + p_decommit_node->m_Size, p_decommit_node);
					bool add_result = m_DecommitNodeMap.Add(range);
					VMEM_ASSERT(add_result, "add should never fail due to oom because we just removed an item");
					VMEM_UNREFERENCED_PARAM(add_result);
				}
				else
				{
					m_DecommitNodeList.Remove(p_decommit_node);
					m_DecommitNodePool.Free(p_decommit_node);
				}
			
				if(!total_bytes_to_commit)
					return true;

				// try and get the next range
				p_decommit_node = NULL;
				m_DecommitNodeMap.TryRemoveRange(p, total_bytes_to_commit, p_decommit_node);
			}

			if(total_bytes_to_commit)
			{
				if(!VMem::VirtualCommit(p, total_bytes_to_commit, m_PageSize, commit_flags))
				{
					VirtualMem::Decommit(p_commit, committed_bytes, commit_flags);
					return false;
				}
			}

			return true;
		}
	}
#endif

	//------------------------------------------------------------------------
	void VirtualMem::Decommit(void* p, size_t size, int commit_flags)
	{
#ifdef VMEM_DECOMMIT_CACHE
		if(size > VMEM_DECOMMIT_CACHE_MAX_SIZE)
		{
			VMem::VirtualDecommit(p, size, m_PageSize, commit_flags);
		}
		else
		{
			CriticalSectionScope lock(m_CriticalSection);

			VMEM_ASSERT(p != NULL && ((size_t)p & (m_PageSize-1)) == 0, "Invalid address passed to VirtualDecommit");
			VMEM_ASSERT(size >= 0 && (size & (m_PageSize-1)) == 0, "Invalid size passed to VirtualDecommit");
			VMEM_ASSERT(VMem::Committed(p, size), "Can't decommit range because it is not committed");
	
			VMEM_MEMSET(p, VMEM_DECOMMITTED_MEM, size);

			DecommitNode* p_new_node = m_DecommitNodePool.Alloc();
			if(!p_new_node)
			{
				VMem::VirtualDecommit(p, size, m_PageSize, commit_flags);		// out of memory so decommit immediately
				return;
			}

			m_PendingDecommitBytes += size;

			p_new_node->mp_Mem = p;
			p_new_node->m_Size = size;
			p_new_node->m_CommitFlags = commit_flags;
			p_new_node->mp_Prev = NULL;
			p_new_node->mp_Next = NULL;
			p_new_node->m_FlushFrame = m_CurrentFrame + VMEM_DECOMMIT_CACHE_RELEASE_FRAME_DELAY;
			m_DecommitNodeList.AddHead(p_new_node);

			DecommitNodeMap::Range range(p, (byte*)p + size, p_new_node);
			if(!m_DecommitNodeMap.Add(range))
			{
				m_PendingDecommitBytes -= size;
				m_DecommitNodeList.Remove(p_new_node);
				m_DecommitNodePool.Free(p_new_node);
				VMem::VirtualDecommit(p, size, m_PageSize, commit_flags);		// out of memory so decommit immediately
				return;
			}

			VMEM_MEMSET(p_new_node->mp_Mem, VMEM_DECOMMITTED_MEM, p_new_node->m_Size);

			if(m_PendingDecommitBytes > VMEM_DECOMMIT_CACHE_SIZE)
				FlushDecommitCache();
		}
#else
		VMem::VirtualDecommit(p, size, m_PageSize, commit_flags);
#endif
	}

	//------------------------------------------------------------------------
	void VirtualMem::Flush()
	{
		CriticalSectionScope lock(m_CriticalSection);

#ifdef VMEM_DECOMMIT_CACHE
		FlushMemory(true);
#endif
	}

	//------------------------------------------------------------------------
	void VirtualMem::Update()
	{
	#ifdef VMEM_DECOMMIT_CACHE
		CriticalSectionScope lock(m_CriticalSection);

		FlushMemory(false);

		++m_CurrentFrame;
	#endif
	}

	//------------------------------------------------------------------------
#ifdef VMEM_DECOMMIT_CACHE
	void VirtualMem::FlushDecommitCache(bool force)
	{
		DecommitNode* p_node = m_DecommitNodeList.GetTail();
		DecommitNode* p_node_end = m_DecommitNodeList.GetIterEnd();
		
		while(p_node != p_node_end && (m_PendingDecommitBytes > VMEM_DECOMMIT_CACHE_SIZE || m_CurrentFrame >= p_node->m_FlushFrame || force))
		{
			DecommitNode* p_prev = p_node->mp_Prev;

			DecommitDecommitNode(p_node);

			p_node = p_prev;
		}
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_DECOMMIT_CACHE
	void VirtualMem::DecommitDecommitNode(DecommitNode* p_node)
	{
		size_t size = p_node->m_Size;

		VMem::VirtualDecommit(p_node->mp_Mem, size, m_PageSize, p_node->m_CommitFlags);

		m_PendingDecommitBytes -= size;

		DecommitNodeMap::Range range(p_node->mp_Mem, (byte*)p_node->mp_Mem + size, p_node);
		m_DecommitNodeMap.Remove(range);

		m_DecommitNodeList.Remove(p_node);
		m_DecommitNodePool.Free(p_node);
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_DECOMMIT_CACHE
	void VirtualMem::FlushMemory(bool force)
	{
		// if m_FlushFrame wraps around then all flush frames will be invalid, so simply flush everything
		if(!m_CurrentFrame)
			force = true;

		// must flush the decommit cache first so that we don't release while still committed
		FlushDecommitCache(force);

		FlushReleaseCache(force);
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_DECOMMIT_CACHE
	void VirtualMem::FlushReleaseCache(bool force)
	{
		for(int i=0; i<m_ReleaseNodeBucketCount; ++i)
		{
			List<ReleaseNode>& node_list = m_ReleaseNodeBuckets[i];
			ReleaseNode* p_node = node_list.GetTail();
			ReleaseNode* p_node_end = node_list.GetIterEnd();
			while(p_node != p_node_end && (m_CurrentFrame >= p_node->m_FlushFrame || force))
			{
				ReleaseNode* p_prev = p_node->mp_Prev;

				size_t size = p_node->m_Size;
				size_t aligned_size = p_node->m_AlignedSize;

				VMem::VirtualRelease(p_node->mp_Mem, aligned_size);

				m_PendingReleaseBytes -= size;
				VMEM_STATS(m_ReservedOverhead -= aligned_size - size);

				node_list.Remove(p_node);

				m_ReleaseNodePool.Free(p_node);

				p_node = p_prev;
			}
		}
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	Stats VirtualMem::GetStats()
	{
		CriticalSectionScope lock(m_CriticalSection);
		return GetStatsNoLock();
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	Stats VirtualMem::GetStatsNoLock() const
	{
		size_t allocs_overhead = m_Reservations.GetAllocedMemory();

		Stats stats;
		stats.m_Reserved = m_ReservedOverhead + allocs_overhead;
		stats.m_Overhead = allocs_overhead;

#ifdef VMEM_DECOMMIT_CACHE
		stats.m_Unused = m_PendingDecommitBytes;
		stats.m_Reserved += m_PendingReleaseBytes;
		stats += m_DecommitNodeMap.GetStats();
		stats += m_ReleaseNodePool.GetStats();
		stats += m_DecommitNodePool.GetStats();
#endif
		return stats;
	}
#endif

	//------------------------------------------------------------------------
	void VirtualMem::Lock() const
	{
		m_CriticalSection.Enter();
	}

	//------------------------------------------------------------------------
	void VirtualMem::Release() const
	{
		m_CriticalSection.Leave();
	}

	//------------------------------------------------------------------------
	void VirtualMem::CheckIntegrity()
	{
		{
			CriticalSectionScope lock(m_CriticalSection);

			#if defined(VMEM_ASSERTS) && defined(VMEM_DECOMMIT_CACHE) && defined(VMEM_ENABLE_MEMSET)
				DecommitNode* p_node = m_DecommitNodeList.GetHead();
				DecommitNode* p_end_node = m_DecommitNodeList.GetIterEnd();
				while (p_node != p_end_node)
				{
					CheckMemory(p_node->mp_Mem, p_node->m_Size, VMEM_DECOMMITTED_MEM);
					p_node = p_node->mp_Next;
				}
			#endif
		}

		VMemSysCheckIntegrity();
	}
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

