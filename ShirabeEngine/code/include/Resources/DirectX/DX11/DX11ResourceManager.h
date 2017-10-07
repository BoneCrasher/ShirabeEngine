#ifndef __SHIRABE_DX11RESOURCEMANAGER_H__
#define __SHIRABE_DX11RESOURCEMANAGER_H__

#include "Platform/Platform.h"
#include "Core/EngineTypeHelper.h"

#include "GFXAPI/Config.h"
#include "GFXAPI/DirectX/DX11/DX11Common.h"
#include "GFXAPI/DirectX/DX11/DX11Types.h"
#include "GFXAPI/DirectX/DX11/DX11Device.h"

#include "Resources/System/Core/IResourcePool.h"
#include "Resources/System/Core/IResourceManager.h"
#include "Resources/System/Core/IResourceProxy.h"
#include "Resources/System/Core/Handle.h"

#include "Resources/DirectX/DX11/DX11TextureNDBuilder.h"
#include "Resources/DirectX/DX11/DX11RenderTargetViewBuilder.h"
#include "Resources/DirectX/DX11/DX11ShaderResourceViewBuilder.h"

namespace Engine {
	namespace DX {
		namespace _11 {
			using namespace Resources;		

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

			class DX11ResourceManager
				: public IResourceManager {
				DeclareLogTag(DX11ResourceManager);

				/**********************************************************************************************//**
				 * \fn	template<EResourceType type, EResourceSubType subtype> EEngineStatus DX11ResourceManager::proxyLoadImpl(const Ptr<IResourceProxy<type, subtype>>& proxy)
				 *
				 * \brief	Proxy load implementation
				 *
				 * \tparam	type   	Type of the type.
				 * \tparam	subtype	Type of the subtype.
				 * \param	proxy	The proxy.
				 *
				 * \return	The EEngineStatus.
				 **************************************************************************************************/
				template<EResourceType type, EResourceSubType subtype>
				EEngineStatus proxyLoadImpl(const Ptr<IResourceProxy<type, subtype>>& proxy) {

				}

				/**********************************************************************************************//**
				 * \fn	EEngineStatus DX11ResourceManager::proxyLoad(const Ptr<AnyProxy>& proxy)
				 *
				 * \brief	Proxy load
				 *
				 * \param	proxy	The proxy.
				 *
				 * \return	The EEngineStatus.
				 **************************************************************************************************/
				EEngineStatus proxyLoad(const AnyProxy& proxy) {
					EEngineStatus status = EEngineStatus::Ok;

					// Store any to-be-created resource in here to iteratively store them in the 
					// respective resource-pools and return the handle-list.
					std::map<ResourceHandle, IUnknownPtr> builtResources;

					DX11BuilderParameters builderParameters;
					builderParameters.device = _dxDevice->internalDevice();

					// The resource manager has to fetch and load all dependencies first. 
					// Once done a the effective top-level builder is created and it's resource 
					// loaded.
					typename ResourceProxyMap dependencyResources;

					IResourceProxyBasePtr base = BaseProxyCast(proxy);

					ResourceHandleList dependecyHandles = base->dependencies();
					if( !dependecyHandles.empty() ) {
						for( const ResourceHandle& resourceHandle : dependecyHandles ) {
							AnyProxy subresourceProxy = getResourceProxy(resourceHandle);
							if( !subresourceProxy.has_value() ) {
								// Ohoh
							}

							IResourceProxyBasePtr subresourceBaseProxy = BaseProxyCast(subresourceProxy);
						}
					}

					// Invoke the builder with the mandatory arguments: Device, Descriptor, outPointer and any additional argument.
					status = proxy->load(builtResources);
					if( !CheckEngineError(status) ) {
						// If successful create a handle for the resource and store it in the respective manager 
						// evaluated using the resource-type.
						for( const std::pair<ResourceHandle, IUnknownPtr>& r : builtResources ) {
							if( CheckEngineError(status = store_unknown(r.first, r.second)) ) {
								Log::Error(logTag(), "Failed to store resource.");
								// Todo release
							}
							else if( !__extractKeys(builtResources, outHandles) ) {
								Log::Error(logTag(), "Failed to extract resource handles to list.");
								// Todo release
							}
						}
					}

					return status;
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
					// other creation arguments, as well as its load state.
					//
					// TODO: Check if additional arguments are really required.
					//       It should be possible to make all additional dependencies explicitly 
					//       documented using the descriptor interfaces.
					Ptr<proxy_type> proxy = ProxyCreator<resource_type, resource_subtype>::create(desc);
					if( !proxy ) {
						Log::Error(logTag(), "Unable to create proxy.");
						return EEngineStatus::ResourceManager_ProxyCreationFailed;
					}

					// Store whatever proxy in "AnyProxy (:= std::any)" and access using AnyProxyCast<TProxy>(anyProxy).
					AnyProxy typeErasedProxy = AnyProxy(proxy);

					// Create handle and store
					ResourceHandle handle(desc.name, resource_type, resource_subtype);
					if( !store(handle, proxy) ) {
						Log::Error(logTag(), "Unable to store newly created proxy.");
						return EEngineStatus::ResourceManager_ProxyStoreFailed;
					}

					// If creation is not deferred, immediately load the resources using the proxy.
					if( !creationDeferred ) {
						if( CheckEngineError( (status = proxyLoad(handle, proxy)) ) ) {
							Log::Error(logTag(), "Failed to load resources using proxy.");
							return EEngineStatus::ResourceManager_ProxyLoadFailed;
						}
					}					

					return status;
				}
		

			public:
				DX11ResourceManager(
					const IDXDevicePtr& device
				);

				~DX11ResourceManager();

				EEngineStatus createDevice(
					const DeviceDescriptor &deviceDescriptor,
					ResourceHandle         &outHandle
				);

				EEngineStatus createTexture1D (
					const Texture1DDescriptor &desc,
					ResourceHandle            &outHandle
				);

				EEngineStatus createTexture2D (
					const Texture2DDescriptor &desc,
					ResourceHandle            &outHandle
				);

				EEngineStatus createTexture3D (
					const Texture3DDescriptor &desc,
					ResourceHandle            &outHandle
				);

				EEngineStatus createShaderResource(
					const ShaderResourceDescriptor &shaderResourceDescriptor,
					const ResourceHandle           &inUnderlyingResourceHandle,
					ResourceHandle                 &outShaderResourceHandle
				);

				EEngineStatus createRenderTarget(
					const ResourceHandle &inHandle,
					ResourceHandle       &outHandle
				);

				// EEngineStatus createDepthStencilView(
				// 	const DepthStencilViewDescriptor &desc,
				// 	const ResourceHandle             &inTex2DResourceHandle,
				// 	ResourceHandle                   &outHandle
				// );
				// 
				// EEngineStatus createDepthStencilState(
				// 	const DepthStencilStateDescriptor &desc,
				// 	ResourceHandle                    &outHandle
				// );

			private:
				AnyProxy getResourceProxy(const ResourceHandle& handle);

				template <EResourceType type, EResourceSubType subtype>
				EEngineStatus store_unknown(const ResourceHandle& handle, const IUnknownPtr& resource);

				template <typename TResourcePtr>
				EEngineStatus store(const ResourceHandle& handle, const TResourcePtr& resource);

				IDXDevicePtr _dxDevice;
							
				// Any kind of textures required...
				IIndexedResourcePoolPtr<ResourceHandle, AnyProxy> _resources;
			};
			DeclareSharedPointerType(DX11ResourceManager);

			//--------------------------------------------------------------------------------------------------
			// Specializations of store<Handle, Resource>(const Handle&, const Resource&) : EEngineStatus
			//--------------------------------------------------------------------------------------------------
			
			template <EResourceType type, EResourceSubType subtype>
			EEngineStatus DX11ResourceManager::store_unknown(
				const ResourceHandle &handle,
				const IUnknownPtr    &resource) {
				return this->store<typename DetermineDXResourceType<type, subtype>::type>(handle, resource);
			}

			template <>
			EEngineStatus DX11ResourceManager
				::store<ID3D11Texture1DPtr>
				(const ResourceHandle& handle, const ID3D11Texture1DPtr& resource);

			template <>
			EEngineStatus DX11ResourceManager
				::store<ID3D11Texture2DPtr>
				(const ResourceHandle& handle, const ID3D11Texture2DPtr& resource);

			template <>
			EEngineStatus DX11ResourceManager
				::store<ID3D11Texture3DPtr>
				(const ResourceHandle& handle, const ID3D11Texture3DPtr& resource);

			template <>
			EEngineStatus DX11ResourceManager
				::store<ID3D11ShaderResourceViewPtr>
				(const ResourceHandle& handle, const ID3D11ShaderResourceViewPtr& resource);

			template <>
			EEngineStatus DX11ResourceManager
				::store<ID3D11RenderTargetViewPtr>
				(const ResourceHandle& handle, const ID3D11RenderTargetViewPtr& resource);

			template <>
			EEngineStatus DX11ResourceManager
				::store<ID3D11DepthStencilViewPtr>
				(const ResourceHandle& handle, const ID3D11DepthStencilViewPtr& resource);

			template <>
			EEngineStatus DX11ResourceManager
				::store<ID3D11DepthStencilStatePtr>
				(const ResourceHandle& handle, const ID3D11DepthStencilStatePtr& resource);

			//--------------------------------------------------------------------------------------------------
			// END: Specializations of store<Handle, Resource>(const Handle&, const Resource&) : EEngineStatus
			//--------------------------------------------------------------------------------------------------
			
		}
	}
}

#endif