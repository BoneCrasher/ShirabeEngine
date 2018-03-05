#ifndef __SHIRABE_IDXDEVICE_H__
#define __SHIRABE_IDXDEVICE_H__

#include "Core/EngineStatus.h"
#include "Core/EngineTypeHelper.h"

#include "GFXAPI/DirectX/DX11/Common.h"

namespace Engine {
	namespace DX {

		DeclareInterface(IDXDevice);

		virtual EEngineStatus initialize()   = 0;
		virtual EEngineStatus deinitialize() = 0;

		using InternalDevicePtr = std::shared_ptr<ID3D11Device>;
		virtual InternalDevicePtr& internalDevice() = 0;

		DeclareInterfaceEnd(IDXDevice);
		DeclareSharedPointerType(IDXDevice);

	}
}

#endif