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
            CEngineResult<> const loadOp = aProxy->loadSync(aDependencies);
            std::string     const msg    = "Failed to load underlying resource of resource proxy.";

            EngineStatusPrintOnError(loadOp.result(), logTag(), msg);

            return loadOp;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CResourceManagerBase::proxyUnload(CStdSharedPtr_t<IResourceProxyBase> &aProxy)
        {
            CEngineResult<> const unloadOp = aProxy->unloadSync();

            EngineStatusPrintOnError(unloadOp.result(), logTag(), "Failed to unload underlying resource of resource proxy.");

            return unloadOp;
        }
        //<-----------------------------------------------------------------------------
    }
}
