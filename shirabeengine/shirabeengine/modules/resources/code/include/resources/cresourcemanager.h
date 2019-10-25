//
// Created by dotti on 19.10.19.
//

#ifndef __SHIRABEDEVELOPMENT_CRESOURCEMANAGER_H__
#define __SHIRABEDEVELOPMENT_CRESOURCEMANAGER_H__

#include <platform/platform.h>
#include <graphicsapi/definitions.h>
#include <asset/assettypes.h>
#include <core/datastructures/adjacencytree.h>
#include "resources/cresourceobject.h"
#include "resources/aresourceobjectfactory.h"

namespace engine {
    namespace resources
    {
        using asset::AssetId_t ;
        using ResourceId_t = std::string;
        using datastructures::CAdjacencyTree;
        using datastructures::CAdjacencyTreeHelper;

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
            // requires std::is_base_of_v<IResourceObject, TResource>
            CEngineResult<Shared<IResourceObject>> useDynamicResource(
                      ResourceId_t                     const &aResourceId
                    , typename TResource::Descriptor_t const &aDescriptor
                    , std::vector<ResourceId_t>             &&aDependencies = {});

            CEngineResult<Shared<IResourceObject>> useAssetResource(AssetId_t const &aAssetResourceId);

            CEngineResult<> discardResource(ResourceId_t const &aResourceId);

        private_static_functions:
            static Shared<IResourceObjectPrivate> asPrivate(Shared<IResourceObject> const &aObject);

        private_methods:
            bool storeResourceObject(ResourceId_t               const &aId
                                     , Shared <IResourceObject> const &aObject);

            void removeResourceObject(ResourceId_t const &aId);

        private_members:
            Unique<CResourceObjectFactory>                            mPrivateResourceObjectFactory;
            std::unordered_map<ResourceId_t, Shared<IResourceObject>> mResourceObjects;
            CAdjacencyTree<ResourceId_t>                              mResourceTree;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<Shared<IResourceObject>> CResourceManager::useDynamicResource(
                  ResourceId_t                     const &aResourceId
                , typename TResource::Descriptor_t const &aDescriptor
                , std::vector<ResourceId_t>             &&aDependencies)
        {
            CEngineResult<Shared<IResourceObject>> result = { EEngineStatus::Error, nullptr };

            mResourceTree.add(aResourceId);
            for(auto const &dependency : aDependencies)
            {
                mResourceTree.add(dependency);
                mResourceTree.connect(aResourceId, dependency);
            }

            Shared<IResourceObject>        resource        = makeShared<CResourceObject<typename TResource::Descriptor_t>>(aDescriptor);
            Unique<IResourceObjectPrivate> privateResource = mPrivateResourceObjectFactory->create<TResource>(aDescriptor);
            privateResource->create();

            storeResourceObject(aResourceId, resource);

            return result;
        }
    }
}

#endif //__SHIRABEDEVELOPMENT_CRESOURCEMANAGER_H__
