#ifndef __SHIRABE_PROXYBASEDRESOURCEMANAGER_H__
#define __SHIRABE_PROXYBASEDRESOURCEMANAGER_H__

#include "Log/Log.h"
#include "Platform/Platform.h"
#include "Core/EngineTypeHelper.h"

#include "Resources/System/Core/IResourceManager.h"

#include "Resources/System/Core/IResourcePool.h"
#include "Resources/System/Core/Handle.h"
#include "Resources/System/Core/IResourceProxy.h"
#include "Resources/System/Core/ProxyTreeCreator.h"
#include "Resources/System/Core/ResourceProxyFactory.h"

#include "GFXAPI/ResourceProxyTreeCreators/SwapChain.h"
#include "GFXAPI/ResourceProxyTreeCreators/TextureND.h"
#include "GFXAPI/ResourceProxyTreeCreators/RenderTargetView.h"
#include "GFXAPI/ResourceProxyTreeCreators/ShaderResourceView.h"
#include "GFXAPI/ResourceProxyTreeCreators/DepthStencilView.h"
#include "GFXAPI/ResourceProxyTreeCreators/DepthStencilState.h"
#include "GFXAPI/ResourceProxyTreeCreators/RasterizerState.h"

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
      bool loadDependerHierarchyTopToBottom(
        DependerTreeNode             const&base,
        GFXAPIResourceHandleMap           &inResolvedDependencies,
        ResourceProxyMap                  &outDependencies);

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
      bool unloadDependerHierarchyBottomToTop(DependerTreeNode const& base);

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
        PublicResourceId_t                  const&inId,
        typename TResource::CreationRequest const&request,
        bool                                      creationDeferred,
        typename TResource::Binding              &binding);

      template <typename TResource>
      EEngineStatus updateResource(
        PublicResourceId_t                const&inId,
        typename TResource::UpdateRequest const&request);

      template <typename TResource>
      EEngineStatus destroyResource(
        PublicResourceId_t                     const&inId,
        typename TResource::DestructionRequest const&request);

      template <typename TResource>
      EEngineStatus getResourceInfo(
        PublicResourceId_t      const&id,
        ResourceInfo<TResource>      &outInfo);

      template <typename TResource>
      EEngineStatus storeResourceInfo(
        PublicResourceId_t      const&id,
        ResourceInfo<TResource> const&inInfo);

      template <typename TResource>
      EEngineStatus createImpl(
        PublicResourceId_t                  const&inId,
        typename TResource::CreationRequest const&inRequest);

      template <typename TResource>
      EEngineStatus createDynamicImpl(
        typename TResource::CreationRequest const&inRequest,
        PublicResourceId_t                       &outId);

      template <typename TResource>
      EEngineStatus updateImpl(
        PublicResourceId_t                const&inId,
        typename TResource::UpdateRequest const&inRequest);

      template <typename TResource>
      EEngineStatus destroyImpl(
        PublicResourceId_t                     const&inId,
        typename TResource::DestructionRequest const&inRequest);

      template <typename TResource>
      EEngineStatus getResourceInfoImpl(
        PublicResourceId_t      const&id,
        ResourceInfo<TResource>      &outInfo);

    public:
      ProxyBasedResourceManager(
        const Ptr<ResourceProxyFactory> &proxyFactory)
        : m_proxyFactory(proxyFactory)
      {}

      virtual ~ProxyBasedResourceManager() {
        // m_resources->clear();
        m_proxyFactory = nullptr;
      };

      bool clear();

      void setResourceBackend(Ptr<BasicGFXAPIResourceBackend> const& backend) {
        assert(backend != nullptr);

        m_resourceBackend = backend;
      }

      Ptr<BasicGFXAPIResourceBackend>& backend() {
        return m_resourceBackend;
      }

#define DeclareSupportedResource(Type)             \
      EEngineStatus create##Type(                  \
        PublicResourceId_t    const&inId,          \
        Type::CreationRequest const&inRequest);    \
                                                   \
      EEngineStatus create##Type(                  \
        Type::CreationRequest const&inRequest,     \
        PublicResourceId_t         &outId);        \
                                                   \
      EEngineStatus update##Type(                  \
        PublicResourceId_t  const&inId,            \
        Type::UpdateRequest const&inRequest);      \
                                                   \
      EEngineStatus destroy##Type(                 \
        PublicResourceId_t       const&inId,       \
        Type::DestructionRequest const&inRequest); \
                                                   \
      EEngineStatus get##Type##Info(               \
        PublicResourceId_t const&id,               \
        ResourceInfo<Type>      &outInfo);

      DeclareSupportedResource(SwapChain);
      DeclareSupportedResource(Texture1D);
      DeclareSupportedResource(Texture2D);
      DeclareSupportedResource(Texture3D);
      DeclareSupportedResource(RenderTargetView);
      DeclareSupportedResource(ShaderResourceView);
      DeclareSupportedResource(DepthStencilView);
      DeclareSupportedResource(DepthStencilState);
      DeclareSupportedResource(RasterizerState);

    private:
      inline AnyProxy getResourceProxy(const ResourceHandle& handle) {
        return m_resources->getResource(handle);
      }

      inline bool storeResourceProxy(
        const ResourceHandle &handle,
        const AnyProxy       &proxy)
      {
        return m_resources->addResource(handle, proxy);
      }

      DependerTreeNode const& findHierarchyRoot(ResourceHandle const&handle) const;

      Ptr<ResourceProxyFactory>       m_proxyFactory;
      Ptr<BasicGFXAPIResourceBackend> m_resourceBackend;

      DependerMap m_hierarchyRoots;

      // Any kind of resources, abstracted away entirely.
      IIndexedResourcePoolPtr<ResourceHandle, AnyProxy> m_resources;
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
      ::loadDependerHierarchyTopToBottom(
        DependerTreeNode             const&base,
        GFXAPIResourceHandleMap           &inOutDependencies,
        ResourceProxyMap                  &outDependencies)
    {
      bool result = true;

      // The reason all proxies have to be created by the proxy-creator and 
      // stored before this function call, can be seen below:
      AnyProxy dependencyProxy = getResourceProxy(base.resourceHandle);
      if(!dependencyProxy.has_value()) {
        // Even if we dealt with an internal resource not to be 
        // created here, having an empty proxy stored is a in-
        // consistent state.
        Log::Error(logTag(), String::format("Nullpointer proxy encountered in internal resource manager."));
        return false;
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
        return true;
      }

      EEngineStatus status = dependencyBase->loadSync(inOutDependencies);
      if(CheckEngineError(status)) {
        Log::Error(logTag(), String::format("Failed to load resource of proxy."));
        return false;
      }

      if(dependencyBase->loadState() != ELoadState::LOADED) {
        std::string msg =
          "CRITICAL LOGIC ERROR: Resource should be loaded successfully "
          "at this point, but isn't or state is not updated properly.";
        Log::WTF(logTag(), String::format(msg));
        return false;
      }

      outDependencies[base.resourceHandle]   = dependencyProxy;
      inOutDependencies[base.resourceHandle] = GFXAPIAdapterCast(dependencyProxy)->handle();

      // Top to Bottom: Children last.
      for(DependerTreeNodeList::value_type const& c : base.children)
        result = loadDependerHierarchyTopToBottom(c, inOutDependencies, outDependencies);

      if(!result)
        return result;

      return true;
    }

    DependerTreeNode const&
      ProxyBasedResourceManager::findHierarchyRoot(ResourceHandle const&handle) const {
      static DependerTreeNode gs_invalidNode = DependerTreeNode();

      for(DependerMap::value_type const& d : m_hierarchyRoots)
        if(d.first == handle)
          return d.second;

      return gs_invalidNode;
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
        HandleEngineStatusError(EEngineStatus::ResourceManager_RootProxyFetchFailed, "Cannot find resource proxy for handle: " + handle.id());

      IResourceProxyBasePtr base = BaseProxyCast(root);
      if(!base) {
        HandleEngineStatusError(EEngineStatus::ResourceManager_BaseProxyCastFailed, "Found proxy, but it doesn't seem to be convertible to IResourceProxyBasePtr. Handle: " + handle.id());
      }

      // Store any to-be-created resource in here to iteratively store them in the 
      // respective resource-pools and return the handle-list.
      ResourceProxyMap builtResources;

      // The resource manager has to fetch and load all dependencies first. 
      // Once done a the effective top-level builder is created and it's resource 
      // loaded.
      typename ResourceProxyMap dependencyResources;

      DependerTreeNode const& hierarchyRoot = findHierarchyRoot(handle);
      if(hierarchyRoot.resourceHandle == ResourceHandle::Invalid()) {
        // Error
      }

      GFXAPIResourceHandleMap dependencies={};

      bool dependenciesLoadedSuccessfully = loadDependerHierarchyTopToBottom(hierarchyRoot, dependencies, dependencyResources);
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

        std::string msg = "Failed to load one or more dependencies of resource proxy.";
        HandleEngineStatusError(EEngineStatus::ResourceManager_DependencyLoadFailed, msg);
      }

      // Finally load the root resource
      // 
      status = base->loadSync(dependencies);
      std::string msg = "Failed to load underlying resource of resource proxy.";
      HandleEngineStatusError(status, msg);

      return EEngineStatus::Ok;
    }

    bool ProxyBasedResourceManager
      ::unloadDependerHierarchyBottomToTop(DependerTreeNode const&base)
    {
      bool result = true;

      // Bottom to Top: Children first.
      for(DependerTreeNodeList::value_type const& c : base.children)
        result = unloadDependerHierarchyBottomToTop(c);

      if(!result)
        return result;

      // The reason all proxies have to be created by the proxy-creator and 
      // stored before this function call, can be seen below:
      AnyProxy dependencyProxy = getResourceProxy(base.resourceHandle);
      if(!dependencyProxy.has_value()) {
        // Even if we dealt with an internal resource not to be 
        // created here, having an empty proxy stored is a in-
        // consistent state.
        Log::Error(logTag(), String::format("Nullpointer proxy encountered in internal resource manager."));
        return false;
      }

      IResourceProxyBasePtr dependencyBase = BaseProxyCast(dependencyProxy);
      if(!dependencyBase) {
        // MBT TODO: How to handle this case?
      }

      // Do we even hav to load the resource? If the resource type is internal, there won't 
      // be any necessity to go deeper. Internal resources and all it's children will be created
      // without any control from our side.		
      if(dependencyBase->proxyType() == EProxyType::Internal      // We don't have control anyway
         || dependencyBase->proxyType() == EProxyType::Persistent // Just load, don't unload manually
         || dependencyBase->proxyType() == EProxyType::Unknown)   // We don't know what we deal with, usually invalid state, don't touch.
      {
        // Nothing to do...
        return true;
      }

      EEngineStatus status = dependencyBase->unloadSync();
      if(CheckEngineError(status)) {
        Log::Error(logTag(), String::format("Failed to load resource of proxy."));
        return false;
      }

      if(dependencyBase->loadState() != ELoadState::UNLOADED) {
        std::string msg =
          "CRITICAL LOGIC ERROR: Resource should be unloaded successfully "
          "at this point, but isn't or state is not updated properly.";
        Log::WTF(logTag(), String::format(msg));
        return false;
      }

      return true;
    }

    EEngineStatus ProxyBasedResourceManager
      ::proxyUnload(const ResourceHandle& handle)
    {
      EEngineStatus status = EEngineStatus::Ok;

      AnyProxy root = getResourceProxy(handle);
      if(!root.has_value())
        HandleEngineStatusError(EEngineStatus::ResourceManager_RootProxyFetchFailed, "Cannot find resource proxy for handle: " + handle.id());

      IResourceProxyBasePtr base = BaseProxyCast(root);
      if(!base) {
        HandleEngineStatusError(EEngineStatus::ResourceManager_BaseProxyCastFailed, "Found proxy, but it doesn't seem to be convertible to IResourceProxyBasePtr. Handle: " + handle.id());
      }

      // Store any to-be-created resource in here to iteratively store them in the 
      // respective resource-pools and return the handle-list.
      ResourceProxyMap builtResources;

      DependerTreeNode const& hierarchyRoot = findHierarchyRoot(handle);
      if(hierarchyRoot.resourceHandle == ResourceHandle::Invalid()) {
        // Error
      }

      if(!unloadDependerHierarchyBottomToTop(hierarchyRoot)) {
        HandleEngineStatusError(EEngineStatus::ResourceManager_ProxyUnloadFailed, "Failed to unload dependers first.");
      }

      // Finally unload the root resource
      status = base->unloadSync();
      if(CheckEngineError(status)) {
        HandleEngineStatusError(EEngineStatus::ResourceManager_ResourceLoadFailed, "Failed to unload underlying resource of resource proxy.");
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
        PublicResourceId_t                  const&inId,
        typename TResource::CreationRequest const&request,
        bool                                      creationDeferred,
        typename TResource::Binding              &binding)
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

      bool treeCreationSuccessful = ProxyTreeCreator<TResource>::create(m_proxyFactory, request, binding, outProxies, outDependerHierarchies);
      if(!treeCreationSuccessful) {
        Log::Error(logTag(), "Unable to create root resource proxy.");
        return EEngineStatus::ResourceManager_ProxyCreationFailed;
      }

#define SHIRABE_DEBUG
#ifdef SHIRABE_DEBUG

      try {
        std::function<void(DependerTreeNodeList const&, ResourceProxyMap const&)> fnVerifyHierarchyToProxyAvailability = nullptr;

        // Verify returned state!
        // Hierarchy -> Proxy: Are all proxies available, which are listed in the depender hierarchies?
        fnVerifyHierarchyToProxyAvailability
          = [&] (DependerTreeNodeList const&hierarchy, ResourceProxyMap const&proxies) -> void
        {
          for(const DependerTreeNodeList::value_type &r : hierarchy) {
            fnVerifyHierarchyToProxyAvailability(r.children, proxies);

            ResourceProxyMap::const_iterator it = proxies.find(r.resourceHandle);
            if(it == proxies.end()) {
              std::string msg = String::format("Hierarchy <-> ProxyList mismatch. Missing proxy for hierarchy handle: "); // , r.resourceHandle.type());
              HandleEngineStatusError(EEngineStatus::ResourceManager_ProxyCreationFailed, msg);
            }
          }
        };
        fnVerifyHierarchyToProxyAvailability(outDependerHierarchies, outProxies);

        std::function<bool(DependerTreeNodeList const&, ResourceProxyMap::value_type const&)> fnVerifyProxyToHierarchyAvailability = nullptr;

        fnVerifyProxyToHierarchyAvailability
          = [&] (DependerTreeNodeList const&hierarchy, ResourceProxyMap::value_type const&pair) -> bool
        {
          for(DependerTreeNodeList::value_type const& r : hierarchy) {
            if(fnVerifyProxyToHierarchyAvailability(r.children, pair))
              return true;

            // We have not yet found the proxy at this point, check the current level.
            return (r.resourceHandle == pair.first);
          }
        };

        for(ResourceProxyMap::value_type const& pair : outProxies) {
          if(!fnVerifyProxyToHierarchyAvailability(outDependerHierarchies, pair)) {
            std::string msg = String::format("ProxyList <-> Hierarchy mismatch. Missing hierarchy entry for proxy:"); // % 0", pair.first.type());
            HandleEngineStatusError(EEngineStatus::ResourceManager_ProxyCreationFailed, msg);
          }
        }
      } catch(EngineException &ee) {
        Log::Error(logTag(), ee.message());
      } catch(std::exception &stde) {
        Log::Error(logTag(), stde.what());
      } catch(...) {
        Log::Error(logTag(), "Unknown error occurred during verification of created resource proxy environment.");
      }

#endif

      try {
        // Store all dependencies, roots and dependers, to have the loadProxy function 
        // work out well.
        // Important: 
        //   We will just access outProxies as below, since the ProxyTreeCreator<T>::create-call
        //   is required to return a consistent state!
        std::function<void(DependerTreeNodeList const&, ResourceProxyMap const&)> fnInsert = nullptr;
        fnInsert
          = [&, this] (
            DependerTreeNodeList const& hierarchy,
            ResourceProxyMap     const& proxies) -> void
        {
          for(DependerTreeNodeList::value_type const& r : hierarchy) {
            fnInsert(r.children, proxies);

            if(!storeResourceProxy(r.resourceHandle, proxies.at(r.resourceHandle)))
              HandleEngineStatusError(EEngineStatus::ResourceManager_ProxyCreationFailed, "Failed to store resource proxy " + r.resourceHandle.name());
          }
        };
        fnInsert(outDependerHierarchies, outProxies);

        // Also store hierarchy.
        for(DependerTreeNodeList::value_type const& r : outDependerHierarchies)
          m_hierarchyRoots[r.resourceHandle] = r;

      } catch(EngineException &ee) {
        Log::Error(logTag(), ee.message());
      } catch(std::exception &stde) {
        Log::Error(logTag(), stde.what());
      } catch(...) {
        Log::Error(logTag(), "Unknown error occurred during verification of created resource proxy environment.");
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
                std::string msg = "Failed to unload resources using proxy.";
                HandleEngineStatusError(EEngineStatus::ResourceManager_ProxyUnloadFailed, msg);
              }

            std::string msg = "Failed to load resources using proxy.";
            HandleEngineStatusError(EEngineStatus::ResourceManager_ProxyLoadFailed, msg);
          }

          loadedDependerHierarchies.push_back(root.resourceHandle); // For freeing already loaded hierarchies
        }
      }

      return status;
    }

    template <typename TResource>
    EEngineStatus ProxyBasedResourceManager
      ::updateResource(
        PublicResourceId_t                const&inId,
        typename TResource::UpdateRequest const&request)
    {
      return EEngineStatus::Ok;
    }

    template <typename TResource>
    EEngineStatus ProxyBasedResourceManager
      ::destroyResource(
        PublicResourceId_t                     const&inId,
        typename TResource::DestructionRequest const&request)
    {
      ResourceInfo<TResource> info ={};
      EEngineStatus status = getResourceInfo<TResource>(inId, info);
      HandleEngineStatusError(status, String::format("Failed to fetch resource information (Id: %0).", inId));

      status = proxyUnload(info.handle);
      HandleEngineStatusError(status, String::format("Failed to unload proxy resource in backend (Id: %0).", inId));

      std::function<void(DependerTreeNode const&)> fnEraseRecursively = nullptr;

      fnEraseRecursively
        = [&, this] (DependerTreeNode const&root) -> void
      {
        for(DependerTreeNodeList::value_type const&c : root.children)
          fnEraseRecursively(c);

        m_resources->removeResource(root.resourceHandle);
      };
      fnEraseRecursively(m_hierarchyRoots[info.handle]); // If not available, an empty default will be created, which is subsequently killed. Small overhead, easy algorithm.

      m_hierarchyRoots.erase(info.handle);

      // Any further verification, e.g. with the backend to ensure proper deletion?
      return status;
    }

    template <typename TResource>
    EEngineStatus
      ProxyBasedResourceManager::createImpl(
        PublicResourceId_t                  const&inId,
        typename TResource::CreationRequest const&inRequest)
    {
      typename TResource::Binding binding ={};

      EEngineStatus status = createResource<TResource>(
        inId,
        inRequest,
        false,
        binding);
      HandleEngineStatusError(status, "Failed to create resource.");

      const typename TResource::Descriptor& desc = inRequest.resourceDescriptor();

      // Store binding internally.
      ResourceInfo<TResource> info = ResourceInfo<TResource>(inId, binding.handle, desc, binding);
      storeResourceInfo<TResource>(inId, info);

      return EEngineStatus::Ok;
    }

    template <typename TResource>
    EEngineStatus
      ProxyBasedResourceManager::createDynamicImpl(
        typename TResource::CreationRequest const&inRequest,
        PublicResourceId_t                       &outId)
    {
      // Generate dynamic registry id and invoke createImpl
      PublicResourceId_t id = 0;

      EEngineStatus status = createImpl<TResource>(id, inRequest);
      HandleEngineStatusError(status, "Failed to create dynamic resource.");

      outId = id;
      return status;
    }

    template <typename TResource>
    EEngineStatus 
      ProxyBasedResourceManager::updateImpl(
        PublicResourceId_t                const&inId,
        typename TResource::UpdateRequest const&inRequest)
    {
      EEngineStatus status = updateResource<TResource>(inId, inRequest);
      HandleEngineStatusError(status, "Failed to update resource.");

      return EEngineStatus::Ok;
    }

    template <typename TResource>
    EEngineStatus 
      ProxyBasedResourceManager::destroyImpl(
        PublicResourceId_t                     const&inId,
        typename TResource::DestructionRequest const&inRequest)
    {
      EEngineStatus status = destroyResource<TResource>(inId, inRequest);
      HandleEngineStatusError(status, "Failed to destroy resource.");

      return EEngineStatus::Ok;
    }
    
    template <typename TResource>
    EEngineStatus
      ProxyBasedResourceManager::getResourceInfoImpl(
        PublicResourceId_t      const&id,
        ResourceInfo<TResource>      &outInfo)
    {
      EEngineStatus status = getResourceInfo<TResource>(id, outInfo);
      HandleEngineStatusError(status, "Failed to determine resource info.");
      return EEngineStatus::Ok;
    }

    template <typename TResource>
    EEngineStatus 
      ProxyBasedResourceManager::storeResourceInfo(
        PublicResourceId_t      const&id,
        ResourceInfo<TResource> const&inInfo)
    { 
      return EEngineStatus::Ok;
    }

  }
}

#endif