#ifndef __SHIRABE_IRESOURCEDESCRIPTOR_H__
#define __SHIRABE_IRESOURCEDESCRIPTOR_H__

#include <iostream>
#include <sstream>
#include <string>

#include "Core/EngineTypeHelper.h"
#include "Resources/System/Core/EResourceType.h"

namespace Engine {
	namespace Resources {
		
		template <typename TResource>
		struct ResourceDescriptor
			: public TResource::descriptor_impl_type 
		{
			static const constexpr EResourceType    resource_type    = TResource::resource_type;
			static const constexpr EResourceSubType resource_subtype = TResource::resource_subtype;

			typedef typename TResource::descriptor_impl_type descriptor_type;
		};
		
		template <typename TResource>
		static inline std::ostream& operator <<(std::ostream& s, const ResourceDescriptor<TResource>& d) {
			return (s << d.toString());
		}

		/**********************************************************************************************//**
		 * \class	ResourceDescriptorAdapterBase
		 *
		 * \brief	A resource descriptor adapter base.
		 *
		 * \tparam	type   	Type of the type.
		 * \tparam	subtype	Type of the subtype.
		 **************************************************************************************************/
		template <typename TResource>
		class ResourceDescriptorAdapter {
		public:
			typedef typename ResourceDescriptor<TResource>::descriptor_type descriptor_type;

			inline ResourceDescriptorAdapter(
				const descriptor_type& descriptor
			) : _descriptor(descriptor)
			{}

			inline const descriptor_type&
				descriptor() const { return _descriptor; }


		private:
			descriptor_type _descriptor;
		};
	}
}

#endif