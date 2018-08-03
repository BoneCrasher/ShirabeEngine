#include "Resources/Core/ResourceManager.h"

namespace engine {
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
      ::proxyLoad(CStdSharedPtr_t<IResourceProxyBase> &proxy, PublicResourceIdList const&dependencies)
    {
      EEngineStatus status = EEngineStatus::Ok;

      std::string msg = "Failed to load underlying resource of resource proxy.";
      HandleEngineStatusError(proxy->loadSync(dependencies), msg);

      return EEngineStatus::Ok;
    }

    EEngineStatus ResourceManager
      ::proxyUnload(CStdSharedPtr_t<IResourceProxyBase> &proxy)
    {
      EEngineStatus status = EEngineStatus::Ok;

      // Deal with dependers?

      HandleEngineStatusError(proxy->unloadSync(), "Failed to unload underlying resource of resource proxy.");

      return EEngineStatus::Ok;
    }
  }
}