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
#include "Resources/Subsystems/GFXAPI/GFXAPI.h"
#include "Resources/Subsystems/GFXAPI/GFXAPIResourceSubsystem.h"

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

		protected:
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
			: public ResourceSubsystemProxy<IGFXAPIResourceSubsystem, TResource>
			, public GFXAPIResourceAdapter
		{
		public:
			inline GFXAPIResourceProxy(
				const EProxyType                    &proxyType,
				const Ptr<IGFXAPIResourceSubsystem> &subsystem,
				const ResourceDescriptor<TResource> &descriptor)
				: ResourceSubsystemProxy<IGFXAPIResourceSubsystem, TResource>(proxyType, subsystem, descriptor)
				, GFXAPIResourceAdapter(GFXAPIUninitializedResourceHandle)
			{
			}

			bool loadSync(
				const ResourceHandle   &inHandle,
				const ResourceProxyMap &inDependencies);

			bool unloadSync();

			virtual bool bind(
				const GFXAPIResourceHandle_t &gfxApiHandle,
				const ResourceProxyMap       &inDependencies) = 0;

		protected:
		private:
		};

		template <typename TResource>
		bool GFXAPIResourceProxy<TResource>
			::loadSync(
				const ResourceHandle   &inHandle,
				const ResourceProxyMap &inDependencies)
		{
			_subsystem-> ? ;
			return true;
		}

		template <typename TResource>
		bool GFXAPIResourceProxy<TResource>
			::unloadSync()
		{
			return true;
		}
	}
}

#endif