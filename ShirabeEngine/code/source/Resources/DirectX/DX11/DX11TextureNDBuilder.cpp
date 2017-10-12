#include "GFXAPI/DirectX/DX11/DX11DeviceCapabilities.h"

#include "Resources/DirectX/DX11/Builders/TextureND.h"

namespace Engine {
	namespace DX {
		namespace _11 {

			EEngineStatus createTexture1D
			(
				const ID3D11DevicePtr     &device,
				const Texture1DDescriptor &desc,
				ID3D11Texture1DPtr        &outTexture
			) {
				HRESULT       dxRes = S_OK;
				EEngineStatus eRes  = EEngineStatus::Ok;

				D3D11_TEXTURE1D_DESC texDesc ={};
				texDesc.Width     = desc.dimensions[0];
				texDesc.Format    = DX11DeviceCapsHelper::convertFormatGAPI2DXGI(desc.textureFormat);
				texDesc.MipLevels = (desc.mipMap.useMipMaps    ? desc.mipMap.mipLevels   : 1);
				texDesc.ArraySize = (desc.array.isTextureArray ? desc.array.size : 1);
				texDesc.Usage     = DX11DeviceCapsHelper::convertResourceUsageGAPI2D3D11(desc.cpuGpuUsage);
				texDesc.BindFlags = DX11DeviceCapsHelper::convertBufferBindingGAPI2D3D11(desc.gpuBinding);
				// Additional flags
				texDesc.CPUAccessFlags = 0;
				texDesc.MiscFlags      = 0;

				ID3D11Texture1D *tmpTex = nullptr;
				dxRes = device->CreateTexture1D(&texDesc, nullptr, &tmpTex);
				if (FAILED(dxRes)) {
					eRes = EEngineStatus::DXDevice_Texture1DCreationFailed;
				}
				else {
					outTexture = MakeSharedPointerTypeCustomDeleter<ID3D11Texture1D>(tmpTex, DxResourceDeleter<ID3D11Texture1D>());
				}

				return eRes;
			}

			EEngineStatus createTexture2D
			(
				const ID3D11DevicePtr     &device,
				const Texture2DDescriptor &desc,
				ID3D11Texture2DPtr        &outTexture
			) {
				HRESULT       dxRes = S_OK;
				EEngineStatus eRes = EEngineStatus::Ok;

				D3D11_TEXTURE2D_DESC texDesc ={};
				texDesc.Width     = desc.dimensions[0];
				texDesc.Height    = desc.dimensions[1];
				texDesc.Format    = DX11DeviceCapsHelper::convertFormatGAPI2DXGI(desc.textureFormat);
				texDesc.MipLevels = (desc.mipMap.useMipMaps    ? desc.mipMap.mipLevels   : 1);
				texDesc.ArraySize = (desc.array.isTextureArray ? desc.array.size : 1);
				texDesc.Usage     = DX11DeviceCapsHelper::convertResourceUsageGAPI2D3D11(desc.cpuGpuUsage);
				texDesc.BindFlags = DX11DeviceCapsHelper::convertBufferBindingGAPI2D3D11(desc.gpuBinding);
				// Multisampling
				if (desc.multisampling.useMultisampling) {
					texDesc.SampleDesc.Count   = desc.multisampling.size;
					texDesc.SampleDesc.Quality = desc.multisampling.quality;
				}
				else {
					texDesc.SampleDesc.Count   = 0;
					texDesc.SampleDesc.Quality = 0;
				}

				// Additional flags
				texDesc.CPUAccessFlags = 0;
				texDesc.MiscFlags      = 0;

				ID3D11Texture2D *tmpTex = nullptr;
				dxRes = device->CreateTexture2D(&texDesc, nullptr, &tmpTex);
				if (FAILED(dxRes)) {
					eRes = EEngineStatus::DXDevice_Texture2DCreationFailed;
				}
				else {
					outTexture = MakeSharedPointerTypeCustomDeleter<ID3D11Texture2D>(tmpTex, DxResourceDeleter<ID3D11Texture2D>());
				}

				return eRes;
			}

			EEngineStatus createTexture3D
			(
				const ID3D11DevicePtr     &device,
				const Texture3DDescriptor &desc,
				ID3D11Texture3DPtr        &outTexture
			) {
				HRESULT       dxRes = S_OK;
				EEngineStatus eRes = EEngineStatus::Ok;

				D3D11_TEXTURE3D_DESC texDesc ={};
				texDesc.Width     = desc.dimensions[0];
				texDesc.Height    = desc.dimensions[1];
				texDesc.Depth     = desc.dimensions[2];
				texDesc.Format    = DX11DeviceCapsHelper::convertFormatGAPI2DXGI(desc.textureFormat);
				texDesc.MipLevels = (desc.mipMap.useMipMaps ? desc.mipMap.mipLevels : 1);
				// Invalid for 3D: texDesc.ArraySize = (desc.array.isTextureArray ? desc.array.textureCount : 1);
				texDesc.Usage     = DX11DeviceCapsHelper::convertResourceUsageGAPI2D3D11(desc.cpuGpuUsage);
				texDesc.BindFlags = DX11DeviceCapsHelper::convertBufferBindingGAPI2D3D11(desc.gpuBinding);
				// Additional flags
				texDesc.CPUAccessFlags = 0;
				texDesc.MiscFlags      = 0;

				ID3D11Texture3D *tmpTex = nullptr;
				dxRes = device->CreateTexture3D(&texDesc, nullptr, &tmpTex);
				if (FAILED(dxRes)) {
					eRes = EEngineStatus::DXDevice_Texture3DCreationFailed;
				}
				else {
					outTexture = MakeSharedPointerTypeCustomDeleter<ID3D11Texture3D>(tmpTex, DxResourceDeleter<ID3D11Texture3D>());
				}

				return eRes;
			}


		}
	}
}