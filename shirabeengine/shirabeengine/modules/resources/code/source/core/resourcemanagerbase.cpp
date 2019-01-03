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
        CEngineResult<> CResourceManagerBase::initialize()
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CResourceManagerBase::deinitialize()
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CResourceManagerBase::clear()
        {
            mCreatorFunctions.clear();
            mLoadFunctions.clear();
            mUpdateFunctions.clear();
            mUnloadFunctions.clear();
            mDestructorFunctions.clear();

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CResourceManagerBase::proxyLoad(CStdSharedPtr_t<IResourceProxyBase> aProxy, PublicResourceIdList_t const &aDependencies)
        {
            EEngineStatus const status = aProxy->loadSync(aDependencies);
            std::string   const msg    = "Failed to load underlying resource of resource proxy.";

            EngineStatusPrintOnError(status, logTag(), msg);

            return { status };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CResourceManagerBase::proxyUnload(CStdSharedPtr_t<IResourceProxyBase> &aProxy)
        {
            EEngineStatus const status = aProxy->unloadSync();

            EngineStatusPrintOnError(status, logTag(), "Failed to unload underlying resource of resource proxy.");

            return { status };
        }
        //<-----------------------------------------------------------------------------
    }
}
