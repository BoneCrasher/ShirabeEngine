#ifndef __SHIRABE_DX11_DEVICE_H__
#define __SHIRABE_DX11_DEVICE_H__

#include "Platform/Platform.h"

#include <d3d11.h>

#include "GFXAPI/DirectX/IDXDevice.h"

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

				inline IDXDevice::InternalDevicePtr& internalDevice() {
					return m_internalDevice;
				}

			private:
				IDXDevice::InternalDevicePtr m_internalDevice;
			};
		}
	}
}

#endif