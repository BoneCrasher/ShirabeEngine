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
#ifndef VMEM_INTERNALHEAP_H_INCLUDED
#define VMEM_INTERNALHEAP_H_INCLUDED

//------------------------------------------------------------------------
#include "VMemCore.hpp"
#include "VMemStats.hpp"
#include "BasicCoalesceHeap.hpp"
#include "VMemCriticalSection.hpp"
#include "RelaxedAtomic.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	class VirtualMem;

	//------------------------------------------------------------------------
	class InternalHeap
	{
	public:
		InternalHeap(VirtualMem& virtual_mem);

		VMEM_FORCE_INLINE void* Alloc(int size);

		VMEM_FORCE_INLINE void Free(void* p);

		VMEM_FORCE_INLINE void* Alloc_WithFalseSharingBuffer(int size);

		VMEM_FORCE_INLINE void Free_WithFalseSharingBuffer(void* p);

#ifdef VMEM_ENABLE_STATS
		inline const Stats& GetStats() const;
		inline const Stats& GetStatsNoLock() const;
		inline void SendStatsToMemPro(MemProSendFn send_fn, void* p_context);
#endif

		template<typename T>
		T* New()
		{
			T* p = (T*)Alloc(sizeof(T));
			if(p)
				new (p)T();
			return p;
		}

		template<typename T, typename TArg1, typename TArg2>
		T* NewRef2(TArg1 arg1, TArg2& arg2)
		{
			T* p = (T*)Alloc(sizeof(T));
			if(p)
				new (p)T(arg1, arg2);
			return p;
		}

		template<typename T, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6>
		T* New_WithFalseSharingBufferRef45(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4& arg4, TArg5& arg5, TArg6 arg6)
		{
			T* p = (T*)Alloc_WithFalseSharingBuffer(sizeof(T));
			if (p)
				new (p)T(arg1, arg2, arg3, arg4, arg5, arg6);
			return p;
		}

		template<typename T>
		void Delete(T* p)
		{
			p->~T();
			Free(p);
		}

		template<typename T>
		void Delete_WithFalseSharingBuffer(T* p)
		{
			p->~T();
			Free_WithFalseSharingBuffer((byte*)p);
		}

		static int GetFalseSharingBufferOverhead()
		{
			return 2 * m_FalseSharingBufferSize;
		}

	private:
		InternalHeap(const InternalHeap&);	// not implemented
		InternalHeap& operator=(const InternalHeap&);

#if VMEM_SIMULATE_OOM
		bool SimulateOOM();
#endif
		//------------------------------------------------------------------------
		// data
	private:
		static const int m_FalseSharingBufferSize = 128;

		mutable CriticalSection m_CriticalSection;

		BasicCoalesceHeap m_BasicCoalesceHeap;
	};

	//------------------------------------------------------------------------
	void* InternalHeap::Alloc(int size)
	{
		CriticalSectionScope lock(m_CriticalSection);

#if VMEM_SIMULATE_OOM
		if(SimulateOOM())
			return NULL;
#endif
		return m_BasicCoalesceHeap.Alloc(size);
	}

	//------------------------------------------------------------------------
	void InternalHeap::Free(void* p)
	{
		CriticalSectionScope lock(m_CriticalSection);
		m_BasicCoalesceHeap.Free(p);
	}

	//------------------------------------------------------------------------
	void* InternalHeap::Alloc_WithFalseSharingBuffer(int size)
	{
		CriticalSectionScope lock(m_CriticalSection);

#if VMEM_SIMULATE_OOM
		if(SimulateOOM())
			return NULL;
#endif
		void* p = m_BasicCoalesceHeap.Alloc(size + 2*m_FalseSharingBufferSize);
		if(!p)
			return NULL;

#ifdef VMEM_ENABLE_MEMSET
		memset(p, VMEM_GUARD_MEM, m_FalseSharingBufferSize);
		memset((byte*)p + m_FalseSharingBufferSize + size, VMEM_GUARD_MEM, m_FalseSharingBufferSize);
		VMEM_STATIC_ASSERT(m_FalseSharingBufferSize >= sizeof(int), "m_FalseSharingBufferSize must be larger than sizeof(int)");
		*(int*)p = size;
#endif
		return (byte*)p + m_FalseSharingBufferSize;
	}

	//------------------------------------------------------------------------
	void InternalHeap::Free_WithFalseSharingBuffer(void* p)
	{
		void* p_alloc = (byte*)p - m_FalseSharingBufferSize;

#ifdef VMEM_ENABLE_MEMSET
		int size = *(int*)p_alloc;
		CheckMemory((byte*)p_alloc + sizeof(int), m_FalseSharingBufferSize - sizeof(int), VMEM_GUARD_MEM);
		CheckMemory((byte*)p_alloc + m_FalseSharingBufferSize + size, m_FalseSharingBufferSize, VMEM_GUARD_MEM);
#endif
		CriticalSectionScope lock(m_CriticalSection);
		m_BasicCoalesceHeap.Free(p_alloc);
	}

	//------------------------------------------------------------------------
	#ifdef VMEM_ENABLE_STATS
	const Stats& InternalHeap::GetStats() const
	{
		CriticalSectionScope lock(m_CriticalSection);
		return m_BasicCoalesceHeap.GetStats();
	}
	#endif

	//------------------------------------------------------------------------
	#ifdef VMEM_ENABLE_STATS
	const Stats& InternalHeap::GetStatsNoLock() const
	{
		return m_BasicCoalesceHeap.GetStats();
	}
	#endif

	//------------------------------------------------------------------------
	#ifdef VMEM_ENABLE_STATS
	void InternalHeap::SendStatsToMemPro(MemProSendFn send_fn, void* p_context)
	{
		m_BasicCoalesceHeap.SendStatsToMemPro(send_fn, p_context);
	}
	#endif
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_INTERNALHEAP_H_INCLUDED

