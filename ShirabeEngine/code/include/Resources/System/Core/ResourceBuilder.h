#ifndef __SHIRABE_IRESOURCEBUILDER_H__
#define __SHIRABE_IRESOURCEBUILDER_H__

#include <exception>
#include <sstream>

#include "Core/EngineTypeHelper.h"
#include "Core/EngineStatus.h"

#include "Resources/System/Core/IResourceDescriptor.h"

namespace Engine {
	namespace Resources {

		/**********************************************************************************************//**
		 * \struct	ResourceNotSupportedException
		 *
		 * \brief	Exception for signalling resource not supported errors.
		 **************************************************************************************************/
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

		/**********************************************************************************************//**
		 * \struct	ResourceBuilderTraits
		 *
		 * \brief	Provides information about the specific resource type supported by a 
		 * 			corresponding builder.
		 *
		 * \tparam	TGAPIDevice	Type of the tgapi device.
		 * \tparam	type	   	Type of the type.
		 * \tparam	subtype	   	Type of the subtype.
		 * \tparam	TResource  	Type of the resource.
		 **************************************************************************************************/
		template <
			typename         TGAPIDevice,
			EResourceType    type,
			EResourceSubType subtype,
			typename         TResourceBasePtr
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

			typedef std::map<ResourceHandle, TResourceBasePtr> built_resource_map;
		};

		/**********************************************************************************************//**
		 * \class	ResourceBuilderBase
		 *
		 * \brief	Base-class for any resource builder, encapsulating a traits-class.
		 *
		 * \tparam	TGAPIDevice	Type of the tgapi device.
		 * \tparam	type	   	Type of the type.
		 * \tparam	subtype	   	Type of the subtype.
		 * \tparam	TResource  	Type of the resource.
		 * \tparam	TGAPIDevice	Type of the tgapi device.
		 * \tparam	type	   	Type of the type.
		 * \tparam	subtype	   	Type of the subtype.
		 * \tparam	TResource  	Type of the resource.
		 **************************************************************************************************/
		template <
			typename         TGAPIDevice,
			EResourceType    type,
			EResourceSubType subtype,
			typename         TResourceBasePtr,
			typename         Traits  = ResourceBuilderTraits<TGAPIDevice, type, subtype, TResourceBasePtr>
		>
		class ResourceBuilderBase
		{
		public:
			typedef ResourceBuilderBase<TGAPIDevice, type, subtype, TResource, Traits> my_type;
			typedef typename Traits traits_type;

			static const EResourceType    resource_type    = traits_type::resource_type;
			static const EResourceSubType resource_subtype = traits_type::resource_subtype;
			typedef typename traits_type::descriptor_type    descriptor_type;
			typedef typename traits_type::gapi_device_type   gapi_device_type;
			typedef typename traits_type::built_resource_map built_resource_map;
		};

	}
}

#endif