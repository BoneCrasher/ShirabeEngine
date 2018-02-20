#ifndef __SHIRABE_DX11DSSPROXYCREATOR_H__
#define __SHIRABE_DX11DSSPROXYCREATOR_H__

#include "Core/EngineStatus.h"
#include "Log/Log.h"

#include "Resources/System/Core/Handle.h"
#include "Resources/System/Core/IResourceProxy.h"
#include "Resources/System/Core/ResourceProxyFactory.h"
#include "Resources/System/Core/ProxyTreeCreator.h"

#include "GFXAPI/Types/DepthStencilState.h"

namespace Engine {
	namespace Resources {

		template <>
		class ProxyTreeCreator<DepthStencilState> {
		public:

			static const constexpr EResourceType    resource_type    = DepthStencilState::resource_type;
			static const constexpr EResourceSubType resource_subtype = DepthStencilState::resource_subtype;
      
			static bool create(
				const Ptr<ResourceProxyFactory>          &proxyFactory,
				const DepthStencilState::CreationRequest &request,
        DepthStencilState::Binding               &outBinding,
				ResourceProxyMap                         &outProxyMap,
				DependerTreeNodeList                     &outResourceHierarchy)
			{
        DepthStencilState::Descriptor const& desc = request.resourceDescriptor();
        Ptr<IResourceProxy<DepthStencilState>> proxy
          = proxyFactory->create<DepthStencilState>(EProxyType::Dynamic, request);

        ResourceHandle handle(desc.name, resource_type, resource_subtype);
        outProxyMap[handle] = AnyProxy(proxy);

        DependerTreeNode resourceNode;
        resourceNode.resourceHandle = handle;

        DepthStencilState::Binding binding;
        binding.handle = handle;

        outResourceHierarchy.push_back(resourceNode);

        return true;
			}
		};
		 
	}

}

#endif