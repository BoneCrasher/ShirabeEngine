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
			bool loadDependenciesRecursively(const IResourceProxyBasePtr& base, ResourceProxyMap& outDependencies) {
				bool result = true;

				if( !base ) {
					Log::Error(logTag(), "loadDependenciesRecursively invoked with null-pointer base-proxy.");
					return false;
				}

				// Holds all local depdencies
				ResourceProxyMap inDependencies;

				ResourceHandleList dependecyHandles = base->dependencies();
				if( !dependecyHandles.empty() ) {
					for( const ResourceHandle& resourceHandle : dependecyHandles ) {
						// The reason all proxies have to be created by the proxy-creator and 
						// stored before this function call, can be seen below:
						AnyProxy dependencyProxy = getResourceProxy(resourceHandle);
						if( !dependencyProxy.has_value() ) {
							// Even if we dealt with an internal resource not to be 
							// created here, having an empty proxy stored is a in-
							// consistent state.
							Log::Error(logTag(), String::format("Nullpointer proxy encountered in internal resource manager."));
							result |= false;
							break;
						}

						IResourceProxyBasePtr dependencyBase = BaseProxyCast(dependencyProxy);
						if( !dependencyBase ) {
							// MBT TODO: How to handle this case?
						}

						// Do we even hav to load the resource? If the resource type is internal, there won't 
						// be any necessity to go deeper. Internal resources and all it's children will be created
						// without any control from our side.		
						if(   dependencyBase->proxyType() == EProxyType::Internal
						   || dependencyBase->proxyType() == EProxyType::Unknown )
						{
							// Nothing to do...
							continue;
						}

						// We need to handle the thing. First: load dependencies, then the resource.
						result |= loadDependenciesRecursively(dependencyBase, inDependencies);
						if( !result ) {
							Log::Error(logTag(), String::format("Failed to load dependencies of proxy."));
							break;
						}

						if( !dependencyBase->loadSync(resourceHandle, inDependencies) ) {
							Log::Error(logTag(), String::format("Failed to load resource of proxy."));
							result |= false;
							break;
						}

						if( dependencyBase->loadState() != ELoadState::LOADED ) {
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
			EEngineStatus proxyLoad(const ResourceHandle& handle) {
				EEngineStatus status = EEngineStatus::Ok;

				AnyProxy root = getResourceProxy(handle);
				if( !root.has_value() )
					return EEngineStatus::ResourceManager_RootProxyFetchFailed;

				IResourceProxyBasePtr base = BaseProxyCast(root);
				if( !base ) {
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

				if( !dependenciesLoadedSuccessfully ) {
					// Flag error, since the entire child resource tree was not loaded successfully.
					// Free anything created beforehand though!
					ResourceProxyMap::iterator dep;
					for( dep  = dependencyResources.begin();
						dep != dependencyResources.end();
						dep++ )
					{
						AnyProxy proxy = dep->second;
						if( !proxy.has_value() )
							continue;

						IResourceProxyBasePtr base = BaseProxyCast(proxy);
						if( !base ) {
							continue;
						}

						if( !base->unloadSync() ) {
							Log::Error(logTag(), "Failed to unload resource.");
							continue;
						}
					}

					Log::Error(logTag(), "Failed to load one or more dependencies of resource proxy.");
					return EEngineStatus::ResourceManager_DependencyLoadFailed;
				}

				// Finally load the root resource
				if( !base->loadSync(handle, dependencyResources) ) {
					Log::Error(logTag(), "Failed to load underlying resource of resource proxy.");
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
			EEngineStatus createResource(
				const ResourceDescriptor<TResource> &desc,
				bool                                 creationDeferred,
				typename TResource::binding_type    &binding
			) {
				using resource_type    = typename TResource::resource_type;
				using resource_subtype = typename TResource::resource_subtype;
				using binding_type     = typename TResource::binding_type;

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
				ResourceProxyMap     outProxies;
				DependerTreeNodeList outDependerHierarchies;

				bool treeCreationSuccessful = ProxyTreeCreator<TResource>::create(_gfxApiProxyFactory, desc, {}, binding, outDependerHierarchies);
				if( !treeCreationSuccessful ) {
					Log::Error(logTag(), "Unable to create root resource proxy.");
					return EEngineStatus::ResourceManager_ProxyCreationFailed;
				}

				// Store all dependencies, roots and dependers, to have the loadProxy function 
				// work out well.
				for( const ResourceProxyMap::value_type& r : outProxies ) {
					if( !storeResourceProxy(r.first, r.second) ) {
						Log::Error(logTag(), "Failed to store resource proxy.");
						// TODO: Release madness
					}

					// Make sure to write out the created handles
					outHandlesCreated.push_back(r.first);
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
				if( !creationDeferred ) {
					if( CheckEngineError((status = proxyLoad(rootProxyHandle))) ) {
						Log::Error(logTag(), "Failed to load resources using proxy.");
						return EEngineStatus::ResourceManager_ProxyLoadFailed;
					}
				}

				return status;
			}

		public:
			ProxyBasedResourceManager(
				const Ptr<ResourceProxyFactory> &proxyFactory)
				: _gfxApiProxyFactory(proxyFactory)
			{}

			virtual ~ProxyBasedResourceManager() {
				// _resources->clear();
				_gfxApiProxyFactory = nullptr;
			};

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

			Ptr<ResourceProxyFactory> _gfxApiProxyFactory;

			// Any kind of resources, abstracted away entirely.
			IIndexedResourcePoolPtr<ResourceHandle, AnyProxy> _resources;
		};
		DeclareSharedPointerType(ProxyBasedResourceManager);
	}
}

#endif