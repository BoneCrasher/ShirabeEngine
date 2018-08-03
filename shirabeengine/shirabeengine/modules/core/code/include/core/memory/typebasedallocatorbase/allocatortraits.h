#ifndef __SHIRABE_CORE_MEMORY_TYPE_BASE_ALLOCATOR_TRAITS_BASE_H__
#define __SHIRABE_CORE_MEMORY_TYPE_BASE_ALLOCATOR_TRAITS_BASE_H__
 
namespace engine {
	namespace Memory {

#ifdef construct
#undef construct
#endif 
#ifdef destruct
#undef destruct
#endif

		template<typename T>
		// The Allocator Traits dictate how objects are created/destroyed on allocated memory and provides
		// functionality to interact/analyze the allocated memory.
		class AllocatorTraitsBase {
		public:
			//    convert an AllocatorTraitsBase<T> to AllocatorTraitsBase<U>
			template<typename U>
			struct rebind {
				typedef AllocatorTraitsBase<U> other;
			};

		public:
			inline explicit AllocatorTraitsBase() {}
			inline ~AllocatorTraitsBase() {}
			template <typename U>
			inline explicit AllocatorTraitsBase(AllocatorTraitsBase<U> const&) {}

			//    address
			inline T* address(T& instance) { return &instance; }
			inline T const* address(T const& instance) { return &instance; }

			inline void construct(T* p, const typename T::param_type param, const typename T::value_type val)
			{
				new(p) T(param, val);
			}
			/* Copy construction */
			inline void construct(T* p, const T& instanceToCpy) { new(p) T(instanceToCpy); }
			inline void destruct(T* p) { p->~T(); }
		};

	}
}

#endif