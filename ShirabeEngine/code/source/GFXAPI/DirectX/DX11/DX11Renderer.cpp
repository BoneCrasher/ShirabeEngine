#include "GFXAPI/DirectX/DX11/DX11Renderer.h"

#include "Platform/Platform.h"

#include "GFXAPI/Config.h"
#include "GFXAPI/DirectX/DX11/DX11Common.h"
#include "GFXAPI/DirectX/DX11/DX11DeviceCapabilities.h"

#include "GFXAPI/Types/SwapChain.h"
#include "GFXAPI/Types/TextureND.h" // Implies RTV, SRV, DSV
#include "GFXAPI/Types/DepthStencilState.h"
#include "GFXAPI/Types/RasterizerState.h"

namespace Engine {
	namespace DX {
		namespace _11 {

      RendererConfiguration m_config;
      IResourceManagerPtr   m_resourceManager;

      Ptr<DX11Environment> m_dx11Environment;

      PublicResourceId_t
        m_swapChainId,
        m_defaultDepthStencilTextureId,
        m_defaultDepthStencilStateId;

      std::atomic_bool m_paused;
      DX11Renderer::DX11Renderer()
        : IRenderer()
        , m_config()
        , m_resourceManager(nullptr)
        , m_dx11Environment(nullptr)
        , m_swapChainId(0)
        , m_defaultDepthStencilTextureId(0)
        , m_defaultDepthStencilStateId(0)
			{
			}

			DX11Renderer::~DX11Renderer()
			{
			}

      void DX11Renderer
        ::setDX11Environment(Ptr<DX11Environment> const&pDx11Environment) {
        assert(pDx11Environment != nullptr);

        m_dx11Environment = pDx11Environment;
      }

			EEngineStatus DX11Renderer::initialize(
				ApplicationEnvironment const &environment,
				RendererConfiguration  const &configuration,
				IResourceManagerPtr    const &resourceManager) 
      {

				m_config          = configuration;
        m_resourceManager = resourceManager;

				using namespace Engine::GFXAPI;

				EEngineStatus status = EEngineStatus::Ok;
				HRESULT       dxRes  = S_OK;

        GAPIOutputMode const& outputMode = m_dx11Environment->getOutputMode();

				static const std::size_t BACK_BUFFER_COUNT = 1;

				Ptr<SwapChain>        swapChain    = nullptr;    	
        Ptr<RenderTargetView> swapChainRTV = swapChain->getCurrentBackBufferRenderTargetView();
        if(!swapChainRTV) {
          // Mimimi...
        }

				// Get the swap chain.
				SwapChain::Descriptor swapChainDescriptor ={};
				swapChainDescriptor.name                   = "DefaultSwapChain";
				swapChainDescriptor.backBufferCount        = BACK_BUFFER_COUNT;
				swapChainDescriptor.texture.name           = "DefaultSwapChainTexture2D";
				swapChainDescriptor.texture.textureFormat  = outputMode.format;
				swapChainDescriptor.texture.dimensions[0]  = outputMode.size.x();
				swapChainDescriptor.texture.dimensions[1]  = outputMode.size.y();
				swapChainDescriptor.vsyncEnabled           = configuration.enableVSync;
				swapChainDescriptor.refreshRateNumerator   = outputMode.refreshRate.x();
				swapChainDescriptor.refreshRateDenominator = outputMode.refreshRate.y();
				swapChainDescriptor.fullscreen             = !configuration.requestFullscreen;
				swapChainDescriptor.windowHandle           = reinterpret_cast<unsigned int>(static_cast<void *>(environment.primaryWindowHandle));

        PublicResourceId_t swapChainId = 0;
        SwapChain::CreationRequest swapChainCreationRequest(swapChainDescriptor);

				status = resourceManager->createSwapChain(swapChainDescriptor, swapChainId);
        HandleEngineStatusError(status, String::format("Failed to create swap chain:\n Desc:%0", swapChainDescriptor.toString()));
      
        Texture2D::Descriptor defaultDepthStencilTextureDescriptor ={};
				defaultDepthStencilTextureDescriptor.dimensions            ={ outputMode.size.x(), outputMode.size.y() };
				defaultDepthStencilTextureDescriptor.textureFormat         = Format::D24_UNORM_S8_UINT;
				defaultDepthStencilTextureDescriptor.mipMap.useMipMaps     = false;
				defaultDepthStencilTextureDescriptor.mipMap.mipLevels      = 1;
				defaultDepthStencilTextureDescriptor.array.isTextureArray  = false;
				defaultDepthStencilTextureDescriptor.array.size            = 1;
				defaultDepthStencilTextureDescriptor.cpuGpuUsage           = ResourceUsage::CPU_None_GPU_ReadWrite;
				defaultDepthStencilTextureDescriptor.gpuBinding            = EToUnderlying(BufferBinding::ShaderOutput_DepthStencil);
				defaultDepthStencilTextureDescriptor.multisampling.size    = 1;
				defaultDepthStencilTextureDescriptor.multisampling.quality = 0;

        PublicResourceId_t defaultDepthStencilTextureId   = 0;
				status = resourceManager->createTexture2D(defaultDepthStencilTextureDescriptor, defaultDepthStencilTextureId);
        HandleEngineStatusError(status, String::format("Failed to create depth stencil view:\n Desc:%0", defaultDepthStencilTextureDescriptor.toString()));


    //    ID3D11RenderTargetView *const d3d11SwapChainRTV = nullptr;
    //    ID3D11DepthStencilView *const d3d11DSV          = m_resourceManager. (defaultDepthStencilViewDescriptor);

    //    m_resourceManager->backend()->getUnderlyingHandle<ID3D11RenderTargetView>(_swapChain->binding.)

				//// Bind the device context to our backbuffer (with bound swapchain) and the depth stencil view.
				//dx11Environment.getImmediateContext()->OMSetRenderTargets(1, &d3d11SwapChainRTV, d3d11DSV);

				// m_createDepthStencilState:
					// 
					// Setup the default depth stencil state to be used, unless it is overridden for some effects.
					//
        DepthStencilState::Descriptor defaultDepthStencilStateDescriptor ={};
        defaultDepthStencilStateDescriptor.enableDepth                          = true;
        defaultDepthStencilStateDescriptor.depthMask                            = DepthStencilState::Descriptor::DepthWriteMask::All;
        defaultDepthStencilStateDescriptor.depthFunc                            = GFXAPI::Comparison::Less;
        defaultDepthStencilStateDescriptor.enableStencil                        = true;
        defaultDepthStencilStateDescriptor.stencilWriteMask                     = 0xFF;
        defaultDepthStencilStateDescriptor.stencilReadMask                      = 0xFF;
        defaultDepthStencilStateDescriptor.stencilFrontfaceCriteria.failOp      = DepthStencilState::Descriptor::StencilOp::Keep;
        defaultDepthStencilStateDescriptor.stencilFrontfaceCriteria.depthFailOp = DepthStencilState::Descriptor::StencilOp::Increment;
        defaultDepthStencilStateDescriptor.stencilFrontfaceCriteria.passOp      = DepthStencilState::Descriptor::StencilOp::Keep;
        defaultDepthStencilStateDescriptor.stencilFrontfaceCriteria.stencilFunc = GFXAPI::Comparison::Always;
        defaultDepthStencilStateDescriptor.stencilBackfaceCriteria.failOp       = DepthStencilState::Descriptor::StencilOp::Keep;
        defaultDepthStencilStateDescriptor.stencilBackfaceCriteria.depthFailOp  = DepthStencilState::Descriptor::StencilOp::Decrement;
        defaultDepthStencilStateDescriptor.stencilBackfaceCriteria.passOp       = DepthStencilState::Descriptor::StencilOp::Keep;
        defaultDepthStencilStateDescriptor.stencilBackfaceCriteria.stencilFunc  = GFXAPI::Comparison::Always;
        DepthStencilState::CreationRequest dssCreationRequest(defaultDepthStencilStateDescriptor);

        PublicResourceId_t defaultDepthStencilStateId = 0;
				status = resourceManager->createDepthStencilState(dssCreationRequest, defaultDepthStencilStateId);
        HandleEngineStatusError(status, String::format("Failed to create depth stencil state:\n Desc:%0", defaultDepthStencilStateDescriptor.toString()));
        
        RasterizerState::Descriptor defaultRasterizerStateDescriptor{};
        defaultRasterizerStateDescriptor.fillMode = RasterizerState::FillMode::Solid;
        defaultRasterizerStateDescriptor.cullMode = RasterizerState::CullMode::Back;
        RasterizerState::CreationRequest rasterizerStateCreationRequest(defaultRasterizerStateDescriptor);

        PublicResourceId_t defaultRasterizerStateId{};
				status = resourceManager->createRasterizerState(rasterizerStateCreationRequest, defaultRasterizerStateId);
        HandleEngineStatusError(status, String::format("Failed to create rasterizer state:\n Desc:%0", defaultRasterizerStateDescriptor.toString()));
        
				// m_createDefaultViewPort:
				D3D11_VIEWPORT defaultViewPort;
				defaultViewPort.MinDepth = 0.0f;
				defaultViewPort.MaxDepth = 1.0f;
				defaultViewPort.Width    = configuration.preferredWindowSize.x();
				defaultViewPort.Height   = configuration.preferredWindowSize.y();
				defaultViewPort.TopLeftX = 0.0f;
				defaultViewPort.TopLeftY = 0.0f;
        
        m_swapChainId                  = swapChainId;
        m_defaultDepthStencilTextureId = defaultDepthStencilTextureId;
        m_defaultDepthStencilStateId   = defaultDepthStencilStateId;

				return status;
			}

			EEngineStatus DX11Renderer::deinitialize() {
        EEngineStatus status = EEngineStatus::Ok;
        
        status = m_resourceManager->destroyDepthStencilState(m_defaultDepthStencilStateId);
        HandleEngineStatusError(status, "Failed to destroy the default depth stencil state.");
        status = m_resourceManager->destroyTexture2D(m_defaultDepthStencilTextureId);
        HandleEngineStatusError(status, "Failed to destroy the default depth stencil texture or one of its dependers.");
        status = m_resourceManager->destroySwapChain(m_swapChainId);
        HandleEngineStatusError(status, "Failed to destroy the swap chain.");

				return EEngineStatus::Ok;
			}
			EEngineStatus DX11Renderer::reinitialize() {
				return EEngineStatus::Ok;
			}

			EEngineStatus DX11Renderer::pause() {
				m_paused.store(true);
				return EEngineStatus::Ok;
			}
			EEngineStatus DX11Renderer::resume() {
				m_paused.store(false); // noexcept
				return EEngineStatus::Ok;
			}
			bool DX11Renderer::isPaused() const {
				return m_paused.load(); // noexcept
				return true;
			}

			EEngineStatus DX11Renderer::render(/* insert queue type*/) {
				// if (isPaused())
				// 	return EEngineStatus::Ok;

				float clearColor[4] ={ 1.0f, 0.5f, 0.25f, 1.0f };

				//m_dx11Environment->getImmediateContext()->ClearRenderTargetView(m_backBufferRTV.get(), clearColor);
        //m_dx11Environment->getImmediateContext()->ClearDepthStencilView(m_depthStencilView.get(), D3D11_CLEAR_DEPTH, 1.0f, 0.0f);

				// m_deviceImmediateContext->Begin(nullptr);

				// Render commands

				//_deviceImmediateContext->End(nullptr);

				// Activate VSync only in fullscreen, since in windowed mode the 
				// OS will take care of the presentation behaviour and correctness.
				bool useVSync = m_config.enableVSync & m_config.requestFullscreen;
				//_swapChain->Present(useVSync, 0);

				return EEngineStatus::Ok;
			}
		}
	}
}
