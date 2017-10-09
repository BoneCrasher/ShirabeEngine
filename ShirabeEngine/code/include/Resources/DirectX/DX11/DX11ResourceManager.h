#ifndef __SHIRABE_DX11RESOURCEMANAGER_H__
#define __SHIRABE_DX11RESOURCEMANAGER_H__

#include "Platform/Platform.h"
#include "Core/EngineTypeHelper.h"

#include "GFXAPI/Config.h"
#include "GFXAPI/DirectX/DX11/DX11Common.h"
#include "GFXAPI/DirectX/DX11/DX11Types.h"
#include "GFXAPI/DirectX/DX11/DX11Device.h"

#include "Resources/System/Core/ProxyBasedResourceManager.h"

#include "Resources/DirectX/DX11/Builders/TextureND.h"
#include "Resources/DirectX/DX11/Builders/RenderTargetView.h"
#include "Resources/DirectX/DX11/Builders/ShaderResourceView.h"

namespace Engine {
	namespace DX {
		namespace _11 {
			using namespace Resources;

			class DX11ResourceManager
				: public ProxyBasedResourceManager {
				DeclareLogTag(DX11ResourceManager);
				
			public:
				DX11ResourceManager(
					const IDXDevicePtr& device
				);

				~DX11ResourceManager();

				EEngineStatus createDevice(
					const DeviceDescriptor &deviceDescriptor,
					ResourceHandle         &outHandle
				);

				EEngineStatus createTexture1D(
					const Texture1DDescriptor &desc,
					ResourceHandle            &outHandle
				);

				EEngineStatus createTexture2D(
					const Texture2DDescriptor &desc,
					ResourceHandle            &outHandle
				);

				EEngineStatus createTexture3D(
					const Texture3DDescriptor &desc,
					ResourceHandle            &outHandle
				);

				EEngineStatus createShaderResource(
					const ShaderResourceDescriptor &shaderResourceDescriptor,
					const ResourceHandle           &inUnderlyingResourceHandle,
					ResourceHandle                 &outShaderResourceHandle
				);

				EEngineStatus createRenderTarget(
					const ResourceHandle &inHandle,
					ResourceHandle       &outHandle
				);

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