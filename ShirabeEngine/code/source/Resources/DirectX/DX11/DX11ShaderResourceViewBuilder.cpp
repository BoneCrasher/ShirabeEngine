#include "GFXAPI/DirectX/DX11/DX11DeviceCapabilities.h"

#include "GFXAPI/DirectX/DX11/Tasks/ShaderResourceView.h"

namespace Engine {
	namespace DX {
		namespace _11 {

			EEngineStatus DX11ShaderResourceBuilderImpl::createShaderResource(
				const ID3D11DevicePtr                &device,
				ShaderResourceView::Descriptor const &desc,
				ID3D11ResourcePtr                    &inUnderlyingResource,
				ID3D11ShaderResourceViewPtr          &outRes
			) {
				HRESULT       dxRes = S_OK;
				EEngineStatus eRes  = EEngineStatus::Ok;

				ID3D11ShaderResourceView *tmp = nullptr;

				D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc ={};
				srvDesc.Format = DX11DeviceCapsHelper::convertFormatGAPI2DXGI(desc.format);

				// Map configuration to DX11 creation struct
				if( desc.srvType == ShaderResourceView::Descriptor::EShaderResourceDimension::Texture ) {
					switch( desc.shaderResourceDimension.texture.dimensionNb ) {
					case 1:
						if( desc.shaderResourceDimension.texture.array.isTextureArray ) {
							srvDesc.Texture1DArray.ArraySize       = desc.shaderResourceDimension.texture.array.size;
							srvDesc.Texture1DArray.FirstArraySlice = desc.shaderResourceDimension.texture.array.firstArraySlice;
							srvDesc.Texture1DArray.MipLevels       = desc.shaderResourceDimension.texture.mipMap.mipLevels;
							srvDesc.Texture1DArray.MostDetailedMip = desc.shaderResourceDimension.texture.mipMap.firstMipMapLevel;
						}
						else {
							srvDesc.Texture1D.MipLevels       = desc.shaderResourceDimension.texture.mipMap.mipLevels;
							srvDesc.Texture1D.MostDetailedMip = desc.shaderResourceDimension.texture.mipMap.firstMipMapLevel;
						}
						
						break;
					case 2:
						if( desc.shaderResourceDimension.texture.array.isTextureArray ) {
							srvDesc.Texture2DArray.ArraySize       = desc.shaderResourceDimension.texture.array.size;
							srvDesc.Texture2DArray.FirstArraySlice = desc.shaderResourceDimension.texture.array.firstArraySlice;
							srvDesc.Texture2DArray.MipLevels       = desc.shaderResourceDimension.texture.mipMap.mipLevels;
							srvDesc.Texture2DArray.MostDetailedMip = desc.shaderResourceDimension.texture.mipMap.firstMipMapLevel;
						}
						else {
							srvDesc.Texture2D.MipLevels       = desc.shaderResourceDimension.texture.mipMap.mipLevels;
							srvDesc.Texture2D.MostDetailedMip = desc.shaderResourceDimension.texture.mipMap.firstMipMapLevel;
						}

						break;
					case 3:
						if( desc.shaderResourceDimension.texture.isCube ) {
							if( desc.shaderResourceDimension.texture.array.isTextureArray ) {
								srvDesc.TextureCube.MipLevels       = desc.shaderResourceDimension.texture.mipMap.mipLevels;
								srvDesc.TextureCube.MostDetailedMip = desc.shaderResourceDimension.texture.mipMap.firstMipMapLevel;
							}
							else {
								srvDesc.TextureCubeArray.NumCubes        = desc.shaderResourceDimension.texture.array.size;
								srvDesc.TextureCubeArray.MipLevels       = desc.shaderResourceDimension.texture.mipMap.mipLevels;
								srvDesc.TextureCubeArray.MostDetailedMip = desc.shaderResourceDimension.texture.mipMap.firstMipMapLevel;
							}
						}
						else {
							if( desc.shaderResourceDimension.texture.array.isTextureArray ) {
								// ERROR: NO 3D Texture Arrays supported!
							}
							else {
								srvDesc.Texture3D.MipLevels       = desc.shaderResourceDimension.texture.mipMap.mipLevels;
								srvDesc.Texture3D.MostDetailedMip = desc.shaderResourceDimension.texture.mipMap.firstMipMapLevel;
							}
						}
						break;
					default:
						break;
					}
				}
				else { // Structured Buffer
					srvDesc.Buffer.ElementOffset = desc.shaderResourceDimension.structuredBuffer.firstElementOffset;
					srvDesc.Buffer.ElementWidth  = desc.shaderResourceDimension.structuredBuffer.elementWidthInBytes;
				}
				
				dxRes = device->CreateShaderResourceView(inUnderlyingResource.get(), &srvDesc, &tmp);
				if (FAILED(dxRes)) {
					eRes = EEngineStatus::DXDevice_SRVCreationFailed;
				}
				else {
					outRes = MakeSharedPointerTypeCustomDeleter<ID3D11ShaderResourceView>(tmp, DxResourceDeleter<ID3D11ShaderResourceView>());
				}

				return eRes;
			}
		}
	}
}