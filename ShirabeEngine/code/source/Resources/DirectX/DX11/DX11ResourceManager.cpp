#include "GFXAPI/DirectX/DX11/DX11DeviceCapabilities.h"
#include "Resources/DirectX/DX11/DX11ResourceManager.h"

#include "Resources/DirectX/DX11/Builders/TextureND.h"
#include "Resources/DirectX/DX11/Builders/RenderTargetView.h"
#include "Resources/DirectX/DX11/Builders/SwapChain.h"
#include "Resources/DirectX/DX11/Builders/ShaderResourceView.h"

namespace Engine {
	namespace DX {
		namespace _11 {
			using namespace Engine::Resources;

			DX11ResourceManager::DX11ResourceManager(
				const Ptr<ResourceProxyFactory> &proxyFactory,
				const IDXDevicePtr              &device) 
				: ProxyBasedResourceManager(proxyFactory)
				, _dxDevice(device)
			{ }

			DX11ResourceManager::~DX11ResourceManager() {
				_dxDevice = nullptr;
			}


			EEngineStatus DX11ResourceManager::createSwapChain(
				const SwapChainDescriptor &inDesc,
				Ptr<SwapChain>            &outSwapChain) 
			{				
				SwapChainResourceBinding binding;

				EEngineStatus status = createResource<EResourceType::GAPI_COMPONENT, EResourceSubType::SWAP_CHAIN, SwapChainResourceBinding>(
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

			EEngineStatus DX11ResourceManager::createTexture1D(
				const Texture1DDescriptor &desc,
				ResourceHandleList        &outHandles) 
			{
				return createResource<EResourceType::TEXTURE, EResourceSubType::TEXTURE_1D>(
						desc,
						false,
						outHandles);
			}

			EEngineStatus DX11ResourceManager::createTexture2D(
				const Texture2DDescriptor &desc,
				ResourceHandleList        &outHandles)
			{
				return createResource<EResourceType::TEXTURE, EResourceSubType::TEXTURE_2D>(
						desc,
						false,
						outHandles);
			}			

			EEngineStatus DX11ResourceManager::createTexture3D(
				const Texture3DDescriptor &desc,
				ResourceHandleList        &outHandles)
			{
				return createResource<EResourceType::TEXTURE, EResourceSubType::TEXTURE_3D>(
					desc,
					false,
					outHandles);
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
		}
	}
}