//
// Created by dotti on 19.10.19.
//

#include "resources/cresourcemanager.h"

namespace engine
{
    namespace resources
    {

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CStdSharedPtr_t<IResourceObjectPrivate> CResourceManager::asPrivate(CStdSharedPtr_t<IResourceObject> const &aObject)
        {
            return std::static_pointer_cast<IResourceObjectPrivate>(aObject);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<> CResourceManager::discardResource(ResourceId_t const &aResourceId)
        {
            auto iterator = std::find(mResourceObjects.begin(), mResourceObjects.end(), aResourceId);
            if(mResourceObjects.end() != iterator)
            {
                removeResourceObject(aResourceId);
            }

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        bool CResourceManager::storeResourceObject(ResourceId_t                        const &aId
                                                   , CStdSharedPtr_t <IResourceObject> const &aObject)
        {
            bool const hasObjectForId = (mResourceObjects.end() != std::find(mResourceObjects.begin(), mResourceObjects.end(), aId));
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
            bool const hasObjectForId = (mResourceObjects.end() != std::find(mResourceObjects.begin(), mResourceObjects.end(), aId));
            if(hasObjectForId)
            {
                mResourceObjects.erase(aId);
            }
        }
        //<-----------------------------------------------------------------------------

    }
}
