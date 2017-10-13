#ifndef __SHIRABE_DX11RTCPROXYCREATOR_H__
#define __SHIRABE_DX11RTCPROXYCREATOR_H__

#include "Core/EngineStatus.h"
#include "Log/Log.h"

#include "Resources/System/Core/Handle.h"
#include "Resources/System/Core/IResourceProxy.h"
#include "Resources/System/Core/ResourceProxyFactory.h"
#include "Resources/System/Core/ProxyTreeCreator.h"

#include "Resources/Types/RenderTarget.h"

namespace Engine {
	namespace Resources {

		template <>
		class ProxyTreeCreator<EResourceType::GAPI_VIEW, EResourceSubType::RENDER_TARGET_VIEW, RenderTargetResourceBinding> {
		public:
			using Descriptor = ResourceDescriptor<EResourceType::GAPI_VIEW, EResourceSubType::RENDER_TARGET_VIEW>;

			static bool create(
				const Ptr<ResourceProxyFactory> &proxyFactory,
				const Descriptor                &desc,
				ResourceHandleList              &inDependencyHandles,
				RenderTargetResourceBinding     &outBinding,
				ResourceProxyMap                &outProxyMap,
				DependerTreeNodeList            &outResourceHierarchy)
			{
				throw std::exception("Proxy creation undefined for unspecialized type and subtype.");
			}
		};
		 
	}

}

#endif