#ifndef __SHIRABE_GFXAPIRESOURCEPROXY_H__
#define __SHIRABE_GFXAPIRESOURCEPROXY_H__

#include <type_traits>
#include <typeinfo>
#include <optional>
#include <stdint.h>

#include <core/enginetypehelper.h>
#include <core/enginestatus.h>
#include <core/string.h>
#include <resources/core/resourcebackendproxy.h>

#include "graphicsapi/resources/gfxapi.h"
#include "graphicsapi/resources/gfxapiresourcebackend.h"

namespace engine
{
    namespace gfxapi
    {
        using namespace engine::resources;

        /**
         * Resource proxy implementation to proxy resources types in a graphics API resource backend.
         *
         * @tparam TResource Type of the proxied resource in the graphics API backend.
         */
        template <typename TResource>
        class CGFXAPIResourceProxy
                : public CResourceBackendProxy<CGFXAPIResourceBackend, TResource>
        {
            SHIRABE_DECLARE_LOG_TAG(CGFXAPIResourceProxy<TResource>);

        public_constructors:
            /**
             * Construct a graphics API proxy from it's proxy type, creation request and pointer to
             * the resource backend.
             *
             * @param aProxyType       The type of the proxy to be created.
             * @param aRequest         The creation request used for resource creation.
             * @param aResourceBackend The resource backend to request resource creation in.
             */
            CGFXAPIResourceProxy(
                    EProxyType                              const &aProxyType,
                    typename TResource::CCreationRequest    const &aRequest,
                    CStdSharedPtr_t<CGFXAPIResourceBackend> const &aResourceBackend)
                : CResourceBackendProxy<CGFXAPIResourceBackend, TResource>(aProxyType, aResourceBackend, aRequest)
                , mDestructionRequest("")
            { }

        public_methods:
            /**
             * Load the proxied resource in the graphics API backend.
             *
             * @param aResolvedDependencies A collection of dependencies of the resource to be loaded.
             * @return                      EEngineStatus::Ok, if successful. An error flag otherwise.
             */
            EEngineStatus loadSync(PublicResourceIdList_t const &aResolvedDependencies);

            /**
             * Unload the proxied resource from the graphics API backend.
             *
             * @return EEngineStatus::Ok, if successful. An error flag otherwise.
             */
            EEngineStatus unloadSync();

            /**
             * Return the attached destruction request, if any.
             * The destruction request is set from the specific backend, as destruction
             * might differ tremendously depending on the graphics API selected.
             *
             * @return See brief.
             */
            typename TResource::CDestructionRequest const &destructionRequest() const
            {
                return mDestructionRequest;
            }

        protected_methods:
            /**
             *  Set the destruction request for the specific graphics API and resource type.
             *
             * @param request The request used for destruction.
             */
            void setDestructionRequest(typename TResource::CDestructionRequest const &aRequest)
            {
                mDestructionRequest = aRequest;
            }

        private_members:
            typename TResource::CDestructionRequest mDestructionRequest;
        };

        /**
         * Cast operation to extract a gfxapi proxy from an AnyProxy instance.
         *
         * @tparam TResource The underlying resource type of the proxy in the graphics API backend.
         * @param aProxy     The AnyProxy to extract a GFXAPIResourceProxy from.
         * @return           A valid GFXAPIResourceProxy instance or nullptr on error.
         */
        template <typename TResource>
        static CStdSharedPtr_t<CGFXAPIResourceProxy<TResource>> GFXAPIProxyCast(AnyProxy const &aProxy)
        {
            CStdSharedPtr_t<CGFXAPIResourceProxy<TResource>> const temp = std::static_pointer_cast<CGFXAPIResourceProxy<TResource>>(aProxy);
            return temp;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        EEngineStatus CGFXAPIResourceProxy<TResource>::loadSync(PublicResourceIdList_t const &aResolvedDependencies)
        {
            this->setLoadState(ELoadState::LOADING);

            EEngineStatus status = EEngineStatus::Ok;

            typename TResource::CCreationRequest const &creationRequest = static_cast<CGenericProxyBase<TResource>*>(this)->creationRequest();
            typename TResource::SDescriptor      const &rd              = creationRequest.resourceDescriptor();

            status = this->resourceBackend()->template load<TResource>(creationRequest, aResolvedDependencies, ETaskSynchronization::Sync, nullptr);
            if(CheckEngineError(status))
            {
                // MBT TODO: Consider distinguishing the above returned status a little more in
                //           order to reflect UNLOADED or UNAVAILABLE state.
                this->setLoadState(ELoadState::UNLOADED);

                HandleEngineStatusError(EEngineStatus::GFXAPI_LoadResourceFailed, CString::format("Failed to load GFXAPI resource '%0'", rd.name));
            }

            this->setDestructionRequest(typename TResource::CDestructionRequest(rd.name));
            this->setLoadState(ELoadState::LOADED);

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        EEngineStatus CGFXAPIResourceProxy<TResource>::unloadSync()
        {
            EEngineStatus status = EEngineStatus::Ok;
            status = this->resourceBackend()->template unload<TResource>(typename TResource::CDestructionRequest(this->destructionRequest()));

            if(CheckEngineError(status)) {
                this->setLoadState(ELoadState::UNKNOWN);

                HandleEngineStatusError(EEngineStatus::GFXAPI_UnloadResourceFailed, CString::format("Failed to unload GFXAPI resource '%0'", ""));
            }

            this->setLoadState(ELoadState::UNLOADED);

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------
    }
}

#endif
