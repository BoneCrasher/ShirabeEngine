#ifndef __SHIRABE_CORE__MEMORY_ALLOCATOR__PREALLOCATING_ALLOCATOR_BASE_H__
#define __SHIRABE_CORE__MEMORY_ALLOCATOR__PREALLOCATING_ALLOCATOR_BASE_H__

#include "Memory/AllocatorBase/AllocatorBase.h"

namespace Engine
{
	namespace Memory
	{
		static const size_t PREALLOC_BYTE_SIZE = 256 * 1024 * sizeof(char);

		// Extends the default allocator base with preallocation and alignment functionality.
		// This facilitates specializing this class to stack, list and other allocator types.
		class PreallocatingAllocatorBase
			: public AllocatorBase
		{
		public:
			explicit PreallocatingAllocatorBase();
			virtual ~PreallocatingAllocatorBase();
			
			virtual void* allocate(size_t size) = 0;
			virtual void deallocate(void *ptr)  = 0;

			inline size_t capacity() const { return _capacity; }
			inline size_t committed() const { return _committed; }

		protected:
			virtual inline void __prealloc( size_t preallocSize);
			virtual inline void __grow( size_t requiredSize); // Don't forget to round to next alignment pos
			virtual inline void __shrink( size_t keepSize); // Don't forget to round to next alignment pos
			virtual inline void __free();

			virtual inline void * __align( void *pAddr,  size_t alignment);
			virtual inline size_t __getAlignmentOffset( void *pAddr,  size_t alignment);

			void *_pStart;
			size_t _committed;
			size_t _capacity;
		};
	}
}

#endif