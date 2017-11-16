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
				const Ptr<GFXAPIResourceSubSystem>& gfxApiResourceSubsystem)
				: _gfxApiResourceSubsystem(gfxApiResourceSubsystem)
			{ }

			template <typename TResource>
			Ptr<IResourceProxy<TResource>> create(
				const EProxyType                         &proxyType,
				const ResourceCreationRequest<TResource> &creationRequest,
				const ResourceHandleList                 &dependencyHandles) {
				
				Ptr<IResourceProxy<TResource>> proxy = nullptr;

				switch( creationRequest.resourceDescriptor().type() ) {
				// GFXAPI-types
				case EResourceType::BUFFER:
				case EResourceType::GAPI_COMPONENT:
				case EResourceType::GAPI_STATE:
				case EResourceType::GAPI_VIEW:
				case EResourceType::MESH:
				case EResourceType::SHADER:
				case EResourceType::TEXTURE:
					proxy = MakeSharedPointerType<GFXAPIResourceProxy<TResource>>(proxyType, _gfxApiResourceSubsystem, creationRequest, dependencyHandles);
					break;
			    // Other
				default:
					break;
				}

				return proxy;
			}

		private:
			Ptr<GFXAPIResourceSubSystem> _gfxApiResourceSubsystem;
		};
	}
}

#endif