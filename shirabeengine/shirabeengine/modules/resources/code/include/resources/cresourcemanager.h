//
// Created by dotti on 19.10.19.
//

#ifndef __SHIRABEDEVELOPMENT_CRESOURCEMANAGER_H__
#define __SHIRABEDEVELOPMENT_CRESOURCEMANAGER_H__

#include <platform/platform.h>
#include "resources/iresourceobject.h"
#include "resources/aresourceobjectfactory.h"

namespace engine {
    namespace resources
    {
        using ResourceId_t = uint64_t;


        class
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT CResourceManager
        {
        public_constructors:
            explicit CResourceManager(Unique<CResourceObjectFactory> aPrivateResourceObjectFactory);

        public_destructors:
            ~CResourceManager() = default;

        public_methods:
            template <typename TResource>
            requires std::is_base_of_v<IResourceObject, TResource>
            CEngineResult<Shared<IResourceObject>> useResource(
                    ResourceId_t                      const &aResourceId
                    , typename TResource::SDescriptor const &aDescriptor);

            CEngineResult<> discardResource(ResourceId_t const &aResourceId);

        private_static_functions:
            static Shared<IResourceObjectPrivate> asPrivate(Shared<IResourceObject> const &aObject);

        private_methods:
            bool storeResourceObject(ResourceId_t                        const &aId
                                     , Shared <IResourceObject> const &aObject);

            void removeResourceObject(ResourceId_t const &aId);

        private_members:
            Unique<CResourceObjectFactory>                            mPrivateResourceObjectFactory;
            std::unordered_map<ResourceId_t, Shared<IResourceObject>> mResourceObjects;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CResourceManager::CResourceManager(Unique<CResourceObjectFactory> aPrivateResourceObjectFactory)
            : mPrivateResourceObjectFactory(std::move(aPrivateResourceObjectFactory))
        { }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<Shared<IResourceObject>> CResourceManager::useResource(
                engine::resources::ResourceId_t   const &aResourceId
                , typename TResource::SDescriptor const &aDescriptor)
        {
            CEngineResult<Shared<IResourceObject>> result = { EEngineStatus::Error, nullptr };

            Shared<TResource> resource = makeShared<TResource>(std::forward(aDescriptor));
            resource->create();

            storeResourceObject(aResourceId, resource);

            return result;
        }
    }
}

#endif //__SHIRABEDEVELOPMENT_CRESOURCEMANAGER_H__
