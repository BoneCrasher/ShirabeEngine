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
		template <EResourceType type, EResourceSubType subtype>
		class GFXAPIResourceProxy
			: public ResourceSubsystemProxy<IGFXAPIResourceSubsystem, type, subtype>
			, public GFXAPIResourceAdapter
		{
		public:
			inline GFXAPIResourceProxy(
				const EProxyType                        &proxyType,
				const Ptr<IGFXAPIResourceSubsystem>     &subsystem,
				const ResourceDescriptor<type, subtype> &descriptor)
				: ResourceSubsystemProxy<IGFXAPIResourceSubsystem, type, subtype>(proxyType, subsystem, descriptor)
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

		template <EResourceType type, EResourceSubType subtype>
		bool GFXAPIResourceProxy<type, subtype>
			::loadSync(
				const ResourceHandle   &inHandle,
				const ResourceProxyMap &inDependencies)
		{
			return true;
		}

		template <EResourceType type, EResourceSubType subtype>
		bool GFXAPIResourceProxy<type, subtype>
			::unloadSync()
		{
			return true;
		}
	}
}

#endif