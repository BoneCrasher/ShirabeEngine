#ifndef __SHIRABE_PROXYBASEDCResourceManager_H__
#define __SHIRABE_PROXYBASEDCResourceManager_H__

#include <thread>

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
        class SHIRABE_TEST_EXPORT CResourceManager
        {
            SHIRABE_DECLARE_LOG_TAG(CResourceManager)

        public_constructors:
            /**
             * Create a new resource manager, with a resource proxy factory required
             * for the proxy resource management to work.
             *
             * @param aProxyFactory A point to a resource factory.
             */
            CResourceManager(CStdSharedPtr_t<ResourceProxyFactory> const &aProxyFactory);

        public_destructors:
            /**
             * Destroy and run...
             */
            virtual ~CResourceManager();

       public_methods:
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
                return mResources.getResource(aResourceId);
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
            CStdSharedPtr_t<ResourceProxyFactory>             mProxyFactory;
            IndexedResourcePool<PublicResourceId_t, AnyProxy> mResources;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        EEngineStatus CResourceManager::createResource(
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
        EEngineStatus CResourceManager::loadResource(std::string const &aResourceId)
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
        EEngineStatus CResourceManager::updateResource(
                std::string                       const &aResourceId,
                typename TResource::CUpdateRequest const &aRequest)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        EEngineStatus CResourceManager::unloadResource(std::string const &aResourceId)
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
        EEngineStatus CResourceManager
        ::destroyResource(
                std::string const&resourceId)
        {
            EEngineStatus status = EEngineStatus::Ok;

            std::function<EEngineStatus()> eraseFn
                    = [&, this] () -> EEngineStatus
            {
                bool const resourceRemoved = mResources.removeResource(resourceId);
                if(!resourceRemoved)
                    return HandleEngineStatusError(EEngineStatus::Error, "Failed to remove resource from internal registry.");

                return EEngineStatus::Ok;
            };

            try
            {
                status = unloadResource<TResource>(resourceId);
                status = eraseFn();
            }
            catch(CEngineException ee)
            {
                eraseFn();
                throw;
            }
            catch(std::exception e)
            {
                eraseFn();
                throw;
            }
            catch(...)
            {
                eraseFn();
                throw;
            }

            // Any further verification, e.g. with the backend to ensure proper deletion?
            return status;
        }
        //<-----------------------------------------------------------------------------
    }
}

#endif
