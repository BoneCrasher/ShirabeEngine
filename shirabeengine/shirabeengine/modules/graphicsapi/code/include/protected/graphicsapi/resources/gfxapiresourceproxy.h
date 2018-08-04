#ifndef __SHIRABE_GFXAPIRESOURCEPROXY_H__
#define __SHIRABE_GFXAPIRESOURCEPROXY_H__

#include <type_traits>
#include <typeinfo>
#include <optional>
#include <stdint.h>

#include "core/enginetypehelper.h"
#include "core/enginestatus.h"
#include "Core/IOC/Observer.h"

#include "Resources/Core/ResourceBackendProxy.h"
#include "GraphicsAPI/Resources/GFXAPIResourceBackend.h"
#include "GraphicsAPI/Resources/GFXAPI.h"

namespace engine {
  namespace GFXAPI {
    using namespace engine::Resources;


    /**********************************************************************************************//**
     * \class	PlatformResourceProxy
     *
     * \brief	A platform resource wrapper.
     **************************************************************************************************/
    template <typename TResource>
    class GFXAPIResourceProxy
      : public ResourceBackendProxy<GFXAPIResourceBackend, TResource>
    {
      SHIRABE_DECLARE_LOG_TAG(GFXAPIResourceProxy<TResource>);

    public:
      inline GFXAPIResourceProxy(
        EProxyType                          const&proxyType,
        CStdSharedPtr_t<GFXAPIResourceBackend>          const&resourceBackend,
        typename TResource::CreationRequest const&request)
        : ResourceBackendProxy<GFXAPIResourceBackend, TResource>(proxyType, resourceBackend, request)
        , m_destructionRequest("")
      { }

      EEngineStatus loadSync(
        PublicResourceIdList const&resolvedDependencies);

      EEngineStatus unloadSync();

      typename TResource::DestructionRequest const&destructionRequest() const { return m_destructionRequest; }

    protected:
      void setDestructionRequest(typename TResource::DestructionRequest const&request) { m_destructionRequest = request; }

    private:
      typename TResource::DestructionRequest m_destructionRequest;
    };

    /**********************************************************************************************//**
     * \fn	template <typename TResource> static CStdSharedPtr_t<GFXAPIResourceProxy<TResource>> GFXAPIProxyCast(const AnyProxy& proxy)
     *
     * \brief	Gfxapi proxy cast
     *
     * \tparam	TResource	Type of the resource.
     * \param	proxy	The proxy.
     *
     * \return	A Ptr&lt;GFXAPIResourceProxy&lt;TResource&gt;&gt;
     **************************************************************************************************/
    template <typename TResource>
    static CStdSharedPtr_t<GFXAPIResourceProxy<TResource>> GFXAPIProxyCast(const AnyProxy& proxy) {
      CStdSharedPtr_t<GFXAPIResourceProxy<TResource>> tmp = std::static_pointer_cast<GFXAPIResourceProxy<TResource>>(proxy);
      return tmp;
    }

    template <typename TResource>
    EEngineStatus GFXAPIResourceProxy<TResource>
      ::loadSync(
        PublicResourceIdList const&resolvedDependencies)
    {
      this->setLoadState(ELoadState::LOADING);

      // Request synchronous resource load and if successful, set the internal handle 
      // and load state.
      EEngineStatus status = EEngineStatus::Ok;

      
      typename TResource::CreationRequest const&creationRequest = static_cast<GenericProxyBase<TResource>*>(this)->creationRequest();
      typename TResource::Descriptor      const&rd              = creationRequest.resourceDescriptor();

      status = resourceBackend()->load<TResource>(creationRequest, resolvedDependencies, ETaskSynchronization::Sync, nullptr);
      if(CheckEngineError(status)) {
        // MBT TODO: Consider distinguishing the above returned status a little more in 
        //           order to reflect UNLOADED or UNAVAILABLE state.
        this->setLoadState(ELoadState::UNLOADED);

        HandleEngineStatusError(EEngineStatus::GFXAPI_LoadResourceFailed, String::format("Failed to load GFXAPI resource '%0'", rd.name));
      }

      this->setDestructionRequest(typename TResource::DestructionRequest(rd.name));
      this->setLoadState(ELoadState::LOADED);

      return EEngineStatus::Ok;
    }

    template <typename TResource>
    EEngineStatus GFXAPIResourceProxy<TResource>
      ::unloadSync()
    {
      EEngineStatus status = EEngineStatus::Ok;
      status = resourceBackend()->unload<TResource>(TResource::DestructionRequest(this->destructionRequest()));

      if(CheckEngineError(status)) {
        this->setLoadState(ELoadState::UNKNOWN);

        HandleEngineStatusError(EEngineStatus::GFXAPI_UnloadResourceFailed, String::format("Failed to unload GFXAPI resource '%0'", ""));
      }

      this->setLoadState(ELoadState::UNLOADED);

      return EEngineStatus::Ok;
    }
  }
}

#endif