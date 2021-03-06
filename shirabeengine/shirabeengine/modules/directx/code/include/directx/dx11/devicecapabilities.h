#ifndef __SHIRABE_DX11_DEVICECAPABILITIES_H__
#define __SHIRABE_DX11_DEVICECAPABILITIES_H__

#include "GFXAPI/DirectX/DX11/Common.h"

#include "Core/BasicTypes.h"
#include "core/enginestatus.h"
#include "core/enginetypehelper.h"
#include "Core/BitField.h"
#include "Log/Log.h"

#include "Resources/Subsystems/GFXAPI/Types/Definition.h"

#include "GFXAPI/Capabilities.h"

namespace Engine {
	namespace DX {
		namespace _11 {

			using namespace GFXAPI;
      using Engine::Resources::Format;
      using Engine::Core::BitField;

			class DX11DeviceCapsHelper {
				SHIRABE_DECLARE_LOG_TAG(DX11DeviceCapsHelper)
			public:

				static DXGI_FORMAT convertFormatGAPI2DXGI(const Format& fmt); 
				static Format      convertFormatDXGI2GAPI(const DXGI_FORMAT& fmt);
         
				static D3D11_USAGE   convertResourceUsageGAPI2D3D11(const ResourceUsage&);
				static ResourceUsage convertResourceUsageD3D112GAPI(const D3D11_USAGE&);

				static uint32_t                convertBufferBindingGAPI2D3D11(BitField<BufferBinding> const&);
				static BitField<BufferBinding> convertBufferBindingD3D112GAPI(uint32_t const&);

			};

		}
	}
}

#endif