#include "Resources/System/Core/ProxyBasedResourceManager.h"

namespace Engine {
  namespace Resources {

    bool ProxyBasedResourceManager
      ::clear()
    {
    }

#define ImplementResourceMethods(Type)                   \
    EEngineStatus ProxyBasedResourceManager              \
    ::create##Type(                                      \
      PublicResourceId_t    const&inId,                  \
      Type::CreationRequest const&inRequest)             \
    {                                                    \
      return createImpl<Type>(inId, inRequest);          \
    }                                                    \
                                                         \
    EEngineStatus                                        \
      ProxyBasedResourceManager::create##Type(           \
        Type::CreationRequest const&inRequest,           \
        PublicResourceId_t         &outId)               \
    {                                                    \
      return createDynamicImpl<Type>(inRequest, outId);  \
    }                                                    \
                                                         \
    EEngineStatus ProxyBasedResourceManager              \
        ::update##Type(                                  \
        PublicResourceId_t  const&inId,                  \
        Type::UpdateRequest const&inRequest)             \
    {                                                    \
      return updateImpl<Type>(inId, inRequest);          \
    }                                                    \
                                                         \
     EEngineStatus                                       \
        ProxyBasedResourceManager::destroy##Type(        \
        PublicResourceId_t       const&inId,             \
        Type::DestructionRequest const&inRequest)        \
    {                                                    \
      return destroyImpl<Type>(inId, inRequest);         \
    }                                                    \
                                                         \
    EEngineStatus                                        \
      ProxyBasedResourceManager::get##Type##Info(        \
        PublicResourceId_t const&id,                     \
        Type::Descriptor        &outDescriptor,          \
        Type::Binding           &outBinding)             \
    {                                                    \
      return getResourceInfoImpl<Type>(                  \
                id,                                      \
                outDescriptor,                           \
                outBinding);                             \
    }

    ImplementResourceMethods(SwapChain);
    ImplementResourceMethods(Texture1D);
    ImplementResourceMethods(Texture2D);
    ImplementResourceMethods(Texture3D);
    ImplementResourceMethods(RenderTargetView);
    ImplementResourceMethods(ShaderResourceView);
    ImplementResourceMethods(DepthStencilView);
    ImplementResourceMethods(DepthStencilState);
  }
}