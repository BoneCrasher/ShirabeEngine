#ifndef __SHIRABE_RESOURCE_BINDING_H__
#define __SHIRABE_RESOURCE_BINDING_H__

#include "Resources/System/Core/Handle.h"

namespace Engine {
	namespace Resources {

		template <typename TResource>
		struct ResourceBinding
			: public TResource::binding_type
		{};
		
		template <typename TResource>
		class ResourceBindingAdapter {
		public:
			typedef
				typename TResource::binding_type binding_type;

			inline ResourceBindingAdapter(
				const binding_type& binding
			) : _binding(binding)
			{}

			inline const binding_type&
				binding() const { return _binding; }

		private:
			binding_type _binding;
		};

	}
}

#endif