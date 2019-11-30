//
// Created by dotti on 19.10.19.
//

#include <unordered_map>
#include <asset/assettypes.h>
#include "resources/resourcetypes.h"
#include "resources/cresourcemanager.h"


namespace engine
{
    namespace resources
    {

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CResourceManager::CResourceManager(Unique<CGpuApiResourceObjectFactory> aPrivateResourceObjectFactory
                                         , Shared<asset::IAssetStorage>         aAssetStorage)
                : mGpuApiResourceObjectFactory(std::move(aPrivateResourceObjectFactory))
                , mAssetStorage               (std::move(aAssetStorage))
                , mAssetLoaders               ()
                , mResourceObjects            ()
                , mResourceTree               ()
        { }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<Shared<ILogicalResourceObject>> CResourceManager::useAssetResource(  ResourceId_t const &aResourceId
                                                                                         , AssetId_t    const &aAssetResourceId)
        {
            Shared<ILogicalResourceObject> object = getResourceObject(aResourceId);
            if(nullptr != object)
            {
                return { EEngineStatus::Ok, object };
            }

            auto const &[result, asset] = mAssetStorage->loadAsset(aAssetResourceId);
            if(CheckEngineError(result))
            {
                CLog::Error(logTag(), "Failed to load asset w/ ID {}", aAssetResourceId);
                return EEngineStatus::Error;
            }

            switch(asset.type)
            {
                case asset::EAssetType::Mesh:
                    // return genericAssetLoading<SMesh>(aResourceId, aAssetResourceId);
                    break;
                case asset::EAssetType::Material:
                    return genericAssetLoading<SMaterial>(aResourceId, aAssetResourceId);
                    break;
                case asset::EAssetType::Buffer:
                    return genericAssetLoading<SBuffer>(aResourceId, aAssetResourceId);
                    break;
                case asset::EAssetType::Texture:
                    return genericAssetLoading<STexture>(aResourceId, aAssetResourceId);
                    break;
                default:
                    break;
            }

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
        bool CResourceManager::storeResourceObject(ResourceId_t                      const &aId
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
        Shared<ILogicalResourceObject> CResourceManager::getResourceObject(ResourceId_t const &aId)
        {
            bool const hasObjectForId = (mResourceObjects.end() != mResourceObjects.find(aId));
            if(hasObjectForId)
            {
                mResourceObjects.at(aId);
            }

            return nullptr;
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
            auto const adjacent = mResourceTree.getAdjacentFor(aId);
            for(auto const &dependencyId : adjacent)
            {
                Shared<ILogicalResourceObject> logicalResource = mResourceObjects.at(dependencyId);
                dependencies.insert({ dependencyId, logicalResource->getGpuApiResourceHandle() });
            }
            return dependencies;
        }
        //<-----------------------------------------------------------------------------

    }
}
