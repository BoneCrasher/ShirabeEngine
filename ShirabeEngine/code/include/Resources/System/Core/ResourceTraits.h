#ifndef __SHIRABE_RESOURCES_TRAITS_H__
#define __SHIRABE_RESOURCES_TRAITS_H__

#include "Resources/System/Core/EResourceType.h"

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
        typedef TBinding            binding_impl_type;
        typedef TDescriptor         descriptor_impl_type;
        typedef TCreationRequest    creation_request_impl_type;
        typedef TUpdateRequest      update_request_impl_type;
        typedef TQueryRequest       query_request_impl_type;
        typedef TDestructionRequest destruction_request_impl_type;

        typedef ResourceDescriptor<TClass>         descriptor_public_type;
        typedef ResourceBinding<TClass>            binding_public_type;
        typedef ResourceCreationRequest<TClass>    creation_request_public_type;
        typedef ResourceUpdateRequest<TClass>      update_request_public_type;
        typedef ResourceQueryRequest<TClass>       query_request_public_type;
        typedef ResourceDestructionRequest<TClass> destruction_request_public_type;
    };

#define DeclareResourceTraits(Prefix, Class, Type, Subtype, Binding, Descriptor, Creation, Update, Query, Destruction) \
    class Prefix##Traits                                                                                        \
      : public ResourceTraits<Class, Type, Subtype, Binding, Descriptor, Creation, Update, Query, Destruction>         \
    {}      

#define DeclareTemplatedResourceTraits(Tpl, Prefix, Class, Type, Subtype, Binding, Descriptor, Creation, Update, Query, Destruction) \
    Tpl                                                                                                                       \
    class Prefix##Traits                                                                                                      \
      : public ResourceTraits<Class, Type, Subtype, Binding, Descriptor, Creation, Update, Query, Destruction>                       \
    {}

#define DefineTraitsPublicTypes(Prefix, Traits)                                 \
    using Prefix##Binding              = Traits::binding_public_type            ; \
    using Prefix##Descriptor           = Traits::descriptor_public_type         ; \
    using Prefix##CreationRequest      = Traits::creation_request_public_type   ; \
    using Prefix##UpdateRequest        = Traits::update_request_public_type     ; \
    using Prefix##QueryRequest         = Traits::query_request_public_type      ; \
    using Prefix##DesctructionRequest  = Traits::destruction_request_public_type; 

#define DefineTraitsPublicTemplateTypes(Tpl, Prefix, Traits)                                 \
    Tpl using Prefix##Binding              = typename Traits::binding_public_type            ; \
    Tpl using Prefix##Descriptor           = typename Traits::descriptor_public_type         ; \
    Tpl using Prefix##CreationRequest      = typename Traits::creation_request_public_type   ; \
    Tpl using Prefix##UpdateRequest        = typename Traits::update_request_public_type     ; \
    Tpl using Prefix##QueryRequest         = typename Traits::query_request_public_type      ; \
    Tpl using Prefix##DesctructionRequest  = typename Traits::destruction_request_public_type; 

  }
}

#endif