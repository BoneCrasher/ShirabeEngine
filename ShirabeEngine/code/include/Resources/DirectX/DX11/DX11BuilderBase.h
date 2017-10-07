#ifndef __SHIRABE_RESOURCES_BUILDERBASE_H__
#define __SHIRABE_RESOURCES_BUILDERBASE_H__

#include "GFXAPI/DirectX/DX11/DX11Types.h"

#include "Resources/System/Core/ResourceBuilder.h"

namespace Engine {
	namespace Resources {

		using namespace Engine::DX::_11;

		struct DX11BuilderParameters {
			ID3D11DevicePtr device;
		};
		
		template <
			EResourceType    type,
			EResourceSubType subtype,
			typename         TResourceBasePtr>
	    using DX11ResourceBuilderBase = ResourceBuilderBase<type, subtype, DX11BuilderParameters, TResourceBasePtr>;
	}
}

#endif 