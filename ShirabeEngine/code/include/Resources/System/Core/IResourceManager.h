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

#define DeclareCreationMethod(resource)            \
  virtual EEngineStatus create##resource(          \
    const resource::CreationRequest &inRequest,    \
    Ptr<resource>                   &out##resource \
  ) = 0;
    
		 DeclareInterface(IResourceManager);

    virtual bool clear() = 0;

    DeclareCreationMethod(SwapChain);
    DeclareCreationMethod(Texture1D);
    DeclareCreationMethod(Texture2D);
    DeclareCreationMethod(Texture3D);

		DeclareInterfaceEnd(IResourceManager);
		DeclareSharedPointerType(IResourceManager);
	}
}

#endif