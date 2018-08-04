#ifndef __SHIRABE_IRESOURCEPROXY_H__
#define __SHIRABE_IRESOURCEPROXY_H__

#include <any>
#include <map>
#include <vector>

#include "core/enginetypehelper.h"
#include "core/enginestatus.h"

#include "resources/core/eresourcetype.h"
#include "resources/core/resourcedto.h"
#include "resources/core/resourcedomaintransfer.h"

namespace engine
{
    namespace resources
    {
        /**
         * Represents the current state of a platform resource.
         */
        enum class ELoadState
        {
            UNKNOWN  = 0,
            UNLOADED,
            UNLOADING,
            LOADED,
            LOADING,
            AVAILABLE,
            UNAVAILABLE
        };

        /**
         * Values that represent proxy types
         */
        enum class EProxyType
        {
            Unknown    = 0,
            Internal,
            Persistent,
            Dynamic
        };

        /**
         * The IResourceProxyBase interface declares the common interface for any kind of resource proxy
         * being specialized, assuming that any implementation will have a loadState and provides
         * synchronous load/unload opoerations.
         */
        class IResourceProxyBase
        {
            SHIRABE_DECLARE_INTERFACE(IResourceProxyBase);

        public_api:
            virtual EProxyType proxyType() const = 0;
            virtual ELoadState loadState() const = 0;

            virtual EEngineStatus loadSync(PublicResourceIdList const &inResolvedDependencies) = 0;
            virtual EEngineStatus unloadSync() = 0;
        };

        /**
         * Declares a shared pointer to base class for polymorphic type erasure storage.
         */
        using AnyProxy = CStdSharedPtr_t<IResourceProxyBase>;

        /**
         * Permits storage of a list of any proxy.
         */
        using ResourceProxyList = std::vector<AnyProxy>;

        /**
         * Permits mapping from a resource id to a list of any proxy.
         */
        using ResourceProxyMap  = std::map<PublicResourceId_t, AnyProxy>;

        /**
         * The IResourceProxy interface declares the capability of a proxy to destroy a resource.
         *
         * @tparam TResource
         */
        template <typename TResource>
        class IResourceProxy
                : public IResourceProxyBase
        {
            SHIRABE_DECLARE_INTERFACE(IResourceProxy);

        public_api:
            virtual bool destroy() = 0;
        };

        /**
         * @brief BaseProxyCast
         * @param proxy
         * @return
         */
        static CStdSharedPtr_t<IResourceProxyBase> BaseProxyCast(const AnyProxy& proxy) {
            return proxy;
        }

        /**
         *
         */
        template <typename TResource>
        static CStdSharedPtr_t<IResourceProxy<TResource>> ProxyCast(const AnyProxy& proxy) {
            CStdSharedPtr_t<IResourceProxy<TResource>> tmp = std::static_pointer_cast<IResourceProxy<TResource>>(proxy);
            return tmp;
        }

        /**
         *
         */
        template <typename TResource>
        class GenericProxyBase
                : public IResourceProxy<TResource>
                , public ResourceCreationRequestAdapter<typename TResource::CreationRequest>
        {
        public_constructors:
            GenericProxyBase(
                    EProxyType                          const &aProxyType,
                    typename TResource::CreationRequest const &aCreationRequest)
                : IResourceProxy<TResource>()
                , ResourceCreationRequestAdapter<typename TResource::CreationRequest>(aCreationRequest)
                , mProxyType(proxyType)
                , mProxyLoadState(ELoadState::UNKNOWN)
            { }

        public_destructors:
            virtual ~GenericProxyBase() = default;

        public_methods:
            EProxyType proxyType() const
            {
                return mProxyType;
            }

            ELoadState loadState() const
            {
                return mProxyLoadState;
            }

            bool destroy()
            {
                return !CheckEngineError(this->unloadSync());
            }

        protected_methods:
            void setLoadState(ELoadState const & aNewLoadState)
            {
                mProxyLoadState = aNewLoadState;
            }

        private:
            EProxyType mProxyType;
            ELoadState mProxyLoadState;
        };

        /**
         *
         */
        template <typename TResource>
        static CStdSharedPtr_t<GenericProxyBase<TResource>> GenericProxyBaseCast(AnyProxy const &proxy)
        {
            CStdSharedPtr_t<GenericProxyBase<TResource>> const temporary = std::static_pointer_cast<GenericProxyBase<TResource>>(proxy);
            return temporary;
        }

    }
}

#endif
