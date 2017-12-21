#ifndef __SHIRABE_GFXAPIRESOURCEPROXY_H__
#define __SHIRABE_GFXAPIRESOURCEPROXY_H__

#include <type_traits>
#include <typeinfo>
#include <optional>
#include <stdint.h>

#include "Core/EngineTypeHelper.h"
#include "Core/EngineStatus.h"
#include "IOC/Observer.h"

#include "Resources/System/Core/ResourceSubsystemProxy.h"
#include "Resources/Subsystems/GFXAPI/GFXAPIResourceSubsystem.h"
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
			inline const GFXAPIResourceHandle_t& handle() const { return _handle; }

		protected:
			inline GFXAPIResourceAdapter(const GFXAPIResourceHandle_t& handle = GFXAPIUninitializedResourceHandle)
				: _handle(handle)
			{ }

			inline bool setHandle(const GFXAPIResourceHandle_t& handle) {
				if( _handle > 0 )
					return false; // Overwrite not allowed!
				_handle = handle;
			}

		private:
			GFXAPIResourceHandle_t _handle;
		};

		/**********************************************************************************************//**
		 * \class	PlatformResourceProxy
		 *
		 * \brief	A platform resource wrapper.
		 **************************************************************************************************/
		template <typename TResource>
		class GFXAPIResourceProxy
			: public ResourceSubsystemProxy<GFXAPIResourceSubSystem, TResource>
			, public GFXAPIResourceAdapter
		{
			DeclareLogTag(GFXAPIResourceProxy<TResource>);

		public:
			inline GFXAPIResourceProxy(
				EProxyType                          const&proxyType,
				Ptr<GFXAPIResourceSubSystem>        const&subsystem,
				typename TResource::CreationRequest const&request)
				: ResourceSubsystemProxy<GFXAPIResourceSubSystem, TResource>(proxyType, subsystem, request)
				, GFXAPIResourceAdapter(GFXAPIUninitializedResourceHandle)
			{ }

			EEngineStatus loadSync(
				const ResourceHandle   &inHandle,
				const ResourceProxyMap &inDependencies);

			EEngineStatus unloadSync();

			virtual bool bind(
				const GFXAPIResourceHandle_t &gfxApiHandle,
				const ResourceProxyMap       &inDependencies) = 0;

		protected:
		private:
			Ptr<GFXAPIResourceSubSystem> _subsystem;
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
				const ResourceHandle   &inHandle,
				const ResourceProxyMap &inDependencies)
		{
			this->setLoadState(ELoadState::LOADING);

			// Make sure to extract the GFXAPI-resource handles from the dependencies
			GFXAPIResourceHandleMap inDependencyHandles;
			for( const ResourceProxyMap::value_type& pair : inDependencies ) {
				Ptr<GFXAPIResourceProxy<TResource>> dependencyProxy = GFXAPIProxyCast<TResource>(pair.second);
				if( !dependencyProxy )
					continue;

				inDependencyHandles[pair.first] = dependencyProxy->handle();
			}
			
			// Request synchronous resource load and if successful, set the internal handle 
			// and load state.
			EEngineStatus status = EEngineStatus::Ok; 

      const typename TResource::Descriptor& rd = static_cast<GenericProxyBase<TResource>*>(this)->creationRequest().resourceDescriptor();

			GFXAPIResourceHandle_t handle = 0;

			status = _subsystem->load<TResource>(rd, inDependencyHandles, ETaskSynchronization::Sync, nullptr, handle);
			if( CheckEngineError(status) ) {
				// MBT TODO: Consider distinguishing the above returned status a little more in 
				//           order to reflect UNLOADED or UNAVAILABLE state.
				this->setLoadState(ELoadState::UNLOADED);

				Log::Error(logTag(), String::format("Failed to load GFXAPI resource '%0'", rd.name));

				return EEngineStatus::GFXAPI_LoadResourceFailed;
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
			status = _subsystem->unload<TResource>(handle(), ETaskSynchronization::Sync);

			if( CheckEngineError(status) ) {
				this->setLoadState(ELoadState::UNKNOWN);

				Log::Error(logTag(), String::format("Failed to unload GFXAPI resource '%0'", handle()));

				return EEngineStatus::GFXAPI_UnloadResourceFailed;
			}

			this->setLoadState(ELoadState::UNLOADED);

			return EEngineStatus::Ok;
		}
	}
}

#endif