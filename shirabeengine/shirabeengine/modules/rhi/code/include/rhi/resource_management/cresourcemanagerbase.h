//
// Created by dotti on 19.10.19.
//

#ifndef __SHIRABEDEVELOPMENT_CRESOURCEMANAGERBASE_H__
#define __SHIRABEDEVELOPMENT_CRESOURCEMANAGERBASE_H__

#include <typeindex>
#include <variant>
#include <base/stl_container_helpers.h>
#include <platform/platform.h>
#include <graphicsapi/definitions.h>
#include <asset/assettypes.h>
#include <asset/assetstorage.h>
#include <core/bitfield.h>
#include <core/threading/looper.h>
#include <base/stl_container_helpers.h>

#include "rhi/resource_management/extensibility.h"
#include "rhi/resource_management/cresourceobject.h"

namespace engine::rhi
{
    using asset::AssetId_t;

    /**
     *
     */
    template <typename TRHIEnvironment, typename... TResources>
    class
        [[nodiscard]]
        SHIRABE_LIBRARY_EXPORT CRHIResourceManagerBase
    {
        SHIRABE_DECLARE_LOG_TAG(CRHIResourceManager);

    private_typedefs:
        using My_t               = CRHIResourceManagerBase<TRHIEnvironment, TResources...>;
        using ResourceVariants_t = std::variant<TResources...>;

    private_members:
        engine::threading::CLooper<EEngineStatus>            mResourceThreadLooper;
        std::unordered_map<ResourceId_t, ResourceVariants_t> mResourceObjects;
        Shared<TRHIEnvironment>                              mRhiEnvironment;

    public_constructors:
        CRHIResourceManagerBase();

    public_destructors:
        ~CRHIResourceManagerBase();

    public_methods:
        template <typename TResource>
        CEngineResult<typename TResource::RHIResourceDescriptor_t> const getResourceDescription(ResourceId_t const &aResourceId) const;

        template <typename TResource>
        CEngineResult<OptionalRef_t <TResource>> useResource(ResourceId_t                                const &aResourceId
                                                           , typename TResource::RHIResourceDescriptor_t const &aDescriptor
                                                           , bool                                               aInitializeImmediately = false);

        template <typename TResource, typename... TArgs>
        CEngineResult<OptionalRef_t<TResource>> getResource(ResourceId_t const &aId, TArgs &&...aArgs);

        template <typename TResource, typename... TArgs>
        CEngineResult<> initializeResource(ResourceId_t const &aId, TArgs &&...aArgs);

        template <typename TResource, typename... TArgs>
        CEngineResult<> updateResource(ResourceId_t const &aId, TArgs &&...aArgs);

        template <typename TResource, typename... TArgs>
        CEngineResult<> deinitializeResource(ResourceId_t const &aId, TArgs &&...aArgs);

        template <typename TResource, typename... TArgs>
        CEngineResult<> uploadResource(ResourceId_t const &aId, TArgs &&...aArgs);

        template <typename TResource, typename... TArgs>
        CEngineResult<> discardResource(ResourceId_t const &aResourceId, TArgs &&...aArgs);

    private_methods:
        template <typename TResource, typename... TArgs>
        EEngineStatus initializeResourceImpl(TResource &aResource
                                             , TArgs &&...aArgs);

        template <typename TResource, typename... TArgs>
        EEngineStatus deinitializeResourceImpl(TResource &aResource
                                               , TArgs &&...aArgs);

        template <typename TResource, typename... TArgs>
        EEngineStatus transferResourceImpl(TResource &aResourceId
                                           , TArgs &&...aArgs);

        template <typename TResource, typename... TArgs>
        EEngineStatus updateResourceImpl(TResource &aResource
                                         , TArgs &&...aArgs);

        template <typename TResource>
        OptionalRef_t<TResource> getResourceObject(ResourceId_t const &aId)
        {
            bool const hasObjectForId = assoc_container_contains(mResourceObjects, aId);
            if(hasObjectForId)
            {
                ResourceVariants_t &variant = mResourceObjects.at(aId);
                return OptionalRef_t<TResource>(std::get<TResource>(variant));
            }

            return {};
        }

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
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename TRHIEnvironment, typename... TResources>
    CRHIResourceManagerBase<TRHIEnvironment, TResources...>::CRHIResourceManagerBase()
        : mResourceObjects    ()
    {
        mResourceThreadLooper.initialize();
        mResourceThreadLooper.run();
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename TRHIEnvironment, typename... TResources>
    CRHIResourceManagerBase<TRHIEnvironment, TResources...>::~CRHIResourceManagerBase()
    {
        mResourceThreadLooper.abortAndJoin();
        mResourceThreadLooper.deinitialize();
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename TRHIEnvironment, typename... TResources>
    template <typename TLogicalResource>
    CEngineResult<typename TLogicalResource::RHIResourceDescriptor_t> const
        CRHIResourceManagerBase<TRHIEnvironment, TResources...>
            ::getResourceDescription(ResourceId_t const &aResourceId) const
    {
        auto const &[result, resourceOpt] = getResource<SRHIResourceState<TLogicalResource>>(aResourceId);
        if(CheckEngineError(result))
        {
            return EEngineStatus::Error;
        }

        SRHIResourceState<TLogicalResource>                resource   = *resourceOpt;
        typename TLogicalResource::RHIResourceDescriptor_t descriptor = resource.rhiCreateDesc;

        return { EEngineStatus::Ok, descriptor };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename TRHIEnvironment, typename... TResources>
    template <typename TResource>
    CEngineResult<OptionalRef_t<TResource>>
        CRHIResourceManagerBase<TRHIEnvironment, TResources...>
            ::useResource(
                  ResourceId_t                                const &aResourceId
                , typename TResource::RHIResourceDescriptor_t const &aDescriptor
                , bool                                               aInitializeImmediately )
    {
        //
        // If the resource already exists, return instead.
        //
        auto const &resourceOpt = getResourceObject<TResource>(aResourceId);
        if(resourceOpt.has_value())
        {
            return { EEngineStatus::Ok, resourceOpt };
        }

        //
        // Wrap the gpu api resource operations with dependency resolving operations.
        //
        TResource resource {};
        resource.rhiCreateDesc = aDescriptor;
        resource.rhiHandles    = {};
        resource.rhiLoadState  = EGpuApiResourceState::Unloaded;

        storeResourceObject<TResource>(aResourceId, resource);

        if(aInitializeImmediately)
        {
            initializeResource<TResource>(aResourceId);
        }

        return { EEngineStatus::Ok, resource };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------

    template <typename TRHIEnvironment, typename... TResources>
    template <typename TResource, typename... TArgs>
    CEngineResult<OptionalRef_t<TResource>>
        CRHIResourceManagerBase<TRHIEnvironment, TResources...>
            ::getResource(
                ResourceId_t const &aId, TArgs &&...aArgs)
    {
        OptionalRef_t<TResource> resource = getResourceObject<TResource>(aId);
        if(false == resource.has_value())
        {
            return {EEngineStatus::ResourceError_NotFound};
        }

        return { EEngineStatus::Ok, resource };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------

    template <typename TRHIEnvironment, typename... TResources>
    template <typename TResource, typename... TArgs>
    CEngineResult<>
        CRHIResourceManagerBase<TRHIEnvironment, TResources...>
            ::initializeResource(
                ResourceId_t const &aId, TArgs &&...aArgs)
    {
        OptionalRef_t<TResource> resourceOpt = getResourceObject<TResource>(aId);
        if(resourceOpt.has_value())
        {
            TResource &resource = *resourceOpt;
            auto const &result = initializeResourceImpl(resource, std::forward<TArgs>(aArgs)...);
            switch(result)
            {
                case EEngineStatus::Ok:
                    return { result };
                case EEngineStatus::Resource_NotReady:
                    return { result };
                default:
                    return EEngineStatus::Error;
            }
        }

        return EEngineStatus::ResourceError_NotFound;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------

    template <typename TRHIEnvironment, typename... TResources>
    template <typename TResource, typename... TArgs>
    CEngineResult<>
        CRHIResourceManagerBase<TRHIEnvironment, TResources...>
            ::updateResource(
                ResourceId_t const &aId, TArgs &&...aArgs)
    {
        OptionalRef_t<TResource> resourceOpt = getResourceObject<TResource>(aId);
        if(resourceOpt.has_value())
        {
            TResource &resource = *resourceOpt;
            auto const &[result] = updateResourceImpl(resource, std::forward<TArgs>(aArgs)...);
            switch(result)
            {
                case EEngineStatus::Ok:
                    return { result };
                default:
                    return EEngineStatus::Error;
            }
        }

        return EEngineStatus::ResourceError_NotFound;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------

    template <typename TRHIEnvironment, typename... TResources>
    template <typename TResource, typename... TArgs>
    CEngineResult<>
        CRHIResourceManagerBase<TRHIEnvironment, TResources...>
            ::deinitializeResource(
                ResourceId_t const &aId, TArgs &&...aArgs)
    {
        OptionalRef_t<TResource> resourceOpt = getResourceObject<TResource>(aId);
        if(resourceOpt.has_value())
        {
            TResource &resource = *resourceOpt;
            auto const &[result] = deinitializeResourceImpl(resource, std::forward<TArgs>(aArgs)...);
            switch(result)
            {
                case EEngineStatus::Ok:
                    return { result };
                default:
                    return EEngineStatus::Error;
            }
        }

        return EEngineStatus::ResourceError_NotFound;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename TRHIEnvironment, typename... TResources>
    template <typename TResource, typename... TArgs>
    CEngineResult<>
        CRHIResourceManagerBase<TRHIEnvironment, TResources...>
            ::uploadResource(
                  ResourceId_t const &aId, TArgs &&...aArgs)
    {
        OptionalRef_t<TResource> resourceOpt = getResourceObject<TResource>(aId);
        if(resourceOpt.has_value())
        {
            TResource &resource = *resourceOpt;
            auto const result   = transferResourceImpl(resource, std::forward<TArgs>(aArgs)...);
            switch(result)
            {
                case EEngineStatus::Ok:
                    return result;
                default:
                    return EEngineStatus::Error;
            }
        }

        return EEngineStatus::ResourceError_NotFound;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename TRHIEnvironment, typename... TResources>
    template <typename TResource, typename... TArgs>
    CEngineResult<>
        CRHIResourceManagerBase<TRHIEnvironment, TResources...>
            ::discardResource(
                ResourceId_t const &aResourceId, TArgs &&...aArgs)
    {
        OptionalRef_t<TResource> resourceOpt = getResourceObject<TResource>(aResourceId);
        if(resourceOpt.has_value())
        {
            TResource &resource = *resourceOpt;
            EEngineStatus const deinitialized = deinitializeResourceImpl(resource, std::forward<TArgs>(aArgs)...);
            if(CheckEngineError(deinitialized))
            {
                CLog::Error(logTag(), "Failed to deinitialize resource with ID '{}'", aResourceId);
                return EEngineStatus::Error;
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
    template <typename TRHIEnvironment, typename... TResources>
    template <typename TResource, typename... TArgs>
    EEngineStatus
        CRHIResourceManagerBase<TRHIEnvironment, TResources...>
            ::initializeResourceImpl(
                TResource &aResource, TArgs &&...aArgs)
    {
        using namespace core;

        // Resource is being created and/or loaded.
        // Do not check for ::Created, since otherwise the resource won't be loaded.
        if(aResource.rhiLoadState.checkAny(EGpuApiResourceState::Creating
                                           | EGpuApiResourceState::Created
                                           | EGpuApiResourceState::Loading))
        {
            return EEngineStatus::Resource_NotReady;
        }

        if(aResource.rhiLoadState.checkAny(EGpuApiResourceState::Loaded))
        {
            return EEngineStatus::Ok;
        }

        // Run asynchronously
        // auto const run = [=] () -> EEngineStatus
        // {
        if(not aResource.rhiLoadState.checkAny(EGpuApiResourceState::Creating
                                               | EGpuApiResourceState::Created))
        {
            aResource.rhiLoadState.set(EGpuApiResourceState::Creating);

            EEngineStatus const initResult = TResource::RHIMappedResource_t::template initialize<My_t>(aResource.rhiCreateDesc, aResource.rhiHandles, this, mRhiEnvironment.get(), std::forward<TArgs>(aArgs)...);
            if(CheckEngineError(initResult))
            {
                aResource.rhiLoadState.set(EGpuApiResourceState::Error);
                return initResult;
            }

            aResource.rhiLoadState.reset(EGpuApiResourceState::Created);
        }

        return EEngineStatus::Ok;
        // };

        // if(aAsync)
        // {
        //     mResourceThreadLooper.getDispatcher().post(run);
        // }
        // else
        // {
        //    run();
        // }

        // return EEngineStatus::Ok;
    }

    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename TRHIEnvironment, typename... TResources>
    template <typename TResource, typename... TArgs>
    EEngineStatus
        CRHIResourceManagerBase<TRHIEnvironment, TResources...>
            ::updateResourceImpl(
                TResource &aResource, TArgs &&...aArgs)
    {
        using namespace core;

        // Resource is being created and/or loaded.
        // Do not check for ::Created, since otherwise the resource won't be loaded.
        if(aResource.rhiLoadState.checkAny(EGpuApiResourceState::Loading))
        {
            return EEngineStatus::Resource_NotReady;
        }

        if(aResource.rhiLoadState.checkAny(EGpuApiResourceState::Loaded))
        {
            return EEngineStatus::Ok;
        }

        if constexpr(TResource::is_loadable)
        {
            if(not aResource.rhiLoadState.checkAny(EGpuApiResourceState::Loading
                                                   | EGpuApiResourceState::Loaded))
            {
                aResource.rhiLoadState.set(EGpuApiResourceState::Loading);

                EEngineStatus const loadResult = TResource::RHIMappedResource_t::template load<My_t>(aResource.rhiCreateDesc, aResource.rhiHandles, this, mRhiEnvironment.get(), std::forward<TArgs>(aArgs)...);
                if(CheckEngineError(loadResult))
                {
                    aResource.rhiLoadState.set(EGpuApiResourceState::Error);
                    return loadResult;
                }

                aResource.rhiLoadState.unset(EGpuApiResourceState::Loading);
                aResource.rhiLoadState.set(EGpuApiResourceState::Loaded);
            }
        }

        return EEngineStatus::Ok;
        // };

        // if(aAsync)
        // {
        //     mResourceThreadLooper.getDispatcher().post(run);
        // }
        // else
        // {
        // run();
        // }

        // return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename TRHIEnvironment, typename... TResources>
    template <typename TResource, typename... TArgs>
    EEngineStatus
        CRHIResourceManagerBase<TRHIEnvironment, TResources...>
            ::deinitializeResourceImpl(
                    TResource &aResource, TArgs       &&...aArgs)
    {
        using namespace core;

        // Resource is being created or loaded?
        if(aResource.rhiLoadState.checkAny(EGpuApiResourceState::Creating
                                           | EGpuApiResourceState::Loading))
        {
            aResource.rhiLoadState.set(EGpuApiResourceState::Error);
            return EEngineStatus::Error;
        }

        // Resource is created or loaded?
        // Don't change the order of checks, since it will break behaviour.
        if(not aResource.rhiLoadState.checkAny(EGpuApiResourceState::Created)
        || not aResource.rhiLoadState.checkAny(EGpuApiResourceState::Loaded))
        {
            aResource.rhiLoadState.set(EGpuApiResourceState::Error);
            return EEngineStatus::Ok;
        }

        // In any case:
        aResource.rhiLoadState.unset(EGpuApiResourceState::Transferred);
        if constexpr(TResource::is_unloadable)
        {
            if(not aResource.rhiLoadState.checkAny(EGpuApiResourceState::Unloading
                                                   | EGpuApiResourceState::Unloaded))
            {
                aResource.rhiLoadState.set(EGpuApiResourceState::Unloading);

                EEngineStatus const unloadResult = TResource::RHIMappedResource_t::template unload<My_t>(aResource.rhiCreateDesc, aResource.rhiHandles, this, mRhiEnvironment.get(), std::forward<TArgs>(aArgs)...);
                if(CheckEngineError(unloadResult))
                {
                    aResource.rhiLoadState.set(EGpuApiResourceState::Error);
                    return unloadResult;
                }

                aResource.rhiLoadState.unset(EGpuApiResourceState::Unloading | EGpuApiResourceState::Loaded);
                aResource.rhiLoadState.set(EGpuApiResourceState::Unloaded);
            }
        }

        if(not aResource.rhiLoadState.checkAny(EGpuApiResourceState::Discarding
                                               | EGpuApiResourceState::Discarded))
        {
            aResource.rhiLoadState.reset(EGpuApiResourceState::Discarding);

            EEngineStatus const deinitResult = TResource::RHIMappedResource_t::template deinitialize<My_t>(aResource.rhiCreateDesc, aResource.rhiHandles, this, mRhiEnvironment.get(), std::forward<TArgs>(aArgs)...);
            if(not CheckEngineError(deinitResult))
            {
                return deinitResult;
            }

            aResource.rhiLoadState.unset(EGpuApiResourceState::Discarding | EGpuApiResourceState::Created);
            aResource.rhiLoadState.reset(EGpuApiResourceState::Discarded);
        }

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename TRHIEnvironment, typename... TResources>
    template <typename TResource, typename... TArgs>
    EEngineStatus
        CRHIResourceManagerBase<TRHIEnvironment, TResources...>
            ::transferResourceImpl(
                TResource &aResourceId, TArgs &&...aArgs)
    {
        using namespace core;

        if constexpr(TResource::is_transferrable)
        {
            // Resource is created or loaded?
            if(not aResourceId.rhiLoadState.checkAny(EGpuApiResourceState::Loaded))
            {
                aResourceId.rhiLoadState.set(EGpuApiResourceState::Error);
                return EEngineStatus::Error;
            }

            if(not aResourceId.rhiLoadState.checkAny(EGpuApiResourceState::Transferring
                                                     | EGpuApiResourceState::Transferred))
            {
                aResourceId.rhiLoadState.set(EGpuApiResourceState::Transferring);

                EEngineStatus const transferResult = TResource::RHIMappedResource_t::template transfer<My_t>(aResourceId.rhiCreateDesc, aResourceId.rhiHandles, this, mRhiEnvironment.get(), std::forward<TArgs>(aArgs)...);
                if(CheckEngineError(transferResult))
                {
                    return transferResult;
                }

                aResourceId.rhiLoadState.unset(EGpuApiResourceState::Transferring);
                aResourceId.rhiLoadState.set(EGpuApiResourceState::Transferred);
            }
        }

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------
}


#endif //__SHIRABEDEVELOPMENT_CRESOURCEMANAGER_H__
