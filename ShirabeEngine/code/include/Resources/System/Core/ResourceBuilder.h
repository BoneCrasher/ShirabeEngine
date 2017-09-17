#ifndef __SHIRABE_IRESOURCEBUILDER_H__
#define __SHIRABE_IRESOURCEBUILDER_H__

#include <exception>
#include <sstream>

#include "Core/EngineTypeHelper.h"
#include "Core/EngineStatus.h"

#include "Resources/IResourceDescriptor.h"

namespace Engine {
	namespace Resources {

		struct ResourceNotSupportedException 
			: public std::exception
		{
			ResourceNotSupportedException() throw()
				: exception()
			{ }

			explicit ResourceNotSupportedException(char const* const message) throw()
				: exception(message)
			{ }

			ResourceNotSupportedException(char const* const message, int k) throw()
				: exception(message, k)
			{ }

			ResourceNotSupportedException(ResourceNotSupportedException const& other) throw()
				: exception(other)
			{ }

			ResourceNotSupportedException& operator=(ResourceNotSupportedException const& other) throw() {
				exception::operator=((std::exception) other);
				return *this;
			}

			virtual ~ResourceNotSupportedException() throw() {
			}

			virtual char const* what() const {
				return exception::what();
			}
		};

		template <
			typename         TGAPIDevice,
			EResourceType    type,
			EResourceSubType subtype,
			typename         TResource
		>
		struct ResourceBuilderTraits {
			static const EResourceType    resource_type    = type;
			static const EResourceSubType resource_subtype = subtype;

			typedef 
				typename ResourceDescriptor<resource_type, resource_subtype>  
				descriptor_type;
			typedef
				TGAPIDevice
				gapi_device_type;

			DeclareSharedPointerType(TGAPIDevice);
			using resource_type_ptr = std::shared_ptr<TResource>;
		};

		template <
			typename         TGAPIDevice,
			EResourceType    type,
			EResourceSubType subtype,
			typename         TResource,
			typename         Traits  = ResourceBuilderTraits<TGAPIDevice, type, subtype, TResource>
		>
		class ResourceBuilderBase
		{
		public:
			typedef ResourceBuilderBase<TGAPIDevice, type, subtype, TResource, Traits>    my_type;
			typedef typename Traits traits_type;

			static const EResourceType    resource_type    = traits_type::resource_type;
			static const EResourceSubType resource_subtype = traits_type::resource_subtype;
			typedef typename traits_type::descriptor_type   descriptor_type;
			typedef typename traits_type::gapi_device_type  gapi_device_type;
			typedef typename traits_type::resource_type_ptr resource_type_ptr;
		};

	}
}

#endif