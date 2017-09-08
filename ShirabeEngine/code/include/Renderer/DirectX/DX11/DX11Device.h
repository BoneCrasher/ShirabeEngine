#ifndef __SHIRABE_DX11_DEVICE_H__
#define __SHIRABE_DX11_DEVICE_H__

#include "Renderer/DirectX/IDXDevice.h"

namespace Engine {
	namespace DX {
		namespace _11 {

			class DX11Device
				: public IDXDevice
			{
			public:
				DX11Device();
				~DX11Device();

				EEngineStatus initialize();
				EEngineStatus deinitialize();
			};
		}
	}
}

#endif