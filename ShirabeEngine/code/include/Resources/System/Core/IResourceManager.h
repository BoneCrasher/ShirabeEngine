#ifndef __SHIRABE_IRESOURCEMANAGER_H__
#define __SHIRABE_IRESOURCEMANAGER_H__

#include "Core/EngineTypeHelper.h"
#include "Core/EngineStatus.h"

#include "Handle.h"
#include "IResourceDescriptor.h"

#include "GFXAPI/Types/SwapChain.h"
#include "GFXAPI/Types/ShaderResourceView.h"
#include "GFXAPI/Types/TextureND.h"

namespace Engine {
	namespace Resources {

		DeclareInterface(IResourceManager);

		virtual EEngineStatus createSwapChain(
			const ResourceCreationRequest<SwapChain> &inRequest,
			Ptr<SwapChain>                           &outSwapChain) = 0;

		virtual EEngineStatus createTexture1D
		(
			const ResourceCreationRequest<Texture1D> &request,
			Ptr<Texture1D>                           &outTexture1D
		) = 0;
		virtual EEngineStatus createTexture2D
		(
			const ResourceCreationRequest<Texture2D> &request,
			Ptr<Texture2D>                           &outTexture2D
		) = 0;
		virtual EEngineStatus createTexture3D
		(
			const ResourceCreationRequest<Texture3D> &request,
			Ptr<Texture3D>                           &outTexture3D
		) = 0;

		DeclareInterfaceEnd(IResourceManager);
		DeclareSharedPointerType(IResourceManager);
	}
}

#endif