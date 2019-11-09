//
// Created by dotti on 19.10.19.
//

#include <unordered_map>
#include "resources/cresourcemanager.h"
#include "cgpuapiresourcestorage.cpp"

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
        Shared<IGpuApiResourceObject> CResourceManager::asPrivate(Shared<ILogicalResourceObject> const &aObject)
        {
            return std::static_pointer_cast<IGpuApiResourceObject>(aObject);
        }
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

                Shared<ILogicalResourceObject>  p = mResourceObjects[aResourceId];
                Unique<IGpuApiResourceObject>  &q = p->getGpuApiResourceInterface();

                SHIRABE_EXPLICIT_DISCARD(p->unbind());
                SHIRABE_EXPLICIT_DISCARD(q->unload());
                SHIRABE_EXPLICIT_DISCARD(q->destroy());

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
        CGpuApiResourceStorage CResourceManager::getGpuApiDependencies(ResourceId_t const &aId)
        {
            CGpuApiResourceStorage dependencies {};
            for(auto const         &dependencyId : mResourceTree.getAdjacentFor(aId))
            {
                Shared<ILogicalResourceObject> logicalResource = mResourceObjects.at(dependencyId);
                Unique<IGpuApiResourceObject> &gpuapiResource  = logicalResource->getGpuApiResourceInterface();
                dependencies.add( dependencyId, gpuapiResource );
            }
            return dependencies;
        }
        //<-----------------------------------------------------------------------------

    }
}
