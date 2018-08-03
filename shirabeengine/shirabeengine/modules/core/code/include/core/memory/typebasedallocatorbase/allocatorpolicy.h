#ifndef __SHIRABE_CORE_MEMORY_TYPE_BASED_ALLOCATOR_POLICY_BASE_H__
#define __SHIRABE_CORE_MEMORY_TYPE_BASED_ALLOCATOR_POLICY_BASE_H__

#include <stddef.h>


namespace engine {
	namespace Memory {

#undef new

		template<typename T>
		// An allocator policy dictates how allocation and deallocation happens. In addition it provides size limitations.
		class AllocatorPolicyBase {
		public:
			// Allocator typedefs mandatory due to STL-Std
			// Usually to be put into the allocator if no 
			// Traits and Policy separation was used!
			typedef T               value_type;
			typedef std::size_t     size_type;
			typedef std::ptrdiff_t  difference_type;
			typedef value_type*     pointer;
			typedef const pointer   const_pointer;
			typedef value_type&     reference;
			typedef const reference const_reference;

		public:
			// Convert an AllocatorPolicyBase<T> to AllocatorPolicyBase<U>
			template<typename U>
			struct rebind {
				typedef AllocatorPolicyBase<U> other;
			};

		public:
			explicit AllocatorPolicyBase() {}
			explicit AllocatorPolicyBase(AllocatorPolicyBase const&) {}
			template <typename U>
			explicit AllocatorPolicyBase(AllocatorPolicyBase<U> const&) {}

			~AllocatorPolicyBase() {}

			//    memory allocation
			inline pointer allocate(const size_type cnt,
									typename std::allocator<void>::const_pointer = 0)
			{
				// Route to standard new
				void *ptr = malloc(cnt * sizeof(T));
				if (ptr == 0)
					throw new bad_alloc();

				return reinterpret_cast<pointer>(ptr);
			}

			inline void deallocate(pointer p, size_type size = 0) {
				// Route to standard delete!
				if (p)
					free(p);
			}

			//    size
			inline size_type max_size() const {
				return std::numeric_limits<size_type>::max();
			}
		};

		// determines if memory from another
		// allocator can be deallocated from this one
		template<typename T, typename U>
		inline bool operator==(AllocatorPolicyBase<T> const&,
							   AllocatorPolicyBase<U> const&) {
			return true;
		};

		template<typename T, typename m_OtherAllocator>
		inline bool operator==(AllocatorPolicyBase<T> const&,
							   m_OtherAllocator const&) {
			return false;
		};

	}
}

#endif