//
// Created by dotti on 19.10.19.
//

#ifndef __SHIRABEDEVELOPMENT_CRESOURCEMANAGER_H__
#define __SHIRABEDEVELOPMENT_CRESOURCEMANAGER_H__

#include <typeindex>
#include <platform/platform.h>
#include <graphicsapi/definitions.h>
#include <asset/assettypes.h>
#include <asset/assetstorage.h>
#include <core/datastructures/adjacencytree.h>
#include "resources/cresourceobject.h"
#include "resources/agpuapiresourceobject.h"
#include "resources/agpuapiresourceobjectfactory.h"
#include "cgpuapiresourcestorage.h"

namespace engine {
    namespace resources
    {
        using asset::AssetId_t ;
        using datastructures::CAdjacencyTree;
        using datastructures::CAdjacencyTreeHelper;

        template <typename TResource>
        class SHIRABE_LIBRARY_EXPORT CResourceFromAssetResourceObjectCreator
            : public CResourceObjectCreator<TResource, Shared<ILogicalResourceObject>, ResourceId_t const&, AssetId_t const&>
        {
        public_constructors:
            using CResourceObjectCreator<TResource, Shared<ILogicalResourceObject>, ResourceId_t const&, AssetId_t const&>::CResourceObjectCreator;
        };

        class
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT CResourceManager
        {
            SHIRABE_DECLARE_LOG_TAG(CResourceManager);

        public_constructors:
            explicit CResourceManager(Unique<CGpuApiResourceObjectFactory> aPrivateResourceObjectFactory
                                    , Shared<asset::IAssetStorage>         aAssetStorage);

        public_destructors:
            ~CResourceManager() = default;

        public_methods:
            template <typename TResource>
            CEngineResult<> addAssetLoader(Shared<CResourceFromAssetResourceObjectCreator<TResource>> aLoader);

            template <typename TResource>
            // requires std::is_base_of_v<ILogicalResourceObject, TResource>
            CEngineResult<Shared<ILogicalResourceObject>> useDynamicResource(
                      ResourceId_t                     const &aResourceId
                    , typename TResource::Descriptor_t const &aDescriptor);

            CEngineResult<Shared<ILogicalResourceObject>> useAssetResource(  ResourceId_t const &aResourceId
                                                                           , AssetId_t    const &aAssetResourceId);

            CEngineResult<> discardResource(ResourceId_t const &aResourceId);

        private_methods:
            template <typename TResource>
            Shared<CResourceFromAssetResourceObjectCreator<TResource>> getLoader();

            template <typename TResource>
            CEngineResult<Shared<ILogicalResourceObject>> genericAssetLoading(ResourceId_t const &aResourceId
                                                                            , AssetId_t    const &aAssetResourceId);

            bool storeResourceObject(ResourceId_t                      const &aId
                                     , Shared <ILogicalResourceObject> const &aObject);

            Shared<ILogicalResourceObject> getResourceObject(ResourceId_t const &aId);

            void removeResourceObject(ResourceId_t const &aId);

            GpuApiResourceDependencies_t getGpuApiDependencies(ResourceId_t const &aId);

        private_members:
            Unique<CGpuApiResourceObjectFactory>                                    mGpuApiResourceObjectFactory;
            Shared<asset::IAssetStorage>                                            mAssetStorage;

            std::unordered_map<std::type_index, Shared<IResourceObjectCreatorBase>> mAssetLoaders;

            std::unordered_map<ResourceId_t, Shared<ILogicalResourceObject>>        mResourceObjects;
            CAdjacencyTree<ResourceId_t>                                            mResourceTree;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<> CResourceManager::addAssetLoader(Shared<CResourceFromAssetResourceObjectCreator<TResource>> aLoader)
        {
            std::type_index const index = std::type_index(typeid(TResource));

            auto const it = mAssetLoaders.find(index);
            if(mAssetLoaders.end() == it)
            {
                mAssetLoaders.insert({ index, std::move(aLoader) });
                return EEngineStatus::Ok;
            }

            return EEngineStatus::Error;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        Shared<CResourceFromAssetResourceObjectCreator<TResource>> CResourceManager::getLoader()
        {
            static Shared<CResourceFromAssetResourceObjectCreator<TResource>> sEmptyLoader = nullptr;

            std::type_index const index = std::type_index(typeid(TResource));

            auto const it = mAssetLoaders.find(index);
            if(mAssetLoaders.end() != it)
            {
                return std::static_pointer_cast<CResourceFromAssetResourceObjectCreator<TResource>>(mAssetLoaders.at(index));
            }

            return sEmptyLoader;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<Shared<ILogicalResourceObject>> CResourceManager::genericAssetLoading(  ResourceId_t const &aResourceId
                                                                                            , AssetId_t    const &aAssetResourceId)
        {
            Shared<CResourceFromAssetResourceObjectCreator<TResource>> const &loader = getLoader<TResource>();
            if(nullptr == loader)
            {
                return { EEngineStatus::Error, nullptr };
            }

            Shared<ILogicalResourceObject> resourceObject = loader->create(aResourceId, aAssetResourceId);
            if(nullptr == resourceObject)
            {
                return {EEngineStatus::Error, nullptr};
            }

            storeResourceObject(aResourceId, resourceObject);

            return { EEngineStatus::Ok, resourceObject };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        static void insertDependencies(CAdjacencyTree<ResourceId_t> &aTree, ResourceId_t const &aResourceId, std::vector<ResourceId_t> &&aDependencies)
        {
            // Add static dependencies
            aTree.add(aResourceId);
            for(auto const &dependency : aDependencies)
            {
                aTree.add    (dependency);
                aTree.connect(aResourceId, dependency);
            }
        }

        static void removeDependencies(CAdjacencyTree<ResourceId_t> &aTree, ResourceId_t const &aResourceId, std::vector<ResourceId_t> &&aDependencies)
        {
            // Add static dependencies
            for(auto const &dependency : aDependencies)
            {
                aTree.disconnect(aResourceId, dependency);
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<Shared<ILogicalResourceObject>> CResourceManager::useDynamicResource(
                  ResourceId_t                     const &aResourceId
                , typename TResource::Descriptor_t const &aDescriptor)
        {
            CEngineResult<Shared<ILogicalResourceObject>> result = {EEngineStatus::Error, nullptr };

            auto const alreadyFoundIt = mResourceObjects.find(aResourceId);
            if(mResourceObjects.end() != mResourceObjects.find(aResourceId))
            {
                return { EEngineStatus::Ok, alreadyFoundIt->second };
            }

            auto const [handle, ops] = mGpuApiResourceObjectFactory->create<TResource>();

            GpuApiHandle_t        gpuApiResourceId = handle;
            SGpuApiOps<TResource> gpuApiOps        = ops;

            SLogicalOps< typename TResource::Descriptor_t
                       , typename TResource::Dependencies_t> logicalResourceOps {};

            //
            // Wrap the gpu api resource operations with dependency resolving operations.
            //
            logicalResourceOps.initialize =
                    [aResourceId, aDescriptor, gpuApiResourceId, gpuApiOps, this] (typename TResource::Dependencies_t const &aDependencies) -> CEngineResult<>
            {
                Vector<ResourceId_t> resolveDependenciesList = aDependencies.resolve();

                insertDependencies(mResourceTree, aResourceId, std::move(resolveDependenciesList));
                auto dependenciesResolved = getGpuApiDependencies(aResourceId);

                CEngineResult<> const result = gpuApiOps.initialize(aDescriptor, aDependencies, dependenciesResolved);

                return result.result();
            };
            logicalResourceOps.deinitialize =
                    [aResourceId, gpuApiOps, this] (typename TResource::Dependencies_t const &aDependencies) -> CEngineResult<>
            {
                CEngineResult<> const result = gpuApiOps.deinitialize();

                Vector<ResourceId_t> resolveDependenciesList = aDependencies.resolve();
                removeDependencies(mResourceTree, aResourceId, std::move(resolveDependenciesList));

                return result.result();
            };
            logicalResourceOps.load     = [gpuApiOps] () -> CEngineResult<> { return gpuApiOps.load();     };
            logicalResourceOps.unload   = [gpuApiOps] () -> CEngineResult<> { return gpuApiOps.unload();   };
            logicalResourceOps.transfer = [gpuApiOps] () -> CEngineResult<> { return gpuApiOps.transfer(); };

            Shared<ILogicalResourceObject> resource         = makeShared<TResource>(aDescriptor);
            auto                           resourceObject   = std::static_pointer_cast<CResourceObject<typename TResource::Descriptor_t, typename TResource::Dependencies_t>>(resource);
            resource      ->setGpuApiResourceHandle(gpuApiResourceId);
            resourceObject->setLogicalOps(logicalResourceOps);

            storeResourceObject(aResourceId, resource);

            return { EEngineStatus::Ok, resource };
        }
        //<-----------------------------------------------------------------------------
    }
}

#endif //__SHIRABEDEVELOPMENT_CRESOURCEMANAGER_H__
