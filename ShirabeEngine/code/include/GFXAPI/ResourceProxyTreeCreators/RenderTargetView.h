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
      
      static bool create(
        const Ptr<ResourceProxyFactory>         &proxyFactory,
        const RenderTargetView::CreationRequest &request,
				RenderTargetView::Binding               &outBinding,
				ResourceProxyMap                        &outProxyMap,
				DependerTreeNodeList                    &outResourceHierarchy)
			{
        RenderTargetView::Descriptor const& desc = request.resourceDescriptor();
        Ptr<IResourceProxy<RenderTargetView>> proxy
          = proxyFactory->create<RenderTargetView>(EProxyType::Dynamic, request);

        ResourceHandle handle(desc.name, EResourceType::GAPI_VIEW, EResourceSubType::RENDER_TARGET_VIEW);
        outProxyMap[handle] = AnyProxy(proxy);

        DependerTreeNode resourceNode;
        resourceNode.resourceHandle = handle;

        RenderTargetView::Binding binding;
        binding.handle = handle;

        outResourceHierarchy.push_back(resourceNode);
			}
		};
		 
	}

}

#endif