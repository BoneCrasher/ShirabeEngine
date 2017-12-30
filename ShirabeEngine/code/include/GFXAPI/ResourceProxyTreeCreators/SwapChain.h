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
      
			static bool create(
				const Ptr<ResourceProxyFactory>  &proxyFactory,
				const SwapChain::CreationRequest &request,
				SwapChain::Binding               &outBinding,
				ResourceProxyMap                 &outProxyMap,
				DependerTreeNodeList             &outResourceHierarchy)
			{
        SwapChain::Descriptor const& desc = request.resourceDescriptor();
				if( desc.backBufferCount < 1 ) {
					Log::Error(logTag(), "Invalid swapchain back buffer count. Expected 'count >= 1'.");
					return false; // Must be at least 1 for double buffering
				}

				ResourceHandle swapChainHandle(desc.name, SwapChain::resource_type, SwapChain::resource_subtype);
				Ptr<IResourceProxy<SwapChain>> swapChainProxy
					= proxyFactory->create<SwapChain>(EProxyType::Persistent, request);

        SwapChain::Binding binding   ={};
        DependerTreeNode   hierarchy ={};
        ResourceProxyMap   proxies   ={};

        binding.swapChainHandle  = swapChainHandle;
        hierarchy.resourceHandle = swapChainHandle;
        proxies[swapChainHandle] = swapChainProxy;

        binding.backBufferRenderTargetBindings.resize(desc.backBufferCount);

        for(uint32_t k=0; k < desc.backBufferCount; ++k) {
          // Create BackBuffer resource proxy used to "Hook into" the swap chain's internal
          // buffers (Pass the swapchain as dependency to the request...)
          // As these buffers are not necessarily regular textures but can be 
          // accessed as if, we can nonetheless store 'em in the manager as regular texture resources.
          // The only difference is the load-mode (Persistent), since we have only one real chance
          // (or it should appear as if) to fetch them.
          // They may not be manually freed!
          SwapChainBuffer::Descriptor backBufferDesc    ={};
          backBufferDesc.name    = desc.name + "_BackBuffer_" + (char)('0' + k);
          backBufferDesc.texture = desc.texture;
          SwapChainBuffer::CreationRequest backBufferCreationRequest(backBufferDesc, swapChainHandle);          

          Ptr<IResourceProxy<SwapChainBuffer>> backBufferProxy
            = proxyFactory->create<SwapChainBuffer>(EProxyType::Persistent, backBufferCreationRequest);

          ResourceHandle backBufferHandle(backBufferDesc.name, SwapChainBuffer::resource_type, SwapChainBuffer::resource_subtype);
          proxies[backBufferHandle] = AnyProxy(backBufferProxy);

          // Create an RTV for the swapchain back buffer, so that we can "render into it" as the final 
          // per-frame render step!
          // Since we stored the backbuffers as regular textures, we can simply register them as 
          // underlying texture dependencies in the RTV.
          // Here as well, we treat the RTV's as "immutable", so that we don't mess around with the 
          // basic infrastructure, once created.
          RenderTargetView::Descriptor backBufferRTVDesc ={};
          backBufferRTVDesc.dimensionNb   = backBufferDesc.texture.dimensionNb;
          backBufferRTVDesc.array         = backBufferDesc.texture.array;
          backBufferRTVDesc.mipMap        = backBufferDesc.texture.mipMap;
          backBufferRTVDesc.textureFormat = backBufferDesc.texture.textureFormat;
          backBufferRTVDesc.name          = desc.name + "_BackBufferRTV_" + (char)('0' + k);
          RenderTargetView::CreationRequest backBufferRTVCreationRequest(backBufferRTVDesc, backBufferHandle);       

          Ptr<IResourceProxy<RenderTargetView>> backBufferRTVProxy
            = proxyFactory->create<RenderTargetView>(EProxyType::Persistent, backBufferRTVCreationRequest);

          ResourceHandle backBufferRTVHandle(backBufferRTVDesc.name, RenderTargetView::resource_type, RenderTargetView::resource_subtype);   
          proxies[backBufferRTVHandle] = AnyProxy(backBufferRTVProxy);
          
          // Setup the public bindings of the engine resource objects to the resource manager.
          RenderTargetView::Binding backBufferRTVBinding ={};
          backBufferRTVBinding.handle = backBufferRTVHandle;

          SwapChainBuffer::Binding backBufferBinding={};
          backBufferBinding.handle           = backBufferHandle;
          backBufferBinding.renderTargetView = backBufferRTVBinding;

          binding.backBufferRenderTargetBindings[k] = backBufferBinding;

          // Finally, setup the resource hierarchy for proper resource destruction!
          DependerTreeNode backBufferNode={};   
          DependerTreeNode backBufferRTVNode={};   
          backBufferNode.resourceHandle    = backBufferHandle;
          backBufferRTVNode.resourceHandle = backBufferRTVHandle;
          backBufferNode.children.push_back(backBufferRTVNode);
          hierarchy.children.push_back(backBufferNode);
        }

        outBinding           = binding;
        outProxyMap          = proxies;
        outResourceHierarchy.push_back(hierarchy);

        return true;
			}
		};
		 
	}

}

#endif