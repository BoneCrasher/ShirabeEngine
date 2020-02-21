//
// Created by dotti on 19.10.19.
//

#ifndef __SHIRABEDEVELOPMENT_CRESOURCEMANAGER_H__
#define __SHIRABEDEVELOPMENT_CRESOURCEMANAGER_H__

#include <typeindex>
#include <variant>
#include <base/stl_container_helpers.h>
#include <platform/platform.h>
#include <graphicsapi/definitions.h>
#include <asset/assettypes.h>
#include <asset/assetstorage.h>
#include <core/datastructures/adjacencytree.h>
#include <core/threading/looper.h>
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
    template <typename TLogicalResource, typename TGpuApiResource>
    struct SHIRABE_LIBRARY_EXPORT SResourceState
    {
        using ResourceState_t   = core::CBitField<EGpuApiResourceState>;
        using LogicalResource_t = TLogicalResource;
        using GpuApiResource_t  = TGpuApiResource;
        using Descriptor_t      = typename LogicalResource_t::Descriptor_t;
        using Dependencies_t    = typename LogicalResource_t::Dependencies_t;
        using GpuApiHandles_t   = typename GpuApiResource_t ::Handles_t;

        Descriptor_t    description;
        Dependencies_t  dependencies;
        GpuApiHandles_t gpuApiHandles;
        ResourceState_t loadState;
    };
    
    template <typename TResource>
    using FetchedResource_t = std::optional<std::reference_wrapper<TResource>>;

    /**
     *
     */
    template <typename... TResources>
    class
        [[nodiscard]]
        SHIRABE_LIBRARY_EXPORT CResourceManager
    {
        SHIRABE_DECLARE_LOG_TAG(CResourceManager);

    private_typedefs:
        using ResourceVariants_t = std::variant<TResources...>;

    public_constructors:
        CResourceManager();

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
        CEngineResult<FetchedResource_t<TResource>> useDynamicResource(ResourceId_t                     const &aResourceId
                                                                     , typename TResource::Descriptor_t const &aDescriptor);

        template <typename TResource>
        CEngineResult<FetchedResource_t<TResource>> useAssetResource(ResourceId_t const &aResourceId
                                                                   , AssetId_t    const &aAssetResourceId);

        template <typename TResource>
        SHIRABE_INLINE
        FetchedResource_t<TResource> getResource(ResourceId_t const &aId)
        {
            return getResourceObject<TResource>(aId);
        }

        template <typename TResource, typename... TArgs>
        SHIRABE_INLINE
        CEngineResult<> discardResource(ResourceId_t const &aResourceId, TArgs &&...aArgs);

        template <typename TResource, typename... TArgs>
        CEngineResult<> initializeResource(ResourceId_t                       const &aResourceId
                                         , typename TResource::Dependencies_t const &aDependencies
                                         , TArgs                                &&...aArgs);

        template <typename TResource, typename... TArgs>
        CEngineResult<> deinitializeResource(ResourceId_t                       const &aResourceId
                                           , typename TResource::Dependencies_t const &aDependencies
                                           , TArgs                                &&...aArgs);

        template <typename TResource, typename... TArgs>
        CEngineResult<> transferResource(ResourceId_t const &aResourceId
                                       , TArgs          &&...aArgs);

        template <typename TResource>
        FetchedResource_t<TResource> getResourceObject(ResourceId_t const &aId)
        {
            bool const hasObjectForId = assoc_container_contains(mResourceObjects, aId);
            if(hasObjectForId)
            {
                ResourceVariants_t &variant = mResourceObjects.at(aId);
                return std::get<TResource>(variant);
            }

            return {};
        }

    private_methods:
        template <typename TResource>
        Shared<CResourceFromAssetResourceObjectCreator<TResource>> getLoader();

        template <typename TResource>
        CEngineResult<FetchedResource_t<TResource>> genericAssetLoading(ResourceId_t const &aResourceId
                                                                      , AssetId_t    const &aAssetResourceId);

        template <typename TResource>
        bool storeResourceObject(ResourceId_t const &aId
                               , TResource    const &aObject)
        {
            bool const hasObjectForId = assoc_container_contains(mResourceObjects, aId);
            if(not hasObjectForId)
            {
                mResourceObjects[aId] = aObject;
            }

            return (not hasObjectForId);
        }

        void removeResourceObject(ResourceId_t const &aId)
        {
            bool const hasObjectForId = (mResourceObjects.end() != mResourceObjects.find(aId));
            if(hasObjectForId)
            {
                mResourceObjects.erase(aId);
            }
        }

    private_members:
        engine::threading::CLooper<EEngineStatus> mResourceThreadLooper;

        std::unordered_map<std::type_index, Shared<IResourceObjectCreatorBase>> mAssetLoaders;

        std::unordered_map<ResourceId_t, ResourceVariants_t> mResourceObjects;
        CAdjacencyTree<ResourceId_t>                         mResourceTree;
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename... TResources>
    CResourceManager<TResources...>::CResourceManager()
            : mAssetLoaders               ()
            , mResourceObjects            ()
            , mResourceTree               ()
    { }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename... TResources>
    template <typename TResource>
    CEngineResult<> CResourceManager<TResources...>::addAssetLoader(Shared<CResourceFromAssetResourceObjectCreator<TResource>> aLoader)
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
    template <typename... TResources>
    template <typename TResource>
    Shared<CResourceFromAssetResourceObjectCreator<TResource>> CResourceManager<TResources...>::getLoader()
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
    template <typename... TResources>
    template <typename TResource>
    CEngineResult<FetchedResource_t<TResource>>
        CResourceManager<TResources...>::genericAssetLoading(ResourceId_t const &aResourceId
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

        return { EEngineStatus::Ok, { resourceObject }};
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename... TResources>
    template <typename TResource>
    CEngineResult<FetchedResource_t<TResource>> CResourceManager<TResources...>::useDynamicResource(
              ResourceId_t                     const &aResourceId
            , typename TResource::Descriptor_t const &aDescriptor)
    {
        CEngineResult<FetchedResource_t<TResource>> result = {EEngineStatus::Error, {} };

        auto const alreadyFoundIt = mResourceObjects.find(aResourceId);
        if(mResourceObjects.end() != mResourceObjects.find(aResourceId))
        {
            return { EEngineStatus::Ok, alreadyFoundIt->second.logicalResource };
        }

        //
        // Wrap the gpu api resource operations with dependency resolving operations.
        //

        Shared<ILogicalResourceObject> resource         = makeShared<TResource>(aDescriptor);
        auto                           resourceObject   = std::static_pointer_cast<CResourceObject<typename TResource::Descriptor_t, typename TResource::Dependencies_t>>(resource);

        storeResourceObject(aResourceId, { resource, nullptr, EGpuApiResourceState::Unloaded });

        return { EEngineStatus::Ok, resource };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename... TResources>
    template <typename TResource>
    CEngineResult<FetchedResource_t<TResource>> CResourceManager<TResources...>::useAssetResource(ResourceId_t const &aResourceId
                                                                                                , AssetId_t    const &aAssetResourceId)
    {
        FetchedResource_t<TResource> object = getResourceObject(aResourceId);
        if(object.has_value())
        {
            return { EEngineStatus::Ok, object };
        }

        return genericAssetLoading<TResource>(aResourceId, aAssetResourceId);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename... TResources>
    template <typename TResource, typename... TArgs>
    CEngineResult<> CResourceManager<TResources...>::discardResource(ResourceId_t const &aResourceId, TArgs &&...aArgs)
    {
        auto iterator = mResourceObjects.find(aResourceId);
        if(mResourceObjects.end() != iterator)
        {
            TResource &resource = std::get<TResource>(*iterator);
            if(EGpuApiResourceState::Discarded != resource.state)
            {
                EEngineStatus const deinitResult = deinitializeResource(aResourceId, resource.dependencies, std::forward<TArgs>(aArgs)...);
                if(CheckEngineError(deinitResult))
                {
                    CLog::Error(logTag(), "Failed to deinitialize resource with ID '{}'", aResourceId);
                    return EEngineStatus::Error;
                }
            }

            removeResourceObject(aResourceId);
            return EEngineStatus::Ok;
        }

        return EEngineStatus::ResourceError_NotFound;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename... TResources>
    template <typename TResource, typename... TArgs>
    CEngineResult<> CResourceManager<TResources...>::initializeResource(ResourceId_t                      const &aResourceId
                                                                      , typename TResource::Dependencies_t const &aDependencies
                                                                      , TArgs                                &&...aArgs)
    {
        using namespace core;

        FetchedResource_t<TResource> resource = getResourceObject<TResource>(aResourceId);
        // Resource available?
        if(not resource.has_value())
        {
            resource->loadState.set(EGpuApiResourceState::Error);
            return EEngineStatus::Error;
        }

        // Resource is being created and/or loaded.
        if(resource->loadState.checkAny(EGpuApiResourceState::Creating
                                      | EGpuApiResourceState::Created
                                      | EGpuApiResourceState::Loading
                                      | EGpuApiResourceState::Loaded))
        {
            return EEngineStatus::Ok;
        }

        // Run asynchronously
        mResourceThreadLooper.getDispatcher().post([=] () -> EEngineStatus
        {
            if(not resource->loadState.checkAny(EGpuApiResourceState::Creating
                                              | EGpuApiResourceState::Created))
            {
                resource->loadState.set(EGpuApiResourceState::Creating);

                EEngineStatus const initResult = TResource::GpuApiResource_t::initialize(resource.descriptor, aDependencies, resource.gpuApiHandles, std::forward<TArgs>(aArgs)...);
                if(not CheckEngineError(initResult))
                {
                    resource->loadState.set(EGpuApiResourceState::Error);
                    return initResult;
                }

                resource->dependencies = aDependencies;
                resource->loadState.reset(EGpuApiResourceState::Created);
            }

            if(not resource->loadState.checkAny(EGpuApiResourceState::Loading
                                              | EGpuApiResourceState::Loaded))
            {
                resource->loadState.set(EGpuApiResourceState::Loading);

                EEngineStatus const loadResult = TResource::GpuApiResource_t::load(resource.descriptor, aDependencies, resource.gpuApiHandles, std::forward<TArgs>(aArgs)...);
                if(not CheckEngineError(loadResult))
                {
                    resource->loadState.set(EGpuApiResourceState::Error);
                    return loadResult;
                }

                resource->loadState.unset(EGpuApiResourceState::Loading);
                resource->loadState.set(EGpuApiResourceState::Loaded);
            }

            return EEngineStatus::Ok;
        });

        return EEngineStatus::Ok;
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename... TResources>
    template <typename TResource, typename... TArgs>
    CEngineResult<> CResourceManager<TResources...>::deinitializeResource(ResourceId_t                       const &aResourceId
                                                                        , typename TResource::Dependencies_t const &aDependencies
                                                                        , TArgs                                &&...aArgs)
    {
        using namespace core;

        FetchedResource_t<TResource> resource = getResourceObject(aResourceId);
        // Resource available?
        if(not resource.has_value())
        {
            resource->loadState.set(EGpuApiResourceState::Error);
            return EEngineStatus::Error;
        }

        // Resource is not being created or loaded?
        if(resource->loadState.checkAny(EGpuApiResourceState::Creating
                                      | EGpuApiResourceState::Loading))
        {
            resource->loadState.set(EGpuApiResourceState::Error);
            return EEngineStatus::Error;
        }

        // Resource is created or loaded?
        if(not resource->loadState.checkAny(EGpuApiResourceState::Created)
        || not resource->loadState.checkAny(EGpuApiResourceState::Loaded))
        {
            resource->loadState.set(EGpuApiResourceState::Error);
            return EEngineStatus::Ok;
        }

        // In any case:
        resource->loadState.unset(EGpuApiResourceState::Transferred);

        if(not resource->loadState.checkAny(EGpuApiResourceState::Unloading
                                          | EGpuApiResourceState::Unloaded))
        {
            resource->loadState.set(EGpuApiResourceState::Unloading);

            EEngineStatus const unloadResult = TResource::GpuApiResource_t::unload(resource.descriptor, aDependencies, resource.gpuApiHandles, std::forward<TArgs>(aArgs)...);
            if(not CheckEngineError(unloadResult))
            {
                resource->loadState.set(EGpuApiResourceState::Error);
                return unloadResult;
            }

            resource->loadState.unset(EGpuApiResourceState::Unloading | EGpuApiResourceState::Loaded);
            resource->loadState.set  (EGpuApiResourceState::Unloaded);
        }

        if(not resource->loadState.checkAny(EGpuApiResourceState::Discarding
                                      | EGpuApiResourceState::Discarded))
        {
            resource->loadState.reset(EGpuApiResourceState::Discarding);

            EEngineStatus const deinitResult = TResource::GpuApiResource_t::deinitialize(resource.descriptor, aDependencies, resource.gpuApiHandles, std::forward<TArgs>(aArgs)...);
            if(not CheckEngineError(deinitResult))
            {
                return deinitResult;
            }

            resource->loadState.unset(EGpuApiResourceState::Discarding | EGpuApiResourceState::Created);
            resource->loadState.reset(EGpuApiResourceState::Discarded);
        }

        return EEngineStatus::Ok;
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename... TResources>
    template <typename TResource, typename... TArgs>
    CEngineResult<> CResourceManager<TResources...>::transferResource(ResourceId_t const &aResourceId
                                                                    , TArgs          &&...aArgs)
    {
        using namespace core;

        FetchedResource_t<TResource> resource = getResourceObject(aResourceId);
        // Resource available?
        if(not resource.has_value())
        {
            resource->loadState.set(EGpuApiResourceState::Error);
            return EEngineStatus::Error;
        }

        // Resource is created or loaded?
        if(not resource->loadState.checkAny(EGpuApiResourceState::Loaded))
        {
            resource->loadState.set(EGpuApiResourceState::Error);
            return EEngineStatus::Error;
        }

        if(not resource->loadState.checkAny(EGpuApiResourceState::Transferring
                                      | EGpuApiResourceState::Transferred))
        {
            resource->loadState.set(EGpuApiResourceState::Transferring);

            EEngineStatus const transferResult = TResource::GpuApiResource_t::transfer(resource.descriptor, resource.dependencies, resource.gpuApiHandles, std::forward<TArgs>(aArgs)...);
            if(not CheckEngineError(transferResult))
            {
                return transferResult;
            }

            resource->loadState.unset(EGpuApiResourceState::Transferring);
            resource->loadState.set  (EGpuApiResourceState::Transferred);
        }

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------
}


#endif //__SHIRABEDEVELOPMENT_CRESOURCEMANAGER_H__
