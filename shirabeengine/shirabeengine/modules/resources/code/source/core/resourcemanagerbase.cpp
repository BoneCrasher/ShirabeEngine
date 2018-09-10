#include "resources/core/resourcemanagerbase.h"

namespace engine
{
    namespace resources
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CResourceManagerBase::CResourceManagerBase(CStdSharedPtr_t<CResourceProxyFactory> const &aProxyFactory)
            : mProxyFactory(aProxyFactory)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CResourceManagerBase::~CResourceManagerBase()
        {
            // m_resources->clear();
            mProxyFactory = nullptr;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CResourceManagerBase::initialize()
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CResourceManagerBase::deinitialize()
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CResourceManagerBase::clear()
        {
            mCreatorFunctions.clear();
            mLoadFunctions.clear();
            mUpdateFunctions.clear();
            mUnloadFunctions.clear();
            mDestructorFunctions.clear();



            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CResourceManagerBase::proxyLoad(CStdSharedPtr_t<IResourceProxyBase> proxy, PublicResourceIdList_t const&dependencies)
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
        EEngineStatus CResourceManagerBase
        ::proxyUnload(CStdSharedPtr_t<IResourceProxyBase> &aProxy)
        {
            EEngineStatus const status = aProxy->unloadSync();

            HandleEngineStatusError(status, "Failed to unload underlying resource of resource proxy.");

            return status;
        }
        //<-----------------------------------------------------------------------------
    }
}
