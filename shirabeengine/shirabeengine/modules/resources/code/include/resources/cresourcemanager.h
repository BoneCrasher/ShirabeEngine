//
// Created by dotti on 19.10.19.
//

#ifndef __SHIRABEDEVELOPMENT_CRESOURCEMANAGER_H__
#define __SHIRABEDEVELOPMENT_CRESOURCEMANAGER_H__

#include <typeindex>
#include <base/stl_container_helpers.h>
#include <platform/platform.h>
#include <graphicsapi/definitions.h>
#include <asset/assettypes.h>
#include <asset/assetstorage.h>
#include <core/datastructures/adjacencytree.h>
#include <base/stl_container_helpers.h>
#include "resources/cresourceobject.h"
#include "resources/agpuapiresourceobject.h"
#include "resources/agpuapiresourceobjectfactory.h"
#include "cgpuapiresourcestorage.h"

namespace engine::resources
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

    /**
     * The SResourceState struct groups a logical resource, an optional gpu api
     * resource and it's current load state.
     */
    template <typename TLogicalResource>
    struct SHIRABE_LIBRARY_EXPORT SResourceState
    {
        TLogicalResource                      logicalResource;
        Shared<IGpuApiResourceObject>         gpuApiResource;
        core::CBitField<EGpuApiResourceState> state;
    };

    /**
     *
     */
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

        /**
         * Add an asset loader instance for a given type TResource
         *
         * @tparam TResource
         * @param aLoader
         * @return
         */
        template <typename TResource>
        CEngineResult<> addAssetLoader(Shared<CResourceFromAssetResourceObjectCreator<TResource>> aLoader);

        template <typename TResource>
        CEngineResult<SResourceState<TResource>> useDynamicResource( ResourceId_t                     const &aResourceId
                                                                   , typename TResource::Descriptor_t const &aDescriptor);

        template <typename TResource>
        CEngineResult<SResourceState<TResource>> useAssetResource(  ResourceId_t const &aResourceId
                                                                  , AssetId_t    const &aAssetResourceId);

        template <typename TLogicalResource>
        std::optional<SResourceState<TLogicalResource>> getResource(ResourceId_t const &aId)
        {
            return getResourceObject<TLogicalResource>(aId);
        }

        CEngineResult<> discardResource(ResourceId_t const &aResourceId);

    private_methods:
        template <typename TResource>
        Shared<CResourceFromAssetResourceObjectCreator<TResource>> getLoader();

        template <typename TResource>
        CEngineResult<Shared<ILogicalResourceObject>> genericAssetLoading(ResourceId_t const &aResourceId
                                                                        , AssetId_t    const &aAssetResourceId);

        template <typename TLogicalResource>
        bool storeResourceObject(ResourceId_t                     const &aId
                               , SResourceState<TLogicalResource> const &aObject)
        {
            bool const hasObjectForId = assoc_container_contains(mResourceObjects, aId);
            if(not hasObjectForId)
            {
                mResourceObjects[aId] = aObject;
            }

            return (not hasObjectForId);
        }

        template <typename TLogicalResource>
        std::optional<SResourceState<TLogicalResource>> getResourceObject(ResourceId_t const &aId)
        {
            bool const hasObjectForId = assoc_container_contains(mResourceObjects, aId);
            if(hasObjectForId)
            {
                return mResourceObjects.at(aId);
            }

            return {};
        }

        void removeResourceObject(ResourceId_t const &aId);

        GpuApiResourceDependencies_t getGpuApiDependencies(ResourceId_t const &aId);

    private_members:
        Unique<CGpuApiResourceObjectFactory>                                    mGpuApiResourceObjectFactory;
        Shared<asset::IAssetStorage>                                            mAssetStorage;
        std::unordered_map<std::type_index, Shared<IResourceObjectCreatorBase>> mAssetLoaders;


        std::unordered_map<ResourceId_t, SResourceState>                        mResourceObjects;
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

        storeResourceObject(aResourceId, { resourceObject, { nullptr }, EGpuApiResourceState::Unloaded });

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
            return { EEngineStatus::Ok, alreadyFoundIt->second.logicalResource };
        }

        auto const [handle, ops] = mGpuApiResourceObjectFactory->create<TResource>();

        GpuApiHandle_t        gpuApiResourceId = handle;
        SGpuApiOps<TResource> gpuApiOps        = ops;

        SLogicalOps< typename TResource::Descriptor_t
                   , typename TResource::Dependencies_t> logicalResourceOps {};

        using namespace core;

        //
        // Wrap the gpu api resource operations with dependency resolving operations.
        //
        logicalResourceOps.initialize =
                [aResourceId, aDescriptor, gpuApiResourceId, gpuApiOps, this] (typename TResource::Dependencies_t const &aDependencies) -> CEngineResult<>
        {
            std::optional<SResourceState> resource = getResourceObject(aResourceId);
            if(not resource.has_value())
            {
                return EEngineStatus::Error;
            }

            if(resource->state.checkAny(EGpuApiResourceState::Creating | EGpuApiResourceState::Created))
            {
                return EEngineStatus::Ok;
            }
            resource->state.set(EGpuApiResourceState::Creating);

            Shared<ILogicalResourceObject> logicalResourceObject = resource->logicalResource;
            if(nullptr == logicalResourceObject)
            {
                return EEngineStatus::Error;
            }

            Shared<TResource> resourceObject = std::static_pointer_cast<TResource>(logicalResourceObject);
            if(nullptr == resourceObject)
            {
                return EEngineStatus::Error;
            }

            resourceObject->setCurrentDependencies(aDependencies);

            Vector<ResourceId_t> resolveDependenciesList = aDependencies.resolve();

            insertDependencies(mResourceTree, aResourceId, std::move(resolveDependenciesList));
            auto dependenciesResolved = getGpuApiDependencies(aResourceId);

            CEngineResult<> const result = gpuApiOps.initialize(aDescriptor, aDependencies, dependenciesResolved);

            resource->state.reset(EGpuApiResourceState::Created);

            return result.result();
        };
        logicalResourceOps.deinitialize =
                [aResourceId, gpuApiOps, this] (typename TResource::Dependencies_t const &aDependencies) -> CEngineResult<>
        {
            std::optional<SResourceState> resource = getResourceObject(aResourceId);
            if(not resource.has_value())
            {
                return EEngineStatus::Error;
            }

            if(not resource->state.check(EGpuApiResourceState::Unloaded))
            {
                return EEngineStatus::Error;
            }

            if(resource->state.checkAny(EGpuApiResourceState::Discarding | EGpuApiResourceState::Discarded))
            {
                return EEngineStatus::Ok;
            }
            resource->state.set(EGpuApiResourceState::Discarding);

            CEngineResult<> const result = gpuApiOps.deinitialize();

            Vector<ResourceId_t> resolveDependenciesList = aDependencies.resolve();
            removeDependencies(mResourceTree, aResourceId, std::move(resolveDependenciesList));

            resource->state.reset(EGpuApiResourceState::Discarded);

            return result.result();
        };
        logicalResourceOps.load = [gpuApiOps, aResourceId, this] () -> CEngineResult<>
        {
            std::optional<SResourceState> resource = getResourceObject(aResourceId);
            if(not resource.has_value())
            {
                return EEngineStatus::Error;
            }

            if(not resource->state.check(EGpuApiResourceState::Created))
            {
                return EEngineStatus::Error;
            }

            if(resource->state.checkAny(EGpuApiResourceState::Loading | EGpuApiResourceState::Loaded))
            {
                return EEngineStatus::Ok;
            }
            resource->state.set(EGpuApiResourceState::Loading);

            CEngineResult<> const result = gpuApiOps.load();

            resource->state.unset(EGpuApiResourceState::Loading);
            resource->state.set  (EGpuApiResourceState::Loaded);
        };
        logicalResourceOps.unload = [gpuApiOps, aResourceId, this] () -> CEngineResult<>
        {
            std::optional<SResourceState> resource = getResourceObject(aResourceId);
            if(not resource.has_value())
            {
                return EEngineStatus::Error;
            }

            if(not resource->state.check(EGpuApiResourceState::Loaded))
            {
                return EEngineStatus::Error;
            }

            if(resource->state.checkAny(EGpuApiResourceState::Unloading | EGpuApiResourceState::Unloaded))
            {
                return EEngineStatus::Ok;
            }
            resource->state.set(EGpuApiResourceState::Unloading);

            CEngineResult<> const result = gpuApiOps.unload();

            resource->state.unset(EGpuApiResourceState::Loading | EGpuApiResourceState::Loaded);
            resource->state.unset(EGpuApiResourceState::Unloading);
            resource->state.set  (EGpuApiResourceState::Unloaded);
        };
        logicalResourceOps.transfer= [gpuApiOps, aResourceId, this] () -> CEngineResult<>
        {
            std::optional<SResourceState> resource = getResourceObject(aResourceId);
            if(not resource.has_value())
            {
                return EEngineStatus::Error;
            }
            
            if(not resource->state.check(EGpuApiResourceState::Loaded))
            {
                return EEngineStatus::Error;
            }

            if(resource->state.checkAny(EGpuApiResourceState::Transferring | EGpuApiResourceState::Transferred))
            {
                return EEngineStatus::Ok;
            }
            resource->state.set(EGpuApiResourceState::Transferring);

            CEngineResult<> const result = gpuApiOps.transfer();

            resource->state.unset(EGpuApiResourceState::Transferring);
            resource->state.set  (EGpuApiResourceState::Transferred);
        };

        Shared<ILogicalResourceObject> resource         = makeShared<TResource>(aDescriptor);
        auto                           resourceObject   = std::static_pointer_cast<CResourceObject<typename TResource::Descriptor_t, typename TResource::Dependencies_t>>(resource);
        resource      ->setGpuApiResourceHandle(gpuApiResourceId);
        resourceObject->setLogicalOps(logicalResourceOps);

        storeResourceObject(aResourceId, { resource, nullptr, EGpuApiResourceState::Unloaded });

        return { EEngineStatus::Ok, resource };
    }
    //<-----------------------------------------------------------------------------
}


#endif //__SHIRABEDEVELOPMENT_CRESOURCEMANAGER_H__
