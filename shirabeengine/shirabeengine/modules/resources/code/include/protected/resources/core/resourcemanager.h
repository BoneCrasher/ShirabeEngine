#ifndef __SHIRABE_PROXYBASEDRESOURCEMANAGER_H__
#define __SHIRABE_PROXYBASEDRESOURCEMANAGER_H__

#include <thread>

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
      SHIRABE_DECLARE_LOG_TAG(ResourceManager);

    public:
      ResourceManager(
        const CStdSharedPtr_t<ResourceProxyFactory> &proxyFactory)
        : m_proxyFactory(proxyFactory)
      {}

      virtual ~ResourceManager() {
        // m_resources->clear();
        m_proxyFactory = nullptr;
      };

      bool clear();

      /*void setResourceBackend(CStdSharedPtr_t<GFXAPIResourceBackend> const& backend) {
        assert(backend != nullptr);

        m_resourceBackend = backend;
      }*/
      
      template <typename TResource>
      EEngineStatus createResource(
        typename TResource::CreationRequest const&request,
        std::string                         const&resourceId,
        bool                                      creationDeferred = false);

      template <typename TResource>
      EEngineStatus loadResource(
        std::string const&resourceId);

      template <typename TResource>
      EEngineStatus updateResource(
        std::string                       const&resourceId,
        typename TResource::UpdateRequest const&request);

      template <typename TResource>
      EEngineStatus unloadResource(
        std::string const&resourceId);

      template <typename TResource>
      EEngineStatus destroyResource(
        std::string const&resourceId);

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
      EEngineStatus proxyLoad(CStdSharedPtr_t<IResourceProxyBase> &proxy, PublicResourceIdList const&dependencies);

      /**********************************************************************************************//**
       * \fn  EEngineStatus ResourceManager::proxyUnload(const ResourceHandle& handle);
       *
       * \brief Proxy unload
       *
       * \param handle  The handle.
       *
       * \return  The EEngineStatus.
       **************************************************************************************************/
      EEngineStatus proxyUnload(CStdSharedPtr_t<IResourceProxyBase> &proxy);

      inline AnyProxy getResourceProxy(std::string const& id) {
        return m_resources.getResource(id);
      }

      inline bool storeResourceProxy(
        std::string const&id,
        AnyProxy    const&proxy)
      {
        return m_resources.addResource(id, proxy);
      }

      Random::RandomState m_idGenerator;

      CStdSharedPtr_t<ResourceProxyFactory>  m_proxyFactory;
      // CStdSharedPtr_t<GFXAPIResourceBackend> m_resourceBackend;

      // Any kind of resources, abstracted away entirely.
      IndexedResourcePool<PublicResourceId_t, AnyProxy> m_resources;
    };
    DeclareSharedPointerType(ResourceManager);

    /**********************************************************************************************//**
     * \fn	template <typename TBuilder> EEngineStatus ResourceManager::createResource( const typename TBuilder::traits_type::descriptor_type &desc, bool creationDeferred, CStdSharedPtr_t<typename TBuilder::proxy> &outProxy, std::vector<ResourceHandle> &outHandles )
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
        std::string                         const&resourceId,
        bool                                      creationDeferred)
    {
      typename TResource::Descriptor const& desc = request.resourceDescriptor();

      AnyProxy resourceProxy = getResourceProxy(resourceId);
      if(!resourceProxy) {
        CStdSharedPtr_t<IResourceProxy<TResource>> proxy
          = m_proxyFactory->create<TResource>(EProxyType::Dynamic, request);
        if(!proxy)
          HandleEngineStatusError(EEngineStatus::Error, "Failed to create proxy for resource.");

        if(!storeResourceProxy(resourceId, AnyProxy(proxy)))
          HandleEngineStatusError(EEngineStatus::ResourceManager_ProxyCreationFailed, "Failed to store resource proxy.");
      }

      // If creation is not deferred, immediately load the resources using the proxy.
      if(!creationDeferred)
        HandleEngineStatusError(loadResource<TResource>(resourceId), "Failed to load resource");

      return EEngineStatus::Ok;
    }

    template <typename TResource>
    EEngineStatus
      ResourceManager
      ::loadResource(
        std::string const&resourceId)
    {
      AnyProxy resourceProxy = getResourceProxy(resourceId);
      if(!resourceProxy)
        HandleEngineStatusError(EEngineStatus::Error, "Cannot find resource proxy.");

      IResourceProxyBasePtr baseProxy = BaseProxyCast(resourceProxy);

      CStdSharedPtr_t<GenericProxyBase<TResource>> genericProxy = GenericProxyBaseCast<TResource>(resourceProxy);
      if(!genericProxy)
        HandleEngineStatusError(EEngineStatus::Error, "Proxy is not a GenericResourceProxy");

      typename TResource::CreationRequest const&creator = genericProxy->creationRequest();
      typename TResource::Descriptor      const&desc    = creator.resourceDescriptor();

      PublicResourceIdList dependencies = desc.dependencies;
      if(!dependencies.empty()) {
        for(PublicResourceId_t const&dependencyId : dependencies) {
          AnyProxy dependencyProxy = this->getResourceProxy(dependencyId);
          if(!dependencyProxy)
            // throw EngineException(EEngineStatus::Error, "Dependency not registered.");
            continue;

          IResourceProxyBasePtr base = BaseProxyCast(dependencyProxy);
          if(!(base->loadState() == ELoadState::LOADED)) {
            while(base->loadState() == ELoadState::LOADING)
              std::this_thread::sleep_for(std::chrono::microseconds(100));

            // TODO:
            //   Even more state handling is required here.. what if the resource is currently being unloaded?
            //   Or unavailable?

            PublicResourceIdList placeholder={};
            HandleEngineStatusError(proxyLoad(base, placeholder), "Failed to load dependency proxy underlying resource.");

            // Recheck for availability?
            //if(base->loadState() != ELoadState::LOADED)
            //  HandleEngineStatusError(EEngineStatus::Error, "Resource loading failed despite previous attempt!");
          }
        }
      }

      if(!(baseProxy->loadState() == ELoadState::LOADED)) {
        while(baseProxy->loadState() == ELoadState::LOADING)
          std::this_thread::sleep_for(std::chrono::microseconds(100));

        if(baseProxy->loadState() != ELoadState::LOADED)
          HandleEngineStatusError(proxyLoad(baseProxy, dependencies), "Failed to load resource proxy ");
      }

      return EEngineStatus::Ok;
    }

    template <typename TResource>
    EEngineStatus ResourceManager
      ::updateResource(
        std::string                       const&resourceId,
        typename TResource::UpdateRequest const&request)
    {
      return EEngineStatus::Ok;
    }

    template <typename TResource>
    EEngineStatus
      ResourceManager
      ::unloadResource(
        std::string const&resourceId)
    {
      AnyProxy resourceProxy = getResourceProxy(resourceId);
      if(!resourceProxy)
        HandleEngineStatusError(EEngineStatus::Error, "Cannot find resource proxy.");

      IResourceProxyBasePtr base = BaseProxyCast(resourceProxy);

      // TODO: Unload all dependers if available...
      return HandleEngineStatusError(proxyUnload(base), String::format("Failed to unload proxy resource in backend (Id: %0).", resourceId));
    }

    template <typename TResource>
    EEngineStatus ResourceManager
      ::destroyResource(
        std::string const&resourceId)
    {
      EEngineStatus status = EEngineStatus::Ok;

      std::function<EEngineStatus()> eraseFn
        = [&, this] () -> EEngineStatus
      {
        if(!m_resources.removeResource(resourceId))
          return HandleEngineStatusError(EEngineStatus::Error, "Failed to remove resource from internal registry.");

        return EEngineStatus::Ok;
      };

      try {
        status = unloadResource<TResource>(resourceId);
        status = eraseFn();
      }
      catch(EngineException ee) {
        eraseFn();
        throw;
      }
      catch(std::exception e) {
        eraseFn();
        throw;
      }
      catch(...) {
        eraseFn();
        throw;
      }

      // Any further verification, e.g. with the backend to ensure proper deletion?
      return status;
    }

  }
}

#endif