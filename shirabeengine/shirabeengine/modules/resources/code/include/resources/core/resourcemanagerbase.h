#ifndef __SHIRABE_RESOURCEMANAGER_H__
#define __SHIRABE_RESOURCEMANAGER_H__

#include <thread>
#include <typeindex>

#include <log/log.h>
#include <platform/platform.h>

#include <core/enginetypehelper.h>
#include <core/random.h>
#include <base/string.h>

#include "resources/core/iresourceproxy.h"
#include "resources/core/resourcepool.h"
#include "resources/core/resourceproxyfactory.h"

namespace engine
{
    namespace resources
    {
        //static class CMapHelper
        //{
        template <typename K, typename V>
        static bool extractKeys(
                std::map<K, V> const &aMap,
                std::vector<K>       &aOutKeys)
        {
            struct SFetchKey
            {
                K operator()(std::pair<K, V> const &keyValuePair) const
                {
                    return keyValuePair.first;
                }
            };

            // Retrieve all keys
            std::transform(aMap.begin(), aMap.end(), back_inserter(aOutKeys), SFetchKey());

            return true;
        }

        template <typename K, typename V>
        static bool extractValues(
                std::map<K, V> const &aMap,
                std::vector<V>       &aOutValues)
        {
            struct SFetchValue
            {
                V operator()(std::pair<K, V> const &aPair) const
                {
                    return aPair.second;
                }
            };

            // Retrieve all keys
            std::transform(aMap.begin(), aMap.end(), back_inserter(aOutValues), SFetchValue());

            return true;
        }
        //};

        /**
         * The resource manager provides CRUD operations to create, load, update, read, unload and destroy
         * different resource types in various types of resource backends.
         */
        class SHIRABE_TEST_EXPORT CResourceManagerBase
        {
            SHIRABE_DECLARE_LOG_TAG(CResourceManager)

        public_destructors:
            /**
             * Destroy and run...
             */
            virtual ~CResourceManagerBase();

       public_methods:
            /**
             * Setup this resource manager for use.
             *
             * @return EEngineStatus::Ok on success. An error code otherwise.
             */
            virtual CEngineResult<> initialize();

            /**
             * Deinitialize the resource manager and free..
             *
             * @return EEngineStatus::Ok on success. An error code otherwise.
             */
            virtual CEngineResult<> deinitialize();

            /**
             * Unload, destroy and remove all stored resources.
             *
             * @return
             */
            CEngineResult<> clear();

            /**
             * Create a new resource from a creation request and a human readable
             * resource id.
             * Optionally, the creation can be deferred to an explicit call to loadResource.
             *
             * @tparam TResource       The underlying resource type create.
             * @param aRequest         The creation request containing at least a resource descriptor.
             * @param aResourceId      The human readable resource id, by which the resource can be accessed.
             * @param creationDeferred A flag indicating immediate or deferred resource loading in it's backednd.
             * @return                 EEngineStatus::Ok, if successful. Any error code otherwise.
             */
            template <typename TResource>
            CEngineResult<> createResource(
                    typename TResource::CCreationRequest const &aRequest,
                    std::string                          const &aResourceId,
                    bool                                        aCreationDeferred = false);

            /**
             * Load a resource in it's respective backend, if not done so yet.
             *
             * @tparam TResource  The underlying resource type load.
             * @param aResourceId The resource id of the resource to be loaded.
             * @return            EEngineStatus::Ok, if successful. Any error code otherwise.
             */
            template <typename TResource>
            CEngineResult<> loadResource(std::string const &aResourceId);

            /**
             * Update a resource based on the update request provided.
             *
             * @tparam TResource  The underlying resource type load.
             * @param aResourceId The resource id of the resource to be updated.
             * @param aRequest    An update request containing update data for the backend resource.
             * @return            EEngineStatus::Ok, if successful. Any error code otherwise.
             */
            template <typename TResource>
            CEngineResult<> updateResource(
                    std::string                        const &aResourceId,
                    typename TResource::CUpdateRequest const &aRequest);

            /**
             * Unload a resource in it's backend. This will not remove it from the resourcemanager.
             *
             * @tparam TResource  The underlying resource type load.
             * @param aResourceId The resource id of the resource to be unloaded.
             * @return            EEngineStatus::Ok, if successful. Any error code otherwise.
             */
            template <typename TResource>
            CEngineResult<> unloadResource(std::string const &aResourceId);

            /**
             * Destroy a resource, which will also remove it from the resource manager.
             *
             * @tparam TResource  The underlying resource type load.
             * @param aResourceId The resource id of the resource to be destroyed.
             * @return            EEngineStatus::Ok, if successful. Any error code otherwise.
             */
            template <typename TResource>
            CEngineResult<> destroyResource(std::string const &aResourceId);

        protected_typedefs:
            /**
             * Function-signature alias to make addition of creators for resources simple.
             */
            template <typename TResource>
            using CreatorFn_t = std::function<CEngineResult<>(
                    typename TResource::CCreationRequest const &,
                    std::string                          const &,
                    bool)>;

            /**
             * Function-signature alias to make addition of loaders for resources simple.
             */
            template <typename TResource>
            using LoadFn_t = std::function<CEngineResult<>(std::string const &)>;

            /**
             * Function-signature alias to make addition of updaters for resources simple.
             */
            template <typename TResource>
            using UpdateFn_t = std::function<CEngineResult<>(
                    std::string                        const &,
                    typename TResource::CUpdateRequest const &)>;

            /**
             * Function-signature alias to make addition of unloaders for resources simple.
             */
            template <typename TResource>
            using UnloadFn_t = std::function<CEngineResult<>(std::string const &)>;

            /**
             * Function-signature alias to make addition of destroyers for resources simple.
             */
            template <typename TResource>
            using DestroyFn_t = std::function<CEngineResult<>(std::string const &)>;

        protected_constructors:
            /**
             * Create a new resource manager, with a resource proxy factory required
             * for the proxy resource management to work.
             *
             * @param aProxyFactory A point to a resource factory.
             */
            CResourceManagerBase(CStdSharedPtr_t<CResourceProxyFactory> const &aProxyFactory);

        protected_methods:
            /**
             * Set/Overwrite the creator for the resource type TResource.
             *
             * @tparam TResource The resource type to register a creator for.
             * @param aCreator   The creator to overwrite with.
             * @return           True, if successful. False on error.
             */
            template <typename TResource>
            CEngineResult<> addCreator(CreatorFn_t<TResource> const &aCreator);

            /**
             * Set/Overwrite the loader for the resource type TResource.
             *
             * @tparam TResource The resource type to register a loader for.
             * @param aLoader    The loader to overwrite with.
             * @return           True, if successful. False on error.
             */
            template <typename TResource>
            CEngineResult<> addLoader(LoadFn_t<TResource> const &aLoader);

            /**
             * Set/Overwrite the updater for the resource type TResource.
             *
             * @tparam TResource The resource type to register an updater for.
             * @param aUpdater   The updater to overwrite with.
             * @return           True, if successful. False on error.
             */
            template <typename TResource>
            CEngineResult<> addUpdater(UpdateFn_t<TResource> const &aUpdater);

            /**
             * Set/Overwrite the unloader for the resource type TResource.
             *
             * @tparam TResource The resource type to register an unloader for.
             * @param aUnloader  The unloader to overwrite with.
             * @return           True, if successful. False on error.
             */
            template <typename TResource>
            CEngineResult<> addUnloader(UnloadFn_t<TResource> const &aUnloader);

            /**
             * Set/Overwrite the destructor for the resource type TResource.
             *
             * @tparam TResource  The resource type to register a destructor for.
             * @param aDestructor The destructor to overwrite with.
             * @return            True, if successful. False on error.
             */
            template <typename TResource>
            CEngineResult<> addDestructor(DestroyFn_t<TResource> const &aDestructor);

            /**
             * Create a new resource from a creation request and a human readable
             * resource id.
             * Optionally, the creation can be deferred to an explicit call to loadResource.
             *
             * @tparam TResource       The underlying resource type create.
             * @param aRequest         The creation request containing at least a resource descriptor.
             * @param aResourceId      The human readable resource id, by which the resource can be accessed.
             * @param creationDeferred A flag indicating immediate or deferred resource loading in it's backednd.
             * @return                 EEngineStatus::Ok, if successful. Any error code otherwise.
             */
            template <typename TResource>
            CEngineResult<> createResourceImpl(
                    typename TResource::CCreationRequest const &aRequest,
                    std::string                          const &aResourceId,
                    bool                                        aCreationDeferred = false);

            /**
             * Load a resource in it's respective backend, if not done so yet.
             *
             * @tparam TResource  The underlying resource type load.
             * @param aResourceId The resource id of the resource to be loaded.
             * @return            EEngineStatus::Ok, if successful. Any error code otherwise.
             */
            template <typename TResource>
            CEngineResult<> loadResourceImpl(std::string const &aResourceId);

            /**
             * Update a resource based on the update request provided.
             *
             * @tparam TResource  The underlying resource type load.
             * @param aResourceId The resource id of the resource to be updated.
             * @param aRequest    An update request containing update data for the backend resource.
             * @return            EEngineStatus::Ok, if successful. Any error code otherwise.
             */
            template <typename TResource>
            CEngineResult<> updateResourceImpl(
                    std::string                        const &aResourceId,
                    typename TResource::CUpdateRequest const &aRequest);

            /**
             * Unload a resource in it's backend. This will not remove it from the resourcemanager.
             *
             * @tparam TResource  The underlying resource type load.
             * @param aResourceId The resource id of the resource to be unloaded.
             * @return            EEngineStatus::Ok, if successful. Any error code otherwise.
             */
            template <typename TResource>
            CEngineResult<> unloadResourceImpl(std::string const &aResourceId);

            /**
             * Destroy a resource, which will also remove it from the resource manager.
             *
             * @tparam TResource  The underlying resource type load.
             * @param aResourceId The resource id of the resource to be destroyed.
             * @return            EEngineStatus::Ok, if successful. Any error code otherwise.
             */
            template <typename TResource>
            CEngineResult<> destroyResourceImpl(std::string const &aResourceId);

        private_methods:
            /**
             * Load the dependencies and the root of the resource tree.
             * This function only deals with root elements of a resource-tree.
             *
             * @param aProxy        The resource tree root proxy object.
             * @param aDependencies A list of dependencies of the proxy.
             * @return              EEngineStatus::Ok, if successful. Any error code otherwise.
             */
            CEngineResult<> proxyLoad(
                    CStdSharedPtr_t<IResourceProxyBase>        aProxy,
                    PublicResourceIdList_t              const &aDependencies);

            /**
             * Unload the dependencies and the root of the resource tree.
             *
             * @param aProxy The resource tree root proxy object.
             * @return       EEngineStatus::Ok, if successful. Any error code otherwise.
             */
            CEngineResult<> proxyUnload(CStdSharedPtr_t<IResourceProxyBase> &aProxy);

            /**
             * Return the resource proxy for a provided resource id.
             *
             * @param aResourceId The resource id of the resource to access.
             * @return            EEngineStatus::Ok, if successful. Any error code otherwise.
             */
            SHIRABE_INLINE CEngineResult<AnyProxy> getResourceProxy(std::string const &aResourceId)
            {
                CEngineResult<AnyProxy> ref = mResources.getResource(aResourceId);
                if(ref.successful())
                {
                    return { EEngineStatus::Ok, ref.data() };
                }
                else
                {
                    return { EEngineStatus::Error };
                }
            }

            /**
             * Store a resource proxy in the internal resource pool.
             *
             * @param aResourceId The resource id of the resource proxy to store.
             * @param aProxy      The proxy object of the resource proxied.
             * @return            EEngineStatus::Ok, if successful. Any error code otherwise.
             */
            SHIRABE_INLINE CEngineResult<> storeResourceProxy(
                    std::string const &aResourceId,
                    AnyProxy    const &aProxy)
            {
                return mResources.addResource(aResourceId, aProxy);
            }

        private_members:
            random::RandomState                               mUIDGenerator;
            CStdSharedPtr_t<CResourceProxyFactory>            mProxyFactory;
            IndexedResourcePool<PublicResourceId_t, AnyProxy> mResources;            

            Map<std::type_index, Any_t> mCreatorFunctions;
            Map<std::type_index, Any_t> mLoadFunctions;
            Map<std::type_index, Any_t> mUpdateFunctions;
            Map<std::type_index, Any_t> mUnloadFunctions;
            Map<std::type_index, Any_t> mDestructorFunctions;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<> CResourceManagerBase::createResource(
                typename TResource::CCreationRequest const &aRequest,
                std::string                          const &aResourceId,
                bool                                        aCreationDeferred)
        {
            std::type_index const typeIndex = std::type_index(typeid(TResource));
            if(mCreatorFunctions.end() == mCreatorFunctions.find(typeIndex))
            {
                return EEngineStatus::Error;
            }

            CreatorFn_t<TResource> fn = std::any_cast<CreatorFn_t<TResource>>(mCreatorFunctions[typeIndex]);

            CEngineResult<> const status = fn(aRequest, aResourceId, aCreationDeferred);
            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<> CResourceManagerBase::loadResource(std::string const &aResourceId)
        {
            std::type_index const typeIndex = std::type_index(typeid(TResource));
            if(mLoadFunctions.end() == mLoadFunctions.find(typeIndex))
            {
                return EEngineStatus::Error;
            }

            LoadFn_t<TResource> fn = std::any_cast<LoadFn_t<TResource>>(mLoadFunctions[typeIndex]);

            CEngineResult<> const status = fn(aResourceId);
            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<> CResourceManagerBase::updateResource(
                std::string                        const &aResourceId,
                typename TResource::CUpdateRequest const &aRequest)
        {
            std::type_index const typeIndex = std::type_index(typeid(TResource));
            if(mUpdateFunctions.end() == mUpdateFunctions.find(typeIndex))
            {
                return EEngineStatus::Error;
            }

            UpdateFn_t<TResource> fn = std::any_cast<UpdateFn_t<TResource>>(mUpdateFunctions[typeIndex]);

            CEngineResult<> const status = fn(aResourceId, aRequest);
            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<> CResourceManagerBase::unloadResource(std::string const &aResourceId)
        {
            std::type_index const typeIndex = std::type_index(typeid(TResource));
            if(mUnloadFunctions.end() == mUnloadFunctions.find(typeIndex))
            {
                return EEngineStatus::Error;
            }

            UnloadFn_t<TResource> fn = std::any_cast<UnloadFn_t<TResource>>(mUnloadFunctions[typeIndex]);

            CEngineResult<> const status = fn(aResourceId);
            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<> CResourceManagerBase::destroyResource(std::string const &aResourceId)
        {
            std::type_index const typeIndex = std::type_index(typeid(TResource));
            if(mDestructorFunctions.end() == mDestructorFunctions.find(typeIndex))
            {
                return EEngineStatus::Error;
            }

            DestroyFn_t<TResource> fn = std::any_cast<DestroyFn_t<TResource>>(mDestructorFunctions[typeIndex]);

            CEngineResult<> const status = fn(aResourceId);
            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<> CResourceManagerBase::createResourceImpl(
                typename TResource::CCreationRequest const &aRequest,
                std::string                          const &aResourceId,
                bool                                        aCreationDeferred)
        {
            typename TResource::SDescriptor const& desc = aRequest.resourceDescriptor();

            CEngineResult<AnyProxy> resourceProxyFetch = getResourceProxy(aResourceId);
            if(not resourceProxyFetch.successful())
            {
                CStdSharedPtr_t<IResourceProxy<TResource>> proxy = mProxyFactory->create<TResource>(EProxyType::Dynamic, aRequest);
                if(not proxy)
                {
                    EngineStatusPrintOnError(
                                EEngineStatus::Error,
                                logTag(),
                                "Failed to create proxy for resource.");

                    return EEngineStatus::Error;
                }

                CEngineResult<> storage = storeResourceProxy(aResourceId, AnyProxy(proxy));
                if(not storage.successful())
                {
                    EngineStatusPrintOnError(
                                EEngineStatus::ResourceManager_ProxyCreationFailed,
                                logTag(),
                                "Failed to store resource proxy.");

                    return EEngineStatus::Error;
                }
            }

            // If creation is not deferred, immediately load the resources using the proxy.
            CEngineResult<> const status = loadResource<TResource>(aResourceId);
            if(not aCreationDeferred)
            {
                EngineStatusPrintOnError(status.result(), logTag(), "Failed to load resource");
            }

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<> CResourceManagerBase::loadResourceImpl(std::string const &aResourceId)
        {
            CEngineResult<AnyProxy> resourceProxyFetch = getResourceProxy(aResourceId);
            if(not resourceProxyFetch.successful())
            {
                EngineStatusPrintOnError(EEngineStatus::Error, logTag(), "Cannot find resource proxy.");
            }

            CStdSharedPtr_t<IResourceProxyBase>           baseProxy    = baseProxyCast(resourceProxyFetch.data());
            CStdSharedPtr_t<CGenericProxyBase<TResource>> genericProxy = genericProxyBaseCast<TResource>(resourceProxyFetch.data());
            if(not genericProxy)
            {
                EngineStatusPrintOnError(EEngineStatus::Error, logTag(), "Proxy is not a GenericResourceProxy");
            }

            typename TResource::CCreationRequest const &creator = genericProxy->creationRequest();
            typename TResource::SDescriptor      const &desc    = creator.resourceDescriptor();

            PublicResourceIdList_t dependencies = desc.dependencies;
            if(not dependencies.empty())
            {
                for(PublicResourceId_t const &dependencyId : dependencies)
                {
                    CEngineResult<AnyProxy> dependencyProxyFetch = this->getResourceProxy(dependencyId);
                    if(not dependencyProxyFetch.successful())
                    {
                        EngineStatusPrintOnError(EEngineStatus::Error, logTag(), "Dependency not registered.");
                        continue;
                    }

                    CStdSharedPtr_t<IResourceProxyBase> base = baseProxyCast(dependencyProxyFetch.data());
                    if(not (ELoadState::LOADED == base->loadState()))
                    {
                        while(ELoadState::LOADING == base->loadState())
                        {
                            std::this_thread::sleep_for(std::chrono::microseconds(100));
                        }

                        // TODO:
                        //   Even more state handling is required here.. what if the resource is currently being unloaded?
                        //   Or unavailable?

                        CEngineResult<> const load = proxyLoad(base, {});
                        EngineStatusPrintOnError(
                                    load.result(),
                                    logTag(),
                                    "Failed to load dependency proxy underlying resource.");

                        // Recheck for availability?
                        //if(base->loadState() != ELoadState::LOADED)
                        //  HandleEngineStatusError(EEngineStatus::Error, "Resource loading failed despite previous attempt!");
                    }
                }
            }

            if(not (ELoadState::LOADED == baseProxy->loadState()))
            {
                while(ELoadState::LOADING == baseProxy->loadState())
                {
                    std::this_thread::sleep_for(std::chrono::microseconds(100));
                }

                if(ELoadState::LOADED != baseProxy->loadState())
                {
                    CEngineResult<> const load = proxyLoad(baseProxy, dependencies);
                    EngineStatusPrintOnError(load.result(), logTag(), "Failed to load resource proxy ");
                }
            }

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<> CResourceManagerBase::updateResourceImpl(
                std::string                        const &aResourceId,
                typename TResource::CUpdateRequest const &aRequest)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<> CResourceManagerBase::unloadResourceImpl(std::string const &aResourceId)
        {
            CEngineResult<AnyProxy> resourceProxyFetch = getResourceProxy(aResourceId);
            if(not resourceProxyFetch.successful())
            {
                EngineStatusPrintOnError(EEngineStatus::Error, logTag(), "Cannot find resource proxy.");
                return resourceProxyFetch.result();
            }

            CStdSharedPtr_t<IResourceProxyBase> &base = baseProxyCast(resourceProxyFetch.data());

            // TODO: Unload all dependers if available...
            CEngineResult<> const status = proxyUnload(base);
            EngineStatusPrintOnError(
                        status.result(),
                        logTag(),
                        CString::format("Failed to unload proxy resource in backend (Id: %0).", aResourceId));

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<> CResourceManagerBase::destroyResourceImpl(std::string const &aResourceId)
        {
            CEngineResult<> unload = EEngineStatus::Ok;

            auto const eraseFn
                    = [&, this] () -> CEngineResult<>
            {
                CEngineResult<> const removal = mResources.removeResource(aResourceId);
                if(not removal.successful())
                {
                    EngineStatusPrintOnError(EEngineStatus::Error, logTag(), "Failed to remove resource from internal registry.");

                    return { EEngineStatus::Error };

                }

                return { EEngineStatus::Ok };
            };

            try
            {
                unload = unloadResource<TResource>(aResourceId);
                unload = eraseFn();
            }
            catch(std::exception e)
            {
                CLog::Error(logTag(), CString::format("Error while trying to destroy a resource in the resource manager. Error: %0", e.what()));                
                unload = eraseFn();
            }
            catch(...)
            {
                CLog::Error(logTag(), CString::format("Unknown Error while trying to destroy a resource in the resource manager."));                
                unload = eraseFn();
            }

            // Any further verification, e.g. with the backend to ensure proper deletion?
            return unload;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource, typename TFunction>
        CEngineResult<> addFunctor(TFunction const &aFunction, Map<std::type_index, Any_t> &aRegistry)
        {
            std::type_index const typeIndex = std::type_index(typeid(TResource));
            if(aRegistry.end() != aRegistry.find(typeIndex))
            {
                return { EEngineStatus::Error };
            }

            aRegistry[typeIndex] = aFunction;
            return { EEngineStatus::Ok };
        }

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<> CResourceManagerBase::addCreator(CreatorFn_t<TResource> const &aFunction)
        {
            return addFunctor<TResource, CreatorFn_t<TResource>>(aFunction, mCreatorFunctions);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<> CResourceManagerBase::addLoader(LoadFn_t<TResource> const &aFunction)
        {
            return addFunctor<TResource, LoadFn_t<TResource>>(aFunction, mLoadFunctions);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<> CResourceManagerBase::addUpdater(UpdateFn_t<TResource> const &aFunction)
        {
            return addFunctor<TResource, UpdateFn_t<TResource>>(aFunction, mUpdateFunctions);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<> CResourceManagerBase::addUnloader(LoadFn_t<TResource> const &aFunction)
        {            
            return addFunctor<TResource, LoadFn_t<TResource>>(aFunction, mUnloadFunctions);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<> CResourceManagerBase::addDestructor(DestroyFn_t<TResource> const &aFunction)
        {
            return addFunctor<TResource, DestroyFn_t<TResource>>(aFunction, mDestructorFunctions);
        }
        //<-----------------------------------------------------------------------------
    }
}

#endif
