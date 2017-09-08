#include "Resources/DirectX/DX11/DX11RenderTargetViewBuilder.h"
#include "GAPI/DirectX/DX11/DX11DeviceCapabilities.h"

namespace Engine {
	namespace DX {
		namespace _11 {

			EEngineStatus createRenderTargetView (
				const ID3D11DevicePtr        &device,
				const RenderTargetDescriptor &desc,
				ID3D11ResourcePtr            &sourceResource,
				ID3D11RenderTargetViewPtr    &outRTV
			) {
				HRESULT       dxRes = S_OK;
				EEngineStatus eRes  = EEngineStatus::Ok;

				D3D11_RENDER_TARGET_VIEW_DESC rtvDesc ={};
				rtvDesc.Format = DX11DeviceCapsHelper::convertFormatGAPI2DXGI(desc._textureFormat);

				switch (desc._dimensions) {
				case 1:
					if (desc._array._isTextureArray) {
						rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
						rtvDesc.Texture1DArray.FirstArraySlice = 0;
						rtvDesc.Texture1DArray.ArraySize       = desc._array._textureCount;
						rtvDesc.Texture1DArray.MipSlice        = 0;
					}
					else {
						rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1D;
						rtvDesc.Texture1D.MipSlice = 0;
					}
					break;
				case 2:
					if (desc._array._isTextureArray) {
						rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
						rtvDesc.Texture2DArray.FirstArraySlice = 0;
						rtvDesc.Texture2DArray.ArraySize       = desc._array._textureCount;
						rtvDesc.Texture2DArray.MipSlice        = 0;
					}
					else {
						rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
						rtvDesc.Texture2D.MipSlice = 0;
					}
					break;
				case 3:
					rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
					rtvDesc.Texture3D.FirstWSlice    = 0;
					rtvDesc.Texture3D.WSize          = desc._array._textureCount;
					rtvDesc.Texture3D.MipSlice       = 0;					
					break;
				}

				ID3D11RenderTargetView *tmp = nullptr;
				dxRes = device->CreateRenderTargetView(sourceResource.get(), &rtvDesc, &tmp);
				if (FAILED(dxRes)) {
					eRes = EEngineStatus::DXDevice_AttachSwapChainToBackBuffer_CreateRTV_Failed;
				}
				else {
					outRTV = MakeSharedPointerTypeCustomDeleter<ID3D11RenderTargetView>(tmp, DxResourceDeleter<ID3D11RenderTargetView>());
				}

				return eRes;
			}
			
		}
	}
}