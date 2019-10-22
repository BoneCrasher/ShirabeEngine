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
        Shared<IResourceObjectPrivate> CResourceManager::asPrivate(Shared<IResourceObject> const &aObject)
        {
            return std::static_pointer_cast<IResourceObjectPrivate>(aObject);
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
                Shared <IResourceObject>       p = mResourceObjects[aResourceId];
                Shared<IResourceObjectPrivate> q = p->getPrivateObject();

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
                                                   , Shared <IResourceObject> const &aObject)
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

    }
}
