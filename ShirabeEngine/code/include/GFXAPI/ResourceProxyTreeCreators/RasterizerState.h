#ifndef __SHIRABE_DX11RSPROXYCREATOR_H__
#define __SHIRABE_DX11RSPROXYCREATOR_H__

#include "Core/EngineStatus.h"
#include "Log/Log.h"

#include "Resources/System/Core/Handle.h"
#include "Resources/System/Core/IResourceProxy.h"
#include "Resources/System/Core/ResourceProxyFactory.h"
#include "Resources/System/Core/ProxyTreeCreator.h"

#include "GFXAPI/Types/RasterizerState.h"

namespace Engine {
	namespace Resources {

		template <>
		class ProxyTreeCreator<RasterizerState> {
		public:

			static const constexpr EResourceType    resource_type    = RasterizerState::resource_type;
			static const constexpr EResourceSubType resource_subtype = RasterizerState::resource_subtype;
      
			static bool create(
				const Ptr<ResourceProxyFactory>        &proxyFactory,
				const RasterizerState::CreationRequest &request,
        RasterizerState::Binding               &outBinding,
				ResourceProxyMap                       &outProxyMap,
				DependerTreeNodeList                   &outResourceHierarchy)
			{
        RasterizerState::Descriptor const& desc = request.resourceDescriptor();
        Ptr<IResourceProxy<RasterizerState>> proxy
          = proxyFactory->create<RasterizerState>(EProxyType::Dynamic, request);

        ResourceHandle handle(desc.name, resource_type, resource_subtype);
        outProxyMap[handle] = AnyProxy(proxy);

        DependerTreeNode resourceNode;
        resourceNode.resourceHandle = handle;

        RasterizerState::Binding binding;
        binding.handle = handle;

        outResourceHierarchy.push_back(resourceNode);

        return true;
			}
		};
		 
	}

}

#endif