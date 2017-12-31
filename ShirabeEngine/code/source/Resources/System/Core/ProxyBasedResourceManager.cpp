#include "Resources/System/Core/ProxyBasedResourceManager.h"

namespace Engine {
  namespace Resources {

    bool ProxyBasedResourceManager
      ::clear()
    {
    }

#define ImplementResourceMethods(Type)                    \
    EEngineStatus ProxyBasedResourceManager               \
    ::create##Type(                                       \
      Type::CreationRequest const&inRequest,              \
      Ptr<Type>                  &out)                    \
    {                                                     \
      return createImpl<Type>(inRequest, out);            \
    }                                                     \
                                                          \
      EEngineStatus ProxyBasedResourceManager             \
        ::update##Type(                                   \
        Type::UpdateRequest const&inRequest,              \
        ResourceHandle      const&inHandle)               \
    {                                                     \
      return updateImpl<Type>(inRequest, inHandle);       \
    }                                                     \
                                                          \
     EEngineStatus ProxyBasedResourceManager              \
        ::destroy##Type(                                  \
        Type::DestructionRequest const&inRequest,         \
        ResourceHandle           const&inHandle)          \
    {                                                     \
      return destroyImpl<Type>(inRequest, inHandle);      \
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