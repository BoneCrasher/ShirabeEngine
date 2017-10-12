#ifndef __SHIRABE_SWAPCHAINPROXYCREATOR_H__
#define __SHIRABE_SWAPCHAINPROXYCREATOR_H__

#include "Core/EngineStatus.h"
#include "Log/Log.h"

#include "Resources/Types/SwapChain.h"

#include "Resources/System/Core/Handle.h"
#include "Resources/System/Core/IResourceProxy.h"
#include "Resources/System/Core/ResourceProxyFactory.h"
#include "Resources/System/Core/ProxyTreeCreator.h"

namespace Engine {
	namespace Resources {

		template <>
		class ProxyTreeCreator<EResourceType::GAPI_COMPONENT, EResourceSubType::SWAP_CHAIN, SwapChainResourceBinding> 
		{
			DeclareLogTag(ProxyTreeCreator<SwapChain>);

		public:
			using Descriptor = ResourceDescriptor<EResourceType::GAPI_COMPONENT, EResourceSubType::SWAP_CHAIN>;

			static bool create(
				const Ptr<ResourceProxyFactory> &proxyFactory,
				const Descriptor                &desc,
				ResourceHandleList              &inDependencyHandles,
				SwapChainResourceBinding        &outBinding,
				DependerTreeNodeList            &outResourceHierarchy)
			{
				if( desc.backBufferCount < 1 ) {
					Log::Error(logTag(), "Invalid swapchain back buffer count. Expected 'count >= 1'.");
					return false; // Must be at least 1 for double buffering
				}

				ResourceHandle rootHandle(desc.name, EResourceType::GAPI_COMPONENT, EResourceSubType::SWAP_CHAIN);
				Ptr<IResourceProxy<EResourceType::GAPI_COMPONENT, EResourceSubType::SWAP_CHAIN>> rootProxy
					= proxyFactory->create<EResourceType::GAPI_COMPONENT, EResourceSubType::SWAP_CHAIN>(EProxyType::Persistent, desc, inDependencyHandles);

				outBinding.swapChainHandle = rootHandle;
			}
		};
		 
	}

}

#endif