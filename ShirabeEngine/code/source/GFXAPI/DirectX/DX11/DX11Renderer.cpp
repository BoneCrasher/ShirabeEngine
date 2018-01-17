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

      void DX11Renderer
        ::setDX11Environment(Ptr<DX11Environment> const&pDx11Environment) {
        assert(pDx11Environment != nullptr);

        _dx11Environment = pDx11Environment;
      }

			EEngineStatus DX11Renderer::initialize(
				ApplicationEnvironment const &environment,
				RendererConfiguration  const &configuration,
				IResourceManagerPtr    const &resourceManager) 
      {

				_config          = configuration;
        _resourceManager = resourceManager;

				using namespace Engine::GFXAPI;

				EEngineStatus status = EEngineStatus::Ok;
				HRESULT       dxRes  = S_OK;

        GAPIOutputMode const& outputMode = _dx11Environment->getOutputMode();

				static const std::size_t BACK_BUFFER_COUNT = 1;

				Ptr<SwapChain>        swapChain    = nullptr;    	
        Ptr<RenderTargetView> swapChainRTV = swapChain->getCurrentBackBufferRenderTargetView();
        if(!swapChainRTV) {
          // Mimimi...
        }

				// Get the swap chain.
				SwapChain::Descriptor swapChainDesc ={};
				swapChainDesc.name                   = "DefaultSwapChain";
				swapChainDesc.backBufferCount        = BACK_BUFFER_COUNT;
				swapChainDesc.texture.name           = "DefaultSwapChainTexture2D";
				swapChainDesc.texture.textureFormat  = outputMode.format;
				swapChainDesc.texture.dimensions[0]  = outputMode.size.x();
				swapChainDesc.texture.dimensions[1]  = outputMode.size.y();
				swapChainDesc.vsyncEnabled           = configuration.enableVSync;
				swapChainDesc.refreshRateNumerator   = outputMode.refreshRate.x();
				swapChainDesc.refreshRateDenominator = outputMode.refreshRate.y();
				swapChainDesc.fullscreen             = !configuration.requestFullscreen;
				swapChainDesc.windowHandle           = reinterpret_cast<unsigned int>(static_cast<void *>(environment.primaryWindowHandle));

        PublicResourceId_t swapChainId = 0;
        SwapChain::CreationRequest swapChainCreationRequest(swapChainDesc);

				status = resourceManager->createSwapChain(swapChainDesc, swapChainId);
        HandleEngineStatusError(status, String::format("Failed to create swap chain:\n Desc:%0", swapChainDesc.toString()));

        _swapChain = swapChain;

        Ptr<Texture2D>        defaultDSTex   = nullptr;
        Ptr<DepthStencilView> defaultDSView  = nullptr;

        ResourceHandle depthStencilHandle ={};

        Texture2D::Descriptor dsTexDesc ={};
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

        PublicResourceId_t dsTexId   = 0;
				status = resourceManager->createTexture2D(dsTexDesc, dsTexId);
        HandleEngineStatusError(status, String::format("Failed to create depth stencil view:\n Desc:%0", dsTexDesc.toString()));

        defaultDSView = defaultDSTex->depthStencilView();
        if(!defaultDSView) {
          // Mimimi...
        }

    //    ID3D11RenderTargetView *const d3d11SwapChainRTV = nullptr;
    //    ID3D11DepthStencilView *const d3d11DSV          = _resourceManager. (defaultDSView);

    //    _resourceManager->backend()->getUnderlyingHandle<ID3D11RenderTargetView>(_swapChain->binding.)

				//// Bind the device context to our backbuffer (with bound swapchain) and the depth stencil view.
				//dx11Environment.getImmediateContext()->OMSetRenderTargets(1, &d3d11SwapChainRTV, d3d11DSV);

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

        PublicResourceId_t dssId = 0;
				dxRes = resourceManager->createDepthStencilState(dsDesc, dssId);
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

				dxRes = _dx11Environment->getDevice()->CreateRasterizerState(&rasterizerState, &tmpRasterizerState);
				if( FAILED(dxRes) ) {
					status = EEngineStatus::DXDevice_RasterizerStateCreationFailed;
					goto _return_failed;
				}
        _dx11Environment->getImmediateContext()->RSSetState(tmpRasterizerState);

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
				_swapChain              = MakeSharedPointerTypeCustomDeleter(tmpSwapChain, DxResourceDeleter<IDXGISwapChain>());
				_backBufferRTV          = MakeSharedPointerTypeCustomDeleter(tmpBackBufferRTV, DxResourceDeleter<ID3D11RenderTargetView>());
				_depthStencilTexture    = MakeSharedPointerTypeCustomDeleter(tmpDepthStencilTexture, DxResourceDeleter<ID3D11Texture2D>());
				_depthStencilView       = MakeSharedPointerTypeCustomDeleter(tmpDepthStencilView, DxResourceDeleter<ID3D11DepthStencilView>());

				_depthStencilState = MakeSharedPointerTypeCustomDeleter(tmpDefaultDepthStencilState, DxResourceDeleter<ID3D11DepthStencilState>());
				_rasterizerState   = MakeSharedPointerTypeCustomDeleter(tmpRasterizerState, DxResourceDeleter<ID3D11RasterizerState>());
        
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

				_dx11Environment->getImmediateContext()->ClearRenderTargetView(_backBufferRTV.get(), clearColor);
        _dx11Environment->getImmediateContext()->ClearDepthStencilView(_depthStencilView.get(), D3D11_CLEAR_DEPTH, 1.0f, 0.0f);

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
