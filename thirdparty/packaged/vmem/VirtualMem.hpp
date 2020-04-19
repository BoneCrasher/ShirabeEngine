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
#ifndef VMEM_VIRTUALMEM_H_INCLUDED
#define VMEM_VIRTUALMEM_H_INCLUDED

//------------------------------------------------------------------------
#include "VMemCore.hpp"
#include "VMemStats.hpp"
#include "VMemCriticalSection.hpp"
#include "VMemHashMap.hpp"
#include "List.hpp"
#include "BasicFSA.hpp"
#include "RangeMap.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	struct Reservation
	{
		size_t m_Size;
		size_t m_AlignedSize;
		void* mp_BaseAddr;
		int m_Align;
		int m_ReserveFlags;
	};

	//------------------------------------------------------------------------
	struct DecommitNode
	{
		DecommitNode* mp_Prev;
		DecommitNode* mp_Next;

		void* mp_Mem;
		size_t m_Size;

		int m_CommitFlags;

		unsigned int m_FlushFrame;
	};

	//------------------------------------------------------------------------
	struct ReleaseNode
	{
		size_t m_Size;
		size_t m_AlignedSize;
		void* mp_Mem;
		ReleaseNode* mp_Prev;
		ReleaseNode* mp_Next;
		int m_Align;
		int m_ReserveFlags;
		unsigned int m_FlushFrame;
	};

	//------------------------------------------------------------------------
	class RangeMapAllocator : public InternalAllocator
	{
	public:
		RangeMapAllocator(int page_size)
		:	m_PageSize(page_size)
		{
		}

		void* Alloc(size_t size)
		{
			void* p = VMem::VirtualReserve(size, m_PageSize, VMEM_DEFAULT_RESERVE_FLAGS);

			if (p)
			{
				if (!VMem::VirtualCommit(p, size, m_PageSize, VMEM_DEFAULT_COMMIT_FLAGS))
				{
					VMem::VirtualRelease(p, size);
					p = NULL;
				}
			}

			return p;
		}

		void Free(void* p, size_t size)
		{
			VMem::VirtualDecommit(p, size, m_PageSize, VMEM_DEFAULT_COMMIT_FLAGS);
			VMem::VirtualRelease(p, size);
		}

		//------------------------------------------------------------------------
		// data
	private:
		int m_PageSize;
	};

	//------------------------------------------------------------------------
	class VirtualMem
	{
		typedef RangeMap<struct DecommitNode*> DecommitNodeMap;

	public:
		VirtualMem(int page_size);

		~VirtualMem();

		void Flush();

		void Update();

		int GetPageSize() const { return m_PageSize; }

		void* Reserve(size_t size, int align, int reserve_flags);

		void Release(void* p);

		bool Commit(void* p, size_t size, int commit_flags);

		void Decommit(void* p, size_t size, int commit_flags);

#ifdef VMEM_ENABLE_STATS
		Stats GetStats();
		Stats GetStatsNoLock() const;
#endif
		void Lock() const;

		void Release() const;

		void CheckIntegrity();

	private:
#ifdef VMEM_DECOMMIT_CACHE
		VMEM_FORCE_INLINE int GetReleaseNodeBucketIndex(size_t size);

		void DecommitNodesInRange(void* p, size_t size);

		bool TryCommit(void* p_commit, size_t commit_size, int commit_flags);

		void DecommitDecommitNode(DecommitNode* p_node);

		void FlushMemory(bool force);

		void FlushReleaseCache(bool force);

		void FlushDecommitCache(bool force = false);
#endif
		VirtualMem(const VirtualMem&);
		void operator=(const VirtualMem&);

		//------------------------------------------------------------------------
		// data
	private:
		mutable CriticalSection m_CriticalSection;

		int m_PageSize;

		HashMap<AddrKey, Reservation> m_Reservations;

#ifdef VMEM_ENABLE_STATS
		size_t m_ReservedOverhead;
#endif

#ifdef VMEM_DECOMMIT_CACHE
		static const int m_ReleaseNodeBucketCount = 256;

		List<ReleaseNode> m_ReleaseNodeBuckets[m_ReleaseNodeBucketCount];		// nodes are sorted by age (newest at head)

		size_t m_PendingDecommitBytes;
		size_t m_PendingReleaseBytes;

		BasicFSA<ReleaseNode> m_ReleaseNodePool;
		BasicFSA<DecommitNode> m_DecommitNodePool;

		RangeMapAllocator m_RangeMapAllocator;

		DecommitNodeMap m_DecommitNodeMap;

		List<DecommitNode> m_DecommitNodeList;		// sorted newest to oldest

		unsigned int m_CurrentFrame;
#endif
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_VIRTUALMEM_H_INCLUDED

