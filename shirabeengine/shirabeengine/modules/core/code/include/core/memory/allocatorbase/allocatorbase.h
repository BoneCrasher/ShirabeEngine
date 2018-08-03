#ifndef __SHIRABE_CORE__MEMORY_ALLOCATOR_H__
#define __SHIRABE_CORE__MEMORY_ALLOCATOR_H__

#include <stddef.h>
#include <new>
#include <limits>

namespace engine {
	namespace Memory {


		// Allocator Base-class wrapping standard C/C++ memory allocation/deallocation but no de-/construction.
		class AllocatorBase {
		public:
			explicit AllocatorBase();

			// Allow overloading, since maybe a more specialized allocator
			// should be able to implement this basic behaviour!
			virtual ~AllocatorBase();

			virtual inline void* allocate(const size_t size);
			virtual inline void deallocate(void* p);

			// size
			inline size_t max_size() const;
		};
	}
}

#endif