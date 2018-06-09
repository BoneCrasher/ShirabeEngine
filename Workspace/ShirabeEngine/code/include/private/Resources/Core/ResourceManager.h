#ifndef __SHIRABE_PROXYBASEDRESOURCEMANAGER_H__
#define __SHIRABE_PROXYBASEDRESOURCEMANAGER_H__

#include "Log/Log.h"
#include "Platform/Platform.h"

#include "Core/EngineTypeHelper.h"
#include "Core/Random.h"

#include "Resources/Core/IResourceProxy.h"
#include "Resources/Core/ResourcePool.h"
#include "Resources/Core/ResourceProxyFactory.h"

namespace Engine {
  namespace Resources {

    /**********************************************************************************************//**
     * \fn	template <typename K, typename V> static bool __extractKeys(const std::map<K, V>& map, std::vector<K>& outKeys)
     *
     * \brief	Extracts the keys
     *
     * \tparam	K	Generic type parameter.
     * \tparam	V	Generic type parameter.
     * \param 		  	map	   	The map.
     * \param [in,out]	outKeys	The out keys.
     *
     * \return	True if it succeeds, false if it fails.
     **************************************************************************************************/
    template <typename K, typename V>
    static bool __extractKeys(const std::map<K, V>& map, std::vector<K>& outKeys) {
      struct FetchKey
      {
        template <typename T>
        typename T::first_type operator()(T keyValuePair) const
        {
          return keyValuePair.first;
        }
      };

      // Retrieve all keys
      std::transform(map.begin(), map.end(), back_inserter(outKeys), FetchKey());

      return true;
    }

    /**********************************************************************************************//**
     * \fn	template <typename K, typename V> static bool __extractValues(const std::map<K, V>& map, std::vector<V>& outValues)
     *
     * \brief	Extracts the values
     *
     * \tparam	K	Generic type parameter.
     * \tparam	V	Generic type parameter.
     * \param 		  	map		 	The map.
     * \param [in,out]	outValues	The out values.
     *
     * \return	True if it succeeds, false if it fails.
     **************************************************************************************************/
    template <typename K, typename V>
    static bool __extractValues(const std::map<K, V>& map, std::vector<V>& outValues) {
      struct FetchValue
      {
        template <typename T>
        typename T::first_type operator()(T keyValuePair) const
        {
          return keyValuePair.second;
        }
      };

      // Retrieve all keys
      std::transform(map.begin(), map.end(), back_inserter(outValues), FetchValue());

      return true;
    }

    /**********************************************************************************************//**
     * \class	ResourceManager
     *
     * \brief	Manager for proxy based resources.
     **************************************************************************************************/
    class SHIRABE_TEST_EXPORT ResourceManager {
      DeclareLogTag(ResourceManager);

    public:
      ResourceManager(
        const Ptr<ResourceProxyFactory> &proxyFactory)
        : m_proxyFactory(proxyFactory)
      {}

      virtual ~ResourceManager() {
        // m_resources->clear();
        m_proxyFactory = nullptr;
      };

      bool clear();

      void setResourceBackend(Ptr<GFXAPIResourceBackend> const& backend) {
        assert(backend != nullptr);

        m_resourceBackend = backend;
      }

      Ptr<GFXAPIResourceBackend>& backend() {
        return m_resourceBackend;
      }

      template <typename TResource>
      EEngineStatus createResource(
        typename TResource::CreationRequest const&request,
        PublicResourceId_t                       &outId,
        bool                                      creationDeferred = false);

      template <typename TResource>
      EEngineStatus loadResource(
        PublicResourceId_t const&id);

      template <typename TResource>
      EEngineStatus updateResource(
        PublicResourceId_t                const&inId,
        typename TResource::UpdateRequest const&request);

      template <typename TResource>
      EEngineStatus unloadResource(
        PublicResourceId_t const&id);

      template <typename TResource>
      EEngineStatus destroyResource(
        PublicResourceId_t const&inId);

    private:

      /**********************************************************************************************//**
       * \fn	EEngineStatus ResourceManager::proxyLoad(const ResourceHandle& handle, const AnyProxy& proxy)
       *
       * \brief	Load dependencies and root of resource tree. This function only deals with root
       * 			elements of a resource-tree.
       *
       * \param	handle	The handle.
       * \param	proxy 	The proxy.
       *
       * \return	The EEngineStatus.
       **************************************************************************************************/
      EEngineStatus proxyLoad(IResourceProxyBasePtr &proxy, ResourceProxyList const&dependencies);

      /**********************************************************************************************//**
       * \fn  EEngineStatus ResourceManager::proxyUnload(const ResourceHandle& handle);
       *
       * \brief Proxy unload
       *
       * \param handle  The handle.
       *
       * \return  The EEngineStatus.
       **************************************************************************************************/
      EEngineStatus proxyUnload(IResourceProxyBasePtr &proxy);

      inline AnyProxy getResourceProxy(const PublicResourceId_t& id) {
        return m_resources.getResource(id);
      }

      inline bool storeResourceProxy(
        const PublicResourceId_t &id,
        const AnyProxy           &proxy)
      {
        return m_resources.addResource(id, proxy);
      }

      Random::RandomState m_idGenerator;

      Ptr<ResourceProxyFactory>  m_proxyFactory;
      Ptr<GFXAPIResourceBackend> m_resourceBackend;

      // Any kind of resources, abstracted away entirely.
      IndexedResourcePool<PublicResourceId_t, AnyProxy> m_resources;
    };
    DeclareSharedPointerType(ResourceManager);

    /**********************************************************************************************//**
     * \fn	template <typename TBuilder> EEngineStatus ResourceManager::createResource( const typename TBuilder::traits_type::descriptor_type &desc, bool creationDeferred, Ptr<typename TBuilder::proxy> &outProxy, std::vector<ResourceHandle> &outHandles )
     *
     * \brief	Unified algorithm to invoke a specific builder and store the data in the respective
     * 			storage.
     *
     * \tparam	TBuilder	Type of the builder.
     * \param 		  	desc				The description.
     * \param 		  	creationDeferred	Handle of the out.
     * \param [in,out]	outProxy			Variable arguments providing  The arguments.
     * \param [in,out]	outHandles			The out handles.
     *
     * \return	The new resource.
     *
     **************************************************************************************************/
    template <typename TResource>
    EEngineStatus ResourceManager
      ::createResource(
        typename TResource::CreationRequest const&request,
        PublicResourceId_t                       &outId,
        bool                                      creationDeferred)
    {
      typename TResource::Descriptor const& desc = request.resourceDescriptor();

      Ptr<IResourceProxy<TResource>> proxy
        = m_proxyFactory->create<TResource>(EProxyType::Dynamic, request);
      if(!proxy)
        HandleEngineStatusError(EEngineStatus::Error, "Failed to create proxy for resource.");

      PublicResourceId_t id = m_idGenerator.generate();

      if(!storeResourceProxy(id, AnyProxy(proxy)))
        HandleEngineStatusError(EEngineStatus::ResourceManager_ProxyCreationFailed, "Failed to store resource proxy.");

      // If creation is not deferred, immediately load the resources using the proxy.
      if(!creationDeferred)
        HandleEngineStatusError(loadResource<TResource>(id), "Failed to load resource");

      outId = id;

      return EEngineStatus::Ok;
    }

    template <typename TResource>
    EEngineStatus
      ResourceManager
      ::loadResource(
        PublicResourceId_t const&id)
    {
      AnyProxy resourceProxy = getResourceProxy(id);
      if(!resourceProxy)
        HandleEngineStatusError(EEngineStatus::Error, "Cannot find resource proxy.");

      IResourceProxyBasePtr baseProxy = BaseProxyCast(resourceProxy);

      Ptr<GenericProxyBase<TResource>> genericProxy = GenericProxyBaseCast<TResource>(resourceProxy);
      if(!genericProxy)
        HandleEngineStatusError(EEngineStatus::Error, "Proxy is not a GenericResourceProxy");

      typename TResource::CreationRequest const&creator = genericProxy->creationRequest();
      typename TResource::Descriptor      const&desc    = creator.resourceDescriptor();

      ResourceProxyList    resolvedDependencies ={};
      PublicResourceIdList dependencies         = desc.dependencies;
      if(!dependencies.empty()) {
        for(PublicResourceId_t const&dependencyId : dependencies) {
          AnyProxy dependencyProxy = this->getResourceProxy(dependencyId);
          if(!dependencyProxy)
            throw EngineException(EEngineStatus::Error, "Dependency not registered.");

          IResourceProxyBasePtr base = BaseProxyCast(dependencyProxy);
          if(!(base->loadState() == ELoadState::LOADED)) {
            while(base->loadState() == ELoadState::LOADING)
              std::this_thread::sleep_for(std::chrono::microseconds(100));

            // TODO:
            //   Even more state handling is required here.. what if the resource is currently being unloaded?
            //   Or unavailable?

            ResourceProxyList placeholder={};
            HandleEngineStatusError(proxyLoad(base, placeholder), "Failed to load dependency proxy underlying resource.");

            // Recheck for availability?
            if(base->loadState() != ELoadState::LOADED)
              HandleEngineStatusError(EEngineStatus::Error, "Resource loading failed despite previous attempt!");
          }

          resolvedDependencies.push_back(dependencyProxy);
        }
      }

      HandleEngineStatusError(proxyLoad(baseProxy, resolvedDependencies), "Failed to load resource proxy ");

      return EEngineStatus::Ok;
    }

    template <typename TResource>
    EEngineStatus ResourceManager
      ::updateResource(
        PublicResourceId_t                const&inId,
        typename TResource::UpdateRequest const&request)
    {
      return EEngineStatus::Ok;
    }

    template <typename TResource>
    EEngineStatus
      ResourceManager
      ::unloadResource(
        PublicResourceId_t const&id)
    {
      AnyProxy resourceProxy = getResourceProxy(id);
      if(!resourceProxy)
        HandleEngineStatusError(EEngineStatus::Error, "Cannot find resource proxy.");

      IResourceProxyBasePtr base = BaseProxyCast(resourceProxy);

      // TODO: Unload all dependers if available...
      return HandleEngineStatusError(proxyUnload(base), String::format("Failed to unload proxy resource in backend (Id: %0).", id));
    }

    template <typename TResource>
    EEngineStatus ResourceManager
      ::destroyResource(
        PublicResourceId_t const&id)
    {
      EEngineStatus status = EEngineStatus::Ok;

      std::function<EEngineStatus()> eraseFn
        = [&, this] () -> EEngineStatus 
      {
        if(!m_resources.removeResource(id))
          return HandleEngineStatusError(EEngineStatus::Error, "Failed to remove resource from internal registry.");

        return EEngineStatus::Ok;
      };

      try {
        status = unloadResource<TResource>(id);
        status = eraseFn();
      } catch(EngineException ee) {
        eraseFn();
        throw;
      } catch(std::exception e) {
        eraseFn();
        throw;
      } catch(...) {
        eraseFn();
        throw;
      }

      // Any further verification, e.g. with the backend to ensure proper deletion?
      return status;
    }

  }
}

#endif