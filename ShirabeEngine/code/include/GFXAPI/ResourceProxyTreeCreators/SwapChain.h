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

			using binding_type    = SwapChain::binding_type;
			using descriptor_type = ResourceDescriptor<SwapChain>

			static bool create(
				const Ptr<ResourceProxyFactory> &proxyFactory,
				const descriptor_type           &desc,
				ResourceHandleList              &inDependencyHandles,
				binding_type                    &outBinding,
				ResourceProxyMap                &outProxyMap,
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