#ifndef __SHIRABE_GFXAPIRESOURCEPROXY_H__
#define __SHIRABE_GFXAPIRESOURCEPROXY_H__

#include <type_traits>
#include <typeinfo>
#include <optional>
#include <stdint.h>

#include "Core/EngineTypeHelper.h"
#include "Core/EngineStatus.h"
#include "IOC/Observer.h"

#include "Resources/System/Core/ResourceBackendProxy.h"
#include "Resources/Subsystems/GFXAPI/GFXAPIResourceBackend.h"
#include "Resources/Subsystems/GFXAPI/GFXAPI.h"

namespace Engine {
	namespace GFXAPI {
		using namespace Engine::Resources;

		/**********************************************************************************************//**
		 * \class	GFXAPIResourceAdapter
		 *
		 * \brief	Simple storage base class to hold a GFXAPIResourceHandle_t and make it accessible.
	     **************************************************************************************************/
		class GFXAPIResourceAdapter
		{
		public:
			inline const GFXAPIResourceHandle_t& handle() const { return m_handle; }

		protected:
			inline GFXAPIResourceAdapter(const GFXAPIResourceHandle_t& handle = GFXAPIUninitializedResourceHandle)
				: m_handle(handle)
			{ }

			inline bool setHandle(const GFXAPIResourceHandle_t& handle) {
				if( m_handle > 0 )
					return false; // Overwrite not allowed!
				m_handle = handle;
			}

		private:
			GFXAPIResourceHandle_t m_handle;
		};

    static Ptr<GFXAPIResourceAdapter> GFXAPIAdapterCast(const AnyProxy& proxy) {
      try {
        Ptr<GFXAPIResourceAdapter> tmp = std::any_cast<Ptr<GFXAPIResourceAdapter>>(proxy);
        return tmp;
      } catch( std::bad_any_cast& ) {
        return nullptr;
      }
    }

		/**********************************************************************************************//**
		 * \class	PlatformResourceProxy
		 *
		 * \brief	A platform resource wrapper.
		 **************************************************************************************************/
		template <typename TResource>
		class GFXAPIResourceProxy
			: public ResourceBackendProxy<BasicGFXAPIResourceBackend, TResource>
			, public GFXAPIResourceAdapter
		{
			DeclareLogTag(GFXAPIResourceProxy<TResource>);

		public:
			inline GFXAPIResourceProxy(
				EProxyType                          const&proxyType,
				Ptr<BasicGFXAPIResourceBackend>     const&resourceBackend,
				typename TResource::CreationRequest const&request)
				: ResourceBackendProxy<BasicGFXAPIResourceBackend, TResource>(proxyType, resourceBackend, request)
				, GFXAPIResourceAdapter(GFXAPIUninitializedResourceHandle)
			{ }

			EEngineStatus loadSync(
        GFXAPIResourceHandleMap const&resolvedDependencies);

			EEngineStatus unloadSync();
      
		protected:
		private:
			Ptr<BasicGFXAPIResourceBackend> m_backend;
		};

		/**********************************************************************************************//**
		 * \fn	template <typename TResource> static Ptr<GFXAPIResourceProxy<TResource>> GFXAPIProxyCast(const AnyProxy& proxy)
		 *
		 * \brief	Gfxapi proxy cast
		 *
		 * \tparam	TResource	Type of the resource.
		 * \param	proxy	The proxy.
		 *
		 * \return	A Ptr&lt;GFXAPIResourceProxy&lt;TResource&gt;&gt;
		 **************************************************************************************************/
		template <typename TResource>
		static Ptr<GFXAPIResourceProxy<TResource>> GFXAPIProxyCast(const AnyProxy& proxy) {
			try {
				Ptr<GFXAPIResourceProxy<TResource>> tmp = std::any_cast<Ptr<GFXAPIResourceProxy<TResource>>>(proxy);
				return tmp;
			} catch( std::bad_any_cast& ) {
				return nullptr;
			}
		}
		
		template <typename TResource>
		EEngineStatus GFXAPIResourceProxy<TResource>
			::loadSync(
        GFXAPIResourceHandleMap const&resolvedDependencies)
		{
			this->setLoadState(ELoadState::LOADING);
			
			// Request synchronous resource load and if successful, set the internal handle 
			// and load state.
			EEngineStatus status = EEngineStatus::Ok; 

      typename TResource::Descriptor const&rd = static_cast<GenericProxyBase<TResource>*>(this)->creationRequest().resourceDescriptor();

			GFXAPIResourceHandle_t handle = 0;

			status = m_backend->load<TResource>(static_cast<GenericProxyBase<TResource>*>(this)->creationRequest(), resolvedDependencies, ETaskSynchronization::Sync, nullptr, handle);
			if( CheckEngineError(status) ) {
				// MBT TODO: Consider distinguishing the above returned status a little more in 
				//           order to reflect UNLOADED or UNAVAILABLE state.
				this->setLoadState(ELoadState::UNLOADED);

				HandleEngineStatusError(EEngineStatus::GFXAPI_LoadResourceFailed, String::format("Failed to load GFXAPI resource '%0'", rd.name));
			}

			this->setHandle(handle);
			this->setLoadState(ELoadState::LOADED);

			return EEngineStatus::Ok;
		}

		template <typename TResource>
		EEngineStatus GFXAPIResourceProxy<TResource>
			::unloadSync()
		{
			EEngineStatus status = EEngineStatus::Ok;
			status = m_backend->unload<TResource>(TResource::DestructionRequest(resourceHandle()));

			if( CheckEngineError(status) ) {
				this->setLoadState(ELoadState::UNKNOWN);

        HandleEngineStatusError(EEngineStatus::GFXAPI_UnloadResourceFailed, String::format("Failed to unload GFXAPI resource '%0'", handle()));
			}

			this->setLoadState(ELoadState::UNLOADED);

			return EEngineStatus::Ok;
		}
	}
}

#endif