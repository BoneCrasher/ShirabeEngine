#ifndef __SHIRABE_IRESOURCEDESCRIPTOR_H__
#define __SHIRABE_IRESOURCEDESCRIPTOR_H__

#include <iostream>
#include <sstream>
#include <string>

#include "Core/EngineTypeHelper.h"
#include "Resources/System/Core/EResourceType.h"

namespace Engine {
	namespace Resources {

		struct ResourceDescriptorBase {
		};

		DeclareListType(ResourceDescriptorBase, ResourceDescriptorBase);

		template <EResourceType type, EResourceSubType subtype>
		struct ResourceDescriptor
			: ResourceDescriptorBase {
			static const constexpr EResourceType    resource_type    = type;
			static const constexpr EResourceSubType resource_subtype = subtype;
		};

		template <EResourceType type, EResourceSubType subtype>
		DeclareListType(Template(ResourceDescriptor<type, subtype>), ResourceDescriptor);

		template <EResourceType type, EResourceSubType subtype>
		static inline std::ostream& operator <<(std::ostream& s, const ResourceDescriptor<type, subtype>& d) {
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
		template <EResourceType type, EResourceSubType subtype>
		class ResourceDescriptorAdapterBase {
		public:
			typedef
				typename ResourceDescriptor<type, subtype>::type
				descriptor_type;

			inline ResourceDescriptorAdapterBase(
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