#include "Resources/System/Core/ProxyBasedResourceManager.h"

namespace Engine {
	namespace Resources {

		EEngineStatus ProxyBasedResourceManager::createSwapChain(
			const ResourceCreationRequest<SwapChain> &inRequest,
			Ptr<SwapChain>                           &outSwapChain)
		{
			SwapChainResourceBinding binding;

			EEngineStatus status = createResource<SwapChain>(
				inRequest,
				false,
				binding);

			if( CheckEngineError(status) ) {
				Log::Error(logTag(), "Failed to create swapchain resource.");
				return status;
			}

			const SwapChainDescriptor& desc = static_cast<const SwapChainDescriptor&>(inRequest.resourceDescriptor());
			outSwapChain = SwapChain::create(desc, binding);
			return EEngineStatus::Ok;
		}

		EEngineStatus ProxyBasedResourceManager::createTexture1D(
			const ResourceCreationRequest<Texture1D> &request,
			Ptr<Texture1D>                           &outTexture1D)
		{
			TextureNDResourceBinding binding;

			EEngineStatus status = createResource<Texture1D>(
				request,
				false,
				binding);
			if( CheckEngineError(status) ) {
				Log::Error(logTag(), "Failed to create swapchain resource.");
				return status;
			}

			const ResourceDescriptor<Texture1D>& desc = static_cast<const ResourceDescriptor<Texture1D>&>(request.resourceDescriptor());

			outTexture1D = Texture1D::create(desc, binding);
			return EEngineStatus::Ok;
		}

		EEngineStatus ProxyBasedResourceManager::createTexture2D(
			const ResourceCreationRequest<Texture2D> &request,
			Ptr<Texture2D>                           &outTexture2D)
		{
			TextureNDResourceBinding binding;

			EEngineStatus status = createResource<Texture2D>(
				request,
				false,
				binding);
			if( CheckEngineError(status) ) {
				Log::Error(logTag(), "Failed to create swapchain resource.");
				return status;
			}

			const ResourceDescriptor<Texture2D>& desc = static_cast<const ResourceDescriptor<Texture2D>&>(request.resourceDescriptor());

			outTexture2D = Texture2D::create(desc, binding);
			return EEngineStatus::Ok;
		}

		EEngineStatus ProxyBasedResourceManager::createTexture3D(
			const ResourceCreationRequest<Texture3D> &request,
			Ptr<Texture3D>                           &outTexture3D)
		{
			TextureNDResourceBinding binding;

			EEngineStatus status = createResource<Texture3D>(
				request,
				false,
				binding);
			if( CheckEngineError(status) ) {
				Log::Error(logTag(), "Failed to create swapchain resource.");
				return status;
			}

			const ResourceDescriptor<Texture3D>& desc = static_cast<const ResourceDescriptor<Texture3D>&>(request.resourceDescriptor());

			outTexture3D = Texture3D::create(desc, binding);
			return EEngineStatus::Ok;
		}
	}
}