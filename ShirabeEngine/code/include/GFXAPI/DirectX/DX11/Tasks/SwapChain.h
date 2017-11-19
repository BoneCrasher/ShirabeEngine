#ifndef __SHIRABE_DX11SWAPCHAINTask_H__
#define __SHIRABE_DX11SWAPCHAINTask_H__

#include "Core/EngineStatus.h"
#include "Log/Log.h"

#include "Resources/System/Core/Handle.h"

#include "GFXAPI/Types/SwapChain.h"

#include "GFXAPI/DirectX/DX11/DX11Types.h"
#include "GFXAPI/DirectX/DX11/Tasks/TaskBase.h"

namespace Engine {
	namespace DX {
		namespace _11 {
			using namespace Engine::Resources;
			using namespace GFXAPI;

			/**********************************************************************************************//**
			 * \class	SwapChainTaskImpl
			 *
			 * \brief	A swap chain Task implementation.
			 **************************************************************************************************/
			class SwapChainTaskImpl {
				friend class DX11SwapChainResourceTask;

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
			 * \class	DX11SwapChainResourceTask
			 *
			 * \brief	A dx 11 swap chain resource Task.
			 **************************************************************************************************/
			class DX11SwapChainResourceTask
				: public DX11ResourceTaskBase<SwapChain, IUnknownPtr>
			{
				DeclareLogTag(DX11RenderTargetResourceTask_ID3D11Device);

			public:
				static EEngineStatus build(
					descriptor_type const        &descriptor,
					gfxapi_parameter_struct_type &gfxapiParams,
					built_resource_map           &outResources)
				{
					EEngineStatus status = EEngineStatus::Ok;

					IDXGISwapChainPtr   pSC = nullptr;
					ID3D11Texture2DList backbufferTexturePointers;

					status = DX::_11::SwapChainTaskImpl::createSwapChain(gfxapiParams.device, descriptor, pSC);
					if (CheckEngineError(status)) {
						Log::Error(logTag(), String::format("Cannot create RenderTargetView internal resource for descriptor: %s", descriptor.toString().c_str()));
					}
					else {
						// What to pass to the texND to encapsulate the internal handle and resource? How to recreated it?
						if (CheckEngineError(DX::_11::SwapChainTaskImpl::createBackBufferTextureResources(pSC, backbufferTexturePointers))) {
							status = EEngineStatus::DXSwapChain_AttachSwapChainToBackBuffer_GetBackBufferPtr_Failed;
						}
						else {
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

			DeclareTemplatedSharedPointerType(DX11SwapChainResourceTask, DX11SwapChainResourceTask);
		}

	}

  namespace Resources {
    template <>
    class ResourceTask<SwapChain> {
      typedef DX11SwapChainResourceTask type;
    };
  }
}

#endif