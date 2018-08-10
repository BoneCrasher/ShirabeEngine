#ifndef __SHIRABE_RESOURCESUBSYSTEMPROXY_H__
#define __SHIRABE_RESOURCESUBSYSTEMPROXY_H__

#include "core/enginetypehelper.h"
#include "core/enginestatus.h"

#include "resources/core/iresourceproxy.h"
#include "resources/core/resourcedomaintransfer.h"

namespace engine
{
    namespace resources
    {
        /**
         * Proxy-Class proxying a resource backend implementation.
         *
         * @tparam TBackend  The resource backend to proxy.
         * @tparam TResource The resource type proxied.
         */
        template <
                typename TBackend,
                typename TResource
                >
        class ResourceBackendProxy
                : public CGenericProxyBase<TResource>
        {
        public_constructors:
            /**
             * Create a new resource backend proxy from the proxytype, creation
             * request and the respective resource backend.
             *
             * @param proxyType
             * @param resourceBackend
             * @param request
             */
            ResourceBackendProxy(
                    EProxyType                          const &aProxyType,
                    CStdSharedPtr_t<TBackend>           const &aResourceBackend,
                    typename TResource::CreationRequest const &aRequest)
                : CGenericProxyBase<TResource>(aProxyType, aRequest)
                , m_backend(aResourceBackend)
            { }

        protected_methods:
            /**
             * Return the pointer to the attached resource backend.
             *
             * @return See brief.
             */
            CStdSharedPtr_t<TBackend> resourceBackend()
            {
                return m_backend;
            }

        private_members:
            CStdSharedPtr_t<TBackend> m_backend;
        };
    }
}

#endif
