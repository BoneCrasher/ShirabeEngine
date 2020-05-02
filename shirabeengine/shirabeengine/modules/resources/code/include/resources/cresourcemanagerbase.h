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

#include "resources/extensibility.h"
#include "resources/cresourceobject.h"

namespace engine::resources
{
    using asset::AssetId_t ;

    /**
     *
     */
    template <typename... TResources>
    class
        [[nodiscard]]
        SHIRABE_LIBRARY_EXPORT CResourceManagerBase
    {
        SHIRABE_DECLARE_LOG_TAG(CResourceManager);

    private_typedefs:
        using My_t               = CResourceManagerBase<TResources...>;
        using ResourceVariants_t = std::variant<TResources...>;

    public_constructors:
        CResourceManagerBase();

    public_destructors:
        ~CResourceManagerBase();

    public_methods:
        template <typename TResource>
        CEngineResult<OptionalRef_t <TResource>> useResource(ResourceId_t                     const &aResourceId
                                                           , typename TResource::Descriptor_t const &aDescriptor
                                                           , bool                                    aInitializeImmediately = false);

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
        CEngineResult<> initializeResourceImpl(TResource const &aResource
                                             , TArgs       &&...aArgs);

        template <typename TResource, typename... TArgs>
        CEngineResult<> deinitializeResourceImpl(TResource const &aResource
                                               , TArgs       &&...aArgs);

        template <typename TResource, typename... TArgs>
        CEngineResult<> transferResourceImpl(TResource const &aResourceId
                                           , TArgs       &&...aArgs);

        template <typename TResource, typename... TArgs>
        CEngineResult<> updateResourceImpl(TResource const &aResource
                                         , TArgs       &&...aArgs);

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

    private_members:
        engine::threading::CLooper<EEngineStatus> mResourceThreadLooper;

        std::unordered_map<ResourceId_t, ResourceVariants_t> mResourceObjects;
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename... TResources>
    CResourceManagerBase<TResources...>::CResourceManagerBase()
        : mResourceObjects    ()
    {
        mResourceThreadLooper.initialize();
        mResourceThreadLooper.run();
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename... TResources>
    CResourceManagerBase<TResources...>::~CResourceManagerBase()
    {
        mResourceThreadLooper.abortAndJoin();
        mResourceThreadLooper.deinitialize();
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename... TResources>
    template <typename TResource>
    CEngineResult<OptionalRef_t<TResource>>
        CResourceManagerBase<TResources...>
            ::useResource(
                  ResourceId_t                     const &aResourceId
                , typename TResource::Descriptor_t const &aDescriptor
                , bool                                    aInitializeImmediately )
    {
        CEngineResult<OptionalRef_t<TResource>> result = {EEngineStatus::Error, {} };

        auto const alreadyFoundIt = mResourceObjects.find(aResourceId);
        if(mResourceObjects.end() != mResourceObjects.find(aResourceId))
        {
            ResourceVariants_t &variant = alreadyFoundIt->second;
            TResource          &value   = std::get<TResource>(variant);
            return { EEngineStatus::Ok, OptionalRef_t<TResource>(value) };
        }

        //
        // Wrap the gpu api resource operations with dependency resolving operations.
        //
        TResource resource {};
        resource.descriptor    = aDescriptor;
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
    CEngineResult<OptionalRef_t<TResource>>
        CResourceManagerBase<TResources...>
            ::getResource(
                ResourceId_t const &aId
                , TArgs        &&...aArgs)
    {
        OptionalRef_t<TResource> resource = getResourceObject<TResource>(aId);
        if(false == resource.has_value())
            return { EEngineStatus::ResourceError_NotFound };

        return { EEngineStatus::Ok, resource };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------

    template <typename... TResources>
    template <typename TResource, typename... TArgs>
    CEngineResult<>
        CResourceManagerBase<TResources...>
            ::initializeResource(
                ResourceId_t const &aId
                , TArgs        &&...aArgs)
    {
        OptionalRef_t<TResource> resource = getResourceObject<TResource>(aId);
        if(resource.has_value())
        {
            auto const &[result] = initializeResourceImpl(*resource, this, std::forward<TArgs>(aArgs)...);
            switch(result)
            {
                case EEngineStatus::Ok:
                    return { result };
                case EEngineStatus::Resource_NotReady:
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

    template <typename... TResources>
    template <typename TResource, typename... TArgs>
    CEngineResult<>
        CResourceManagerBase<TResources...>
            ::updateResource(
                ResourceId_t const &aId
                , TArgs        &&...aArgs)
    {
        OptionalRef_t<TResource> resource = getResourceObject<TResource>(aId);
        if(resource.has_value())
        {
            auto const &[result] = updateResourceImpl(*resource, this, std::forward<TArgs>(aArgs)...);
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

    template <typename... TResources>
    template <typename TResource, typename... TArgs>
    CEngineResult<>
        CResourceManagerBase<TResources...>
            ::deinitializeResource(
                ResourceId_t const &aId
                , TArgs        &&...aArgs)
    {
        OptionalRef_t<TResource> resource = getResourceObject<TResource>(aId);
        if(resource.has_value())
        {
            auto const &[result] = deinitializeResourceImpl(*resource, this, std::forward<TArgs>(aArgs)...);
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
    template <typename... TResources>
    template <typename TResource, typename... TArgs>
    CEngineResult<>
        CResourceManagerBase<TResources...>
            ::uploadResource(
                  ResourceId_t const &aId
                , TArgs          &&...aArgs)
    {
        OptionalRef_t<TResource> resource = getResourceObject<TResource>(aId);
        if(resource.has_value())
        {
            auto const &[result] = transferResourceImpl(*resource, this, std::forward<TArgs>(aArgs)...);
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
    template <typename... TResources>
    template <typename TResource, typename... TArgs>
    CEngineResult<>
        CResourceManagerBase<TResources...>
            ::discardResource(
                ResourceId_t const &aResourceId
                , TArgs        &&...aArgs)
    {
        OptionalRef_t<TResource> resource = getResourceObject<TResource>(aResourceId);
        if(resource.has_value())
        {
            EEngineStatus const deinitialized = deinitializeResourceImpl(*resource, std::forward<TArgs>(aArgs)...);
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
    template<typename TResourceManager, typename T>
    struct is_loadable
    {
    private:
        template<typename UResourceManager, typename U> static auto check(int) -> decltype(std::declval<U>().template load<UResourceManager>() == 1, std::true_type());

        template<typename> static std::false_type check(...);

    public:
        static constexpr bool value = std::is_same<decltype(check<TResourceManager, T>(0)), std::true_type>::value;
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename... TResources>
    template <typename TResource, typename... TArgs>
    CEngineResult<>
        CResourceManagerBase<TResources...>
            ::initializeResourceImpl(
                TResource const &aResource
              , TArgs       &&...aArgs)
    {
        using namespace core;

        // Resource is being created and/or loaded.
        // Do not check for ::Created, since otherwise the resource won't be loaded.
        if(aResource.loadState.checkAny(EGpuApiResourceState::Creating
                                      | EGpuApiResourceState::Created
                                      | EGpuApiResourceState::Loading))
        {
            return EEngineStatus::Resource_NotReady;
        }

        if(aResource.loadState.checkAny(EGpuApiResourceState::Loaded))
        {
            return EEngineStatus::Ok;
        }

        // Run asynchronously
        // auto const run = [=] () -> EEngineStatus
        // {
        if(not aResource.loadState.checkAny(EGpuApiResourceState::Creating
                                          | EGpuApiResourceState::Created))
        {
            aResource.loadState.set(EGpuApiResourceState::Creating);

            EEngineStatus const initResult = TResource::GpuApiResource_t::template initialize<My_t>(aResource.descriptor, aResource.gpuApiHandles, std::forward<TArgs>(aArgs)...);
            if(not CheckEngineError(initResult))
            {
                aResource.loadState.set(EGpuApiResourceState::Error);
                return initResult;
            }

            aResource.loadState.reset(EGpuApiResourceState::Created);
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
    };

    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename... TResources>
    template <typename TResource, typename... TArgs>
    CEngineResult<>
        CResourceManagerBase<TResources...>
            ::updateResourceImpl(
                TResource const &aResource
              , TArgs       &&...aArgs)
    {
        using namespace core;

        // Resource is being created and/or loaded.
        // Do not check for ::Created, since otherwise the resource won't be loaded.
        if(aResource.loadState.checkAny(EGpuApiResourceState::Loading))
        {
            return EEngineStatus::Resource_NotReady;
        }

        if(aResource.loadState.checkAny(EGpuApiResourceState::Loaded))
        {
            return EEngineStatus::Ok;
        }

        if constexpr(is_loadable<My_t, TResource>::value)
        {
            if(not aResource.loadState.checkAny(EGpuApiResourceState::Loading
                                              | EGpuApiResourceState::Loaded))
            {
                aResource.loadState.set(EGpuApiResourceState::Loading);

                EEngineStatus const loadResult = TResource::GpuApiResource_t::template load<My_t>(aResource.descriptor, aResource.gpuApiHandles, std::forward<TArgs>(aArgs)...);
                if(not CheckEngineError(loadResult))
                {
                    aResource.loadState.set(EGpuApiResourceState::Error);
                    return loadResult;
                }

                aResource.loadState.unset(EGpuApiResourceState::Loading);
                aResource.loadState.set(EGpuApiResourceState::Loaded);
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
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template<typename TResourceManager, typename T>
    struct is_unloadable
    {
    private:
        template<typename UResourceManager, typename U> static auto check(int) -> decltype(std::declval<U>()::template unload<UResourceManager>() == 1, std::true_type());

        template<typename> static std::false_type check(...);

    public:
        static constexpr bool value = std::is_same<decltype(check<TResourceManager, T>(0)), std::true_type>::value;
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename... TResources>
    template <typename TResource, typename... TArgs>
    CEngineResult<> CResourceManagerBase<TResources...>::deinitializeResourceImpl(TResource const &aResourceId
                                                                            , TArgs       &&...aArgs)
    {
        using namespace core;

        // Resource is being created or loaded?
        if(aResourceId.loadState.checkAny(EGpuApiResourceState::Creating
                                        | EGpuApiResourceState::Loading))
        {
            aResourceId.loadState.set(EGpuApiResourceState::Error);
            return EEngineStatus::Error;
        }

        // Resource is created or loaded?
        // Don't change the order of checks, since it will break behaviour.
        if(not aResourceId.loadState.checkAny(EGpuApiResourceState::Created)
        || not aResourceId.loadState.checkAny(EGpuApiResourceState::Loaded))
        {
            aResourceId.loadState.set(EGpuApiResourceState::Error);
            return EEngineStatus::Ok;
        }

        // In any case:
        aResourceId.loadState.unset(EGpuApiResourceState::Transferred);
        if constexpr(is_unloadable<My_t, TResource>::value)
        {
            if(not aResourceId.loadState.checkAny(EGpuApiResourceState::Unloading
                                              | EGpuApiResourceState::Unloaded))
            {
                aResourceId.loadState.set(EGpuApiResourceState::Unloading);

                EEngineStatus const unloadResult = TResource::GpuApiResource_t::template unload<My_t>(aResourceId.descriptor, aResourceId.gpuApiHandles, std::forward<TArgs>(aArgs)...);
                if(not CheckEngineError(unloadResult))
                {
                    aResourceId.loadState.set(EGpuApiResourceState::Error);
                    return unloadResult;
                }

                aResourceId.loadState.unset(EGpuApiResourceState::Unloading | EGpuApiResourceState::Loaded);
                aResourceId.loadState.set  (EGpuApiResourceState::Unloaded);
            }
        }

        if(not aResourceId.loadState.checkAny(EGpuApiResourceState::Discarding
                                            | EGpuApiResourceState::Discarded))
        {
            aResourceId.loadState.reset(EGpuApiResourceState::Discarding);

            EEngineStatus const deinitResult = TResource::GpuApiResource_t::template deinitialize<My_t>(aResourceId.descriptor, aResourceId.gpuApiHandles, std::forward<TArgs>(aArgs)...);
            if(not CheckEngineError(deinitResult))
            {
                return deinitResult;
            }

            aResourceId.loadState.unset(EGpuApiResourceState::Discarding | EGpuApiResourceState::Created);
            aResourceId.loadState.reset(EGpuApiResourceState::Discarded);
        }

        return EEngineStatus::Ok;
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template<typename TResourceManager, typename T>
    struct is_transferrable
    {
    private:
        template<typename UResourceManager, typename U> static auto check(int) -> decltype(std::declval<U>().template transfer<UResourceManager>() == 1, std::true_type());

        template<typename> static std::false_type check(...);

    public:
        static constexpr bool value = std::is_same<decltype(check<TResourceManager, T>(0)), std::true_type>::value;
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename... TResources>
    template <typename TResource, typename... TArgs>
    CEngineResult<>
        CResourceManagerBase<TResources...>
            ::transferResourceImpl(
                TResource const &aResourceId, TArgs &&...aArgs)
    {
        using namespace core;

        if constexpr(is_transferrable<My_t, TResource>::value)
        {
            // Resource is created or loaded?
            if(not aResourceId.loadState.checkAny(EGpuApiResourceState::Loaded))
            {
                aResourceId.loadState.set(EGpuApiResourceState::Error);
                return EEngineStatus::Error;
            }

            if(not aResourceId.loadState.checkAny(EGpuApiResourceState::Transferring
                                                | EGpuApiResourceState::Transferred))
            {
                aResourceId.loadState.set(EGpuApiResourceState::Transferring);

                EEngineStatus const transferResult = TResource::GpuApiResource_t::template transfer<My_t>(aResourceId.descriptor, aResourceId.gpuApiHandles, std::forward<TArgs>(aArgs)...);
                if(not CheckEngineError(transferResult))
                {
                    return transferResult;
                }

                aResourceId.loadState.unset(EGpuApiResourceState::Transferring);
                aResourceId.loadState.set  (EGpuApiResourceState::Transferred);
            }
        }

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------
}


#endif //__SHIRABEDEVELOPMENT_CRESOURCEMANAGER_H__
