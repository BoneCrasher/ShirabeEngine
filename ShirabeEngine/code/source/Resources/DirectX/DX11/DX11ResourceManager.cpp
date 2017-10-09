#include "GFXAPI/DirectX/DX11/DX11DeviceCapabilities.h"
#include "Resources/DirectX/DX11/DX11ResourceManager.h"

#include "Resources/DirectX/DX11/DX11TextureNDBuilder.h"
#include "Resources/DirectX/DX11/DX11RenderTargetViewBuilder.h"
#include "Resources/DirectX/DX11/DX11SwapChainBuilder.h"
#include "Resources/DirectX/DX11/DX11ShaderResourceBuilder.h"

namespace Engine {
	namespace DX {
		namespace _11 {
			using namespace Engine::Resources;

			DX11ResourceManager::DX11ResourceManager(
				const IDXDevicePtr& device
			) : _dxDevice(device)
			{ }

			DX11ResourceManager::~DX11ResourceManager() {
				_dxDevice = nullptr;
			}

			EEngineStatus DX11ResourceManager::createTexture1D
			(
				const Texture1DDescriptor &desc,
				ResourceHandle            &outHandle) 
			{
				// NEXT-TASK:
				// TODO: The createResource-Function has to output
				//       all non-internal resources created.
				//       This is especially important for SRV's and RTV's.
				return createResource<DX11Texture1DResourceBuilder>(
						desc,
						false,
						outHandle
						);
			}

			EEngineStatus DX11ResourceManager::createTexture2D
			(
				const Texture2DDescriptor &desc,
				ResourceHandle            &outHandle) 
			{
				return createResource<DX11Texture2DResourceBuilder>(
						desc,
						outHandle
						);
			}			

			EEngineStatus DX11ResourceManager::createTexture3D
			(
				const Texture3DDescriptor &desc,
				ResourceHandle            &outHandle) 
			{
				return createResource<DX11Texture3DResourceBuilder>(
						desc,
						outHandle
					);
			}

			EEngineStatus DX11ResourceManager::createShaderResource(
				const ShaderResourceDescriptor &shaderResourceDescriptor,
				const ResourceHandle           &inUnderlyingResourceHandle,
				ResourceHandle                 &outShaderResourceHandle)
			{
				EEngineStatus eRes = EEngineStatus::Ok;

				return createResource<DX11ShaderResourceBuilder>(
					shaderResourceDescriptor,
					outShaderResourceHandle,
					inUnderlyingResourceHandle
					);

				return eRes;
			}

			EEngineStatus DX11ResourceManager::createRenderTarget(
				const ResourceHandle &inHandle,
				ResourceHandle       &outHandle)
			{
				if (inHandle.type() != EResourceType::TEXTURE)
					return EEngineStatus::GAPI_InvalidHandle;

				std::string       textureName;
				GFXAPI::Format    textureFormat;
				ID3D11ResourcePtr textureResource;

                #define FetchCase(case_condition, type, handle, src, name_var, format_var, resource_var) \
					case case_condition:				             	 \
					{                                                    \
						type tND = src->getResource(inHandle);           \
						if (tND) {										 \
							name_var     = tND->name();				     \
							format_var   = tND->format();				 \
						}                                                \
					}

				// Fetch resource for handle.
				switch (inHandle.subtype()) {
					FetchCase(EResourceSubType::TEXTURE_1D, GAPITexture1DPtr, inHandle, _tex1DPool, textureName, textureFormat, textureResource); break;
					FetchCase(EResourceSubType::TEXTURE_2D, GAPITexture2DPtr, inHandle, _tex2DPool, textureName, textureFormat, textureResource); break;
					FetchCase(EResourceSubType::TEXTURE_3D, GAPITexture3DPtr, inHandle, _tex3DPool, textureName, textureFormat, textureResource); break;
				}

				RenderTargetDescriptor desc ={};
				desc._name          = String::format("RenderTargetViewOfTexture_%0", textureName);
				desc._textureFormat = textureFormat;

				return createResource<DX11RenderTargetResourceBuilder>(
					desc,
					outHandle,
					textureResource
					);
			}

			// EEngineStatus DX11ResourceManager::createDepthStencilView(
			// 	const DepthStencilViewDescriptor &desc,
			// 	const ResourceHandle             &inTex2DResourceHandle,
			// 	ResourceHandle                   &outHandle
			// ) {
			// 	return EEngineStatus::Ok;
			// }
			// 
			// EEngineStatus DX11ResourceManager::createDepthStencilState(
			// 	const DepthStencilStateDescriptor &desc,
			// 	ResourceHandle                    &outHandle) 
			// {
			// 	EEngineStatus res = EEngineStatus::Ok;
			// 
			// 	//
			// 	// The formats that support stenciling are DXGI_FORMAT_D24_UNORM_S8_UINT and DXGI_FORMAT_D32_FLOAT_S8X24_UINT.
			// 	// 
			// 	D3D11_DEPTH_STENCIL_DESC dsDesc;
			// 	memset(&dsDesc, 0, sizeof(D3D11_DEPTH_STENCIL_DESC));
			// 
			// 	// Set up the description of the stencil state.
			// 	dsDesc.DepthEnable    = desc._enableDepth;
			// 	dsDesc.DepthWriteMask = convertToDX11DepthWriteMask(desc._depthMask);
			// 	dsDesc.DepthFunc      = convertToDX11ComparisonFunc(desc._depthFunc);
			// 
			// 	dsDesc.StencilEnable    = desc._enableStencil;
			// 	dsDesc.StencilReadMask  = desc._stencilReadMask;
			// 	dsDesc.StencilWriteMask = desc._stencilWriteMask;
			// 
			// 	// Stencil operations if pixel is front-facing.
			// 	dsDesc.FrontFace.StencilFailOp      = convertToDX11StencilOp(desc._stencilFrontfaceCriteria._failOp);
			// 	dsDesc.FrontFace.StencilDepthFailOp = convertToDX11StencilOp(desc._stencilFrontfaceCriteria._depthFailOp);
			// 	dsDesc.FrontFace.StencilPassOp      = convertToDX11StencilOp(desc._stencilFrontfaceCriteria._passOp);
			// 	dsDesc.FrontFace.StencilFunc        = convertToDX11ComparisonFunc(desc._stencilFrontfaceCriteria._stencilFunc);
			// 
			// 	// Stencil operations if pixel is back-facing.
			// 	dsDesc.BackFace.StencilFailOp      = convertToDX11StencilOp(desc._stencilBackfaceCriteria._failOp);
			// 	dsDesc.BackFace.StencilDepthFailOp = convertToDX11StencilOp(desc._stencilBackfaceCriteria._depthFailOp);
			// 	dsDesc.BackFace.StencilPassOp      = convertToDX11StencilOp(desc._stencilBackfaceCriteria._passOp);
			// 	dsDesc.BackFace.StencilFunc        = convertToDX11ComparisonFunc(desc._stencilBackfaceCriteria._stencilFunc);
			// 
			// 	ID3D11DepthStencilState *tmpDSS = nullptr;
			// 	HRESULT dxRes = _dxDevice->internalDevice()->CreateDepthStencilState(&dsDesc, &tmpDSS);
			// 	if (FAILED(dxRes)) {
			// 		res = EEngineStatus::DXDevice_DepthStencilTextureCreationFailed;
			// 	}
			// 	else {
			// 		ResourceHandle p("", EResourceType::GAPI_STATE, EResourceSubType::DEPTH_STENCIL_STATE);
			// 		res = store(p, MakeSharedPointerTypeCustomDeleter<ID3D11DepthStencilState>(tmpDSS, DxResourceDeleter<ID3D11DepthStencilState>()));
			// 		if (CheckEngineError(res)) {
			// 			// ?
			// 		}
			// 
			// 		outHandle = p;
			// 	}
			// 	
			// 	return res;
			// }



			//--------------------------------------------------------------------------------------------------
			// Specializations of store<Handle, Resource>(const Handle&, const Resource&) : EEngineStatus
			//--------------------------------------------------------------------------------------------------

			template <>
			EEngineStatus DX11ResourceManager
				::store<ID3D11Texture1DPtr>
				(const ResourceHandle& handle, const ID3D11Texture1DPtr& resource) {
				if (!_tex1DPool->addResource(handle, resource)) {
					// return EEngineStatus:: ? ;
					return EEngineStatus::Error;  // MBT: Make appropriate error conditions.
				}
				else
					return EEngineStatus::Ok;
			}

			template <>
			EEngineStatus DX11ResourceManager
				::store<ID3D11Texture2DPtr>
				(const ResourceHandle& handle, const ID3D11Texture2DPtr& resource) {
				if (!_tex2DPool->addResource(handle, resource)) {
					// return EEngineStatus:: ? ;
					return EEngineStatus::Error;  // MBT: Make appropriate error conditions.
				}
				else
					return EEngineStatus::Ok;
			}

			template <>
			EEngineStatus DX11ResourceManager
				::store<ID3D11Texture3DPtr>
				(const ResourceHandle& handle, const ID3D11Texture3DPtr& resource) {
				if (!_tex3DPool->addResource(handle, resource)) {
					// return EEngineStatus:: ? ;
					return EEngineStatus::Error;  // MBT: Make appropriate error conditions.
				}
				else
					return EEngineStatus::Ok;
			}

			template <>
			EEngineStatus DX11ResourceManager
				::store<ID3D11ShaderResourceViewPtr>
				(const ResourceHandle& handle, const ID3D11ShaderResourceViewPtr& resource) {
				if (!_srvPool->addResource(handle, resource)) {
					// return EEngineStatus:: ? ;
					return EEngineStatus::Error;  // MBT: Make appropriate error conditions.
				}
				else
					return EEngineStatus::Ok;
			}

			template <>
			EEngineStatus DX11ResourceManager
				::store<ID3D11RenderTargetViewPtr>
				(const ResourceHandle& handle, const ID3D11RenderTargetViewPtr& resource) {
				if (!_rtvPool->addResource(handle, resource)) {
					// return EEngineStatus:: ? ;
					return EEngineStatus::Error;  // MBT: Make appropriate error conditions.
				}
				else
					return EEngineStatus::Ok;
			}

			template <>
			EEngineStatus DX11ResourceManager
				::store<ID3D11DepthStencilViewPtr>
				(const ResourceHandle& handle, const ID3D11DepthStencilViewPtr& resource) {
				if (!_dsvPool->addResource(handle, resource)) {
					// return EEngineStatus:: ? ;
					return EEngineStatus::Error;  // MBT: Make appropriate error conditions.
				}
				else
					return EEngineStatus::Ok;
			}

			template <>
			EEngineStatus DX11ResourceManager
				::store<ID3D11DepthStencilStatePtr>
				(const ResourceHandle& handle, const ID3D11DepthStencilStatePtr& resource) {
				if (!_dssPool->addResource(handle, resource)) {
					// return EEngineStatus:: ? ;
					return EEngineStatus::Error;  // MBT: Make appropriate error conditions.
				}
				else
					return EEngineStatus::Ok;
			}

			//--------------------------------------------------------------------------------------------------
			// END: Specializations of store<Handle, Resource>(const Handle&, const Resource&) : EEngineStatus
			//--------------------------------------------------------------------------------------------------


		}
	}
}