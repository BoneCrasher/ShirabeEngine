#ifndef __SHIRABE_IRESOURCEBUILDER_H__
#define __SHIRABE_IRESOURCEBUILDER_H__

#include <map>
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
			typename TResource,
			typename TGFXAPIParametersStruct,
			typename TResourceBasePtr
		>
		struct ResourceBuilderTraits {
			static const EResourceType    resource_type    = TResource::resource_type;
			static const EResourceSubType resource_subtype = TResource::resource_subtype;

			typedef 
				ResourceDescriptor<TResource>  
				descriptor_type;

			typedef
				TGFXAPIParametersStruct
				gfxapi_parameter_struct_type;

			typedef std::map<ResourceHandle, TResourceBasePtr> built_resource_map;
		};

		/**********************************************************************************************//**
		 * \class	ResourceXMMATRIX
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
			typename TResource,
			typename TGFXAPIParametersStruct,
			typename TResourceBasePtr,
			typename Traits  = ResourceBuilderTraits<TResource, TGFXAPIParametersStruct, TResourceBasePtr>
		>
		class ResourceBuilderBase
		{
		public:
			typedef ResourceBuilderBase<TResource, TGFXAPIParametersStruct, TResourceBasePtr, Traits> my_type;
			typedef typename Traits traits_type;

			static const EResourceType    resource_type    = traits_type::resource_type;
			static const EResourceSubType resource_subtype = traits_type::resource_subtype;

			typedef typename traits_type::descriptor_type              descriptor_type;
			typedef typename traits_type::gfxapi_parameter_struct_type gfxapi_parameter_struct_type;
			typedef typename traits_type::built_resource_map           built_resource_map;
		};

		template <typename TResource>
		class ResourceBuilder 
		{

		};
	}
}

#endif