#ifndef __SHIRABE_PROXYBASEDRESOURCEMANAGER_H__
#define __SHIRABE_PROXYBASEDRESOURCEMANAGER_H__

#include "Log/Log.h"
#include "Platform/Platform.h"
#include "Core/EngineTypeHelper.h"

#include "Resources/System/Core/IResourcePool.h"
#include "Resources/System/Core/IResourceManager.h"
#include "Resources/System/Core/IResourceProxy.h"
#include "Resources/System/Core/Handle.h"

namespace Engine {
	namespace Resources {

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

		class ProxyBasedResourceManager
			: public IResourceManager {
			DeclareLogTag(ProxyBasedResourceManager);

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

						}

						// Do we even hav to load the resource? If the resource type is internal, there won't 
						// be any necessity to go deeper. Internal resources and all it's children will be created
						// without any control from our side.		
						if( dependencyBase->type() == EProxyType::Internal
						   || dependencyBase->type() == EProxyType::Unknown )
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

						// TODO: Consider having each call of 'loadDependenciesRecursively' 
						//       use its own map, until everything was loaded successfully.
						//       Finally, merge.
						//       
						//       This is due to the loadSync function not requiring any previous dependencies in the maps.
						if( !dependencyBase->loadSync(resourceHandle, inDependencies) ) {
							Log::Error(logTag(), String::format("Failed to load resource of proxy."));
							result |= false;
							break;
						}
						else {
							if( dependencyBase->loadState() != ELoadState::LOADED ) {
								std::string msg = "CRITICAL LOGIC ERROR: Resource should be loaded successfully "
									"at this point, but isn't or state is not updated properly.";
								Log::WTF(logTag(), String::format(msg));
								break;
							}

							// Yay... Finally store the thing... (and it's dependencies as they need be persisted, too)!
							outDependencies.insert(inDependencies.begin(), inDependencies.end());
							outDependencies[resourceHandle] = dependencyProxy;
						}
					}
				}

				return result;
			}

			/**********************************************************************************************//**
			 * \fn	EEngineStatus DX11ResourceManager::proxyLoad(const Ptr<AnyProxy>& proxy)
			 *
			 * \brief	Load dependencies and root of resource tree.
			 * 			This function only deals with root elements of a resource-tree.
			 *
			 * \param	proxy	The proxy.
			 *
			 * \return	The EEngineStatus.
			 **************************************************************************************************/
			EEngineStatus proxyLoad(const ResourceHandle& handle, const AnyProxy& proxy) {
				EEngineStatus status = EEngineStatus::Ok;

				IResourceProxyBasePtr base = BaseProxyCast(proxy);
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

				if( !base->loadSync(handle, dependencyResources) ) {
					Log::Error(logTag(), "Failed to load underlying resource of resource proxy.");
					return EEngineStatus::ResourceManager_ResourceLoadFailed;
				}
				else {
					// If successful create a handle for the resource and store it in the respective manager 
					// evaluated using the resource-type.
					for( const ResourceProxyMap::value_type& r : builtResources ) {
						if( !storeResourceProxy(r.first, r.second) ) {
							Log::Error(logTag(), "Failed to store resource.");
							// TODO: Release madness
						}
					}

				}

				return EEngineStatus::Ok;
			}
			/**********************************************************************************************//**
			 * \fn	template < typename TBuilder, typename... TAdditionalArgs > EEngineStatus DX11ResourceManager::createResource( const typename TBuilder::traits_type::descriptor_type &desc, ResourceHandle &outHandle, TAdditionalArgs&&... args )
			 *
			 * \brief	Unified algorithm to invoke a specific builder and store the data in the respective storage.
			 *
			 * \remarks Any manageable dx-resource inherits IUnknown, specifying the signature of AddRef & Release.
			 * 			Since this is the only effective common denominator for, use this for storage.
			 *
			 * \tparam	TBuilder	   	Type of the builder.
			 * \tparam	TAdditionalArgs	Type of the additional arguments.
			 * \param 		  	desc	 	The description.
			 * \param [in,out]	outHandle	Handle of the out.
			 * \param 		  	args	 	Variable arguments providing [in,out] The arguments.
			 *
			 * \return	The new resource.
			 **************************************************************************************************/
			template <typename TBuilder>
			EEngineStatus createResource(
				const typename TBuilder::traits_type::descriptor_type &desc,
				bool                                                   creationDeferred,
				Ptr<typename TBuilder::proxy>                         &outProxy,
				std::vector<ResourceHandle>                           &outHandles
			) {
				EEngineStatus status = EEngineStatus::Ok;

				using resource_ptr     = typename TBuilder::traits_type::resource_type_ptr;
				using resource_type    = typename TBuilder::traits_type::resource_type;
				using resource_subtype = typename TBuilder::traits_type::resource_subtype;
				using proxy_type       = ResourceProxy<resource_type, resource_subtype>;

				// Create a resource proxy, which serves as a handle to the effective
				// underlying resource.
				//
				// The proxy will be responsible for persisting the descriptor,
				// enqueue creation tasks in the resource thread and maintain 
				// the resource load state.
				Ptr<proxy_type> proxy = ProxyCreator<resource_type, resource_subtype>::create(desc);
				if( !proxy ) {
					Log::Error(logTag(), "Unable to create proxy.");
					return EEngineStatus::ResourceManager_ProxyCreationFailed;
				}

				// Store whatever proxy in "AnyProxy (:= std::any)" and access using 
				// BaseProxyCast(anyProxy) or AnyProxyCast<TProxy>(anyProxy).
				AnyProxy typeErasedProxy = AnyProxy(proxy);

				// Create handle and store
				ResourceHandle handle(desc.name, resource_type, resource_subtype);
				if( !store(handle, proxy) ) {
					Log::Error(logTag(), "Unable to store newly created proxy.");
					return EEngineStatus::ResourceManager_ProxyStoreFailed;
				}

				// If creation is not deferred, immediately load the resources using the proxy.
				if( !creationDeferred ) {
					if( CheckEngineError((status = proxyLoad(handle, proxy))) ) {
						Log::Error(logTag(), "Failed to load resources using proxy.");
						return EEngineStatus::ResourceManager_ProxyLoadFailed;
					}
				}

				return status;
			}


		public:
			ProxyBasedResourceManager();
			~ProxyBasedResourceManager();

		private:
			AnyProxy getResourceProxy(const ResourceHandle& handle);

			bool storeResourceProxy(
				const ResourceHandle &handle,
				const AnyProxy       &proxy) {
				return _resources->addResource(handle, proxy);
			}

			// Any kind of textures required...
			IIndexedResourcePoolPtr<ResourceHandle, AnyProxy> _resources;
		};
		DeclareSharedPointerType(ProxyBasedResourceManager);
	}
}

#endif