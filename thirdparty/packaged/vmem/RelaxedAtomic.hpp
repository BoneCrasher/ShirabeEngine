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
#ifndef VMEM_RELAXEDATOMIC_H_INCLUDED
#define VMEM_RELAXEDATOMIC_H_INCLUDED

//------------------------------------------------------------------------
#include "VMemCore.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#if defined(VMEM_OS_WIN)
	#define VMEM_OS_SUPPORTS_ALIGNED_ATOMICS
#endif

//------------------------------------------------------------------------
#if defined(VMEM_PLATFORM_PS4)
	#define VMEM_OS_SUPPORTS_ALIGNED_ATOMICS
#endif

//------------------------------------------------------------------------
// std::atomic with relaxed memory ordering should do what we want, but
// on some windows platforms it is still adding in memory barriers! so we
// simply use aligned read/writes which on almost all platforms are atomic.
#ifdef VMEM_OS_SUPPORTS_ALIGNED_ATOMICS
	#ifdef VMEM_OS_WIN
		#include <intrin.h>
	#elif defined(VMEM_PLATFORM_PS4)
		#include <sce_atomic.h>
	#endif
#else
	#include <atomic>
#endif

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	template<typename T>
	class RelaxedAtomic64
	{
		VMEM_STATIC_ASSERT(sizeof(T) == sizeof(int64), "Template argument for RelaxedAtomic64 needs to be 8 bytes in size");

	public:
		RelaxedAtomic64()
		{
			VMEM_ASSERT((((uint64)this) & 7) == 0, "RelaxedAtomic misalignment");
		}

		RelaxedAtomic64(T value)
#ifdef VMEM_OS_SUPPORTS_ALIGNED_ATOMICS
		:	m_Value(value)
#endif
		{
#ifndef VMEM_OS_SUPPORTS_ALIGNED_ATOMICS
			m_Value.store(value, std::memory_order_relaxed);
#endif
			VMEM_ASSERT((((uint64)this) & 7) == 0, "RelaxedAtomic misalignment");
		}

		void operator=(T p)
		{
#ifdef VMEM_OS_SUPPORTS_ALIGNED_ATOMICS
			m_Value = p;
#else
			return m_Value.store(p, std::memory_order_relaxed);
#endif
		}

		operator T()
		{
#ifdef VMEM_OS_SUPPORTS_ALIGNED_ATOMICS
			return m_Value;
#else
			return m_Value.load(std::memory_order_relaxed);
#endif
		}

		VMEM_FORCE_INLINE bool CompareExchange(T expected, T new_value)
		{
#ifdef VMEM_OS_SUPPORTS_ALIGNED_ATOMICS
			#ifdef VMEM_OS_WIN
				return _InterlockedCompareExchange64((volatile __int64*)&m_Value, (__int64)new_value, (__int64)expected) == (__int64)expected;
			#elif defined(VMEM_PLATFORM_PS4)
				return sceAtomicCompareAndSwap64((volatile int64_t*)&m_Value, (int64_t)expected, (int64_t)new_value) == (int64_t)expected;
			#else
				#error		// CAS not implemented for OS
			#endif
#else
			return m_Value.compare_exchange_weak(expected, new_value, std::memory_order_release, std::memory_order_relaxed);
#endif
		}

		//------------------------------------------------------------------------
		// data
	private:
#ifdef VMEM_OS_SUPPORTS_ALIGNED_ATOMICS
		T m_Value;
#else
		std::atomic<T> m_Value;
#endif
	};

	//------------------------------------------------------------------------
	template<typename T>
	class RelaxedAtomic32
	{
		VMEM_STATIC_ASSERT(sizeof(T) == sizeof(int), "Template argument for RelaxedAtomic32 needs to be 4 bytes in size");

	public:
		RelaxedAtomic32()
		{
			VMEM_ASSERT((((uint64)this) & 3) == 0, "RelaxedAtomic misalignment");
		}

		RelaxedAtomic32(T value)
#ifdef VMEM_OS_SUPPORTS_ALIGNED_ATOMICS
		:	m_Value(value)
#endif
		{
#ifndef VMEM_OS_SUPPORTS_ALIGNED_ATOMICS
			m_Value.store(value, std::memory_order_relaxed);
#endif
			VMEM_ASSERT((((uint64)this) & 3) == 0, "RelaxedAtomic misalignment");
		}

		void operator=(T p)
		{
#ifdef VMEM_OS_SUPPORTS_ALIGNED_ATOMICS
			m_Value = p;
#else
			return m_Value.store(p, std::memory_order_relaxed);
#endif
		}

		operator T()
		{
#ifdef VMEM_OS_SUPPORTS_ALIGNED_ATOMICS
			return m_Value;
#else
			return m_Value.load(std::memory_order_relaxed);
#endif
		}

		VMEM_FORCE_INLINE bool CompareExchange(T expected, T new_value)
		{
#ifdef VMEM_OS_SUPPORTS_ALIGNED_ATOMICS
			#ifdef VMEM_OS_WIN
				return _InterlockedCompareExchange((volatile long*)&m_Value, (long)new_value, (long)expected) == (long)expected;
			#elif defined(VMEM_PLATFORM_PS4)
				return sceAtomicCompareAndSwap32((volatile int32_t*)&m_Value, (int32_t)expected, (int32_t)new_value) == (int32_t)expected;
			#else
				#error		// CAS not implemented for OS
			#endif
#else
			return m_Value.compare_exchange_weak(expected, new_value, std::memory_order_release, std::memory_order_relaxed);
#endif
		}

		//------------------------------------------------------------------------
		// data
	private:
#ifdef VMEM_OS_SUPPORTS_ALIGNED_ATOMICS
		T m_Value;		// std::atomic relaxed uses locks on some windows compilers, so we use raw aligned reads/writes
#else
		std::atomic<T> m_Value;
#endif
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_RELAXEDATOMIC_H_INCLUDED

