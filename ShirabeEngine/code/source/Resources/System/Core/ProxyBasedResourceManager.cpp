#include "Resources/System/Core/ProxyBasedResourceManager.h"

namespace Engine {
  namespace Resources {

    bool ProxyBasedResourceManager
      ::clear()
    {
      return true;
    }

    #define ImplementResourceMethods(Type)                      \
    EEngineStatus                                               \
      ProxyBasedResourceManager                                 \
      ::create##Type(                                           \
      Type::CreationRequest const&inRequest,                    \
      PublicResourceId_t         &outId,                        \
      bool                        deferLoad)                    \
    {                                                           \
      return createResource<Type>(inRequest, outId, deferLoad); \
    }                                                           \
                                                                \
    EEngineStatus                                               \
      ProxyBasedResourceManager                                 \
        ::load##Type(                                           \
          PublicResourceId_t const&id)                          \
    {                                                           \
      return loadResource<Type>(id);                            \
    }                                                           \
                                                                \
    EEngineStatus                                               \
      ProxyBasedResourceManager                                 \
      ::update##Type(                                           \
        PublicResourceId_t  const&inId,                         \
        Type::UpdateRequest const&inRequest)                    \
    {                                                           \
      return updateResource<Type>(inId, inRequest);             \
    }                                                           \
                                                                \
    EEngineStatus                                               \
      ProxyBasedResourceManager                                 \
      ::unload##Type(                                           \
        PublicResourceId_t const&id)                            \
    {                                                           \
      return unloadResource<Type>(id);                          \
    }                                                           \
                                                                \
     EEngineStatus                                              \
      ProxyBasedResourceManager                                 \
      ::destroy##Type(                                          \
        PublicResourceId_t const&inId)                          \
    {                                                           \
      return destroyResource<Type>(inId);                       \
    }                                                    


    ImplementResourceMethods(SwapChain);
    ImplementResourceMethods(Texture1D);
    ImplementResourceMethods(Texture2D);
    ImplementResourceMethods(Texture3D);
    ImplementResourceMethods(RenderTargetView);
    ImplementResourceMethods(ShaderResourceView);
    ImplementResourceMethods(DepthStencilView);
    ImplementResourceMethods(DepthStencilState);
    ImplementResourceMethods(RasterizerState);

    /**********************************************************************************************//**
     * \fn	EEngineStatus ProxyBasedResourceManager::proxyLoad(const ResourceHandle& handle, const AnyProxy& proxy)
     *
     * \brief	Load dependencies and root of resource tree. This function only deals with root
     * 			elements of a resource-tree.
     *
     * \param	handle	The handle.
     * \param	proxy 	The proxy.
     *
     * \return	The EEngineStatus.
     **************************************************************************************************/
    EEngineStatus ProxyBasedResourceManager
      ::proxyLoad(IResourceProxyBasePtr &proxy, ResourceProxyList const&dependencies)
    {
      EEngineStatus status = EEngineStatus::Ok;

      SubjacentResourceIdList proxyDependencies={};
      for(AnyProxy const&dependency : dependencies) {
        Ptr<IResourceProxyBase> base = BaseProxyCast(dependency);
        proxyDependencies.push_back(base->subjacentResourceId());
      }

      std::string msg = "Failed to load underlying resource of resource proxy.";
      HandleEngineStatusError(proxy->loadSync(proxyDependencies), msg);

      return EEngineStatus::Ok;
    }

    EEngineStatus ProxyBasedResourceManager
      ::proxyUnload(IResourceProxyBasePtr &proxy)
    {
      EEngineStatus status = EEngineStatus::Ok;

      // Deal with dependers?

      HandleEngineStatusError(proxy->unloadSync(), "Failed to unload underlying resource of resource proxy.");

      return EEngineStatus::Ok;
    }
  }
}