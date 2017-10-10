#ifndef __SHIRABE_DX11SRVPROXYCREATOR_H__
#define __SHIRABE_DX11SRVPROXYCREATOR_H__

#include "Core/EngineStatus.h"
#include "Log/Log.h"

#include "Resources/System/Core/Handle.h"
#include "Resources/System/Core/IResourceProxy.h"

namespace Engine {
	namespace Resources {

		template <>
		class ProxyCreator<EResourceType::GAPI_VIEW, EResourceSubType::SHADER_RESOURCE_VIEW> {
		public:
			using Descriptor = ResourceDescriptor<EResourceType::GAPI_VIEW, EResourceSubType::SHADER_RESOURCE_VIEW>;

			static bool create(
				const Descriptor      &desc,
				ResourceProxyMap      &outProxies/*,
												 ResourceHierarchyNode &outResourceHierarchy*/)
			{
				throw std::exception("Proxy creation undefined for unspecialized type and subtype.");
			}
		};

	}

}

#endif