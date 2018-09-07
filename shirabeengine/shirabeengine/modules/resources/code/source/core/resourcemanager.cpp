#include "resources/core/resourcemanager.h"

namespace engine
{
    namespace resources
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CResourceManager::CResourceManager(CStdSharedPtr_t<CResourceProxyFactory> const &aProxyFactory)
            : mProxyFactory(aProxyFactory)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CResourceManager::~CResourceManager()
        {
            // m_resources->clear();
            mProxyFactory = nullptr;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CResourceManager::clear()
        {
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CResourceManager::proxyLoad(CStdSharedPtr_t<IResourceProxyBase> proxy, PublicResourceIdList_t const&dependencies)
        {
            EEngineStatus const status = proxy->loadSync(dependencies);
            std::string   const msg    = "Failed to load underlying resource of resource proxy.";
            HandleEngineStatusError(status, msg);

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CResourceManager
        ::proxyUnload(CStdSharedPtr_t<IResourceProxyBase> &aProxy)
        {
            EEngineStatus const status = aProxy->unloadSync();

            HandleEngineStatusError(status, "Failed to unload underlying resource of resource proxy.");

            return status;
        }
        //<-----------------------------------------------------------------------------
    }
}
