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
#include <core/bitfield.h>
#include <core/datastructures/adjacencytree.h>
#include <core/threading/looper.h>
#include <base/stl_container_helpers.h>
#include "resources/cresourceobject.h"

namespace engine::resources
{
    using asset::AssetId_t ;
    using datastructures::CAdjacencyTree;
    using datastructures::CAdjacencyTreeHelper;

    enum class
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT EGpuApiResourceState
    {
          Unknown      = 0
        , Unavailable  = 1
        , Creating     = 2
        , Created      = 4
        , Loading      = 8
        , Loaded       = 16
        , Transferring = 32
        , Transferred  = 64
        , Unloading    = 128
        , Unloaded     = 256
        , Discarding   = 512
        , Discarded    = 1024
        , Error        = 2048
    };

    using ResourceId_t = std::string;

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
    using OptioanlRef_t = std::optional<std::reference_wrapper<TResource>>;

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

        template <typename TResource>
        CEngineResult<OptionalRef_t <TResource>> useDynamicResource(ResourceId_t                     const &aResourceId
                                                                  , typename TResource::Descriptor_t const &aDescriptor);

        template <typename TResource>
        SHIRABE_INLINE
        OptioanlRef_t<TResource> getResource(ResourceId_t const &aId)
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
        OptioanlRef_t<TResource> getResourceObject(ResourceId_t const &aId)
        {
            bool const hasObjectForId = assoc_container_contains(mResourceObjects, aId);
            if(hasObjectForId)
            {
                ResourceVariants_t &variant = mResourceObjects.at(aId);
                return OptioanlRef_t<TResource>(std::get<TResource>(variant));
            }

            return {};
        }

    private_methods:

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

        std::unordered_map<ResourceId_t, ResourceVariants_t> mResourceObjects;
        CAdjacencyTree<ResourceId_t>                         mResourceTree;
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename... TResources>
    CResourceManager<TResources...>::CResourceManager()
            : mResourceObjects            ()
            , mResourceTree               ()
    { }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename... TResources>
    template <typename TResource>
    CEngineResult<OptioanlRef_t<TResource>> CResourceManager<TResources...>
            ::useDynamicResource(
                  ResourceId_t                     const &aResourceId
                , typename TResource::Descriptor_t const &aDescriptor)
    {
        CEngineResult<OptioanlRef_t<TResource>> result = {EEngineStatus::Error, {} };

        auto const alreadyFoundIt = mResourceObjects.find(aResourceId);
        if(mResourceObjects.end() != mResourceObjects.find(aResourceId))
        {
            ResourceVariants_t &variant = alreadyFoundIt->second;
            TResource          &value   = std::get<TResource>(variant);
            return { EEngineStatus::Ok, OptioanlRef_t<TResource>(value) };
        }

        //
        // Wrap the gpu api resource operations with dependency resolving operations.
        //
        TResource resource {};
        resource.descriptor    = aDescriptor;
        resource.dependencies  = {};
        resource.gpuApiHandles = {};
        resource.loadState     = EGpuApiResourceState::Unloaded;

        storeResourceObject<TResource>(aResourceId, resource);

        return { EEngineStatus::Ok, resource };
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
            ResourceVariants_t &variant = *iterator;
            TResource          &value   = std::get<TResource>(variant);

            TResource &resource = std::get<TResource>(value);
            if(EGpuApiResourceState::Discarded != resource.state)
            {
                EEngineStatus const deinitResult = deinitializeResource<TResource>(aResourceId, resource.dependencies, std::forward<TArgs>(aArgs)...).result();
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
    CEngineResult<> CResourceManager<TResources...>::initializeResource(ResourceId_t                       const &aResourceId
                                                                      , typename TResource::Dependencies_t const &aDependencies
                                                                      , TArgs                                &&...aArgs)
    {
        using namespace core;

        OptioanlRef_t<TResource> resourceOpt = getResourceObject<TResource>(aResourceId);
        if(not resourceOpt.has_value())
        {
            return EEngineStatus::ResourceError_NotFound;
        }

        TResource &resource = *resourceOpt;

        // Resource is being created and/or loaded.
        if(resource.loadState.checkAny(EGpuApiResourceState::Creating
                                      | EGpuApiResourceState::Created
                                      | EGpuApiResourceState::Loading
                                      | EGpuApiResourceState::Loaded))
        {
            return EEngineStatus::Ok;
        }

        // Run asynchronously
        /*
         * mResourceThreadLooper.getDispatcher().post([=] () -> EEngineStatus
         * {
         *     if(not resource.loadState.checkAny(EGpuApiResourceState::Creating
         *                                       | EGpuApiResourceState::Created))
         *     {
         *         resource.loadState.set(EGpuApiResourceState::Creating);
         *
         *         EEngineStatus const initResult = TResource::GpuApiResource_t::initialize(resource.descriptor, aDependencies, resource.gpuApiHandles, std::forward<TArgs>(aArgs)...);
         *         if(not CheckEngineError(initResult))
         *         {
         *             resource.loadState.set(EGpuApiResourceState::Error);
         *             return initResult;
         *         }
          *
         *         resource->dependencies = aDependencies;
         *         resource.loadState.reset(EGpuApiResourceState::Created);
         *     }
          *
         *     if(not resource.loadState.checkAny(EGpuApiResourceState::Loading
         *                                       | EGpuApiResourceState::Loaded))
         *     {
         *         resource.loadState.set(EGpuApiResourceState::Loading);
         *
         *         EEngineStatus const loadResult = TResource::GpuApiResource_t::load(resource.descriptor, aDependencies, resource.gpuApiHandles, std::forward<TArgs>(aArgs)...);
         *         if(not CheckEngineError(loadResult))
         *         {
         *             resource.loadState.set(EGpuApiResourceState::Error);
         *             return loadResult;
         *         }
         *
         *         resource.loadState.unset(EGpuApiResourceState::Loading);
         *         resource.loadState.set(EGpuApiResourceState::Loaded);
         *     }
         *
         *     return EEngineStatus::Ok;
         * });
         */
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

        OptioanlRef_t<TResource> resourceOpt = getResourceObject<TResource>(aResourceId);
        if(not resourceOpt.has_value())
        {
            return EEngineStatus::ResourceError_NotFound;
        }

        TResource &resource = *resourceOpt;

        // Resource is not being created or loaded?
        if(resource.loadState.checkAny(EGpuApiResourceState::Creating
                                      | EGpuApiResourceState::Loading))
        {
            resource.loadState.set(EGpuApiResourceState::Error);
            return EEngineStatus::Error;
        }

        // Resource is created or loaded?
        if(not resource.loadState.checkAny(EGpuApiResourceState::Created)
        || not resource.loadState.checkAny(EGpuApiResourceState::Loaded))
        {
            resource.loadState.set(EGpuApiResourceState::Error);
            return EEngineStatus::Ok;
        }

        // In any case:
        resource.loadState.unset(EGpuApiResourceState::Transferred);

        if(not resource.loadState.checkAny(EGpuApiResourceState::Unloading
                                          | EGpuApiResourceState::Unloaded))
        {
            resource.loadState.set(EGpuApiResourceState::Unloading);

            EEngineStatus const unloadResult = TResource::GpuApiResource_t::unload(resource.descriptor, aDependencies, resource.gpuApiHandles, std::forward<TArgs>(aArgs)...);
            if(not CheckEngineError(unloadResult))
            {
                resource.loadState.set(EGpuApiResourceState::Error);
                return unloadResult;
            }

            resource.loadState.unset(EGpuApiResourceState::Unloading | EGpuApiResourceState::Loaded);
            resource.loadState.set  (EGpuApiResourceState::Unloaded);
        }

        if(not resource.loadState.checkAny(EGpuApiResourceState::Discarding
                                      | EGpuApiResourceState::Discarded))
        {
            resource.loadState.reset(EGpuApiResourceState::Discarding);

            EEngineStatus const deinitResult = TResource::GpuApiResource_t::deinitialize(resource.descriptor, aDependencies, resource.gpuApiHandles, std::forward<TArgs>(aArgs)...);
            if(not CheckEngineError(deinitResult))
            {
                return deinitResult;
            }

            resource.loadState.unset(EGpuApiResourceState::Discarding | EGpuApiResourceState::Created);
            resource.loadState.reset(EGpuApiResourceState::Discarded);
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

        OptioanlRef_t<TResource> resourceOpt = getResourceObject<TResource>(aResourceId);
        if(not resourceOpt.has_value())
        {
            return EEngineStatus::ResourceError_NotFound;
        }

        TResource &resource = *resourceOpt;

        // Resource is created or loaded?
        if(not resource.loadState.checkAny(EGpuApiResourceState::Loaded))
        {
            resource.loadState.set(EGpuApiResourceState::Error);
            return EEngineStatus::Error;
        }

        if(not resource.loadState.checkAny(EGpuApiResourceState::Transferring
                                      | EGpuApiResourceState::Transferred))
        {
            resource.loadState.set(EGpuApiResourceState::Transferring);

            EEngineStatus const transferResult = TResource::GpuApiResource_t::transfer(resource.descriptor, resource.dependencies, resource.gpuApiHandles, std::forward<TArgs>(aArgs)...);
            if(not CheckEngineError(transferResult))
            {
                return transferResult;
            }

            resource.loadState.unset(EGpuApiResourceState::Transferring);
            resource.loadState.set  (EGpuApiResourceState::Transferred);
        }

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------
}


#endif //__SHIRABEDEVELOPMENT_CRESOURCEMANAGER_H__
