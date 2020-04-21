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
        using GpuApiHandles_t   = typename GpuApiResource_t ::Handles_t;

        Descriptor_t    description;
        GpuApiHandles_t gpuApiHandles;
        ResourceState_t loadState;
    };


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
        using My_t               = CResourceManager<TResources...>;
        using ResourceVariants_t = std::variant<TResources...>;

    public_constructors:
        CResourceManager();

    public_destructors:
        ~CResourceManager();

    public_methods:
        template <typename TResource>
        CEngineResult<OptionalRef_t <TResource>> useResource(ResourceId_t                     const &aResourceId
                                                           , typename TResource::Descriptor_t const &aDescriptor);

        template <typename TResource>
        CEngineResult<OptionalRef_t<TResource>> getResource(ResourceId_t const &aId);

        template <typename TResource>
        CEngineResult<OptionalRef_t<TResource>> uploadResource(ResourceId_t const &aId);

        template <typename TResource>
        CEngineResult<> discardResource(ResourceId_t const &aResourceId);

    private_methods:
        template <typename TResource, typename... TArgs>
        CEngineResult<> initializeResource(TResource const &aResource
                                         , TArgs       &&...aArgs);

        template <typename TResource, typename... TArgs>
        CEngineResult<> deinitializeResource(TResource const &aResource
                                           , TArgs       &&...aArgs);

        template <typename TResource, typename... TArgs>
        CEngineResult<> transferResource(TResource const &aResourceId
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
        CAdjacencyTree<ResourceId_t>                         mResourceTree;
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename... TResources>
    CResourceManager<TResources...>::CResourceManager()
        : mResourceObjects    ()
        , mResourceTree       ()
    {
        mResourceThreadLooper.initialize();
        mResourceThreadLooper.run();
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename... TResources>
    CResourceManager<TResources...>::~CResourceManager()
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
    CEngineResult<OptionalRef_t<TResource>> CResourceManager<TResources...>
            ::useResource(
                  ResourceId_t                     const &aResourceId
                , typename TResource::Descriptor_t const &aDescriptor)
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
    template <typename TResource>
    CEngineResult<OptionalRef_t<TResource>> CResourceManager<TResources...>::getResource(ResourceId_t const &aId)
    {
        OptionalRef_t<TResource> resource = getResourceObject<TResource>(aId);
        if(resource.has_value())
        {
            auto const &[result] = initializeResource(*resource, this);
            switch(result)
            {
                case EEngineStatus::Ok:
                    return { result, resource };
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
    template <typename TResource>
    CEngineResult<OptionalRef_t<TResource>> CResourceManager<TResources...>::uploadResource(ResourceId_t const &aId)
    {
        OptionalRef_t<TResource> resource = getResourceObject<TResource>(aId);
        if(resource.has_value())
        {
            auto const &[result] = transferResource(*resource, this);
            switch(result)
            {
                case EEngineStatus::Ok:
                    return { result, resource };
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
    template <typename TResource>
    CEngineResult<> CResourceManager<TResources...>::discardResource(ResourceId_t const &aResourceId)
    {
        auto iterator = mResourceObjects.find(aResourceId);
        if(mResourceObjects.end() != iterator)
        {
            ResourceVariants_t &variant = *iterator;
            TResource          &value   = std::get<TResource>(variant);

            TResource &resource = std::get<TResource>(value);
            if(EGpuApiResourceState::Discarded != resource.state)
            {
                EEngineStatus const deinitResult = deinitializeResource<TResource>(aResourceId, this).result();
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
    CEngineResult<> CResourceManager<TResources...>::initializeResource(TResource const &aResource
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
        auto const run = [=] () -> EEngineStatus
        {
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
        };

        mResourceThreadLooper.getDispatcher().post(run);

        return EEngineStatus::Ok;
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template<typename TResourceManager, typename T>
    struct is_unloadable
    {
    private:
        template<typename UResourceManager, typename U> static auto check(int) -> decltype(std::declval<U>().template unload<UResourceManager>() == 1, std::true_type());

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
    CEngineResult<> CResourceManager<TResources...>::deinitializeResource(TResource const &aResourceId
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
    CEngineResult<> CResourceManager<TResources...>::transferResource(TResource const &aResourceId
                                                                    , TArgs       &&...aArgs)
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
