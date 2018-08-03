#ifndef __SHIRABE_CORE_MEMORY__STL_TYPE_BASE_H__
#define __SHIRABE_CORE_MEMORY__STL_TYPE_BASE_H__
		
namespace engine {
	namespace Memory {
		template <typename T>
		class STLTypeBase {
			// Typedefs mandatory due to STL-Std.
			// Base definitions in Policy, override for convenience.
			typedef typename size_t          size_type;
			typedef typename size_t          difference_type;
			typedef typename T*              pointer;
			typedef typename const pointer   const_pointer;
			typedef typename T&              reference;
			typedef typename const reference const_reference;
			typedef typename T               value_type;
		};
	}
}

#endif