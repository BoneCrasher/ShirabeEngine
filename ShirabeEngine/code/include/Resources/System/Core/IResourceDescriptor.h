#ifndef __SHIRABE_IRESOURCEDESCRIPTOR_H__
#define __SHIRABE_IRESOURCEDESCRIPTOR_H__

#include <iostream>
#include <sstream>
#include <string>

#include "Core/EngineTypeHelper.h"
#include "Resources/System/Core/EResourceType.h"

namespace Engine {
	namespace Resources {

	#define DeclareAdapter(type, impl, alias, name)         \
		template <typename TResource>						\
		class type##Adapter {								\
		public:												\
			typedef typename type<TResource>::impl alias;   \
															\
			inline type##Adapter(							\
				const alias& name							\
			) : _##name(name)								\
			{}												\
															\
			inline const alias&								\
				name() const { return _##name; }			\
															\
		private:											\
			alias _##name;									\
		};												    
		
		template <typename TResource>
		struct ResourceDescriptor
			: public TResource::descriptor_impl_type 
		{
			static const constexpr EResourceType    resource_type    = TResource::resource_type;
			static const constexpr EResourceSubType resource_subtype = TResource::resource_subtype;

			typedef typename TResource::descriptor_impl_type descriptor_type;
		};
		
		/**********************************************************************************************//**
		 * \struct	ResourceCreationRequest
		 *
		 * \brief	A resource creation request.
		 *
		 * \tparam	TResource	Type of the resource.
		 **************************************************************************************************/
		template <typename TResource>
		struct ResourceCreationRequest
			: public TResource::creation_request_impl_type
		{
			typedef typename TResource::creation_request_impl_type creation_request_type;
		};
		
		/**********************************************************************************************//**
		 * \struct	ResourceUpdateRequest
		 *
		 * \brief	A resource update request.
		 *
		 * \tparam	TResource	Type of the resource.
		 **************************************************************************************************/
		template <typename TResource>
		struct ResourceUpdateRequest
			: public TResource::update_request_impl_type
		{
			typedef typename TResource::update_request_impl_type update_request_type;
		};

		/**********************************************************************************************//**
		 * \struct	ResourceQueryRequest
		 *
		 * \brief	A resource query request.
		 *
		 * \tparam	TResource	Type of the resource.
		 **************************************************************************************************/
		template <typename TResource>
		struct ResourceQueryRequest
			: public TResource::query_request_impl_type
		{
			typedef typename TResource::query_request_impl_type query_request_type;
		};

		/**********************************************************************************************//**
		 * \struct	ResourceDestructionRequest
		 *
		 * \brief	A resource destruction request.
		 *
		 * \tparam	TResource	Type of the resource.
		 **************************************************************************************************/
		template <typename TResource>
		struct ResourceDestructionRequest
			: public TResource::destruction_request_impl_type
		{
			typedef typename TResource::destruction_request_impl_type destruction_request_type;
		};

		DeclareAdapter(ResourceDescriptor,         descriptor_type,          descriptor_type,          descriptor);
		DeclareAdapter(ResourceCreationRequest,    creation_request_type,    creation_request_type,    creationRequest);
		DeclareAdapter(ResourceUpdateRequest,      update_request_type,      update_request_type,      updateRequest);
		DeclareAdapter(ResourceQueryRequest,       query_request_type,       query_request_type,       queryRequest);
		DeclareAdapter(ResourceDestructionRequest, destruction_request_type, destruction_request_type, destructionRequest);

		template <typename TResource>
		static inline std::ostream& operator <<(std::ostream& s, const ResourceDescriptor<TResource>& d) {
			return (s << d.toString());
		}
		
		template <typename TResource>
		static inline std::ostream& operator <<(std::ostream& s, const ResourceCreationRequest<TResource>& d) {
			return (s << d.toString());
		}

		template <typename TResource>
		static inline std::ostream& operator <<(std::ostream& s, const ResourceUpdateRequest<TResource>& d) {
			return (s << d.toString());
		}

		template <typename TResource>
		static inline std::ostream& operator <<(std::ostream& s, const ResourceQueryRequest<TResource>& d) {
			return (s << d.toString());
		}

		template <typename TResource>
		static inline std::ostream& operator <<(std::ostream& s, const ResourceDestructionRequest<TResource>& d) {
			return (s << d.toString());
		}
	}
}

#endif