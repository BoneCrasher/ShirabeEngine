#ifndef __SHIRABE_RESOURCES_REQUEST_DEFAULT_IMPL_H__
#define __SHIRABE_RESOURCES_REQUEST_DEFAULT_IMPL_H__

#include "resources/core/resourcedomaintransfer.h"
#include "resources/core/eresourcetype.h"
#include "resources/core/iresource.h"
#include "resources/core/resourcedatasource.h"

namespace engine
{
    namespace resources
    {

        using namespace engine::resources;

        /**
         * Container-Class around base request default implementations.
         */
        class CBaseDeclaration
        {
        public:
            /**
             * Base implementation for any CreationRequest within the resource system.
             * Each CreationRequest encapsulates at least a resource descriptor,
             * accessible calling the "resourceDescriptor"-method.
             *
             * @tparam TDescriptor Describes the resource descriptor type to encapsulate.
             */
            template <typename TDescriptor>
            class CCreationRequestBase
            {
            public_constructors:
                /**
                 * Construct a new CreationRequest from a descriptor.
                 *
                 * @param desc
                 */
                CCreationRequestBase(TDescriptor const &desc)
                    : mResourceDescriptor(desc)
                {}

            public_methods:
                /**
                 * Return the encapsulated resource descriptor.
                 *
                 * @return See brief.
                 */
                TDescriptor const &resourceDescriptor() const
                {
                    return mResourceDescriptor;
                }

                /**
                 * Check, whether the descriptor should be serialized on destruction.
                 *
                 * @return
                 */
                bool serializeOnDestruct() const
                {
                    return mSerializeOnDestruct;
                }

                /**
                 * Return a string representation of the descriptor.
                 *
                 * @return
                 */
                virtual std::string toString() const = 0;

            private_members:
                TDescriptor mResourceDescriptor;
                bool        mSerializeOnDestruct;
            };

            /**
             * Base implementation for any request, which is based on a previously created resource,
             * encapsulating a public resource id.
             */
            class CExistingResourceRequestBase
            {
            public_constructors:
                /**
                 * Construct a new request for a provided public resource id.
                 *
                 * @param inPublicResourceId
                 */
                SHIRABE_INLINE CExistingResourceRequestBase(PublicResourceId_t const &aPublicResourceId)
                    : mPublicResourceId(aPublicResourceId)
                {}

            public_methods:
                /**
                 * Return the encapsulated public resource id.
                 *
                 * @return See brief.
                 */
                SHIRABE_INLINE PublicResourceId_t const &publicResourceId() const
                {
                    return mPublicResourceId;
                }

            private_members:
                PublicResourceId_t mPublicResourceId;
            };

            /**
             * Base request for any resource update operation.
             */
            class CUpdateRequestBase
                    : public CExistingResourceRequestBase
            {
            public_constructors:
                /**
                 * Construct a new request from a public resource id.
                 *
                 * @param inPublicResourceId
                 */
                SHIRABE_INLINE CUpdateRequestBase(PublicResourceId_t const &aPublicResourceId)
                    : CExistingResourceRequestBase(aPublicResourceId)
                {}
            };

            /**
             * Base request for any resource descrution operation.
             */
            class CDestructionRequestBase
                    : public CExistingResourceRequestBase
            {
            public_constructors:
                /**
                 * Construct a new request from a public resource id.
                 *
                 * @param aPublicResourceId
                 */
                SHIRABE_INLINE CDestructionRequestBase(PublicResourceId_t const &aPublicResourceId)
                    : CExistingResourceRequestBase(aPublicResourceId)
                {}
            };

            /**
             * Base request for any resource query operation.
             */
            class CQueryBase
                    : public CExistingResourceRequestBase
            {
            public:
                /**
                 * Construct a new request from a public resource id.
                 * @param inPublicResourceId
                 */
                SHIRABE_INLINE CQueryBase(PublicResourceId_t const &aPublicResourceId)
                    : CExistingResourceRequestBase(aPublicResourceId)
                {}
            };
        };

    }
}

#endif
