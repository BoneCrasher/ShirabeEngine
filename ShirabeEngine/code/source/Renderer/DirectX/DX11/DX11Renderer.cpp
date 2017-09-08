#include "Renderer/DirectX/DX11/DX11Renderer.h"

#include "Platform/Platform.h"

#include "Renderer/DirectX/DX11/DX11Common.h"
#include "Renderer/DirectX/DX11/DX11DeviceCapabilities.h"

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

			EEngineStatus DX11Renderer::initialize(const ApplicationEnvironment& environment,
												   const RendererConfiguration&  configuration
			/* Insert Resourcemanager pointer here */) {
				_config = configuration;
				
				using namespace Engine::GAPI;

				EEngineStatus status = EEngineStatus::Ok;
				HRESULT       dxRes  = S_OK;

				ID3D11Device            *tmpDevice                   = nullptr;
				ID3D11DeviceContext     *tmpDeviceContext            = nullptr;
				IDXGISwapChain          *tmpSwapChain                = nullptr;
				ID3D11Texture2D         *tmpBackBufferPtr            = nullptr;
				ID3D11RenderTargetView  *tmpBackBufferRTV            = nullptr;
				ID3D11Texture2D         *tmpDepthStencilTexture      = nullptr;
				ID3D11DepthStencilView  *tmpDepthStencilView         = nullptr;
				ID3D11DepthStencilState *tmpDefaultDepthStencilState = nullptr;
				ID3D11RasterizerState   *tmpRasterizerState          = nullptr;

				GAPIOutputMode outputMode ={};

				//
				// Get the currenntly available device capabilities and try to find a matching display mode for the 
				// desired backbuffer format in the primary adapter.
				// 
				GAPIDeviceCapabilities capabilities;
				status = DX11DeviceCapsHelper::GetDeviceCapabilities(GAPIFormat::RGBA_8_UNORM, &capabilities);
				if (!CheckEngineError(status)) {
					// No adapters, or adapterindex > 0? --> No primary adapter. 
					// TODO: Is the effective adapter index of any importance?
					if (capabilities._adapters.empty()
						|| capabilities._adapters[0]._adapterIndex > 0) {
						status = EEngineStatus::GAPI_NoPrimaryAdapter;
						goto _return_failed;
					}

					GAPIAdapter primaryAdapter = capabilities._adapters[0];

					// If this is no "Output"-Adapter, return.
					if (primaryAdapter._outputs.empty()) {
						status = EEngineStatus::GAPI_NoPrimaryAdapterOutputs;
						goto _return_failed;
					}

					// TODO: How about non-output adapters for offline-stuff?

					// Loop all outputs and find an output with a valid output mode configuration!
					for (GAPIOutput output : primaryAdapter._outputs) {
						if (output._outputModes.empty()) {
							status = EEngineStatus::GAPI_NoPrimaryAdapterOutputModes;
							// No failure createria, if there's at least another output for our desired modes.
						}

						for (GAPIOutputMode mode : output._outputModes) {
							if (mode._format != GAPIFormat::RGBA_8_UNORM)
								// The format is mandatory!!!
								break;

							if (mode._size.xy() == configuration._preferredBackBufferSize) {
								outputMode = mode;
								goto _createDeviceAndSwapChain;
							}
						}
					}

					// If we dropped here, no valid output mode was found in any of the adapters
					status = EEngineStatus::GAPI_NoPrimaryAdapterOutputModes;
					goto _return_failed;
				}

			_createDeviceAndSwapChain:
				DXGI_SWAP_CHAIN_DESC scDesc ={};
				// Setup backbuffer
				scDesc.BufferCount                 = 2;
				scDesc.BufferDesc.Width            = outputMode._size.x();
				scDesc.BufferDesc.Height           = outputMode._size.y();
				scDesc.BufferDesc.Format           = DX11DeviceCapsHelper::convertFormatGAPI2DXGI(outputMode._format);
				scDesc.BufferDesc.Scaling          = DXGI_MODE_SCALING_UNSPECIFIED;
				scDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
				if (configuration._enableVSync) {
					scDesc.BufferDesc.RefreshRate.Numerator   = outputMode._refreshRate.x();
					scDesc.BufferDesc.RefreshRate.Denominator = outputMode._refreshRate.y();
				} else {
					scDesc.BufferDesc.RefreshRate.Numerator   = 0;
					scDesc.BufferDesc.RefreshRate.Denominator = 1;
				}
				scDesc.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;

				// Multisampling: For now deactivate.
				scDesc.SampleDesc.Count   = 1;
				scDesc.SampleDesc.Quality = 0;
				// Fullscreen?
				scDesc.Windowed = !configuration._requestFullscreen;
				// Attach to window
				scDesc.OutputWindow = environment._primaryWindowHandle;
				scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

				// Define the desired DX11-Device feature level
				D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

				// CREATE				
				dxRes = D3D11CreateDeviceAndSwapChain(NULL,                     // Adapter
													  D3D_DRIVER_TYPE_HARDWARE, // What kind of device to create?
													  NULL,                     // Software module handle
#ifdef CONFIGURATION_DEBUG
					                                  D3D11_CREATE_DEVICE_DEBUG,// Creation flags for Debug
#else 
					                                  0,                        // Creation flags for Release
#endif
													  &featureLevel,            // Array of desired d3d feature levels
													  1,                        // Number of feature level array
													  D3D11_SDK_VERSION,        // Specific SDK version to use
													  &scDesc,                  // SwapChain creation description
													  &tmpSwapChain,            // SwapChain output
													  &tmpDevice,               // Device output
													  NULL,                     // Feature Level output array of feature levels supported
													  &tmpDeviceContext);       // The primary, immediate context of the DX11 device created.
				if (FAILED(dxRes)) {
					status = EEngineStatus::DXGI_DeviceAndSwapChainCreationFailed;
					goto _return_failed;
				}

			_attachSwapChainToBackBuffer:
				dxRes = tmpSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&tmpBackBufferPtr);
				if (FAILED(dxRes)) {
					status = EEngineStatus::DXSwapChain_AttachSwapChainToBackBuffer_GetBackBufferPtr_Failed;
					goto _return_failed;
				}

				dxRes = tmpDevice->CreateRenderTargetView(tmpBackBufferPtr, NULL /* no RTVDesc */, &tmpBackBufferRTV);
				if (FAILED(dxRes)) {
					status = EEngineStatus::DXDevice_AttachSwapChainToBackBuffer_CreateRTV_Failed;
					goto _return_failed;
				}

				DxRelease(&tmpBackBufferPtr); // Ptr to underlying ID3D11Texture2D-resource of the backbuffer
				                              // is no more needed at this point!

			_createDepthBuffer:
				D3D11_TEXTURE2D_DESC dsTexDesc ={};
				dsTexDesc.Width     = scDesc.BufferDesc.Width;
				dsTexDesc.Height    = scDesc.BufferDesc.Height;
				dsTexDesc.Format    = DX11DeviceCapsHelper::convertFormatGAPI2DXGI(GAPIFormat::D24_UNORM_S8_UINT);
				dsTexDesc.MipLevels = 1;
				dsTexDesc.ArraySize = 1;
				dsTexDesc.Usage     = D3D11_USAGE_DEFAULT;
				dsTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
				// Additional flags
				dsTexDesc.CPUAccessFlags = 0;
				dsTexDesc.MiscFlags      = 0;
				// No depth buffer multisampling
				dsTexDesc.SampleDesc.Count   = 1;
				dsTexDesc.SampleDesc.Quality = 0;

				dxRes = tmpDevice->CreateTexture2D(&dsTexDesc, nullptr, &tmpDepthStencilTexture);
				if (FAILED(dxRes)) {
					status = EEngineStatus::DXDevice_DepthStencilTextureCreationFailed;
					goto _return_failed;
				}

				D3D11_DEPTH_STENCIL_VIEW_DESC dsViewDesc={ };
				dsViewDesc.Format             = DX11DeviceCapsHelper::convertFormatGAPI2DXGI(GAPIFormat::D24_UNORM_S8_UINT);
				dsViewDesc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
				dsViewDesc.Texture2D.MipSlice = 0;

				dxRes = tmpDevice->CreateDepthStencilView(tmpDepthStencilTexture, &dsViewDesc, &tmpDepthStencilView);
				if (FAILED(dxRes)) {
					status = EEngineStatus::DXDevice_DepthStencilStateCreationFailed;
					goto _return_failed;
				}
				// Bind the device context to our backbuffer (with bound swapchain) and the depth stencil view.
				tmpDeviceContext->OMSetRenderTargets(1, &tmpBackBufferRTV, tmpDepthStencilView);
				
			_createDepthStencilState:
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
				if (FAILED(dxRes)) {
					status = EEngineStatus::DXDevice_DepthStencilStateCreationFailed;
					goto _return_failed;
				}

			_createRasterizerState:
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
				if (FAILED(dxRes)) {
					status = EEngineStatus::DXDevice_RasterizerStateCreationFailed;
					goto _return_failed;
				}
				tmpDeviceContext->RSSetState(tmpRasterizerState);

			_createDefaultViewPort:
				D3D11_VIEWPORT vpDesc;
				vpDesc.MinDepth = 0.0f;
				vpDesc.MaxDepth = 1.0f;
				vpDesc.Width    = configuration._preferredWindowSize.x();
				vpDesc.Height   = configuration._preferredWindowSize.y();
				vpDesc.TopLeftX = 0.0f;
				vpDesc.TopLeftY = 0.0f;

				tmpDeviceContext->RSSetViewports(1, &vpDesc);

			_commit:
				_device                 = MakeSharedPointerTypeCustomDeleter(tmpDevice,              DxResourceDeleter<ID3D11Device>());
				_deviceImmediateContext = MakeSharedPointerTypeCustomDeleter(tmpDeviceContext,       DxResourceDeleter<ID3D11DeviceContext>());
				_swapChain              = MakeSharedPointerTypeCustomDeleter(tmpSwapChain,           DxResourceDeleter<IDXGISwapChain>());
				_backBufferRTV          = MakeSharedPointerTypeCustomDeleter(tmpBackBufferRTV,       DxResourceDeleter<ID3D11RenderTargetView>());
				_depthStencilTexture    = MakeSharedPointerTypeCustomDeleter(tmpDepthStencilTexture, DxResourceDeleter<ID3D11Texture2D>());
				_depthStencilView       = MakeSharedPointerTypeCustomDeleter(tmpDepthStencilView,    DxResourceDeleter<ID3D11DepthStencilView>());
				
				_depthStencilState = MakeSharedPointerTypeCustomDeleter(tmpDefaultDepthStencilState, DxResourceDeleter<ID3D11DepthStencilState>());
				_rasterizerState   = MakeSharedPointerTypeCustomDeleter(tmpRasterizerState,          DxResourceDeleter<ID3D11RasterizerState>());

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

				float clearColor[4] = { 1.0f, 0.5f, 0.25f, 1.0f };

				_deviceImmediateContext->ClearRenderTargetView(_backBufferRTV.get(), clearColor);
				_deviceImmediateContext->ClearDepthStencilView(_depthStencilView.get(), D3D11_CLEAR_DEPTH, 1.0f, 0.0f);

				// _deviceImmediateContext->Begin(nullptr);

				// Render commands

				//_deviceImmediateContext->End(nullptr);

				// Activate VSync only in fullscreen, since in windowed mode the 
				// OS will take care of the presentation behaviour and correctness.
				bool useVSync = _config._enableVSync & _config._requestFullscreen;
				_swapChain->Present(useVSync, 0);

				return EEngineStatus::Ok;
			}
		}
	}
}
