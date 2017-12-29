#include "Resources/System/Core/ProxyBasedResourceManager.h"

namespace Engine {
  namespace Resources {


    bool ProxyBasedResourceManager
      ::clear()
    {
    }

#define ImplementCreator(Type)                            \
    EEngineStatus ProxyBasedResourceManager::create##Type(\
      Type::CreationRequest const&inRequest,              \
      Ptr<Type>                  &out)                    \
    {                                                     \
      return createImpl<Type>(inRequest, out);            \
    }

    ImplementCreator(SwapChain);
    ImplementCreator(Texture1D);
    ImplementCreator(Texture2D);
    ImplementCreator(Texture3D);
    ImplementCreator(RenderTargetView);
    ImplementCreator(ShaderResourceView);
    ImplementCreator(DepthStencilView);
    ImplementCreator(DepthStencilState);
  }
}