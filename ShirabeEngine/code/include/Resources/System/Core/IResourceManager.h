#ifndef __SHIRABE_IRESOURCEMANAGER_H__
#define __SHIRABE_IRESOURCEMANAGER_H__

#include "Core/EngineTypeHelper.h"
#include "Core/EngineStatus.h"

#include "Resources/System/Core/Handle.h"

#include "GFXAPI/Types/SwapChain.h"
#include "GFXAPI/Types/ShaderResourceView.h"
#include "GFXAPI/Types/TextureND.h"

namespace Engine {
	namespace Resources {

		DeclareInterface(IResourceManager);

		virtual EEngineStatus createSwapChain(
			const SwapChainDescriptor &inDesc,
			Ptr<SwapChain>            &outSwapChain) = 0;

		virtual EEngineStatus createTexture1D
		(
			const Texture1DDescriptor &desc,
			Ptr<Texture1D>            &outTexture1D
		) = 0;
		virtual EEngineStatus createTexture2D
		(
			const Texture2DDescriptor &desc,
			Ptr<Texture2D>            &outTexture2D
		) = 0;
		virtual EEngineStatus createTexture3D
		(
			const Texture3DDescriptor &desc,
			Ptr<Texture3D>            &outTexture3D
		) = 0;

		DeclareInterfaceEnd(IResourceManager);
		DeclareSharedPointerType(IResourceManager);
	}
}

#endif