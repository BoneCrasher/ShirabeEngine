#ifndef __SHIRABE_DX11RESOURCEMANAGER_H__
#define __SHIRABE_DX11RESOURCEMANAGER_H__

#include "Platform/Platform.h"
#include "Core/EngineTypeHelper.h"

#include "GAPI/Config.h"
#include "GAPI/DirectX/DX11/DX11Common.h"
#include "GAPI/DirectX/DX11/DX11Types.h"
#include "GAPI/DirectX/DX11/DX11Device.h"

#include "Resources/IResourcePool.h"
#include "Resources/IResourceManager.h"
#include "Resources/Handle.h"

#include "Resources/DirectX/DX11/DX11TextureNDBuilder.h"
#include "Resources/DirectX/DX11/DX11RenderTargetViewBuilder.h"

namespace Engine {
	namespace DX {
		namespace _11 {
			using namespace Resources;			

			class DX11ResourceManager
				: public IResourceManager {
				DeclareLogTag(DX11ResourceManager);

				/**********************************************************************************************//**
				 * \fn	template < typename TBuilder, typename... TAdditionalArgs > EEngineStatus DX11ResourceManager::createResource( const typename TBuilder::traits_type::descriptor_type &desc, ResourceHandle &outHandle, TAdditionalArgs&&... args )
				 *
				 * \brief	Unified algorithm to invoke a specific builder and store the data in the respective storage.
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
						ResourceHandle                                        &outHandle,
						TAdditionalArgs&&...                                   args
					) {
					EEngineStatus status = EEngineStatus::Ok;
					
					using build            = TBuilder::build;
					using resource_ptr     = typename TBuilder::traits_type::resource_type_ptr;
					using resource_type    = TBuilder::traits_type::resource_type;
					using resource_subtype = TBuilder::traits_type::resource_subtype;

					resource_ptr res = nullptr;					

					// Invoke the builder with the mandatory arguments: Device, Descriptor, outPointer and any additional argument.
					status = build(_dxDevice->internalDevice(), desc, res, std::forward<TAdditionalArgs>(args)...);
					if (!CheckEngineError(status)) {
						// If successful create a handle for the resource and store it in the respective manager 
						// evaluated using the resource-type.
						ResourceHandle p(desc._name, resource_type, resource_subtype);
						if (CheckEngineError(status = store(p, res))) {
							Log::Error(logTag(), "Failed to store resource. Releasing resource.");
						}
						else {
							outHandle = p;
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
				template <typename TResource>
				EEngineStatus store(const ResourceHandle& handle, const TResource& resource);

				IDXDevicePtr _dxDevice;

				// TODO: Be specific here
				IIndexedResourcePoolPtr<ResourceHandle, ID3D11Texture1DPtr>          _tex1DPool;
				IIndexedResourcePoolPtr<ResourceHandle, ID3D11Texture2DPtr>          _tex2DPool;
				IIndexedResourcePoolPtr<ResourceHandle, ID3D11Texture3DPtr>          _tex3DPool;
				IIndexedResourcePoolPtr<ResourceHandle, ID3D11ShaderResourceViewPtr> _srvPool;
				IIndexedResourcePoolPtr<ResourceHandle, ID3D11RenderTargetViewPtr>   _rtvPool;
				IIndexedResourcePoolPtr<ResourceHandle, ID3D11DepthStencilViewPtr>   _dsvPool;
				IIndexedResourcePoolPtr<ResourceHandle, ID3D11DepthStencilStatePtr>  _dssPool;

			};
			DeclareSharedPointerType(DX11ResourceManager);

			//--------------------------------------------------------------------------------------------------
			// Specializations of store<Handle, Resource>(const Handle&, const Resource&) : EEngineStatus
			//--------------------------------------------------------------------------------------------------

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