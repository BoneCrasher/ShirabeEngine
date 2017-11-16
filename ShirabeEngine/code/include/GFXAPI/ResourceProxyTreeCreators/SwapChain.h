#ifndef __SHIRABE_SWAPCHAINPROXYCREATOR_H__
#define __SHIRABE_SWAPCHAINPROXYCREATOR_H__

#include "Core/EngineStatus.h"
#include "Log/Log.h"

#include "Resources/System/Core/Handle.h"
#include "Resources/System/Core/IResourceProxy.h"
#include "Resources/System/Core/ResourceProxyFactory.h"
#include "Resources/System/Core/ProxyTreeCreator.h"

#include "GFXAPI/Types/SwapChain.h"

namespace Engine {
	namespace Resources {

		template <>
		class ProxyTreeCreator<SwapChain> 
		{
			DeclareLogTag(ProxyTreeCreator<SwapChain>);

		public:
			static const constexpr EResourceType    resource_type    = SwapChain::resource_type;
			static const constexpr EResourceSubType resource_subtype = SwapChain::resource_subtype;

			using binding_type = SwapChain::binding_type;
			using request_type = ResourceCreationRequest<SwapChain>;

			static bool create(
				const Ptr<ResourceProxyFactory> &proxyFactory,
				const request_type              &request,
				ResourceHandleList              &inDependencyHandles,
				binding_type                    &outBinding,
				ResourceProxyMap                &outProxyMap,
				DependerTreeNodeList            &outResourceHierarchy)
			{
				const ResourceDescriptor<SwapChain> desc = request.resourceDescriptor();

				if( desc.backBufferCount < 1 ) {
					Log::Error(logTag(), "Invalid swapchain back buffer count. Expected 'count >= 1'.");
					return false; // Must be at least 1 for double buffering
				}

				ResourceHandle rootHandle(desc.name, EResourceType::GAPI_COMPONENT, EResourceSubType::SWAP_CHAIN);
				Ptr<IResourceProxy<SwapChain>> rootProxy
					= proxyFactory->create<SwapChain>(EProxyType::Persistent, request, inDependencyHandles);

				outBinding.swapChainHandle = rootHandle;
			}
		};
		 
	}

}

#endif