#ifndef __SHIRABE_IRESOURCEDESCRIPTOR_H__
#define __SHIRABE_IRESOURCEDESCRIPTOR_H__

#include <iostream>
#include <sstream>
#include <string>

#include "Core/EngineTypeHelper.h"
#include "Resources/System/Core/EResourceType.h"

namespace Engine {
  namespace Resources {

    // An adapter class will hold the implementation of a DomainTransferObject, e.g.
    // the descriptor_impl_type of ResourceDescriptor<TextureND<1>>.
    // Since the ResourceDescriptor<T> specializiation will derive from the impl. type
    // slicing at this point won't be an issue. 
    // The only thing of matter could be static downcasting and public Interface issues 
    // with knowing the capabilities of the underlying resource class.

#define DeclareAdapter(type, impl, alias, name)                        \
		template <typename TResourceTraits>				                         \
		class type##Adapter {								                               \
		public:												                                     \
			typedef typename TResourceTraits::impl             alias##_impl; \
      typedef type<typename TResourceTraits::class_type> alias;        \
															                                         \
			inline type##Adapter(							                               \
				const alias& name							                                 \
			) : _##name(name)								                                 \
			{}												                                       \
															                                         \
			inline const alias        				                               \
				name() const { return alias(_##name); }                        \
															                                         \
		private:											                                     \
			alias##_impl _##name;									                           \
		};			

    /**********************************************************************************************//**
     * \class DescriptorImplBase
     *
     * \brief A descriptor implementation base.
     *
     * \tparam  TResource Type of the resource.
     **************************************************************************************************/
    template <EResourceType resource_type, EResourceSubType resource_subtype>
    class DescriptorImplBase {
    public:
      EResourceType    type()    const { return resource_type;    }
      EResourceSubType subtype() const { return resource_subtype; }


    };

    template <typename TResource>
    struct ResourceDescriptor
      : public TResource::descriptor_impl_type
    {
      static const constexpr EResourceType    resource_type    = TResource::resource_type;
      static const constexpr EResourceSubType resource_subtype = TResource::resource_subtype;

      typedef typename TResource::descriptor_impl_type base_type;
      typedef typename TResource::descriptor_impl_type descriptor_type;

      ResourceDescriptor() = default;
      ResourceDescriptor(const descriptor_type& other) 
        : base_type(other)
      {}
      ResourceDescriptor(const ResourceDescriptor<TResource>& other) 
        : base_type((descriptor_type) other)
      {}
    };
    
    template <typename TResource>
    struct ResourceBinding
      : public TResource::binding_impl_type
    {
      typedef typename TResource::binding_impl_type binding_type;
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

      inline ResourceCreationRequest(const ResourceDescriptor<TResource>& desc)
        : creation_request_type(desc)
      {}
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

    DeclareAdapter(ResourceDescriptor, descriptor_impl_type, descriptor_type, descriptor);
    DeclareAdapter(ResourceBinding, binding_impl_type, binding_type, binding);
    DeclareAdapter(ResourceCreationRequest, creation_request_impl_type, creation_request_type, creationRequest);
    DeclareAdapter(ResourceUpdateRequest, update_request_impl_type, update_request_type, updateRequest);
    DeclareAdapter(ResourceQueryRequest, query_request_impl_type, query_request_type, queryRequest);
    DeclareAdapter(ResourceDestructionRequest, destruction_request_impl_type, destruction_request_type, destructionRequest);


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