//
// Created by dotti on 19.10.19.
//

#include <unordered_map>
#include "resources/cresourcemanager.h"


namespace engine
{
    namespace resources
    {

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CResourceManager::CResourceManager(Unique<CGpuApiResourceObjectFactory> aPrivateResourceObjectFactory)
                : mGpuApiResourceObjectFactory(std::move(aPrivateResourceObjectFactory))
        { }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<Shared<ILogicalResourceObject>> CResourceManager::useAssetResource(  ResourceId_t const &aResourceId
                                                                                         , AssetId_t    const &aAssetResourceId)
        {
            return { EEngineStatus::Ok, nullptr };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<> CResourceManager::discardResource(ResourceId_t const &aResourceId)
        {
            auto iterator = mResourceObjects.find(aResourceId);
            if(mResourceObjects.end() != iterator)
            {
                mResourceTree.remove(aResourceId);

                Shared<ILogicalResourceObject>        p = mResourceObjects[aResourceId];
                GpuApiHandle_t                 const &q = p->getGpuApiResourceHandle();

                mGpuApiResourceObjectFactory->destroy(q);

                removeResourceObject(aResourceId);
            }

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        bool CResourceManager::storeResourceObject(ResourceId_t               const &aId
                                                   , Shared <ILogicalResourceObject> const &aObject)
        {
            bool const hasObjectForId = (mResourceObjects.end() != mResourceObjects.find( aId));
            if(false == hasObjectForId)
            {
                mResourceObjects[aId] = aObject;
            }

            return (false == hasObjectForId);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        void CResourceManager::removeResourceObject(ResourceId_t const &aId)
        {
            bool const hasObjectForId = (mResourceObjects.end() != mResourceObjects.find(aId));
            if(hasObjectForId)
            {
                mResourceObjects.erase(aId);
            }

        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        GpuApiResourceDependencies_t CResourceManager::getGpuApiDependencies(ResourceId_t const &aId)
        {
            GpuApiResourceDependencies_t dependencies {};
            for(auto const         &dependencyId : mResourceTree.getAdjacentFor(aId))
            {
                Shared<ILogicalResourceObject> logicalResource = mResourceObjects.at(dependencyId);
                dependencies.insert({ dependencyId, logicalResource->getGpuApiResourceHandle() });
            }
            return dependencies;
        }
        //<-----------------------------------------------------------------------------

    }
}
