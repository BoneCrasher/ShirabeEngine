#include "Resources/System/Core/ProxyBasedResourceManager.h"

namespace Engine {
  namespace Resources {

    bool ProxyBasedResourceManager
      ::clear()
    {
      return true;
    }

#define ImplementResourceMethods(Type)                   \
    EEngineStatus ProxyBasedResourceManager              \
    ::create##Type(                                      \
      PublicResourceId_t    const&inId,                  \
      Type::CreationRequest const&inRequest)             \
    {                                                    \
      return createImpl<Type>(inId, inRequest);          \
    }                                                    \
                                                         \
    EEngineStatus                                        \
      ProxyBasedResourceManager::create##Type(           \
        Type::CreationRequest const&inRequest,           \
        PublicResourceId_t         &outId)               \
    {                                                    \
      return createDynamicImpl<Type>(inRequest, outId);  \
    }                                                    \
                                                         \
    EEngineStatus ProxyBasedResourceManager              \
        ::update##Type(                                  \
        PublicResourceId_t  const&inId,                  \
        Type::UpdateRequest const&inRequest)             \
    {                                                    \
      return updateImpl<Type>(inId, inRequest);          \
    }                                                    \
                                                         \
     EEngineStatus                                       \
        ProxyBasedResourceManager::destroy##Type(        \
        PublicResourceId_t const&inId)                   \
    {                                                    \
      return destroyImpl<Type>(inId);                    \
    }                                                    \
                                                         \
    EEngineStatus                                        \
      ProxyBasedResourceManager::get##Type##Info(        \
        PublicResourceId_t const&id,                     \
        ResourceInfo<Type>      &outInfo)                \
    {                                                    \
      return getResourceInfoImpl<Type>(                  \
                id,                                      \
                outInfo);                                \
    }

    ImplementResourceMethods(SwapChain);
    ImplementResourceMethods(Texture1D);
    ImplementResourceMethods(Texture2D);
    ImplementResourceMethods(Texture3D);
    ImplementResourceMethods(RenderTargetView);
    ImplementResourceMethods(ShaderResourceView);
    ImplementResourceMethods(DepthStencilView);
    ImplementResourceMethods(DepthStencilState);
    ImplementResourceMethods(RasterizerState);


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
  }
}