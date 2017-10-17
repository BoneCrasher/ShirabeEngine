#include "Resources/System/Core/ProxyBasedResourceManager.h"

namespace Engine {
	namespace Resources {

		EEngineStatus ProxyBasedResourceManager::createSwapChain(
			const SwapChainDescriptor &inDesc,
			Ptr<SwapChain>            &outSwapChain)
		{
			SwapChainResourceBinding binding;

			EEngineStatus status = createResource<SwapChain>(
				inDesc,
				false,
				binding);

			if( CheckEngineError(status) ) {
				Log::Error(logTag(), "Failed to create swapchain resource.");
				return status;
			}

			outSwapChain = SwapChain::create(inDesc, binding);
			return EEngineStatus::Ok;
		}

		EEngineStatus ProxyBasedResourceManager::createTexture1D(
			const Texture1DDescriptor &desc,
			Ptr<Texture1D>            &outTexture1D)
		{
			TextureNDResourceBinding binding;

			EEngineStatus status = createResource<Texture1D>(
				desc,
				false,
				binding);
			if( CheckEngineError(status) ) {
				Log::Error(logTag(), "Failed to create swapchain resource.");
				return status;
			}

			outTexture1D = Texture1D::create(desc, binding);
			return EEngineStatus::Ok;
		}

		EEngineStatus ProxyBasedResourceManager::createTexture2D(
			const Texture2DDescriptor &desc,
			Ptr<Texture2D>            &outTexture2D)
		{
			TextureNDResourceBinding binding;

			EEngineStatus status = createResource<Texture2D>(
				desc,
				false,
				binding);
			if( CheckEngineError(status) ) {
				Log::Error(logTag(), "Failed to create swapchain resource.");
				return status;
			}

			outTexture2D = Texture2D::create(desc, binding);
			return EEngineStatus::Ok;
		}

		EEngineStatus ProxyBasedResourceManager::createTexture3D(
			const Texture3DDescriptor &desc,
			Ptr<Texture3D>            &outTexture3D)
		{
			TextureNDResourceBinding binding;

			EEngineStatus status = createResource<Texture3D>(
				desc,
				false,
				binding);
			if( CheckEngineError(status) ) {
				Log::Error(logTag(), "Failed to create swapchain resource.");
				return status;
			}

			outTexture3D = Texture3D::create(desc, binding);
			return EEngineStatus::Ok;
		}
	}
}