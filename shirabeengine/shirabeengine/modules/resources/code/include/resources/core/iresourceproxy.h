#ifndef __SHIRABE_IRESOURCEPROXY_H__
#define __SHIRABE_IRESOURCEPROXY_H__

#include <any>
#include <map>
#include <vector>

#include "core/enginetypehelper.h"
#include "core/enginestatus.h"

#include "resources/core/eresourcetype.h"
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
            UNKNOWN  = 0, //< Unknown load state
            UNLOADED,     //< The resource is unloaded the proxied location.
            UNLOADING,    //< The resource is currently being unloaded in the proxied location.
            LOADED,       //< The resource is loaded in the proxied location.
            LOADING,      //< The resource is currently being loaded in the proxied location.
            AVAILABLE,    //< The resource is available (obsolete?)
            UNAVAILABLE   //< The resource is unavailable in the proxied location.
        };

        /**
         * Values that represent proxy types
         */
        enum class EProxyType
        {
            Unknown    = 0, //< Unknown proxy type
            Internal,       //< Internal resource. Un/loading is not possible. We just store the data.
            Persistent,     //< The resource is loaded once, but can't be unloaded, only destroyed in the end.
            Dynamic         //< The resource can be un/loaded dynamically.
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

            virtual EEngineStatus loadSync(PublicResourceIdList_t const &inResolvedDependencies) = 0;
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
         * @tparam TResource The underlying resource type of the proxy.
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
         * Cast operation to extract a base proxy type from an AnyProxy.
         *
         * @param aProxy The proxy container to extract from.
         * @return       Returns the IResourceProxyBase pointer for the AnyProxy or nullptr.
         */
        static CStdSharedPtr_t<IResourceProxyBase> &baseProxyCast(AnyProxy &aProxy)
        {
            return aProxy;
        }

        /**
         * Cast operation to extract an immutable base proxy type from an AnyProxy.
         *
         * @param aProxy The proxy container to extract from.
         * @return       Returns the IResourceProxyBase pointer for the AnyProxy or nullptr.
         */
        static CStdSharedPtr_t<IResourceProxyBase> const &baseProxyCast(AnyProxy const &aProxy)
        {
            return aProxy;
        }

        /**
         * Cast operation to extract a typed resource proxy for a specific resource type.
         *
         * @param aProxy The proxy container to extract from.
         */
        template <typename TResource>
        static CStdSharedPtr_t<IResourceProxy<TResource>> proxyCast(AnyProxy& aProxy)
        {
            CStdSharedPtr_t<IResourceProxy<TResource>> &tmp = std::static_pointer_cast<IResourceProxy<TResource>>(aProxy);
            return tmp;
        }

        /**
         * Cast operation to extract an immutable typed resource proxy for a specific resource type.
         *
         * @param aProxy The proxy container to extract from.
         */
        template <typename TResource>
        static CStdSharedPtr_t<IResourceProxy<TResource>> const &proxyCast(AnyProxy const &aProxy)
        {
            CStdSharedPtr_t<IResourceProxy<TResource>> const &tmp = std::static_pointer_cast<IResourceProxy<TResource>>(aProxy);
            return tmp;
        }

        /**
         * Generic base proxy implementation for any kind of resource type.
         * It stores a creation request to permit load/unload operations with
         * equal creation parameters.
         *
         * @tparam TResource The underlying resource type of the resource proxy.
         */
        template <typename TResource>
        class CGenericProxyBase
                : public IResourceProxy<TResource>
                , public CResourceCreationRequestAdapter<typename TResource::CreationRequest>
        {
        public_constructors:
            /**
             * Construct a new generic resource proxy for a specific proxy type and creation request.
             *
             * @param aProxyType       The specific proxy type to be created.
             * @param aCreationRequest The creation request used for dynamic load/unload
             */
            CGenericProxyBase(
                    EProxyType                          const &aProxyType,
                    typename TResource::CreationRequest const &aCreationRequest)
                : IResourceProxy<TResource>()
                , CResourceCreationRequestAdapter<typename TResource::CreationRequest>(aCreationRequest)
                , mProxyType(proxyType)
                , mProxyLoadState(ELoadState::UNKNOWN)
            { }

        public_destructors:
            /**
             * Destroy and run...
             */
            virtual ~CGenericProxyBase() = default;

        public_methods:
            /**
             * Return the proxy type assigned for this proxy.
             *
             * @return See brief.
             */
            EProxyType proxyType() const
            {
                return mProxyType;
            }

            /**
             * Return the current load state of the proxy to control
             * interaction behaviour with the proxy.
             *
             * @return See brief.
             */
            ELoadState loadState() const
            {
                return mProxyLoadState;
            }

            /**
             * Destroy the proxied resource.
             *
             * @return True, if destroyed successfully. False, otherwise.
             */
            bool destroy()
            {
                bool const error = CheckEngineError(this->unloadSync());

                return (!error);
            }

        protected_methods:
            /**
             * Set the proxies current load state.
             *
             * @param aNewLoadState The new load state.
             */
            void setLoadState(ELoadState const &aNewLoadState)
            {
                mProxyLoadState = aNewLoadState;
            }

        private_members:
            EProxyType mProxyType;
            ELoadState mProxyLoadState;
        };

        /**
         * Cast operation to extract a typed generic proxy from an AnyProxy.
         *
         * @param aProxy The AnyProxy to extract from.
         * @returns      A generic proxy with underlying type TResource or nullptr.
         */
        template <typename TResource>
        static CStdSharedPtr_t<CGenericProxyBase<TResource>> genericProxyBaseCast(AnyProxy const &aProxy)
        {
            CStdSharedPtr_t<CGenericProxyBase<TResource>> const temporary = std::static_pointer_cast<CGenericProxyBase<TResource>>(aProxy);
            return temporary;
        }

    }
}

#endif
