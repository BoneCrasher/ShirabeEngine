#ifndef __SHIRABE_DX11SWAPCHAINBUILDER_H__
#define __SHIRABE_DX11SWAPCHAINBUILDER_H__

#include "Core/EngineStatus.h"
#include "Log/Log.h"

#include "Resources/System/Core/Handle.h"

#include "Resources/Types/SwapChain.h"

#include "GFXAPI/DirectX/DX11/DX11Types.h"
#include "Resources/DirectX/DX11/Builders/BuilderBase.h"

namespace Engine {
	namespace DX {
		namespace _11 {
			using namespace Engine::Resources;
			using namespace GFXAPI;

			/**********************************************************************************************//**
			 * \class	SwapChainBuilderImpl
			 *
			 * \brief	A swap chain builder implementation.
			 **************************************************************************************************/
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

			/**********************************************************************************************//**
			 * \class	DX11SwapChainResourceBuilder
			 *
			 * \brief	A dx 11 swap chain resource builder.
			 **************************************************************************************************/
			class DX11SwapChainResourceBuilder
				: public DX11ResourceBuilderBase<EResourceType::GAPI_COMPONENT, EResourceSubType::SWAP_CHAIN, IUnknownPtr>
			{
				DeclareLogTag(DX11RenderTargetResourceBuilder_ID3D11Device);

			public:
				static EEngineStatus build(
					descriptor_type const        &descriptor,
					gfxapi_parameter_struct_type &gfxapiParams,
					built_resource_map           &outResources)
				{
					EEngineStatus status = EEngineStatus::Ok;

					IDXGISwapChainPtr   pSC = nullptr;
					ID3D11Texture2DList backbufferTexturePointers;

					status = DX::_11::SwapChainBuilderImpl::createSwapChain(gfxapiParams.device, descriptor, pSC);
					if (CheckEngineError(status)) {
						Log::Error(logTag(), String::format("Cannot create RenderTargetView internal resource for descriptor: %s", descriptor.toString().c_str()));
					}
					else {
						// What to pass to the texND to encapsulate the internal handle and resource? How to recreated it?
						if (CheckEngineError(DX::_11::SwapChainBuilderImpl::createBackBufferTextureResources(pSC, backbufferTexturePointers))) {
							status = EEngineStatus::DXSwapChain_AttachSwapChainToBackBuffer_GetBackBufferPtr_Failed;
						}
						else {
							// TODO: Create resource hierarchy/dependencies for automatic release.

							ResourceHandle sc(descriptor.name, resource_type, resource_subtype);
							outResources[sc] = pSC;
							for( unsigned int k=0; k < backbufferTexturePointers.size(); ++k ) {
								const ID3D11Texture2DPtr &t = backbufferTexturePointers[k];
								ResourceHandle pT(String::format("%0_%1", descriptor.name, k), EResourceType::TEXTURE, EResourceSubType::TEXTURE_2D);
								outResources[pT] = t;
							}
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