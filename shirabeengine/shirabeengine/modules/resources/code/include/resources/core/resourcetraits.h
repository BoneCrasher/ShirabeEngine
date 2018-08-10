#ifndef __SHIRABE_RESOURCES_TRAITS_H__
#define __SHIRABE_RESOURCES_TRAITS_H__

#include "resources/core/eresourcetype.h"

namespace engine
{
    namespace resources
    {

        /**
         * Traits class to describe specific type-definitions for a resource,
         * binding together all related resource information for a specific
         * resource type.
         *
         * @tparam TClass
         * @tparam TType
         * @tparam TSubType
         * @tparam TBinding
         * @tparam TDescriptor
         * @tparam TCreationRequest
         * @tparam TUpdateRequest
         * @tparam TQueryRequest
         * @tparam TDestructionRequest
         */
        template<
                typename         TClass,
                EResourceType    TType,
                EResourceSubType TSubType,
                typename         TBinding,
                typename         TDescriptor,
                typename         TCreationRequest,
                typename         TUpdateRequest,
                typename         TQueryRequest,
                typename         TDestructionRequest
                >
        class CResourceTraits
        {
        public_static_constants:
            static const constexpr EResourceType    sResourceType    = TType;
            static const constexpr EResourceSubType sResourceSubType = TSubType;

        public_typedefs:
            using class_type                    = TClass             ;
            using descriptor_impl_type          = TDescriptor        ;
            using creation_request_impl_type    = TCreationRequest   ;
            using update_request_impl_type      = TUpdateRequest     ;
            using query_request_impl_type       = TQueryRequest      ;
            using destruction_request_impl_type = TDestructionRequest;

        };

        /**
         * Binds together publicly relevant resource type typedefs.
         *
         * @tparam T Type of the resource to bind typedefs together for.
         */
        template <typename T>
        class CResourcePublicTraits
        {
        public:
            using descriptor_public_type          = typename T::Descriptor        ;
            using creation_request_public_type    = typename T::CreationRequest   ;
            using update_request_public_type      = typename T::UpdateRequest     ;
            using query_request_public_type       = typename T::Query             ;
            using destruction_request_public_type = typename T::DestructionRequest;
        };

        #define DeclareResourceTraits(Prefix, Class, Type, Subtype, Descriptor, Creation, Update, Query, Destruction)\
            class C##Prefix##Traits                                                                                  \
            : public CResourceTraits<Class, Type, Subtype, Descriptor, Creation, Update, Query, Destruction>         \
                {};

        #define DeclareTemplatedResourceTraits(Tpl, Prefix, Class, Type, Subtype, Descriptor, Creation, Update, Query, Destruction)\
            Tpl                                                                                                                    \
            class C##Prefix##Traits                                                                                                \
            : public CResourceTraits<Class, Type, Subtype, Descriptor, Creation, Update, Query, Destruction>                       \
                {};

        #define DefineTraitsPublicTypes(Type)                                                                  \
            using C##Type##Descriptor           = ResourcePublicTraits<Type>::descriptor_public_type         ; \
            using C##Type##CreationRequest      = ResourcePublicTraits<Type>::creation_request_public_type   ; \
            using C##Type##UpdateRequest        = ResourcePublicTraits<Type>::update_request_public_type     ; \
            using C##Type##QueryRequest         = ResourcePublicTraits<Type>::query_request_public_type      ; \
            using C##Type##DesctructionRequest  = ResourcePublicTraits<Type>::destruction_request_public_type;

        #define DefineTraitsPublicTemplateTypes(Tpl, Prefix, Type)                                                            \
            Tpl using C##Prefix##Descriptor           = typename ResourcePublicTraits<Type>::descriptor_public_type         ; \
            Tpl using C##Prefix##CreationRequest      = typename ResourcePublicTraits<Type>::creation_request_public_type   ; \
            Tpl using C##Prefix##UpdateRequest        = typename ResourcePublicTraits<Type>::update_request_public_type     ; \
            Tpl using C##Prefix##QueryRequest         = typename ResourcePublicTraits<Type>::query_request_public_type      ; \
            Tpl using C##Prefix##DesctructionRequest  = typename ResourcePublicTraits<Type>::destruction_request_public_type;
    }
}

#endif
