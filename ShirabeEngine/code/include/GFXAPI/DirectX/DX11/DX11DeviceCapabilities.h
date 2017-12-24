#ifndef __SHIRABE_DX11_DEVICECAPABILITIES_H__
#define __SHIRABE_DX11_DEVICECAPABILITIES_H__

#include "GFXAPI/DirectX/DX11/DX11Common.h"

#include "Core/BasicTypes.h"
#include "Core/EngineStatus.h"
#include "Core/EngineTypeHelper.h"
#include "Log/Log.h"
#include "GFXAPI/GFXAPIDeviceCapabilities.h"

namespace Engine {
	namespace DX {
		namespace _11 {
			using namespace GFXAPI;

			class DX11DeviceCapsHelper {
				DeclareLogTag(DX11DeviceCapsHelper)
			public:

				static DXGI_FORMAT convertFormatGAPI2DXGI(const Format& fmt); 
				static Format      convertFormatDXGI2GAPI(const DXGI_FORMAT& fmt);
         
				static D3D11_USAGE   convertResourceUsageGAPI2D3D11(const ResourceUsage&);
				static ResourceUsage convertResourceUsageD3D112GAPI(const D3D11_USAGE&);

				static UINT                 convertBufferBindingGAPI2D3D11(const BufferBindingFlags_t&);
				static BufferBindingFlags_t convertBufferBindingD3D112GAPI(const UINT&);

			};

		}
	}
}

#endif