#ifndef __SHIRABE_DX11RTCPROXYCREATOR_H__
#define __SHIRABE_DX11RTCPROXYCREATOR_H__

#include "Core/EngineStatus.h"
#include "Log/Log.h"

#include "Resources/System/Core/Handle.h"
#include "Resources/System/Core/IResourceProxy.h"
#include "Resources/System/Core/ResourceProxyFactory.h"
#include "Resources/System/Core/ProxyTreeCreator.h"

#include "GFXAPI/Types/RenderTargetView.h"

namespace Engine {
	namespace Resources {

		template <>
		class ProxyTreeCreator<RenderTargetView> {
		public:

			static const constexpr EResourceType    resource_type    = RenderTargetView::resource_type;
			static const constexpr EResourceSubType resource_subtype = RenderTargetView::resource_subtype;

			using binding_type    = RenderTargetView::binding_type;
			using descriptor_type = ResourceDescriptor<RenderTargetView>;

			static bool create(
				const Ptr<ResourceProxyFactory> &proxyFactory,
				const descriptor_type           &desc,
				ResourceHandleList              &inDependencyHandles,
				binding_type                    &outBinding,
				ResourceProxyMap                &outProxyMap,
				DependerTreeNodeList            &outResourceHierarchy)
			{
				throw std::exception("Proxy creation undefined for unspecialized type and subtype.");
			}
		};
		 
	}

}

#endif