#ifndef __SHIRABE_DX11DSVPROXYCREATOR_H__
#define __SHIRABE_DX11DSVPROXYCREATOR_H__

#include "Core/EngineStatus.h"
#include "Log/Log.h"

#include "Resources/System/Core/Handle.h"
#include "Resources/System/Core/IResourceProxy.h"
#include "Resources/System/Core/ResourceProxyFactory.h"
#include "Resources/System/Core/ProxyTreeCreator.h"

#include "GFXAPI/Types/DepthStencilView.h"

namespace Engine {
	namespace Resources {

		template <>
		class ProxyTreeCreator<DepthStencilView> {
		public:

			static const constexpr EResourceType    resource_type    = DepthStencilView::resource_type;
			static const constexpr EResourceSubType resource_subtype = DepthStencilView::resource_subtype;
      
			static bool create(
				const Ptr<ResourceProxyFactory>         &proxyFactory,
				const DepthStencilView::CreationRequest &request,
        DepthStencilView::Binding               &outBinding,
				ResourceProxyMap                        &outProxyMap,
				DependerTreeNodeList                    &outResourceHierarchy)
			{
        DepthStencilView::Descriptor const& desc = request.resourceDescriptor();
        Ptr<IResourceProxy<DepthStencilView>> proxy
          = proxyFactory->create<DepthStencilView>(EProxyType::Dynamic, request);

        ResourceHandle handle(desc.name, resource_type, resource_subtype);
        outProxyMap[handle] = AnyProxy(proxy);

        DependerTreeNode resourceNode;
        resourceNode.resourceHandle = handle;

        DepthStencilView::Binding binding;
        binding.handle = handle;

        outResourceHierarchy.push_back(resourceNode);

        return true;
			}
		};
		 
	}

}

#endif