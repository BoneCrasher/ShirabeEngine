#include "GFXAPI/DirectX/DX11/Device.h"

namespace Engine {
	namespace DX {
		namespace _11 {

			DX11Device::DX11Device()
				: IDXDevice()
			{
			}


			DX11Device::~DX11Device()
			{
			}

			EEngineStatus DX11Device::initialize() {
				return EEngineStatus::Ok;
			}

			EEngineStatus DX11Device::deinitialize() {
				return EEngineStatus::Ok;
			}
		}
	}
}
