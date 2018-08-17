#ifndef __SHIRABE_CORE_MEMORY__MEMORY_POOL_H__
#define __SHIRABE_CORE_MEMORY__MEMORY_POOL_H__

#include "Memory/Memory.h"
#include "Memory/TypeBasedAllocatorBase/Allocator.h"

namespace ShirabeCORE {
	namespace Memory {

#if defined(SHIRABE_PLATFORM_WINDOWS)
#include <Windows.h>
#define Malloc m_aligned_malloc
#else 
#define Malloc memalign
#endif

		static const size_t SHIRABE_MEMORY_POOL__MAX_POOL_CHUNK_SIZE = 512;
		static const size_t SHIRABE_MEMORY_POOL__DEFAULT_ALIGNMENT   = SHIRABE_MEMORY_POOL__MAX_POOL_CHUNK_SIZE;

		static const size_t SHIRABE_MEM_POOLITIAL_DEFAULT_ELEMENT_COUNT        = 10;
		static const size_t SHIRABE_GLOBAL_MEM_POOLITIAL_DEFAULT_ELEMENT_COUNT = 10;

		// Calculates pow(2, ceil(log2(x)))
		inline size_t __2PowCeilLog2x(const size_t byte_size)
		{
			size_t pow = 1;
			while (pow < byte_size)
				pow <<= 1;

			return pow;
		}

#pragma region MemoryPoolBase<T>

		// The MemoryPoolBase stores information about the pool size and provides stubs
		// for derivates to implement basic pool behaviour.
		template <typename T, size_t chunk_size = sizeof(T)>
		class MemoryPoolBase {
		public:
			typedef typename MemoryPoolBase<T> m_MyT;

		public:
			MemoryPoolBase() : __size(0), __pStart(NULL), __pEnd(NULL)
			{
			}

			virtual ~MemoryPoolBase()
			{
			};

			virtual inline typename T* Allocate(const size_t size)       = 0;
			virtual inline void        Deallocate(const typename T* ptr) = 0;

		protected:
			virtual inline bool __AllocatePool(size_t size) = 0;
			virtual inline bool __GrowPool(size_t factor)   = 0;
			virtual inline bool __ShrinkPool(size_t factor) = 0;
			virtual inline bool __DeallocatePool() = 0;

		protected:
			// Store a pointer to the first element in the memory range
			// retrieved from the memory manager
			static typename T* __pStart;

			// Store a pointer to the last element in the memory range
			// retrieved from the memory manager
			static typename T* __pEnd;

			// The current size of the pool in bytes.
			// Do not confuse with chuck_size.
			static typename size_t __size;
		};

#pragma endregion

#pragma region ListMemoryPool<T>

		// The ListMemoryPool implements the MemoryPoolBase in a way that allocated memory is managed in a 
		// linked list based on different list modes, i.e. stack, queue, etc.
		template <typename T>
		class ListMemoryPool : public MemoryPoolBase<T> {
		public:
			ListMemoryPool<T>() : MemoryPoolBase<T>
			{
			};

			virtual ~ListMemoryPool<T>()
			{
			};

			virtual inline T* Allocate(size_t size)
			{

			};

			virtual inline void Deallocate(T *ptr)
			{

			};
		private:
			virtual inline bool __AllocatePool(size_t size)
			{
				char *pMem = (char *)Malloc(size, SHIRABE_MEMORY_POOL__DEFAULT_ALIGNMENT);
				if (pMem == 0)
					throw new std::bad_alloc();

				__size = size;

				__pStart = pMem;
				__pEnd = __pStart + size - sizeof(char);
			};

			virtual inline bool __GrowPool(size_t factor)
			{

			};

			virtual inline bool __ShrinkPool(size_t factor)
			{

			};

			virtual inline bool __DeallocatePool()
			{
				free(__pStart);

				__pStart = __pEnd = 0;
				__size = 0;
			};
		};

#pragma endregion

		class GlobalMemoryPool : public ListMemoryPool<char> {
		public:
			static GlobalMemoryPool *getInstance();

			~GlobalMemoryPool()
			{};

			virtual inline char* Allocate(size_t size)
			{

			};

			virtual inline void Deallocate(char *ptr)
			{

			};


		private:
			GlobalMemoryPool() : ListMemoryPool<char>()
			{
				if (!__AllocatePool())
				{
					// Error allocating the pool
				}
			};

			virtual inline bool __AllocatePool(size_t size = SHIRABE_GLOBAL_MEM_POOLITIAL_DEFAULT_ELEMENT_COUNT * sizeof(char))
			{
				char *pMem = (char *)malloc(size);
				if (pMem == 0)
					throw new std::bad_alloc();

				__size = size;

				__pStart = pMem;
				__pEnd = __pStart + size - sizeof(char);
			};

			virtual inline bool __GrowPool(size_t factor)
			{

			};

			virtual inline bool __ShrinkPool(size_t factor)
			{

			};

			virtual inline bool __DeallocatePool()
			{
				free(__pStart);

				__pStart = __pEnd = 0;
				__size = 0;
			};

		private:
			static GlobalMemoryPool * m_pInstance;
		};
	}
}

#endif