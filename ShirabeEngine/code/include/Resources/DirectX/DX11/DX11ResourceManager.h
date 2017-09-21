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
				template < 
					typename    TBuilder, 
					typename... TAdditionalArgs
				>
				EEngineStatus createResource(
						const typename TBuilder::traits_type::descriptor_type &desc,
					    std::vector<ResourceHandle>                           &outHandles,
						TAdditionalArgs&&...                                   args
					) {
					EEngineStatus status = EEngineStatus::Ok;
					
					using build            = TBuilder::build;
					using resource_ptr     = typename TBuilder::traits_type::resource_type_ptr;
					using resource_type    = TBuilder::traits_type::resource_type;
					using resource_subtype = TBuilder::traits_type::resource_subtype;

					// Store any to-be-created resource in here to iteratively store them in the 
					// respective resource-pools and return the handle-list.
					std::map<ResourceHandle, IUnknownPtr> builtResources;

					// Invoke the builder with the mandatory arguments: Device, Descriptor, outPointer and any additional argument.
					status = build(_dxDevice->internalDevice(), desc, builtResources, std::forward<TAdditionalArgs>(args)...);
					if (!CheckEngineError(status)) {
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
				template <EResourceType type, EResourceSubType subtype>
				EEngineStatus store_unknown(const ResourceHandle& handle, const IUnknownPtr& resource);

				template <typename TResourcePtr>
				EEngineStatus store(const ResourceHandle& handle, const TResourcePtr& resource);

				IDXDevicePtr _dxDevice;

				// Be specific for each god-damn resource type to avoid the type-guessing & QueryInterface-overhead.
				
				// Any kind of textures required...
				IIndexedResourcePoolPtr<ResourceHandle, ID3D11Texture1DPtr>          _tex1DPool;
				IIndexedResourcePoolPtr<ResourceHandle, ID3D11Texture2DPtr>          _tex2DPool;
				IIndexedResourcePoolPtr<ResourceHandle, ID3D11Texture3DPtr>          _tex3DPool; 
				// Constant-, Texture-, Structured-, Vertex-, Index-, Instance-, ...
				IIndexedResourcePoolPtr<ResourceHandle, ID3D11Buffer>                _bufferPool;
				// Shader inputs
				IIndexedResourcePoolPtr<ResourceHandle, ID3D11ShaderResourceViewPtr> _srvPool;
				// Render targets
				IIndexedResourcePoolPtr<ResourceHandle, ID3D11RenderTargetViewPtr>   _rtvPool;
				// Depth stencil views
				IIndexedResourcePoolPtr<ResourceHandle, ID3D11DepthStencilViewPtr>   _dsvPool;
				// Depth stencil states
				IIndexedResourcePoolPtr<ResourceHandle, ID3D11DepthStencilStatePtr>  _dssPool;
				// Rasterizer states
				IIndexedResourcePoolPtr<ResourceHandle, ID3D11RasterizerStatePtr>    _rsPool;
				// Blend states
				IIndexedResourcePoolPtr<ResourceHandle, ID3D11BlendState>            _bsPool;
				// Input layouts
				IIndexedResourcePoolPtr<ResourceHandle, ID3D11InputLayout>           _ilPool;
				// Shader types
				IIndexedResourcePoolPtr<ResourceHandle, ID3D11VertexShader>          _vsPool;
				IIndexedResourcePoolPtr<ResourceHandle, ID3D11HullShader>            _hsPool;
				IIndexedResourcePoolPtr<ResourceHandle, ID3D11DomainShader>          _dsPool;
				IIndexedResourcePoolPtr<ResourceHandle, ID3D11GeometryShader>        _gsPool;
				IIndexedResourcePoolPtr<ResourceHandle, ID3D11ComputeShader>         _csPool;
				IIndexedResourcePoolPtr<ResourceHandle, ID3D11PixelShader>           _psPool;

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