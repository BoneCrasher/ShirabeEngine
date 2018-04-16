#ifndef __SHIRABE_IRESOURCEMANAGER_H__
#define __SHIRABE_IRESOURCEMANAGER_H__

#include "Core/EngineTypeHelper.h"
#include "Core/EngineStatus.h"


#include "ResourceDomainTransfer.h"

#include "Resources/Core/ResourceDTO.h"
#include "Resources/Subsystems/GFXAPI/GFXAPIResourceBackend.h"

#include "GFXAPI/Types/SwapChain.h"
#include "GFXAPI/Types/TextureND.h"
#include "GFXAPI/Types/DepthStencilState.h"
#include "GFXAPI/Types/RasterizerState.h"

namespace Engine {
  namespace Resources {

    #define DeclareResourceMethods(resource)          \
                                                      \
  virtual EEngineStatus create##resource(             \
    resource::CreationRequest const&inRequest,        \
    PublicResourceId_t             &outId,            \
    bool                            deferLoad = false \
  ) = 0;                                              \
                                                      \
  virtual EEngineStatus load##resource(               \
    PublicResourceId_t const&inId                     \
  ) = 0;                                              \
                                                      \
  virtual EEngineStatus update##resource(             \
    PublicResourceId_t      const&inId,               \
    resource::UpdateRequest const&inRequest           \
  ) = 0;                                              \
                                                      \
  virtual EEngineStatus unload##resource(             \
    PublicResourceId_t const&inId                     \
  ) = 0;                                              \
                                                      \
  virtual EEngineStatus destroy##resource(            \
    PublicResourceId_t const&inId                     \
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
      DeclareResourceMethods(RasterizerState);

    DeclareInterfaceEnd(IResourceManager);
    DeclareSharedPointerType(IResourceManager);
  }
}

#endif