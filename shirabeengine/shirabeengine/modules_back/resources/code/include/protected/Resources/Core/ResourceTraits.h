#ifndef __SHIRABE_RESOURCES_TRAITS_H__
#define __SHIRABE_RESOURCES_TRAITS_H__

#include "resources/core/eresourcetype.h"

namespace Engine {
  namespace Resources {

    /**********************************************************************************************//**
     * \class ResourceTraits
     *
     * \brief Convenience base class for any engine resource integration.
     *
     * \tparam  type                Type of the type.
     * \tparam  subtype             Type of the subtype.
     * \tparam  TBinding            Type of the binding.
     * \tparam  TDescriptor         Type of the descriptor.
     * \tparam  TCreationRequest    Type of the creation request.
     * \tparam  TUpdateRequest      Type of the update request.
     * \tparam  TQueryRequest       Type of the query request.
     * \tparam  TDestructionRequest Type of the destruction request.
     **************************************************************************************************/
    template<
      typename TClass,
      EResourceType    type,
      EResourceSubType subtype,
      typename TBinding,
      typename TDescriptor,
      typename TCreationRequest,
      typename TUpdateRequest,
      typename TQueryRequest,
      typename TDestructionRequest
    >
      class ResourceTraits {
      public:
        static const constexpr EResourceType    resource_type    = type;
        static const constexpr EResourceSubType resource_subtype = subtype;

        typedef TClass              class_type;
        typedef TDescriptor         descriptor_impl_type;
        typedef TCreationRequest    creation_request_impl_type;
        typedef TUpdateRequest      update_request_impl_type;
        typedef TQueryRequest       query_request_impl_type;
        typedef TDestructionRequest destruction_request_impl_type;

    };

    template <typename T>
    class ResourcePublicTraits {
    public:
      typedef typename T::Descriptor         descriptor_public_type;
      typedef typename T::CreationRequest    creation_request_public_type;
      typedef typename T::UpdateRequest      update_request_public_type;
      typedef typename T::Query              query_request_public_type;
      typedef typename T::DestructionRequest destruction_request_public_type;
    };

#define DeclareResourceTraits(Prefix, Class, Type, Subtype, Descriptor, Creation, Update, Query, Destruction) \
    class Prefix##Traits                                                                                               \
      : public ResourceTraits<Class, Type, Subtype, Descriptor, Creation, Update, Query, Destruction>         \
    {};

#define DeclareTemplatedResourceTraits(Tpl, Prefix, Class, Type, Subtype, Descriptor, Creation, Update, Query, Destruction) \
    Tpl                                                                                                                              \
    class Prefix##Traits                                                                                                             \
      : public ResourceTraits<Class, Type, Subtype, Descriptor, Creation, Update, Query, Destruction>                       \
    {};

#define DefineTraitsPublicTypes(Type)                                                               \
    using Type##Descriptor           = ResourcePublicTraits<Type>::descriptor_public_type         ; \
    using Type##CreationRequest      = ResourcePublicTraits<Type>::creation_request_public_type   ; \
    using Type##UpdateRequest        = ResourcePublicTraits<Type>::update_request_public_type     ; \
    using Type##QueryRequest         = ResourcePublicTraits<Type>::query_request_public_type      ; \
    using Type##DesctructionRequest  = ResourcePublicTraits<Type>::destruction_request_public_type; 

#define DefineTraitsPublicTemplateTypes(Tpl, Prefix, Type)                                                         \
    Tpl using Prefix##Descriptor           = typename ResourcePublicTraits<Type>::descriptor_public_type         ; \
    Tpl using Prefix##CreationRequest      = typename ResourcePublicTraits<Type>::creation_request_public_type   ; \
    Tpl using Prefix##UpdateRequest        = typename ResourcePublicTraits<Type>::update_request_public_type     ; \
    Tpl using Prefix##QueryRequest         = typename ResourcePublicTraits<Type>::query_request_public_type      ; \
    Tpl using Prefix##DesctructionRequest  = typename ResourcePublicTraits<Type>::destruction_request_public_type; 
  }
}

#endif