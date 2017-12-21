#include "GFXAPI/DirectX/DX11/DX11Renderer.h"

#include "Platform/Platform.h"

#include "GFXAPI/Config.h"
#include "GFXAPI/DirectX/DX11/DX11Common.h"
#include "GFXAPI/DirectX/DX11/DX11DeviceCapabilities.h"

#include "GFXAPI/Types/TextureND.h"
#include "GFXAPI/Types/DepthStencilView.h"

namespace Engine {
	namespace DX {
		namespace _11 {

			DX11Renderer::DX11Renderer()
				: IRenderer()
			{
			}

			DX11Renderer::~DX11Renderer()
			{
			}

			EEngineStatus DX11Renderer::initialize(
				const ApplicationEnvironment &environment,
				const RendererConfiguration  &configuration,
				const IResourceManagerPtr    &resourceManager) {
				_config = configuration;

				using namespace Engine::GFXAPI;

				EEngineStatus status = EEngineStatus::Ok;
				HRESULT       dxRes  = S_OK;

				Ptr<SwapChain> swapChain = nullptr;

				ResourceHandle depthStencilHandle ={};

				Texture2D::Descriptor dsTexDesc ={};
				Ptr<Texture2D>        dsTex  = nullptr;
				Ptr<DepthStencilView> dsView = nullptr;

				GAPIOutputMode outputMode ={};

			_createDeviceAndSwapChain:
				

				dsTexDesc     ={};
				dsTexDesc.dimensions            ={ outputMode.size.x(), outputMode.size.y() };
				dsTexDesc.textureFormat         = Format::D24_UNORM_S8_UINT;
				dsTexDesc.mipMap.useMipMaps     = false;
				dsTexDesc.mipMap.mipLevels      = 1;
				dsTexDesc.array.isTextureArray  = false;
				dsTexDesc.array.size            = 1;
				dsTexDesc.cpuGpuUsage           = ResourceUsage::CPU_None_GPU_ReadWrite;
				dsTexDesc.gpuBinding            = EToUnderlying(BufferBinding::ShaderOutput_DepthStencil);
				dsTexDesc.multisampling.size    = 1;
				dsTexDesc.multisampling.quality = 0;

				status = resourceManager->createTexture2D(dsTexDesc, dsTex);
				if( CheckEngineError(status) ) {
					goto _return_failed;
				}

				dsView = resourceManager->getResource<DepthStencilView>(dsTex->binding().dsvBinding);

				// Bind the device context to our backbuffer (with bound swapchain) and the depth stencil view.
				tmpDeviceContext->OMSetRenderTargets(1, &tmpBackBufferRTV, tmpDepthStencilView);

				// _createDepthStencilState:
					// 
					// Setup the default depth stencil state to be used, unless it is overridden for some effects.
					//
				D3D11_DEPTH_STENCIL_DESC dsDesc ={};
				// Setup depth test
				dsDesc.DepthEnable    = true;
				dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
				dsDesc.DepthFunc      = D3D11_COMPARISON_LESS; // Pass criteria: New depth value is less than the current at pixel (x|y)
															   // Setup stencil test
				dsDesc.StencilEnable    = true;
				dsDesc.StencilWriteMask = 0xFF; // Only permit white as passing criteria
				dsDesc.StencilReadMask  = 0xFF; // Only permit white as passing criteria
												// Setup front facing stencil op
				dsDesc.FrontFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;
				dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
				dsDesc.FrontFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;
				dsDesc.FrontFace.StencilFunc        = D3D11_COMPARISON_ALWAYS;
				// Setup back facing stencil op
				dsDesc.BackFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;
				dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
				dsDesc.BackFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;
				dsDesc.BackFace.StencilFunc        = D3D11_COMPARISON_ALWAYS;

				dxRes = tmpDevice->CreateDepthStencilState(&dsDesc, &tmpDefaultDepthStencilState);
				if( FAILED(dxRes) ) {
					status = EEngineStatus::DXDevice_DepthStencilStateCreationFailed;
					goto _return_failed;
				}

				// _createRasterizerState:
				D3D11_RASTERIZER_DESC rasterizerState ={};
				rasterizerState.AntialiasedLineEnable = true;
				rasterizerState.FillMode              = D3D11_FILL_SOLID;
				// Backface culling with front == clockwise
				rasterizerState.CullMode              = D3D11_CULL_BACK;
				rasterizerState.FrontCounterClockwise = false;
				rasterizerState.DepthBias             = 0;
				rasterizerState.DepthBiasClamp        = 0;
				rasterizerState.DepthClipEnable       = true;
				rasterizerState.MultisampleEnable     = false;
				rasterizerState.ScissorEnable         = false;
				rasterizerState.SlopeScaledDepthBias  = 0.0f;

				dxRes = tmpDevice->CreateRasterizerState(&rasterizerState, &tmpRasterizerState);
				if( FAILED(dxRes) ) {
					status = EEngineStatus::DXDevice_RasterizerStateCreationFailed;
					goto _return_failed;
				}
				tmpDeviceContext->RSSetState(tmpRasterizerState);

				// _createDefaultViewPort:
				D3D11_VIEWPORT vpDesc;
				vpDesc.MinDepth = 0.0f;
				vpDesc.MaxDepth = 1.0f;
				vpDesc.Width    = configuration.preferredWindowSize.x();
				vpDesc.Height   = configuration.preferredWindowSize.y();
				vpDesc.TopLeftX = 0.0f;
				vpDesc.TopLeftY = 0.0f;

				tmpDeviceContext->RSSetViewports(1, &vpDesc);

				// _commit:
				_device                 = MakeSharedPointerTypeCustomDeleter(tmpDevice, DxResourceDeleter<ID3D11Device>());
				_deviceImmediateContext = MakeSharedPointerTypeCustomDeleter(tmpDeviceContext, DxResourceDeleter<ID3D11DeviceContext>());
				_swapChain              = MakeSharedPointerTypeCustomDeleter(tmpSwapChain, DxResourceDeleter<IDXGISwapChain>());
				_backBufferRTV          = MakeSharedPointerTypeCustomDeleter(tmpBackBufferRTV, DxResourceDeleter<ID3D11RenderTargetView>());
				_depthStencilTexture    = MakeSharedPointerTypeCustomDeleter(tmpDepthStencilTexture, DxResourceDeleter<ID3D11Texture2D>());
				_depthStencilView       = MakeSharedPointerTypeCustomDeleter(tmpDepthStencilView, DxResourceDeleter<ID3D11DepthStencilView>());

				_depthStencilState = MakeSharedPointerTypeCustomDeleter(tmpDefaultDepthStencilState, DxResourceDeleter<ID3D11DepthStencilState>());
				_rasterizerState   = MakeSharedPointerTypeCustomDeleter(tmpRasterizerState, DxResourceDeleter<ID3D11RasterizerState>());

				goto _return_success; // Make sure not to drop into the deleter!

			_return_failed:
				DxRelease(&tmpRasterizerState);
				DxRelease(&tmpDefaultDepthStencilState);
				DxRelease(&tmpDepthStencilView);
				DxRelease(&tmpDepthStencilTexture);
				DxRelease(&tmpBackBufferRTV);
				DxRelease(&tmpSwapChain);
				DxRelease(&tmpDeviceContext);
				DxRelease(&tmpDevice);

			_return_success:
				return status;
			}

			EEngineStatus DX11Renderer::deinitialize() {
				return EEngineStatus::Ok;

			}
			EEngineStatus DX11Renderer::reinitialize() {
				return EEngineStatus::Ok;
			}

			EEngineStatus DX11Renderer::pause() {
				_paused.store(true);
				return EEngineStatus::Ok;
			}
			EEngineStatus DX11Renderer::resume() {
				_paused.store(false); // noexcept
				return EEngineStatus::Ok;
			}
			bool DX11Renderer::isPaused() const {
				return _paused.load(); // noexcept
				return true;
			}

			EEngineStatus DX11Renderer::render(/* insert queue type*/) {
				// if (isPaused())
				// 	return EEngineStatus::Ok;

				float clearColor[4] ={ 1.0f, 0.5f, 0.25f, 1.0f };

				_deviceImmediateContext->ClearRenderTargetView(_backBufferRTV.get(), clearColor);
				_deviceImmediateContext->ClearDepthStencilView(_depthStencilView.get(), D3D11_CLEAR_DEPTH, 1.0f, 0.0f);

				// _deviceImmediateContext->Begin(nullptr);

				// Render commands

				//_deviceImmediateContext->End(nullptr);

				// Activate VSync only in fullscreen, since in windowed mode the 
				// OS will take care of the presentation behaviour and correctness.
				bool useVSync = _config.enableVSync & _config.requestFullscreen;
				_swapChain->Present(useVSync, 0);

				return EEngineStatus::Ok;
			}
		}
	}
}
