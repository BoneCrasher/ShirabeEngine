#ifndef __SHIRABE_DX11_DEVICECAPABILITIES_H__
#define __SHIRABE_DX11_DEVICECAPABILITIES_H__

#include <dxgi.h>

#include "Core/BasicTypes.h"
#include "Core/EngineStatus.h"
#include "Core/EngineTypeHelper.h"
#include "Log/Log.h"
#include "Renderer/GAPIDeviceCapabilities.h"

namespace Engine {
	namespace DX {
		namespace _11 {
			using namespace GAPI;		

			class DX11DeviceCapsHelper {
				DeclareLogTag(DX11DeviceCapsHelper)
			public:

				static DXGI_FORMAT convertFormatGAPI2DXGI(const GAPIFormat& fmt); 
				static GAPIFormat  convertFormatDXGI2GAPI(const DXGI_FORMAT& fmt);

				static EEngineStatus GetDeviceCapabilities(
					const GAPIFormat&, 
					GAPIDeviceCapabilities*);
			};

		}
	}
}

#endif