#ifndef __SHIRABE_IRESOURCEMANAGER_H__
#define __SHIRABE_IRESOURCEMANAGER_H__

#include "Core/EngineTypeHelper.h"
#include "Core/EngineStatus.h"

#include "Handle.h"
#include "ResourceDomainTransfer.h"

#include "Resources/Subsystems/GFXAPI/GFXAPIResourceBackend.h"

#include "GFXAPI/Types/SwapChain.h"
#include "GFXAPI/Types/TextureND.h"
#include "GFXAPI/Types/DepthStencilState.h"
#include "GFXAPI/Types/RasterizerState.h"

namespace Engine {
  namespace Resources {

    typedef uint64_t PublicResourceId_t;

    template <typename TResource>
    struct ResourceInfo {
      PublicResourceId_t             resourceId;
      ResourceHandle                 handle;
      typename TResource::Descriptor descriptor;
      typename TResource::Binding    binding;

      ResourceInfo()
        : resourceId(0)
        , handle(ResourceHandle::Invalid())
        , descriptor({})
        , binding({})
      {}

      ResourceInfo(
        PublicResourceId_t             inResourceId,
        ResourceHandle                 inHandle,
        typename TResource::Descriptor inDescriptor,
        typename TResource::Binding    inBinding)
        : resourceId(inResourceId)
        , handle(inHandle)
        , descriptor(inDescriptor)
        , binding(inBinding)
      {}
    };

#define DeclareResourceMethods(resource)          \
  virtual EEngineStatus create##resource(         \
    PublicResourceId_t        const&inId,         \
    resource::CreationRequest const&inRequest     \
  ) = 0;                                          \
                                                  \
  virtual EEngineStatus create##resource(         \
    resource::CreationRequest const&inRequest,    \
    PublicResourceId_t             &outId         \
  ) = 0;                                          \
                                                  \
  virtual EEngineStatus update##resource(         \
    PublicResourceId_t      const&inId,           \
    resource::UpdateRequest const&inRequest       \
  ) = 0;                                          \
                                                  \
  virtual EEngineStatus destroy##resource(        \
    PublicResourceId_t const&inId                 \
  ) = 0;                                          \
                                                  \
  virtual EEngineStatus get##resource##Info(      \
    PublicResourceId_t     const&id,              \
    ResourceInfo<resource>      &outInfo          \
  ) =0;
  
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