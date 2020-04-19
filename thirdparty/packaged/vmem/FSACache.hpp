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
#ifndef VMEM_FSACACHE_H_INCLUDED
#define VMEM_FSACACHE_H_INCLUDED

//------------------------------------------------------------------------
#include "VMemCore.hpp"
#include "InternalHeap.hpp"
#include "VMemStats.hpp"
#include "RelaxedAtomic.hpp"
#include <memory.h>
#include <new>

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#ifdef VMEM_FSA_CACHE

//------------------------------------------------------------------------
#define VMEM_USE_RELAXED_ATOMIC 1

//------------------------------------------------------------------------
#if !VMEM_USE_RELAXED_ATOMIC
	#include <atomic>
#endif

//------------------------------------------------------------------------
namespace VMem
{
#if VMEM_USE_RELAXED_ATOMIC
	//------------------------------------------------------------------------
	#ifdef VMEM_X64
		typedef RelaxedAtomic64<void*> FSACacheRelaxedAtomic;
	#else
		typedef RelaxedAtomic32<void*> FSACacheRelaxedAtomic;
	#endif

	//------------------------------------------------------------------------
	class FSACache
	{
	public:
		//------------------------------------------------------------------------
		FSACache(int count, InternalHeap& internal_heap)
		:	m_CountMask(count - 1),
			mp_Cache(NULL),
			m_AllocIndex(0),
			m_FreeIndex(0),
			m_InternalHeap(internal_heap),
			m_Count(count)
		{
			VMEM_ASSERT(IsPow2(count), "FSACache count not a power of 2");
		}

		//------------------------------------------------------------------------
		~FSACache()
		{
			if(mp_Cache)
				m_InternalHeap.Free(mp_Cache);
		}

		//------------------------------------------------------------------------
		bool Initialise()
		{
			mp_Cache = (FSACacheRelaxedAtomic*)m_InternalHeap.Alloc(m_Count*sizeof(FSACacheRelaxedAtomic));
			if(!mp_Cache)
				return false;

			for(int i=0; i<m_Count; ++i)
			{
				new (mp_Cache+i)FSACacheRelaxedAtomic();
				mp_Cache[i] = NULL;
			}

			return true;
		}

		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE void* Alloc()
		{
			int index = m_AllocIndex;
			void* p = mp_Cache[index];

			if(p && mp_Cache[index].CompareExchange(p, NULL))
			{
				m_AllocIndex = (index + 1) & m_CountMask;
				return p;
			}

			return NULL;
		}

		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE bool Free(void* p)
		{
			int index = m_FreeIndex;
			
			if(mp_Cache[index].CompareExchange(NULL, p))
			{
				m_FreeIndex = (index + 1) & m_CountMask;
				return true;
			}

			return false;
		}

		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE bool Set(void* p, int index)
		{
			VMEM_ASSERT(index >= 0 && index < m_Count, "index out of range");

			return mp_Cache[index].CompareExchange(NULL, p);
		}

		//------------------------------------------------------------------------
		int GetCount() const
		{
			return m_Count;
		}

		//------------------------------------------------------------------------
		// only used in the FSA destructor so doesn't need to be thread safe
		void* operator[](int index) const
		{
			VMEM_ASSERT(index >= 0 && index < m_Count, "FSACache index out of range");
			return mp_Cache[index];
		}

		//------------------------------------------------------------------------
		size_t GetMemoryOverhead() const
		{
			return m_Count * sizeof(FSACacheRelaxedAtomic);
		}

		//------------------------------------------------------------------------
		FSACache(const FSACache&);
		FSACache& operator=(const FSACache&);

		//------------------------------------------------------------------------
		// data
	private:
		const int m_CountMask;
		FSACacheRelaxedAtomic* mp_Cache;
		RelaxedAtomic32<int> m_AllocIndex;
		RelaxedAtomic32<int> m_FreeIndex;

		InternalHeap& m_InternalHeap;
		const int m_Count;
	};
#else
	//------------------------------------------------------------------------
	class FSACache
	{
	public:
		//------------------------------------------------------------------------
		FSACache(int count, InternalHeap& internal_heap)
		:	m_CountMask(count - 1),
			mp_Cache(NULL),
			m_AllocIndex(0),
			m_FreeIndex(0),
			m_Count(count),
			m_InternalHeap(internal_heap)
		{
			VMEM_ASSERT(IsPow2(count), "FSACache count not a power of 2");
		}

		//------------------------------------------------------------------------
		~FSACache()
		{
			if(mp_Cache)
				m_InternalHeap.Free(mp_Cache);
		}

		//------------------------------------------------------------------------
		bool Initialise()
		{
			mp_Cache = (std::atomic<void*>*)m_InternalHeap.Alloc(m_Count*sizeof(std::atomic<void*>));
			if(!mp_Cache)
				return false;
			
			for(int i=0; i<m_Count; ++i)
			{
				new (mp_Cache+i)std::atomic<void*>();
				mp_Cache[i] = NULL;
			}

			return true;
		}

		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE void* Alloc()
		{
			int index = m_AllocIndex.load(std::memory_order_relaxed);
			void* p = mp_Cache[index];

			if(p && mp_Cache[index].compare_exchange_weak(p, NULL, std::memory_order_relaxed, std::memory_order_relaxed))
			{
				m_AllocIndex.store((index + 1) & m_CountMask, std::memory_order_relaxed);
				return p;
			}

			return NULL;
		}

		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE bool Free(void* p)
		{
			int index = m_FreeIndex.load(std::memory_order_relaxed);
			
			void* p_null = NULL;
			if(mp_Cache[index].compare_exchange_weak(p_null, p, std::memory_order_relaxed, std::memory_order_relaxed))
			{
				m_FreeIndex.store((index + 1) & m_CountMask, std::memory_order_relaxed);
				return true;
			}

			return false;
		}

		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE bool Set(void* p, int index)
		{
			VMEM_ASSERT(index >= 0 && index < m_Count, "index out of range");

			void* p_null = NULL;
			return mp_Cache[index].compare_exchange_weak(p_null, p, std::memory_order_relaxed, std::memory_order_relaxed);
		}

		//------------------------------------------------------------------------
		int GetCount() const
		{
			return m_Count;
		}

		//------------------------------------------------------------------------
		// only used in the FSA destructor so doesn't need to be thread safe
		void* operator[](int index) const
		{
			VMEM_ASSERT(index >= 0 && index < m_Count, "FSACache index out of range");
			return mp_Cache[index];
		}

		//------------------------------------------------------------------------
		size_t GetMemoryOverhead() const
		{
			return m_Count * sizeof(std::atomic<void*>);
		}

		//------------------------------------------------------------------------
		// data
	private:
		const int m_CountMask;
		std::atomic<void*>* mp_Cache;
		std::atomic<int> m_AllocIndex;
		std::atomic<int> m_FreeIndex;

		InternalHeap& m_InternalHeap;
		const int m_Count;
	};
#endif
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_FSA_CACHE

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_FSACACHE_H_INCLUDED

