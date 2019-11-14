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
#include "cgpuapiresourcestorage.h"

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

        private_methods:
            bool storeResourceObject(ResourceId_t               const &aId
                                     , Shared <ILogicalResourceObject> const &aObject);

            void removeResourceObject(ResourceId_t const &aId);

            GpuApiResourceDependencies_t getGpuApiDependencies(ResourceId_t const &aId);

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
            Shared<ILogicalResourceObject> resource         = makeShared<CResourceObject<typename TResource::Descriptor_t>>(aDescriptor);
            GpuApiHandle_t                 gpuApiResourceId = mGpuApiResourceObjectFactory->create<TResource>(aDescriptor);
            resource->setGpuApiResourceHandle(gpuApiResourceId);

            storeResourceObject(aResourceId, resource);

            // TODO: For now immediately create... in the future make it more intelligent...
            auto dependenciesResolved = getGpuApiDependencies(aResourceId);

            mGpuApiResourceObjectFactory->get(gpuApiResourceId)->create(dependenciesResolved);

            return result;
        }
    }
}

#endif //__SHIRABEDEVELOPMENT_CRESOURCEMANAGER_H__
