#ifndef __SHIRABE_IRESOURCEMANAGER_H__
#define __SHIRABE_IRESOURCEMANAGER_H__

#include "Core/EngineTypeHelper.h"
#include "Core/EngineStatus.h"

#include "Resources/Handle.h"
#include "Resources/ResourceDescriptors.h"

namespace Engine {
	namespace Resources {

		DeclareInterface(IResourceManager);

		virtual EEngineStatus createSwapChain(
			const SwapChainDescriptor &desc,
			ResourceHandle            &outSwapChainHandle,
			ResourceHandleList        &outBackBufferHandles
		) = 0;

		virtual EEngineStatus createTexture1D
		(
			const Texture1DDescriptor &desc,
			ResourceHandle            &outHandle
		) = 0;
		virtual EEngineStatus createTexture2D
		(
			const Texture2DDescriptor &desc,
			ResourceHandle            &outHandle
		) = 0;
		virtual EEngineStatus createTexture3D
		(
			const Texture3DDescriptor &desc,
			ResourceHandle            &outHandle
		) = 0;

		virtual EEngineStatus createShaderResource(
			const ResourceHandle& inHandle
		) = 0;

		virtual EEngineStatus createRenderTarget(
			const ResourceHandle &inHandle,
			ResourceHandle       &outHandle
		) = 0;

		DeclareInterfaceEnd(IResourceManager);
		DeclareSharedPointerType(IResourceManager);
	}
}

#endif