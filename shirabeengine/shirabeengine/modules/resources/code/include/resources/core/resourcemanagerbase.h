#ifndef __SHIRABE_RESOURCEMANAGER_H__
#define __SHIRABE_RESOURCEMANAGER_H__

#include <thread>
#include <typeindex>

#include <log/log.h>
#include <platform/platform.h>

#include <core/enginetypehelper.h>
#include <core/random.h>
#include <core/string.h>

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
            virtual EEngineStatus initialize();

            /**
             * Deinitialize the resource manager and free..
             *
             * @return EEngineStatus::Ok on success. An error code otherwise.
             */
            virtual EEngineStatus deinitialize();

            /**
             * Unload, destroy and remove all stored resources.
             *
             * @return
             */
            bool clear();

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
            EEngineStatus createResource(
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
            EEngineStatus loadResource(std::string const &aResourceId);

            /**
             * Update a resource based on the update request provided.
             *
             * @tparam TResource  The underlying resource type load.
             * @param aResourceId The resource id of the resource to be updated.
             * @param aRequest    An update request containing update data for the backend resource.
             * @return            EEngineStatus::Ok, if successful. Any error code otherwise.
             */
            template <typename TResource>
            EEngineStatus updateResource(
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
            EEngineStatus unloadResource(std::string const &aResourceId);

            /**
             * Destroy a resource, which will also remove it from the resource manager.
             *
             * @tparam TResource  The underlying resource type load.
             * @param aResourceId The resource id of the resource to be destroyed.
             * @return            EEngineStatus::Ok, if successful. Any error code otherwise.
             */
            template <typename TResource>
            EEngineStatus destroyResource(std::string const &aResourceId);

        protected_typedefs:
            /**
             * Function-signature alias to make addition of creators for resources simple.
             */
            template <typename TResource>
            using CreatorFn_t = std::function<EEngineStatus(
                    typename TResource::CCreationRequest const &,
                    std::string                          const &,
                    bool)>;

            /**
             * Function-signature alias to make addition of loaders for resources simple.
             */
            template <typename TResource>
            using LoadFn_t = std::function<EEngineStatus(std::string const &)>;

            /**
             * Function-signature alias to make addition of updaters for resources simple.
             */
            template <typename TResource>
            using UpdateFn_t = std::function<EEngineStatus(
                    std::string                        const &,
                    typename TResource::CUpdateRequest const &)>;

            /**
             * Function-signature alias to make addition of unloaders for resources simple.
             */
            template <typename TResource>
            using UnloadFn_t = std::function<EEngineStatus(std::string const &)>;

            /**
             * Function-signature alias to make addition of destroyers for resources simple.
             */
            template <typename TResource>
            using DestroyFn_t = std::function<EEngineStatus(std::string const &)>;

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
            bool addCreator(CreatorFn_t<TResource> const &aCreator);

            /**
             * Set/Overwrite the loader for the resource type TResource.
             *
             * @tparam TResource The resource type to register a loader for.
             * @param aLoader    The loader to overwrite with.
             * @return           True, if successful. False on error.
             */
            template <typename TResource>
            bool addLoader(LoadFn_t<TResource> const &aLoader);

            /**
             * Set/Overwrite the updater for the resource type TResource.
             *
             * @tparam TResource The resource type to register an updater for.
             * @param aUpdater   The updater to overwrite with.
             * @return           True, if successful. False on error.
             */
            template <typename TResource>
            bool addUpdater(UpdateFn_t<TResource> const &aUpdater);

            /**
             * Set/Overwrite the unloader for the resource type TResource.
             *
             * @tparam TResource The resource type to register an unloader for.
             * @param aUnloader  The unloader to overwrite with.
             * @return           True, if successful. False on error.
             */
            template <typename TResource>
            bool addUnloader(UnloadFn_t<TResource> const &aUnloader);

            /**
             * Set/Overwrite the destructor for the resource type TResource.
             *
             * @tparam TResource  The resource type to register a destructor for.
             * @param aDestructor The destructor to overwrite with.
             * @return            True, if successful. False on error.
             */
            template <typename TResource>
            bool addDestructor(DestroyFn_t<TResource> const &aDestructor);

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
            EEngineStatus createResourceImpl(
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
            EEngineStatus loadResourceImpl(std::string const &aResourceId);

            /**
             * Update a resource based on the update request provided.
             *
             * @tparam TResource  The underlying resource type load.
             * @param aResourceId The resource id of the resource to be updated.
             * @param aRequest    An update request containing update data for the backend resource.
             * @return            EEngineStatus::Ok, if successful. Any error code otherwise.
             */
            template <typename TResource>
            EEngineStatus updateResourceImpl(
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
            EEngineStatus unloadResourceImpl(std::string const &aResourceId);

            /**
             * Destroy a resource, which will also remove it from the resource manager.
             *
             * @tparam TResource  The underlying resource type load.
             * @param aResourceId The resource id of the resource to be destroyed.
             * @return            EEngineStatus::Ok, if successful. Any error code otherwise.
             */
            template <typename TResource>
            EEngineStatus destroyResourceImpl(std::string const &aResourceId);

        private_methods:
            /**
             * Load the dependencies and the root of the resource tree.
             * This function only deals with root elements of a resource-tree.
             *
             * @param aProxy        The resource tree root proxy object.
             * @param aDependencies A list of dependencies of the proxy.
             * @return              EEngineStatus::Ok, if successful. Any error code otherwise.
             */
            EEngineStatus proxyLoad(
                    CStdSharedPtr_t<IResourceProxyBase>        aProxy,
                    PublicResourceIdList_t              const &aDependencies);

            /**
             * Unload the dependencies and the root of the resource tree.
             *
             * @param aProxy The resource tree root proxy object.
             * @return       EEngineStatus::Ok, if successful. Any error code otherwise.
             */
            EEngineStatus proxyUnload(CStdSharedPtr_t<IResourceProxyBase> &aProxy);

            /**
             * Return the resource proxy for a provided resource id.
             *
             * @param aResourceId The resource id of the resource to access.
             * @return            EEngineStatus::Ok, if successful. Any error code otherwise.
             */
            SHIRABE_INLINE AnyProxy getResourceProxy(std::string const &aResourceId)
            {

                AnyProxy proxy = nullptr;

                OptionalRef_t<AnyProxy> ref = mResources.getResource(aResourceId);
                if(ref.has_value())
                {
                    proxy = ref.value().get();
                }

                return proxy;
            }

            /**
             * Store a resource proxy in the internal resource pool.
             *
             * @param aResourceId The resource id of the resource proxy to store.
             * @param aProxy      The proxy object of the resource proxied.
             * @return            EEngineStatus::Ok, if successful. Any error code otherwise.
             */
            SHIRABE_INLINE bool storeResourceProxy(
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
        EEngineStatus CResourceManagerBase::createResource(
                typename TResource::CCreationRequest const &aRequest,
                std::string                          const &aResourceId,
                bool                                        aCreationDeferred)
        {
            std::type_index const typeIndex = std::type_index(typeid(TResource));
            if(mCreatorFunctions.find(typeIndex) == mCreatorFunctions.end())
                return EEngineStatus::Error;

            CreatorFn_t<TResource> fn = std::any_cast<CreatorFn_t<TResource>>(mCreatorFunctions[typeIndex]);

            EEngineStatus const status = fn(aRequest, aResourceId, aCreationDeferred);
            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        EEngineStatus CResourceManagerBase::loadResource(std::string const &aResourceId)
        {
            std::type_index const typeIndex = std::type_index(typeid(TResource));
            if(mLoadFunctions.find(typeIndex) == mLoadFunctions.end())
                return EEngineStatus::Error;

            LoadFn_t<TResource> fn = std::any_cast<LoadFn_t<TResource>>(mLoadFunctions[typeIndex]);

            EEngineStatus const status = fn(aResourceId);
            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        EEngineStatus CResourceManagerBase::updateResource(
                std::string                        const &aResourceId,
                typename TResource::CUpdateRequest const &aRequest)
        {
            std::type_index const typeIndex = std::type_index(typeid(TResource));
            if(mUpdateFunctions.find(typeIndex) == mUpdateFunctions.end())
                return EEngineStatus::Error;

            UpdateFn_t<TResource> fn = std::any_cast<UpdateFn_t<TResource>>(mUpdateFunctions[typeIndex]);

            EEngineStatus const status = fn(aResourceId, aRequest);
            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        EEngineStatus CResourceManagerBase::unloadResource(std::string const &aResourceId)
        {
            std::type_index const typeIndex = std::type_index(typeid(TResource));
            if(mUnloadFunctions.find(typeIndex) == mUnloadFunctions.end())
                return EEngineStatus::Error;

            UnloadFn_t<TResource> fn = std::any_cast<UnloadFn_t<TResource>>(mUnloadFunctions[typeIndex]);

            EEngineStatus const status = fn(aResourceId);
            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        EEngineStatus CResourceManagerBase::destroyResource(std::string const &aResourceId)
        {
            std::type_index const typeIndex = std::type_index(typeid(TResource));
            if(mDestructorFunctions.find(typeIndex) == mDestructorFunctions.end())
                return EEngineStatus::Error;

            DestroyFn_t<TResource> fn = std::any_cast<DestroyFn_t<TResource>>(mDestructorFunctions[typeIndex]);

            EEngineStatus const status = fn(aResourceId);
            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        EEngineStatus CResourceManagerBase::createResourceImpl(
                typename TResource::CCreationRequest const &aRequest,
                std::string                          const &aResourceId,
                bool                                        aCreationDeferred)
        {
            typename TResource::SDescriptor const& desc = aRequest.resourceDescriptor();

            AnyProxy resourceProxy = getResourceProxy(aResourceId);
            if(!resourceProxy)
            {
                CStdSharedPtr_t<IResourceProxy<TResource>> proxy
                        = mProxyFactory->create<TResource>(EProxyType::Dynamic, aRequest);
                if(!proxy)
                    HandleEngineStatusError(
                                EEngineStatus::Error,
                                "Failed to create proxy for resource.");

                bool const storageSuccessful = storeResourceProxy(aResourceId, AnyProxy(proxy));
                if(!storageSuccessful)
                    HandleEngineStatusError(
                                EEngineStatus::ResourceManager_ProxyCreationFailed,
                                "Failed to store resource proxy.");
            }

            // If creation is not deferred, immediately load the resources using the proxy.
            EEngineStatus const status = loadResource<TResource>(aResourceId);
            if(!aCreationDeferred)
                HandleEngineStatusError(status, "Failed to load resource");

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        EEngineStatus CResourceManagerBase::loadResourceImpl(std::string const &aResourceId)
        {
            AnyProxy resourceProxy = getResourceProxy(aResourceId);
            if(!resourceProxy)
                HandleEngineStatusError(EEngineStatus::Error, "Cannot find resource proxy.");

            CStdSharedPtr_t<IResourceProxyBase>           baseProxy    = baseProxyCast(resourceProxy);
            CStdSharedPtr_t<CGenericProxyBase<TResource>> genericProxy = genericProxyBaseCast<TResource>(resourceProxy);
            if(!genericProxy)
                HandleEngineStatusError(EEngineStatus::Error, "Proxy is not a GenericResourceProxy");

            typename TResource::CCreationRequest const &creator = genericProxy->creationRequest();
            typename TResource::SDescriptor      const &desc    = creator.resourceDescriptor();

            PublicResourceIdList_t dependencies = desc.dependencies;
            if(!dependencies.empty())
            {
                for(PublicResourceId_t const &dependencyId : dependencies)
                {
                    AnyProxy dependencyProxy = this->getResourceProxy(dependencyId);
                    if(!dependencyProxy)
                        // throw EngineException(EEngineStatus::Error, "Dependency not registered.");
                        continue;

                    CStdSharedPtr_t<IResourceProxyBase> base = baseProxyCast(dependencyProxy);
                    if(!(ELoadState::LOADED == base->loadState()))
                    {
                        while(ELoadState::LOADING == base->loadState())
                            std::this_thread::sleep_for(std::chrono::microseconds(100));

                        // TODO:
                        //   Even more state handling is required here.. what if the resource is currently being unloaded?
                        //   Or unavailable?

                        PublicResourceIdList_t placeholder={};
                        EEngineStatus const status = proxyLoad(base, placeholder);
                        HandleEngineStatusError(
                                    status,
                                    "Failed to load dependency proxy underlying resource.");

                        // Recheck for availability?
                        //if(base->loadState() != ELoadState::LOADED)
                        //  HandleEngineStatusError(EEngineStatus::Error, "Resource loading failed despite previous attempt!");
                    }
                }
            }

            if(!(ELoadState::LOADED == baseProxy->loadState()))
            {
                while(ELoadState::LOADING == baseProxy->loadState())
                    std::this_thread::sleep_for(std::chrono::microseconds(100));

                if(ELoadState::LOADED != baseProxy->loadState())
                {
                    EEngineStatus const status = proxyLoad(baseProxy, dependencies);
                    HandleEngineStatusError(status, "Failed to load resource proxy ");
                }
            }

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        EEngineStatus CResourceManagerBase::updateResourceImpl(
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
        EEngineStatus CResourceManagerBase::unloadResourceImpl(std::string const &aResourceId)
        {
            AnyProxy resourceProxy = getResourceProxy(aResourceId);
            if(!resourceProxy)
                HandleEngineStatusError(EEngineStatus::Error, "Cannot find resource proxy.");

            CStdSharedPtr_t<IResourceProxyBase> &base = baseProxyCast(resourceProxy);

            // TODO: Unload all dependers if available...
            EEngineStatus const status = proxyUnload(base);
            HandleEngineStatusError(
                        status,
                        CString::format("Failed to unload proxy resource in backend (Id: %0).", aResourceId));
            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        EEngineStatus CResourceManagerBase::destroyResourceImpl(std::string const &aResourceId)
        {
            EEngineStatus status = EEngineStatus::Ok;

            auto const eraseFn
                    = [&, this] () -> EEngineStatus
            {
                bool const resourceRemoved = mResources.removeResource(aResourceId);
                if(!resourceRemoved)
                    return HandleEngineStatusError(EEngineStatus::Error, "Failed to remove resource from internal registry.");

                return EEngineStatus::Ok;
            };

            try
            {
                status = unloadResource<TResource>(aResourceId);
                status = eraseFn();
            }
            catch(CEngineException ee)
            {
                CLog::Error(logTag(), CString::format("Error while trying to destroy a resource in the resource manager. Error: %0, Code: %1", ee.message(), ee.status()));
                eraseFn();
            }
            catch(std::exception e)
            {
                CLog::Error(logTag(), CString::format("Error while trying to destroy a resource in the resource manager. Error: %0", e.what()));
                eraseFn();
            }
            catch(...)
            {
                CLog::Error(logTag(), CString::format("Unknown Error while trying to destroy a resource in the resource manager."));
                eraseFn();
            }

            // Any further verification, e.g. with the backend to ensure proper deletion?
            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        bool CResourceManagerBase::addCreator(CreatorFn_t<TResource> const &aFunction)
        {
            std::type_index const typeIndex = std::type_index(typeid(TResource));
            if(mCreatorFunctions.find(typeIndex) != mCreatorFunctions.end())
                return false;

            mCreatorFunctions[typeIndex] = aFunction;
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        bool CResourceManagerBase::addLoader(LoadFn_t<TResource> const &aFunction)
        {
            std::type_index const typeIndex = std::type_index(typeid(TResource));
            if(mLoadFunctions.find(typeIndex) != mLoadFunctions.end())
                return false;

            mLoadFunctions[typeIndex] = aFunction;
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        bool CResourceManagerBase::addUpdater(UpdateFn_t<TResource> const &aFunction)
        {
            std::type_index const typeIndex = std::type_index(typeid(TResource));
            if(mUpdateFunctions.find(typeIndex) != mUpdateFunctions.end())
                return false;

            mUpdateFunctions[typeIndex] = aFunction;
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        bool CResourceManagerBase::addUnloader(LoadFn_t<TResource> const &aFunction)
        {
            std::type_index const typeIndex = std::type_index(typeid(TResource));
            if(mUnloadFunctions.find(typeIndex) != mUnloadFunctions.end())
                return false;

            mUnloadFunctions[typeIndex] = aFunction;
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        bool CResourceManagerBase::addDestructor(DestroyFn_t<TResource> const &aFunction)
        {
            std::type_index const typeIndex = std::type_index(typeid(TResource));
            if(mDestructorFunctions.find(typeIndex) != mDestructorFunctions.end())
                return false;

            mDestructorFunctions[typeIndex] = aFunction;
            return true;
        }
        //<-----------------------------------------------------------------------------
    }
}

#endif
