#include "Resources/System/Core/ProxyBasedResourceManager.h"

namespace Engine {
	namespace Resources {

		EEngineStatus ProxyBasedResourceManager::createSwapChain(
			const ResourceCreationRequest<SwapChain> &inRequest,
			Ptr<SwapChain>                           &outSwapChain)
		{
			ResourceBinding<SwapChain> binding;

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
			ResourceBinding<Texture1D> binding;

			EEngineStatus status = createResource<Texture1D>(
				request,
				false,
				binding);
			if( CheckEngineError(status) ) {
				Log::Error(logTag(), "Failed to create swapchain resource.");
				return status;
			}

			const ResourceDescriptor<Texture1D>& desc = static_cast<const ResourceDescriptor<Texture1D>&>(request.resourceDescriptor());

			outTexture1D = MakeSharedPointerType<Texture1D>(desc, binding);
			return EEngineStatus::Ok;
		}

		EEngineStatus ProxyBasedResourceManager::createTexture2D(
			const ResourceCreationRequest<Texture2D> &request,
			Ptr<Texture2D>                           &outTexture2D)
		{
			ResourceBinding<Texture2D> binding;

			EEngineStatus status = createResource<Texture2D>(
				request,
				false,
				binding);
			if( CheckEngineError(status) ) {
				Log::Error(logTag(), "Failed to create swapchain resource.");
				return status;
			}

			const ResourceDescriptor<Texture2D>& desc = static_cast<const ResourceDescriptor<Texture2D>&>(request.resourceDescriptor());

			outTexture2D = MakeSharedPointerType<Texture2D>(desc, binding);
			return EEngineStatus::Ok;
		}

		EEngineStatus ProxyBasedResourceManager::createTexture3D(
			const ResourceCreationRequest<Texture3D> &request,
			Ptr<Texture3D>                           &outTexture3D)
		{
			ResourceBinding<Texture3D> binding;

			EEngineStatus status = createResource<Texture3D>(
				request,
				false,
				binding);
			if( CheckEngineError(status) ) {
				Log::Error(logTag(), "Failed to create swapchain resource.");
				return status;
			}

			const ResourceDescriptor<Texture3D>& desc = static_cast<const ResourceDescriptor<Texture3D>&>(request.resourceDescriptor());

			outTexture3D = MakeSharedPointerType<Texture3D>(desc, binding);
			return EEngineStatus::Ok;
		}
	}
}