#ifndef __SHIRABE_IRESOURCEPROXYFACTORY_H__
#define __SHIRABE_IRESOURCEPROXYFACTORY_H__

#include "Core/EngineTypeHelper.h"
#include "Core/EngineStatus.h"

#include "Resources/System/Core/IResourceProxy.h"
#include "Resources/Subsystems/GFXAPI/GFXAPIResourceProxy.h"
#include "Resources/Subsystems/GFXAPI/GFXAPIResourceSubsystem.h"

namespace Engine {
	namespace Resources {
		using namespace GFXAPI;
				
		class ResourceProxyFactory
		{
		public:
			inline ResourceProxyFactory(
				const Ptr<IGFXAPIResourceSubsystem>& gfxApiResourceSubsystem)
				: _gfxApiResourceSubsystem(gfxApiResourceSubsystem)
			{ }

			template <EResourceType type, EResourceSubType subtype>
			Ptr<IResourceProxy<type, subtype>> create(
				const EProxyType                        &proxyType,
				const ResourceDescriptor<type, subtype> &descriptor,
				const ResourceHandleList                &dependencyHandles) {

				using proxy_type = typename ProxyTypeMapper<type, subtype>::type;

				Ptr<IResourceProxy<type, subtype>> proxy = nullptr;

				switch( type ) {
				// GFXAPI-types
				case EResourceType::BUFFER:
				case EResourceType::GAPI_COMPONENT:
				case EResourceType::GAPI_STATE:
				case EResourceType::GAPI_VIEW:
				case EResourceType::MESH:
				case EResourceType::SHADER:
				case EResourceType::TEXTURE:
					proxy = Ptr<GFXAPIResourceProxy<type, subtype>>(new GFXAPIResourceProxy<type, subtype>(proxyType, _gfxApiResourceSubsystem, descriptor, dependencyHandles));
					break;
			    // Other
				default:

				}

				return proxy;
			}

		private:
			Ptr<IGFXAPIResourceSubsystem> _gfxApiResourceSubsystem;
		};
	}
}

#endif