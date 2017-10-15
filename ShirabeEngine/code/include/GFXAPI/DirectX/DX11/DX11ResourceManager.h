#ifndef __SHIRABE_DX11RESOURCEMANAGER_H__
#define __SHIRABE_DX11RESOURCEMANAGER_H__

#include "Platform/Platform.h"
#include "Core/EngineTypeHelper.h"

#include "GFXAPI/Config.h"
#include "GFXAPI/DirectX/DX11/DX11Common.h"
#include "GFXAPI/DirectX/DX11/DX11Types.h"
#include "GFXAPI/DirectX/DX11/DX11Device.h"

#include "Resources/System/Core/ProxyBasedResourceManager.h"

#include "GFXAPI/DirectX/DX11/Builders/TextureND.h"
#include "GFXAPI/DirectX/DX11/Builders/RenderTargetView.h"
#include "GFXAPI/DirectX/DX11/Builders/ShaderResourceView.h"

namespace Engine {
	namespace DX {
		namespace _11 {
			using namespace Resources;

			class DX11ResourceManager
				: public ProxyBasedResourceManager {
				DeclareLogTag(DX11ResourceManager);
				
			public:
				DX11ResourceManager(
					const Ptr<ResourceProxyFactory> &proxyFactory,
					const IDXDevicePtr              &device);

				virtual ~DX11ResourceManager();

				// EEngineStatus createDevice(
				// 	const DeviceDescriptor &deviceDescriptor,
				// 	ResourceHandle         &outHandle);
				EEngineStatus createSwapChain(
					const SwapChainDescriptor &inDesc,
					Ptr<SwapChain>            &outSwapChain);

				EEngineStatus createTexture1D(
					const Texture1DDescriptor &desc,
					Ptr<Texture1D>            &outTexture1D);

				EEngineStatus createTexture2D(
					const Texture2DDescriptor &desc,
					Ptr<Texture2D>            &outTexture2D);

				EEngineStatus createTexture3D(
					const Texture3DDescriptor &desc,
					Ptr<Texture3D>            &outTexture3D);

				// EEngineStatus createDepthStencilView(
				// 	const DepthStencilViewDescriptor &desc,
				// 	const ResourceHandle             &inTex2DResourceHandle,
				// 	ResourceHandle                   &outHandle
				// );
				// 
				// EEngineStatus createDepthStencilState(
				// 	const DepthStencilStateDescriptor &desc,
				// 	ResourceHandle                    &outHandle
				// );

			private:
				IDXDevicePtr _dxDevice;
			};
			DeclareSharedPointerType(DX11ResourceManager);
		}
	}
}

#endif