#include "Resources/System/Core/ProxyBasedResourceManager.h"

namespace Engine {
	namespace Resources {


    bool ProxyBasedResourceManager
      ::clear()
    {
    }

		EEngineStatus ProxyBasedResourceManager::createSwapChain(
			const SwapChain::CreationRequest &inRequest,
			Ptr<SwapChain>                   &outSwapChain)
		{
      SwapChain::Binding binding ={};

			EEngineStatus status = createResource<SwapChain>(
				inRequest,
				false,
				binding);

			if( CheckEngineError(status) ) {
				Log::Error(logTag(), "Failed to create swapchain resource.");
				return status;
			}

			const SwapChain::Descriptor& desc = inRequest.resourceDescriptor();
			outSwapChain = SwapChain::create(desc, binding);

			return EEngineStatus::Ok;
		}

		EEngineStatus ProxyBasedResourceManager::createTexture1D(
			const Texture1D::CreationRequest &request,
			Ptr<Texture1D>                   &outTexture1D)
		{
      Texture1D::Binding binding;

			EEngineStatus status = createResource<Texture1D>(
				request,
				false,
				binding);
			if( CheckEngineError(status) ) {
				Log::Error(logTag(), "Failed to create swapchain resource.");
				return status;
			}

			const Texture1D::Descriptor& desc = request.resourceDescriptor();
			outTexture1D = MakeSharedPointerType<Texture1D>(desc, binding);

			return EEngineStatus::Ok;
		}

		EEngineStatus ProxyBasedResourceManager::createTexture2D(
			const Texture2D::CreationRequest &request,
			Ptr<Texture2D>                   &outTexture2D)
		{
      Texture2D::Binding binding;

			EEngineStatus status = createResource<Texture2D>(
				request,
				false,
				binding);
			if( CheckEngineError(status) ) {
				Log::Error(logTag(), "Failed to create swapchain resource.");
				return status;
			}

			const Texture2D::Descriptor& desc = request.resourceDescriptor();

			outTexture2D = MakeSharedPointerType<Texture2D>(desc, binding);
			return EEngineStatus::Ok;
		}

		EEngineStatus ProxyBasedResourceManager::createTexture3D(
			const Texture3D::CreationRequest &request,
			Ptr<Texture3D>                   &outTexture3D)
		{
      Texture3D::Binding binding;

			EEngineStatus status = createResource<Texture3D>(
				request,
				false,
				binding);
			if( CheckEngineError(status) ) {
				Log::Error(logTag(), "Failed to create swapchain resource.");
				return status;
			}

			const Texture3D::Descriptor& desc = request.resourceDescriptor();

			outTexture3D = MakeSharedPointerType<Texture3D>(desc, binding);
			return EEngineStatus::Ok;
		}

    EEngineStatus ProxyBasedResourceManager::createRenderTargetView(
      const RenderTargetView::CreationRequest &request,
      Ptr<RenderTargetView>                   &out)
    {
    }

    EEngineStatus ProxyBasedResourceManager::createShaderResourceView(
      const ShaderResourceView::CreationRequest &request,
      Ptr<ShaderResourceView>                   &out)
    {
    }

    EEngineStatus ProxyBasedResourceManager::createDepthStencilView(
      const DepthStencilView::CreationRequest &request,
      Ptr<DepthStencilView>                   &out)
    {
    }

    EEngineStatus ProxyBasedResourceManager::createDepthStencilState(
      const DepthStencilState::CreationRequest &request,
      Ptr<DepthStencilState>                   &out)
    {
    }
	}
}