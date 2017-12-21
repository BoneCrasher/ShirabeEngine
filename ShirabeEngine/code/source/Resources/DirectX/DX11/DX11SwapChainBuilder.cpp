#include "GFXAPI/DirectX/DX11/DX11DeviceCapabilities.h"

#include "GFXAPI/DirectX/DX11/Tasks/SwapChain.h"

namespace Engine {
	namespace DX {
		namespace _11 {

			EEngineStatus SwapChainBuilderImpl::createSwapChain (
				const ID3D11DevicePtr       &device,
				const SwapChain::Descriptor &desc,
				IDXGISwapChainPtr           &outRes
			) {
				HRESULT       dxRes = S_OK;
				EEngineStatus eRes  = EEngineStatus::Ok;

				IDXGIFactory1  *pFactory = nullptr;
				IDXGISwapChain *tmp      = nullptr;

				DXGI_SWAP_CHAIN_DESC scDesc ={};
				// Setup backbuffer
				scDesc.BufferCount                 = desc.backBufferCount;
				scDesc.BufferDesc.Width            = desc.texture.dimensions[0];
				scDesc.BufferDesc.Height           = desc.texture.dimensions[1];
				scDesc.BufferDesc.Format           = DX11DeviceCapsHelper::convertFormatGAPI2DXGI(desc.texture.textureFormat);
				scDesc.BufferDesc.Scaling          = DXGI_MODE_SCALING_UNSPECIFIED;
				scDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
				if (desc.vsyncEnabled) {
					scDesc.BufferDesc.RefreshRate.Numerator   = desc.refreshRateNumerator;
					scDesc.BufferDesc.RefreshRate.Denominator = desc.refreshRateDenominator;
				}
				else {
					scDesc.BufferDesc.RefreshRate.Numerator   = 0;
					scDesc.BufferDesc.RefreshRate.Denominator = 1;
				}
				scDesc.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;

				// Multisampling: For now deactivate.
				scDesc.SampleDesc.Count   = 1;
				scDesc.SampleDesc.Quality = 0;
				// Fullscreen?
				scDesc.Windowed     = true; // Do not set immediately! desc.fullscreen;
				// Attach to window
				scDesc.OutputWindow = static_cast<HWND>(reinterpret_cast<void *>(desc.windowHandle));
				scDesc.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;

				dxRes = CreateDXGIFactory(__uuidof(IDXGIFactory1), (void **) &pFactory);
				dxRes = pFactory->CreateSwapChain(device.get(), &scDesc, &tmp);
				if (FAILED(dxRes)) {
					eRes = EEngineStatus::DXGI_SwapChainCreationFailed;
				}
				else {
					outRes = MakeSharedPointerTypeCustomDeleter<IDXGISwapChain>(tmp, DxResourceDeleter<IDXGISwapChain>());
				}

				return eRes;
			}
			
			EEngineStatus SwapChainBuilderImpl::createBackBufferTextureResources(
				const IDXGISwapChainPtr &pInSwapChain,
				ID3D11Texture2DList     &pOutBackBufferResourceHandles
			) {
				// Based on the provided descriptor, we created "n" backbuffers. 
				// Create resource handles for them.

				DXGI_SWAP_CHAIN_DESC  desc;
				ID3D11Texture2D      *tmpBackBufferPtr = nullptr;

				pInSwapChain->GetDesc(&desc);

				for (unsigned int k=0; k<desc.BufferCount; ++k) {
					HRESULT res = pInSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **) &tmpBackBufferPtr);
					if (FAILED(res)) {						
						// What to do here?
						return EEngineStatus::DXGI_SwapChainCreationFailed_BackBufferPointerAcquisition;
					}

					pOutBackBufferResourceHandles.push_back(
						MakeSharedPointerTypeCustomDeleter<ID3D11Texture2D>(tmpBackBufferPtr, DxResourceDeleter<ID3D11Texture2D>())
					);

					// IMPORTANT: MAYBE WE CAN FREE THIS HERE!
					// DxRelease(&tmpBackBufferPtr); // Ptr to underlying ID3D11Texture2D-resource of the backbuffer
												  // is no more needed at this point!
				}

			}
		}
	}
}