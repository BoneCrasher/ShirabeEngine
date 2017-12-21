#ifndef __SHIRABE_IRESOURCEMANAGER_H__
#define __SHIRABE_IRESOURCEMANAGER_H__

#include "Core/EngineTypeHelper.h"
#include "Core/EngineStatus.h"

#include "Handle.h"
#include "ResourceDomainTransfer.h"

#include "GFXAPI/Types/SwapChain.h"
#include "GFXAPI/Types/ShaderResourceView.h"
#include "GFXAPI/Types/TextureND.h"

namespace Engine {
	namespace Resources {

		DeclareInterface(IResourceManager);

    virtual void clear() = 0;

		virtual EEngineStatus createSwapChain(
			const SwapChain::CreationRequest &inRequest,
			Ptr<SwapChain>                   &outSwapChain) = 0;

		virtual EEngineStatus createTexture1D
		(
			const Texture1D::CreationRequest &request,
			Ptr<Texture1D>                   &outTexture1D
		) = 0;
		virtual EEngineStatus createTexture2D
		(
			const Texture2D::CreationRequest &request,
			Ptr<Texture2D>                   &outTexture2D
		) = 0;
		virtual EEngineStatus createTexture3D
		(
			const Texture3D::CreationRequest &request,
			Ptr<Texture3D>                   &outTexture3D
		) = 0;

		DeclareInterfaceEnd(IResourceManager);
		DeclareSharedPointerType(IResourceManager);
	}
}

#endif