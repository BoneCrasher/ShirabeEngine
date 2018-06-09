#include "Resources/Core/ResourceManager.h"

namespace Engine {
  namespace Resources {

    bool ResourceManager
      ::clear()
    {
      return true;
    }
    
    /**********************************************************************************************//**
     * \fn	EEngineStatus ResourceManager::proxyLoad(const ResourceHandle& handle, const AnyProxy& proxy)
     *
     * \brief	Load dependencies and root of resource tree. This function only deals with root
     * 			elements of a resource-tree.
     *
     * \param	handle	The handle.
     * \param	proxy 	The proxy.
     *
     * \return	The EEngineStatus.
     **************************************************************************************************/
    EEngineStatus ResourceManager
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

    EEngineStatus ResourceManager
      ::proxyUnload(IResourceProxyBasePtr &proxy)
    {
      EEngineStatus status = EEngineStatus::Ok;

      // Deal with dependers?

      HandleEngineStatusError(proxy->unloadSync(), "Failed to unload underlying resource of resource proxy.");

      return EEngineStatus::Ok;
    }
  }
}