#ifndef __SHIRABE_RESOURCES_XMMATRIX_H__
#define __SHIRABE_RESOURCES_XMMATRIX_H__

#include "GFXAPI/DirectX/DX11/DX11Types.h"

#include "Resources/System/Core/ResourceTask.h"

namespace Engine {
	namespace Resources {

		using namespace Engine::DX::_11;

		struct DX11TaskParameters {
			ID3D11DevicePtr device;
		};
		
		template <
			typename TResource,
			typename TResourceBasePtr>
	    using DX11ResourceTaskBase = ResourceTaskBase<TResource, DX11TaskParameters, TResourceBasePtr>;
	}
}

#endif 