#ifndef __SHIRABE_DX11SWAPCHAINBUILDER_H__
#define __SHIRABE_DX11SWAPCHAINBUILDER_H__

#include "Core/EngineStatus.h"
#include "Log/Log.h"

#include "Resources/Handle.h"
#include "Resources/ResourceBuilder.h"
#include "Resources/SwapChain.h"

#include "GAPI/DirectX/DX11/DX11Types.h"

namespace Engine {
	namespace DX {
		namespace _11 {
			using namespace Engine::Resources;
			using namespace GAPI;

			class SwapChainBuilderImpl {
				friend class DX11SwapChainResourceBuilder;

				static EEngineStatus createSwapChain (
					const ID3D11DevicePtr        &device,
					const SwapChainDescriptor    &desc,
					IDXGISwapChainPtr            &outRTV
				);

				static EEngineStatus createBackBufferTextureResources(
					const IDXGISwapChainPtr &pInSwapChain,
					ID3D11Texture2DList     &pOutBackBufferResources
				);
			};


			class DX11SwapChainResourceBuilder
				: public ResourceBuilderBase<ID3D11Device, EResourceType::GAPI_COMPONENT, EResourceSubType::SWAP_CHAIN, IDXGISwapChainPtr>
			{
				DeclareLogTag(DX11RenderTargetResourceBuilder_ID3D11Device);

			public:
				static EEngineStatus build(
					typename traits_type::TGAPIDevicePtr        &gapiDevice,
					typename const traits_type::descriptor_type &descriptor,
					typename traits_type::resource_type_ptr     &outResource)
				{
					EEngineStatus status = EEngineStatus::Ok;

					IDXGISwapChainPtr   pRes = nullptr;
					ID3D11Texture2DList pBackbufferPtrs;

					status = DX::_11::SwapChainBuilderImpl::createSwapChain(gapiDevice, descriptor, pRes);
					if (CheckEngineError(status)) {
						Log::Error(logTag(), String::format("Cannot create RenderTargetView internal resource for descriptor: %s", descriptor.toString().c_str()));
					}
					else {
						// What to pass to the texND to encapsulate the internal handle and resource? How to recreated it?
						if (CheckEngineError(DX::_11::SwapChainBuilderImpl::createBackBufferTextureResources(pRes, pBackbufferPtrs))) {
							status = EEngineStatus::DXSwapChain_AttachSwapChainToBackBuffer_GetBackBufferPtr_Failed;
						}
						else {
							// Todo: Here it becomes tricky: The signature only expects a single resource as output! 
							// Consider moving the whole backbuffer fetch into separate helper function visible to the resource manager, OR, open up the interface to create a resource with an existing TEX2D.
							outResource = std::move(pRes);
						}
					}

					return status;
				}
			};

			DeclareTemplatedSharedPointerType(DX11SwapChainResourceBuilder, DX11SwapChainResourceBuilder);
		}

	}
}

#endif