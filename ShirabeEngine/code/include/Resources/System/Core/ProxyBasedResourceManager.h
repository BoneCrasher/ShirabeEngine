#ifndef __SHIRABE_PROXYBASEDRESOURCEMANAGER_H__
#define __SHIRABE_PROXYBASEDRESOURCEMANAGER_H__

#include "Log/Log.h"
#include "Platform/Platform.h"
#include "Core/EngineTypeHelper.h"

#include "Resources/System/Core/IResourcePool.h"
#include "Resources/System/Core/Handle.h"
#include "Resources/System/Core/IResourceManager.h"
#include "Resources/System/Core/IResourceProxy.h"
#include "Resources/System/Core/ProxyTreeCreator.h"
#include "Resources/System/Core/ResourceProxyFactory.h"

#include "GFXAPI/ResourceProxyTreeCreators/SwapChain.h"
#include "GFXAPI/ResourceProxyTreeCreators/TextureND.h"
#include "GFXAPI/ResourceProxyTreeCreators/RenderTargetView.h"
#include "GFXAPI/ResourceProxyTreeCreators/ShaderResourceView.h"

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
     * \class	ProxyBasedResourceManager
     *
     * \brief	Manager for proxy based resources.
     **************************************************************************************************/
    class ProxyBasedResourceManager
      : public IResourceManager {
      DeclareLogTag(ProxyBasedResourceManager);

    protected:
      /**********************************************************************************************//**
       * \fn	bool ProxyBasedResourceManager::loadDependenciesRecursively(const IResourceProxyBasePtr& base, ResourceProxyMap& outDependencies)
       *
       * \brief	Loads dependencies recursively
       *
       * \param 		  	base		   	The base.
       * \param [in,out]	outDependencies	The out dependencies.
       *
       * \return	True if it succeeds, false if it fails.
       **************************************************************************************************/
      bool loadDependenciesRecursively(const IResourceProxyBasePtr& base, ResourceProxyMap& outDependencies);

      /**********************************************************************************************//**
       * \fn	EEngineStatus ProxyBasedResourceManager::proxyLoad(const ResourceHandle& handle, const AnyProxy& proxy)
       *
       * \brief	Load dependencies and root of resource tree. This function only deals with root
       * 			elements of a resource-tree.
       *
       * \param	handle	The handle.
       * \param	proxy 	The proxy.
       *
       * \return	The EEngineStatus.
       **************************************************************************************************/
      EEngineStatus proxyLoad(const ResourceHandle& handle);

      /**********************************************************************************************//**
       * \fn  bool ProxyBasedResourceManager::unloadDependenciesRecursively(const IResourceProxyBasePtr& base);
       *
       * \brief Unload dependencies recursively
       *
       * \param base  The base.
       *
       * \return  True if it succeeds, false if it fails.
       **************************************************************************************************/
      bool unloadDependenciesRecursively(const IResourceProxyBasePtr& base);

      /**********************************************************************************************//**
       * \fn  EEngineStatus ProxyBasedResourceManager::proxyUnload(const ResourceHandle& handle);
       *
       * \brief Proxy unload
       *
       * \param handle  The handle.
       *
       * \return  The EEngineStatus.
       **************************************************************************************************/
      EEngineStatus proxyUnload(const ResourceHandle& handle);

      /**********************************************************************************************//**
       * \fn	template <typename TBuilder> EEngineStatus ProxyBasedResourceManager::createResource( const typename TBuilder::traits_type::descriptor_type &desc, bool creationDeferred, Ptr<typename TBuilder::proxy> &outProxy, std::vector<ResourceHandle> &outHandles )
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
      EEngineStatus createResource(
        const typename TResource::CreationRequest &request,
        bool                                       creationDeferred,
        typename TResource::Binding               &binding
      );

      template <typename TResource>
      EEngineStatus createImpl(
        typename TResource::CreationRequest const&inRequest,
        Ptr<TResource>                           &out);

    public:
      ProxyBasedResourceManager(
        const Ptr<ResourceProxyFactory> &proxyFactory)
        : _gfxApiProxyFactory(proxyFactory)
      {}

      virtual ~ProxyBasedResourceManager() {
        // _resources->clear();
        _gfxApiProxyFactory = nullptr;
      };

      bool clear();

      void setResourceBackend(Ptr<BasicGFXAPIResourceBackend> const& backend) {
        assert(backend != nullptr);

        m_resourceBackend = backend;
      }

      Ptr<BasicGFXAPIResourceBackend>& backend() {
        return m_resourceBackend;
      }

#define DeclareCreator(Type)                    \
      EEngineStatus create##Type(               \
        Type::CreationRequest const&inRequest,  \
        Ptr<Type>                  &out##Type);

      DeclareCreator(SwapChain);
      DeclareCreator(Texture1D);
      DeclareCreator(Texture2D);
      DeclareCreator(Texture3D);
      DeclareCreator(RenderTargetView);
      DeclareCreator(ShaderResourceView);
      DeclareCreator(DepthStencilView);
      DeclareCreator(DepthStencilState);

    private:
      inline AnyProxy getResourceProxy(const ResourceHandle& handle) {
        return _resources->getResource(handle);
      }

      inline bool storeResourceProxy(
        const ResourceHandle &handle,
        const AnyProxy       &proxy)
      {
        return _resources->addResource(handle, proxy);
      }

      Ptr<ResourceProxyFactory>       _gfxApiProxyFactory;
      Ptr<BasicGFXAPIResourceBackend> m_resourceBackend;

      // Any kind of resources, abstracted away entirely.
      IIndexedResourcePoolPtr<ResourceHandle, AnyProxy> _resources;
    };
    DeclareSharedPointerType(ProxyBasedResourceManager);


    /**********************************************************************************************//**
     * \fn	bool ProxyBasedResourceManager::loadDependenciesRecursively(const IResourceProxyBasePtr& base, ResourceProxyMap& outDependencies)
     *
     * \brief	Loads dependencies recursively
     *
     * \param 		  	base		   	The base.
     * \param [in,out]	outDependencies	The out dependencies.
     *
     * \return	True if it succeeds, false if it fails.
     **************************************************************************************************/
    bool ProxyBasedResourceManager
      ::loadDependenciesRecursively(const IResourceProxyBasePtr& base, ResourceProxyMap& outDependencies)
    {
      bool result = true;

      if(!base) {
        Log::Error(logTag(), "loadDependenciesRecursively invoked with null-pointer base-proxy.");
        return false;
      }

      // Holds all local depdencies
      ResourceProxyMap inDependencies;

      ResourceHandleList dependecyHandles = base->dependencies();
      if(!dependecyHandles.empty()) {
        for(const ResourceHandle& resourceHandle : dependecyHandles) {
          // The reason all proxies have to be created by the proxy-creator and 
          // stored before this function call, can be seen below:
          AnyProxy dependencyProxy = getResourceProxy(resourceHandle);
          if(!dependencyProxy.has_value()) {
            // Even if we dealt with an internal resource not to be 
            // created here, having an empty proxy stored is a in-
            // consistent state.
            Log::Error(logTag(), String::format("Nullpointer proxy encountered in internal resource manager."));
            result |= false;
            break;
          }

          IResourceProxyBasePtr dependencyBase = BaseProxyCast(dependencyProxy);
          if(!dependencyBase) {
            // MBT TODO: How to handle this case?
          }

          // Do we even hav to load the resource? If the resource type is internal, there won't 
          // be any necessity to go deeper. Internal resources and all it's children will be created
          // without any control from our side.		
          if(dependencyBase->proxyType() == EProxyType::Internal
             || dependencyBase->proxyType() == EProxyType::Unknown)
          {
            // Nothing to do...
            continue;
          }

          // We need to handle the thing. First: load dependencies, then the resource.
          result |= loadDependenciesRecursively(dependencyBase, inDependencies);
          if(!result) {
            Log::Error(logTag(), String::format("Failed to load dependencies of proxy."));
            break;
          }

          EEngineStatus status = dependencyBase->loadSync(resourceHandle, inDependencies);
          if(CheckEngineError(status)) {
            Log::Error(logTag(), String::format("Failed to load resource of proxy."));
            result |= false;
            break;
          }

          if(dependencyBase->loadState() != ELoadState::LOADED) {
            std::string msg =
              "CRITICAL LOGIC ERROR: Resource should be loaded successfully "
              "at this point, but isn't or state is not updated properly.";
            Log::WTF(logTag(), String::format(msg));
            break;
          }

          // Yay... Finally store the thing... (and it's dependencies as they need be persisted, too)!
          outDependencies.insert(inDependencies.begin(), inDependencies.end());
          outDependencies[resourceHandle] = dependencyProxy;
        }
      }

      return result;
    }

    /**********************************************************************************************//**
     * \fn	EEngineStatus ProxyBasedResourceManager::proxyLoad(const ResourceHandle& handle, const AnyProxy& proxy)
     *
     * \brief	Load dependencies and root of resource tree. This function only deals with root
     * 			elements of a resource-tree.
     *
     * \param	handle	The handle.
     * \param	proxy 	The proxy.
     *
     * \return	The EEngineStatus.
     **************************************************************************************************/
    EEngineStatus ProxyBasedResourceManager
      ::proxyLoad(const ResourceHandle& handle)
    {
      EEngineStatus status = EEngineStatus::Ok;

      AnyProxy root = getResourceProxy(handle);
      if(!root.has_value())
        return EEngineStatus::ResourceManager_RootProxyFetchFailed;

      IResourceProxyBasePtr base = BaseProxyCast(root);
      if(!base) {
        return EEngineStatus::ResourceManager_BaseProxyCastFailed;
      }

      // Store any to-be-created resource in here to iteratively store them in the 
      // respective resource-pools and return the handle-list.
      ResourceProxyMap builtResources;

      // The resource manager has to fetch and load all dependencies first. 
      // Once done a the effective top-level builder is created and it's resource 
      // loaded.
      typename ResourceProxyMap dependencyResources;

      bool dependenciesLoadedSuccessfully = loadDependenciesRecursively(base, dependencyResources);
      if(!dependenciesLoadedSuccessfully) {
        // Flag error, since the entire child resource tree was not loaded successfully.
        // Free anything created beforehand though!
        // 
        for(ResourceProxyMap::value_type const& dep : dependencyResources)
        {
          AnyProxy proxy = dep.second;
          if(!proxy.has_value())
            continue;

          IResourceProxyBasePtr base = BaseProxyCast(proxy);
          if(!base) {
            continue;
          }

          EEngineStatus status = base->unloadSync();
          if(CheckEngineError(status)) {
            Log::Error(logTag(), "Failed to unload resource.");
            continue;
          }
        }

        Log::Error(logTag(), "Failed to load one or more dependencies of resource proxy.");
        return EEngineStatus::ResourceManager_DependencyLoadFailed;
      }

      // Finally load the root resource
      // 
      status = base->loadSync(handle, dependencyResources);
      if(CheckEngineError(status)) {
        Log::Error(logTag(), "Failed to load underlying resource of resource proxy.");
        return EEngineStatus::ResourceManager_ResourceLoadFailed;
      }

      return EEngineStatus::Ok;
    }

    bool ProxyBasedResourceManager
      ::unloadDependenciesRecursively(const IResourceProxyBasePtr& base)
    {
      bool result = true;

      if(!base) {
        Log::Error(logTag(), "loadDependenciesRecursively invoked with null-pointer base-proxy.");
        return false;
      }

      // Release dependencies first
      ResourceHandleList dependecyHandles = base->dependencies();
      if(!dependecyHandles.empty()) {
        for(const ResourceHandle& resourceHandle : dependecyHandles) {
          // The reason all proxies have to be created by the proxy-creator and 
          // stored before this function call, can be seen below:
          AnyProxy dependencyProxy = getResourceProxy(resourceHandle);
          if(!dependencyProxy.has_value()) {
            // Even if we dealt with an internal resource not to be 
            // created here, having an empty proxy stored is a in-
            // consistent state.
            Log::Error(logTag(), String::format("Nullpointer proxy encountered in internal resource manager."));
            // result |= false;
            // break;
          }

          IResourceProxyBasePtr dependencyBase = BaseProxyCast(dependencyProxy);
          if(!dependencyBase) {
            // MBT TODO: How to handle this case?
          }

          // Bottom to top release: Children first
          result |= unloadDependenciesRecursively(dependencyBase);
          if(!result) {
            Log::Error(logTag(), String::format("Failed to load dependencies of proxy."));
            break;
          }

          // Do we even hav to load the resource? If the resource type is internal, there won't 
          // be any necessity to go deeper. Internal resources and all it's children will be created
          // without any control from our side.		
          if(dependencyBase->proxyType() == EProxyType::Internal
             || dependencyBase->proxyType() == EProxyType::Unknown)
          {
            // Nothing to do...
            continue;
          }

          EEngineStatus status = dependencyBase->unloadSync();
          if(CheckEngineError(status)) {
            Log::Error(logTag(), String::format("Failed to load resource of proxy."));
            result |= false;
            break;
          }

          if(dependencyBase->loadState() != ELoadState::UNLOADED) {
            std::string msg =
              "CRITICAL LOGIC ERROR: Resource should be unloaded successfully "
              "at this point, but isn't or state is not updated properly.";
            Log::WTF(logTag(), String::format(msg));
            break;
          }
        }
      }

      return result;
    }

    EEngineStatus ProxyBasedResourceManager
      ::proxyUnload(const ResourceHandle& handle)
    {
      EEngineStatus status = EEngineStatus::Ok;

      AnyProxy root = getResourceProxy(handle);
      if(!root.has_value())
        return EEngineStatus::ResourceManager_RootProxyFetchFailed;

      IResourceProxyBasePtr base = BaseProxyCast(root);
      if(!base) {
        return EEngineStatus::ResourceManager_BaseProxyCastFailed;
      }

      // Store any to-be-created resource in here to iteratively store them in the 
      // respective resource-pools and return the handle-list.
      ResourceProxyMap builtResources;

      // Free all dependencies first.
      bool dependenciesUnloadedSuccessfully = unloadDependenciesRecursively(base);
      if(!dependenciesUnloadedSuccessfully) {
        Log::Error(logTag(), "Failed to unload one or more dependencies of resource proxy.");
        return EEngineStatus::ResourceManager_DependencyLoadFailed;
      }

      // Finally unload the root resource
      status = base->unloadSync();
      if(CheckEngineError(status)) {
        Log::Error(logTag(), "Failed to unload underlying resource of resource proxy.");
        return EEngineStatus::ResourceManager_ResourceLoadFailed;
      }

      return EEngineStatus::Ok;
    }

    /**********************************************************************************************//**
     * \fn	template <typename TBuilder> EEngineStatus ProxyBasedResourceManager::createResource( const typename TBuilder::traits_type::descriptor_type &desc, bool creationDeferred, Ptr<typename TBuilder::proxy> &outProxy, std::vector<ResourceHandle> &outHandles )
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
    EEngineStatus ProxyBasedResourceManager
      ::createResource(
        const typename TResource::CreationRequest &request,
        bool                                       creationDeferred,
        typename TResource::Binding               &binding)
    {
      static const constexpr EResourceType    resource_type    = TResource::resource_type;
      static const constexpr EResourceSubType resource_subtype = TResource::resource_subtype;

      EEngineStatus status = EEngineStatus::Ok;

      //
      // Create a resource proxy, which serves as a handle to the effective
      // underlying resource.
      //
      // The proxy will be responsible for:
      // - persisting the descriptor,
      // - create proxies and hierarchy for the required resource strucutre
      // - enqueue creation tasks in the resource thread to load 
      //   underlying resource 
      // - maintain the resource load state.
      // 
      ResourceHandleList   inDependencies; // Are there dependencies required at all?

      ResourceProxyMap     outProxies;
      DependerTreeNodeList outDependerHierarchies;

      bool treeCreationSuccessful = ProxyTreeCreator<TResource>::create(_gfxApiProxyFactory, request, binding, outProxies, outDependerHierarchies);
      if(!treeCreationSuccessful) {
        Log::Error(logTag(), "Unable to create root resource proxy.");
        return EEngineStatus::ResourceManager_ProxyCreationFailed;
      }

#define SHIRABE_DEBUG
#ifdef SHIRABE_DEBUG

      // Verify returned state!
      // Hierarchy -> Proxy
      for(const DependerTreeNodeList::value_type &r : outDependerHierarchies) {
        ResourceProxyMap::const_iterator it = outProxies.find(r.resourceHandle);
        if(it == outProxies.end()) {
          Log::Error(logTag(), String::format("Hierarchy <-> ProxyList mismatch. Missing proxy for hierarchy handle: %0", r.resourceHandle.type()));
          return EEngineStatus::ResourceManager_ProxyCreationFailed; // Invalid: Proxy missing!
        }
      }

      // Proxy -> Hierarchy
      for(ResourceProxyMap::value_type const& pair : outProxies) {
        DependerTreeNodeList::const_iterator it = std::find(outDependerHierarchies.begin(), outDependerHierarchies.end(), pair.first);
        if(it == outDependerHierarchies.end()) {
          Log::Error(logTag(), String::format("ProxyList <-> Hierarchy mismatch. Missing hierarchy entry for proxy: %0", pair.first.type()));
          return EEngineStatus::ResourceManager_ProxyCreationFailed; // Invalid: Proxy missing!
        }
      }
#endif

      // Store all dependencies, roots and dependers, to have the loadProxy function 
      // work out well.
      // Important: 
      //   We will just access outProxies as below, since the ProxyTreeCreator<T>::create-call
      //   is required to return a consistent state!
      for(const DependerTreeNodeList::value_type& r : outDependerHierarchies) {
        if(!storeResourceProxy(r.resourceHandle, outProxies[r.resourceHandle])) {
          Log::Error(logTag(), "Failed to store resource proxy.");
          // TODO: Release madness
        }
      }

      // Two hierarchy scenarios:
      // 1) Single resource, multiple dependers
      // 2) Multiple resources, single depender (or single resource, multiple dependencies)
      // 
      // The problem:
      // Single-root tree's not possible with scenario 2.
      // So: The functions have to return a list of dependency-roots, having it's dependers as children.
      // Cleanup would release it's leaves iteratively und till none are left and move up one step!
      // By returning dependency roots, we can make sure that no other parent-resource refers the node.


      // If creation is not deferred, immediately load the resources using the proxy.
      if(!creationDeferred) {
        ResourceHandleList loadedDependerHierarchies;

        for(DependerTreeNodeList::value_type const& root : outDependerHierarchies) {
          if(CheckEngineError((status = proxyLoad(root.resourceHandle)))) {
            // Free already loaded hierarchies
            for(ResourceHandle const& handle : loadedDependerHierarchies)
              if(CheckEngineError((status = proxyUnload(handle)))) {
                Log::Error(logTag(), "Failed to load resources using proxy.");
                // return EEngineStatus::ResourceManager_ProxyUnloadFailed;
              }

            Log::Error(logTag(), "Failed to load resources using proxy.");
            return EEngineStatus::ResourceManager_ProxyLoadFailed;
          }

          loadedDependerHierarchies.push_back(root.resourceHandle); // For freeing already loaded hierarchies
        }
      }

      return status;
    }

    template <typename TResource>
    EEngineStatus ProxyBasedResourceManager
      ::createImpl(
        typename TResource::CreationRequest const&inRequest,
        Ptr<TResource>                           &out)
    {
      typename TResource::Binding binding ={};

      EEngineStatus status = createResource<TResource>(
        inRequest,
        false,
        binding);

      if(CheckEngineError(status)) {
        Log::Error(logTag(), "Failed to create resource.");
        return status;
      }

      const typename TResource::Descriptor& desc = inRequest.resourceDescriptor();
      out = TResource::create(desc, binding);

      return EEngineStatus::Ok;
    }

  }
}

#endif