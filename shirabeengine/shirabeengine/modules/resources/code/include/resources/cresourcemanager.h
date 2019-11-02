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
#include "resources/agpuapiresourceobjectfactory.h"

namespace engine {
    namespace resources
    {
        using asset::AssetId_t ;
        using datastructures::CAdjacencyTree;
        using datastructures::CAdjacencyTreeHelper;

        class
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT CResourceManager
        {
        public_constructors:
            explicit CResourceManager(Unique<CGpuApiResourceObjectFactory> aPrivateResourceObjectFactory);

        public_destructors:
            ~CResourceManager() = default;

        public_methods:
            template <typename TResource>
            // requires std::is_base_of_v<ILogicalResourceObject, TResource>
            CEngineResult<Shared<ILogicalResourceObject>> useDynamicResource(
                      ResourceId_t                     const &aResourceId
                    , typename TResource::Descriptor_t const &aDescriptor
                    , std::vector<ResourceId_t>             &&aDependencies = {});

            CEngineResult<Shared<ILogicalResourceObject>> useAssetResource(  ResourceId_t const &aResourceId
                                                                           , AssetId_t    const &aAssetResourceId);

            CEngineResult<> discardResource(ResourceId_t const &aResourceId);

        private_static_functions:
            static Shared<IGpuApiResourceObject> asPrivate(Shared<ILogicalResourceObject> const &aObject);

        private_methods:
            bool storeResourceObject(ResourceId_t               const &aId
                                     , Shared <ILogicalResourceObject> const &aObject);

            void removeResourceObject(ResourceId_t const &aId);

            CGpiApiDependencyCollection getGpuApiDependencies(ResourceId_t const &aId);

        private_members:
            Unique<CGpuApiResourceObjectFactory>                             mGpuApiResourceObjectFactory;
            std::unordered_map<ResourceId_t, Shared<ILogicalResourceObject>> mResourceObjects;
            CAdjacencyTree<ResourceId_t>                                     mResourceTree;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<Shared<ILogicalResourceObject>> CResourceManager::useDynamicResource(
                  ResourceId_t                     const &aResourceId
                , typename TResource::Descriptor_t const &aDescriptor
                , std::vector<ResourceId_t>             &&aDependencies)
        {
            CEngineResult<Shared<ILogicalResourceObject>> result = {EEngineStatus::Error, nullptr };

            mResourceTree.add(aResourceId);
            for(auto const &dependency : aDependencies)
            {
                mResourceTree.add    (dependency);
                mResourceTree.connect(aResourceId, dependency);
            }



            // TODO: Dependency check. Already loaded? Etc...
            Shared<ILogicalResourceObject> resource       = makeShared<CResourceObject<typename TResource::Descriptor_t>>(aDescriptor);
            Unique<IGpuApiResourceObject>  gpuApiResource = mGpuApiResourceObjectFactory->create<TResource>(aDescriptor);
            resource->bindGpuApiResourceInterface(std::move(gpuApiResource));

            auto dependenciesResolved = getGpuApiDependencies(aResourceId);

            resource->getGpuApiResourceInterface()->create(dependenciesResolved);
            resource->getGpuApiResourceInterface()->load(); // TODO: Load on demand? Auto-Unload if not accessed?

            storeResourceObject(aResourceId, resource);

            return result;
        }
    }
}

#endif //__SHIRABEDEVELOPMENT_CRESOURCEMANAGER_H__
