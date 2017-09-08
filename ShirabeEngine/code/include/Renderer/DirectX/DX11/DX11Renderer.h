#ifndef __SHIRABE_DX11_RENDERER_H__
#define __SHIRABE_DX11_RENDERER_H__

#include <atomic>

#include <d3d.h>
#include <d3d11.h>

#include "Renderer/IRenderer.h"

#include "DX11Linkage.h"
#include "DX11DeviceCapabilities.h"
#include "DX11Device.h"

namespace Engine {
	namespace DX {
		namespace _11 {
			using namespace Engine::Renderer;

			DeclareSharedPointerType(ID3D11Device);
			DeclareSharedPointerType(ID3D11DeviceContext);
			DeclareSharedPointerType(IDXGISwapChain);
			DeclareSharedPointerType(ID3D11RenderTargetView);
			DeclareSharedPointerType(ID3D11Texture2D);
			DeclareSharedPointerType(ID3D11DepthStencilState);
			DeclareSharedPointerType(ID3D11DepthStencilView);
			DeclareSharedPointerType(ID3D11RasterizerState);

			class DX11Renderer
				: public IRenderer
			{
			public:
				DX11Renderer();
				~DX11Renderer();

				EEngineStatus initialize(const ApplicationEnvironment&,
										 const RendererConfiguration&
				/* Insert Resourcemanager pointer here */);
				EEngineStatus deinitialize();
				EEngineStatus reinitialize();

				EEngineStatus pause();
				EEngineStatus resume();
				bool          isPaused() const;

				EEngineStatus render(/* insert queue type*/);

			private:
				RendererConfiguration _config;

				// Fixed DX component handles and resources
				ID3D11DevicePtr            _device;
				ID3D11DeviceContextPtr     _deviceImmediateContext;
				IDXGISwapChainPtr          _swapChain;
				ID3D11RenderTargetViewPtr  _backBufferRTV;
				ID3D11Texture2DPtr         _depthStencilTexture;
				ID3D11DepthStencilViewPtr  _depthStencilView;

				// Mutable DX component states
				ID3D11DepthStencilStatePtr _depthStencilState;
				ID3D11RasterizerStatePtr   _rasterizerState;

				std::atomic_bool _paused;
			};

		}
	}
}

#endif