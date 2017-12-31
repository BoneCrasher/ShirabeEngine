#ifndef __SHIRABE_IRESOURCEMANAGER_H__
#define __SHIRABE_IRESOURCEMANAGER_H__

#include "Core/EngineTypeHelper.h"
#include "Core/EngineStatus.h"

#include "Handle.h"
#include "ResourceDomainTransfer.h"

#include "Resources/Subsystems/GFXAPI/GFXAPIResourceBackend.h"

#include "GFXAPI/Types/SwapChain.h"
#include "GFXAPI/Types/ShaderResourceView.h"
#include "GFXAPI/Types/TextureND.h"

namespace Engine {
  namespace Resources {

#define DeclareResourceMethods(resource)          \
  virtual EEngineStatus create##resource(         \
    resource::CreationRequest const&inRequest,    \
    Ptr<resource>                  &out##resource \
  ) = 0;                                          \
                                                  \
  virtual EEngineStatus update##resource(         \
    resource::UpdateRequest const&inRequest,      \
    ResourceHandle          const&inHandle        \
  ) = 0;                                          \
                                                  \
  virtual EEngineStatus destroy##resource(        \
    resource::DestructionRequest const&inRequest, \
    ResourceHandle               const&inHandle   \
  ) = 0;
  
    DeclareInterface(IResourceManager);

      virtual bool clear() = 0;

      virtual Ptr<BasicGFXAPIResourceBackend>& backend() = 0;

      DeclareResourceMethods(SwapChain);
      DeclareResourceMethods(Texture1D);
      DeclareResourceMethods(Texture2D);
      DeclareResourceMethods(Texture3D);
      DeclareResourceMethods(RenderTargetView);
      DeclareResourceMethods(ShaderResourceView);
      DeclareResourceMethods(DepthStencilView);
      DeclareResourceMethods(DepthStencilState);

    DeclareInterfaceEnd(IResourceManager);
    DeclareSharedPointerType(IResourceManager);
  }
}

#endif