//
// Created by dotti on 19.10.19.
//

#ifndef __SHIRABEDEVELOPMENT_CRESOURCEMANAGER_H__
#define __SHIRABEDEVELOPMENT_CRESOURCEMANAGER_H__

#include <platform/platform.h>
#include "resources/iresourceobjectprivate.h"
#include "resources/iresourceobject.h"

namespace engine {
    namespace resources
    {
        using ResourceId_t = uint64_t;


        class
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT CResourceManager
        {
        public_constructors:
            CResourceManager() = default;

        public_destructors:
            ~CResourceManager() = default;

        public_methods:
            template <typename TResource>
            requires std::is_base_of_v<IResourceObject, TResource>
            CEngineResult<CStdSharedPtr_t<IResourceObject>> useResource(
                    ResourceId_t                      const &aResourceId
                    , typename TResource::SDescriptor const &aDescriptor);

            CEngineResult<> discardResource(ResourceId_t const &aResourceId);

        private_static_functions:
            static CStdSharedPtr_t<IResourceObjectPrivate> asPrivate(CStdSharedPtr_t<IResourceObject> const &aObject);

        private_methods:
            bool storeResourceObject(ResourceId_t                        const &aId
                                     , CStdSharedPtr_t <IResourceObject> const &aObject);

            void removeResourceObject(ResourceId_t const &aId);

        private_members:
            std::unordered_map<ResourceId_t, CStdSharedPtr_t<IResourceObject>> mResourceObjects;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<CStdSharedPtr_t<IResourceObject>> CResourceManager::useResource(
                engine::resources::ResourceId_t   const &aResourceId
                , typename TResource::SDescriptor const &aDescriptor)
        {
            CEngineResult<CStdSharedPtr_t<IResourceObject>> result = { EEngineStatus::Error, nullptr };

            CStdSharedPtr_t<TResource> resource = makeCStdSharedPtr<TResource>(std::forward(aDescriptor));
            resource->create();

            storeResourceObject(aResourceId, resource);

            return result;
        }
    }
}

#endif //__SHIRABEDEVELOPMENT_CRESOURCEMANAGER_H__
