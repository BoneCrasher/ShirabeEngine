#ifndef __SHIRABE_DX11TEXTURENDPROXYCREATOR_H__
#define __SHIRABE_DX11TEXTURENDPROXYCREATOR_H__

#include "Core/EngineStatus.h"
#include "Log/Log.h"

#include "Resources/System/Core/Handle.h"
#include "Resources/System/Core/IResourceDescriptor.h"
#include "Resources/System/Core/IResourceProxy.h"
#include "Resources/System/Core/ResourceProxyFactory.h"
#include "Resources/System/Core/ProxyTreeCreator.h"

#include "GFXAPI/Types/TextureND.h"
#include "GFXAPI/Types/DepthStencilView.h"

#include "Resources/Subsystems/GFXAPI/GFXAPI.h"

#include "GFXAPI/ResourceProxyTreeCreators/ShaderResourceView.h"
#include "GFXAPI/ResourceProxyTreeCreators/RenderTargetView.h"
// #include "GFXAPI/ResourceProxyTreeCreators/DepthStencilView.h"

namespace Engine {
	namespace Resources {

		using TextureNDSRVProxyPtr = Ptr<IResourceProxy<ShaderResourceView>>;
		using TextureNDRTVProxyPtr = Ptr<IResourceProxy<RenderTargetView>>;
		using TextureNDDSVProxyPtr = Ptr<IResourceProxy<DepthStencilView>>;

		template <std::size_t N>
		static bool createTextureNDSRVProxy(
			const std::string             &textureName,
			BufferBindingFlags_t           bindFlags,
			const unsigned int             dimensionNb,
			const VecND<uint32_t, N>      &dimensions,
			const TextureArrayDescriptor  &arrayDesc,
			const TextureMipMapDescriptor &mipMapDesc,
			const Format                  &textureFormat,
			bool                           isCube,
			const ResourceHandle          &textureNDProxyHandle,
			ResourceHandle                &outSRVHandle,
			TextureNDRTVProxyPtr          &outSRVProxy)
		{
			if( (bindFlags.gpuBinding & (std::underlying_type_t<BufferBinding>)BufferBinding::ShaderResource) ) {
				ShaderResourceDescriptor srvDesc;
				srvDesc.name                                        = String::format("%0_SRV", textureName);
				srvDesc.format                                      = textureFormat;
				srvDesc.shaderResourceDimension.texture.dimensionNb = dimensionNb;
				srvDesc.shaderResourceDimension.texture.dimensions  = dimensions; // Will truncate or expand...
				srvDesc.shaderResourceDimension.texture.array       = arrayDesc;
				srvDesc.shaderResourceDimension.texture.isCube      = isCube;
				srvDesc.shaderResourceDimension.texture.mipMap      = mipMapDesc;

				TextureNDSRVProxyPtr srvProxy
					= ProxyTreeCreator<EResourceType::GAPI_VIEW, EResourceSubType::SHADER_RESOURCE_VIEW>::create(srvDesc, { textureNDProxyHandle }, outProxies);

				if( !srvProxy ) {
					// TODO: Log
					return false;
				}
				else {
					ResourceHandle handle(srvDesc.name, EResourceType::GAPI_VIEW, EResourceSubType::SHADER_RESOURCE_VIEW);
					outSRVHandle = handle;
					outSRVProxy  = srvProxy;
				}
			}

			return true;
		}

		template <std::size_t N>
		static bool createTextureNDRTVProxy(
			const std::string             &textureName,
			BufferBindingFlags_t           bindFlags,
			const unsigned int             dimensionNb,
			const TextureArrayDescriptor  &arrayDesc,
			const TextureMipMapDescriptor &mipMapDesc,
			const Format                  &textureFormat,
			const ResourceHandle          &textureNDProxyHandle,
			ResourceHandle                &outRTVHandle,
			TextureNDRTVProxyPtr          &outRTVProxy)
		{
			if( (bindFlags.gpuBinding & (std::underlying_type_t<BufferBinding>)BufferBinding::ShaderOutput_RenderTarget) ) {
				RenderTargetDescriptor rtvDesc;
				rtvDesc.name          = String::format("%0_RTV", textureName);
				rtvDesc.textureFormat = textureFormat;
				rtvDesc.dimensionNb   = dimensionNb;
				rtvDesc.array         = arrayDesc;
				rtvDesc.mipMap        = mipMapDesc;

				TextureNDRTVProxyPtr rtvProxy
					= ProxyTreeCreator<EResourceType::GAPI_VIEW, EResourceSubType::RENDER_TARGET_VIEW>::create(rtvDesc, { textureNDProxyHandle }, outProxies);
				if( !rtvProxy ) {
					// TODO: Log
					return false;
				}
				else {
					ResourceHandle handle(rtvDesc.name, EResourceType::GAPI_VIEW, EResourceSubType::RENDER_TARGET_VIEW);
					outRTVHandle = handle;
					outRTVProxy  = rtvProxy;
				}
			}

			return true;
		}

		template <std::size_t N>
		static bool createTextureNDDSVProxy(
			const std::string             &textureName,
			BufferBindingFlags_t           bindFlags,
			const unsigned int             dimensionNb,
			const VecND<uint32_t, N>      &dimensions,
			const TextureArrayDescriptor  &arrayDesc,
			const TextureMipMapDescriptor &mipMapDesc,
			const Format                  &textureFormat,
			bool                           isCube,
			const ResourceHandle          &textureNDProxyHandle,
			ResourceHandle                &outDSVHandle,       
			TextureNDDSVProxyPtr          &outDSVProxy )
		{
			if( (bindFlags.gpuBinding & (std::underlying_type_t<BufferBinding>)BufferBinding::ShaderOutput_DepthStencil) ) {
				DepthStencilViewDescriptor dsvDesc;
				dsvDesc.name          = String::format("%0_DSV", textureName);
				dsvDesc.textureFormat = textureFormat;
				dsvDesc.dimensionNb   = dimensionNb;
				dsvDesc.array         = arrayDesc;
				dsvDesc.mipMap        = mipMapDesc;

				TextureNDDSVProxyPtr dsvProxy
					= ProxyTreeCreator<EResourceType::GAPI_VIEW, EResourceSubType::DEPTH_STENCIL_VIEW>::create(dsvDesc, { textureNDProxyHandle }, outProxies);
				if( !dsvProxy ) {
					// TODO: Log
					return false;
				}
				else {
					ResourceHandle handle(dsvDesc.name, EResourceType::GAPI_VIEW, EResourceSubType::DEPTH_STENCIL_VIEW);
					outDSVHandle = handle;
					outDSVProxy  = dsvProxy;
				}
			}

			return true;
		}
		
		template <std::size_t N>
		static bool createTextureNDDependerProxies(
			const std::string             &textureName,
			BufferBindingFlags_t           bindFlags,
			const unsigned int             dimensionNb,
			const VecND<uint32_t, N>      &dimensions,
			const TextureArrayDescriptor  &arrayDesc,
			const TextureMipMapDescriptor &mipMapDesc,
			const Format                  &textureFormat,
			bool                           isCube,
			const ResourceHandle          &textureNDProxyHandle,
			ResourceHandle                &outSrvHandle,
			TextureNDSRVProxyPtr          &outSrvProxy,
			ResourceHandle                &outRtvHandle,
			TextureNDRTVProxyPtr          &outRtvProxy,
			ResourceHandle                &outDsvHandle,
			TextureNDDSVProxyPtr          &outDsvProxy
			)
		{
			bool result = true;

			// If the parent resource should be bound as a shader resource view, create a descriptor and proxy for it.			
			result |= createTextureNDSRVProxy<N>(
				textureName,
				bindFlags,
				dimensionNb,
				dimensions,
				arrayDesc,
				mipMapDesc,
				textureFormat,
				isCube,
				textureNDProxyHandle,
				outSrvHandle,
				outSrvProxy);

			// If the parent resource should be bound as a render target view, create a descriptor and proxy for it.
			result |= createTextureNDRTVProxy<N>(
				textureName,
				bindFlags,
				dimensionNb,
				arrayDesc,
				mipMapDesc,
				textureFormat,
				textureNDProxyHandle,
				outRtvHandle,
				outRtvProxy);

			// If the parent resource should be bound as a render target view, create a descriptor and proxy for it.
			result |= createTextureNDDSVProxy<N>(
				textureName,
				bindFlags,
				dimensionNb,
				dimensions,
				arrayDesc,
				mipMapDesc,
				textureFormat,
				isCube,
				textureNDProxyHandle,
				outDsvHandle,
				outDsvProxy);
		}
		
		template <>
		class ProxyTreeCreator<Texture1D> {
		public:
			static const constexpr EResourceType    resource_type    = EResourceType::TEXTURE;
			static const constexpr EResourceSubType resource_subtype = EResourceSubType::TEXTURE_1D;

			using binding_type = TextureNDResourceBinding;
			using request_type = ResourceCreationRequest<Texture1D>;

			static bool create(
				const Ptr<ResourceProxyFactory> &proxyFactory,
				const request_type              &request,
				ResourceHandleList              &inDependencyHandles,
				binding_type                    &outBinding,
				ResourceProxyMap                &outProxies,
				DependerTreeNodeList            &outResourceHierarchy)
			{
				// Down-cast to known type!
				const Texture1DDescriptor& t1DDesc = static_cast<const Texture1DDescriptor&>(request.resourceDescriptor());
				Ptr<IResourceProxy<Texture1D>> proxy
					= proxyFactory->create<Texture1D>(EProxyType::Dynamic, t1DDesc, inDependencyHandles);

				ResourceHandle handle(t1DDesc.name, EResourceType::TEXTURE, EResourceSubType::TEXTURE_1D);
				outProxies[handle] = AnyProxy(proxy);

				DependerTreeNode resourceNode;
				resourceNode.resourceHandle = handle;

				bool isCubeMap      = false; // Not possible for 1D textures
				bool isCubeMapArray = false; // Not possible for 1D textures

				ResourceHandle                    srvHandle;
				TextureNDSRVProxyPtr              srvProxy = nullptr;

				ResourceHandle                  rtvHandle;
				TextureNDRTVProxyPtr            rtvProxy = nullptr;

				ResourceHandle                  dsvHandle;
				TextureNDDSVProxyPtr            dsvProxy = nullptr;

				if( !createTextureNDDependerProxies<1>(
					t1DDesc.name,
					t1DDesc.gpuBinding,
					t1DDesc.dimensionNb,
					t1DDesc.dimensions,
					t1DDesc.array,
					t1DDesc.mipMap,
					t1DDesc.textureFormat,
					isCubeMap,
					handle,
					srvHandle,
					srvProxy,
					rtvHandle,
					rtvProxy,
					dsvHandle,
					dsvProxy) )
				{
				}

				TextureNDResourceBinding binding;
				binding.handle = handle;
				
				if( srvProxy ) {
					outProxies[srvHandle] = AnyProxy(srvProxy);

					DependerTreeNode srvResourceNode;
					srvResourceNode.resourceHandle = srvHandle;
					resourceNode.children.push_back(srvResourceNode);

					binding.srvBinding = srvHandle;
				}

				if( rtvProxy ) {
					outProxies[rtvHandle] = AnyProxy(rtvProxy);

					DependerTreeNode rtvResourceNode;
					rtvResourceNode.resourceHandle = rtvHandle;
					resourceNode.children.push_back(rtvResourceNode);

					binding.rtvBinding = rtvHandle;
				}

				if( dsvProxy ) {
					outProxies[dsvHandle] = AnyProxy(dsvProxy);

					DependerTreeNode dsvResourceNode;
					dsvResourceNode.resourceHandle = dsvHandle;
					resourceNode.children.push_back(dsvResourceNode);

					binding.dsvBinding = dsvHandle;
				}

				outResourceHierarchy.push_back(resourceNode);
			}
		};

		template <>
		class ProxyTreeCreator<Texture2D> {
		public:
			static const constexpr EResourceType    resource_type    = EResourceType::TEXTURE;
			static const constexpr EResourceSubType resource_subtype = EResourceSubType::TEXTURE_2D;

			using binding_type = TextureNDResourceBinding;
			using request_type = ResourceCreationRequest<Texture2D>;

			static bool create(
				const Ptr<ResourceProxyFactory> &proxyFactory,
				const request_type              &request,
				ResourceHandleList              &inDependencyHandles,
				binding_type                    &outBinding,
				ResourceProxyMap                &outProxies,
				DependerTreeNodeList            &outResourceHierarchy)
			{
				const Texture2DDescriptor& t2DDesc = static_cast<const Texture2DDescriptor&>(request.resourceDescriptor());
				Ptr<IResourceProxy<Texture2D>> proxy
					= proxyFactory->create<Texture2D>(EProxyType::Dynamic, request, inDependencyHandles);

				ResourceHandle handle(t2DDesc.name, EResourceType::TEXTURE, EResourceSubType::TEXTURE_2D);
				outProxies[handle] = AnyProxy(proxy);

				DependerTreeNode resourceNode;
				resourceNode.resourceHandle = handle;

				bool isCubeMap      = (t2DDesc.array.isTextureArray && (t2DDesc.array.size % 6) == 0);
				bool isCubeMapArray = isCubeMap && ((t2DDesc.array.size / 6) > 1);

				ResourceHandle       srvHandle;
				TextureNDSRVProxyPtr srvProxy = nullptr;
				ResourceHandle       rtvHandle;
				TextureNDRTVProxyPtr rtvProxy = nullptr;
				ResourceHandle       dsvHandle;
				TextureNDDSVProxyPtr dsvProxy = nullptr;

				if( !createTextureNDDependerProxies<2>(
					t2DDesc.name,
					t2DDesc.gpuBinding,
					t2DDesc.dimensionNb,
					t2DDesc.dimensions,
					t2DDesc.array,
					t2DDesc.mipMap,
					t2DDesc.textureFormat,
					isCubeMap,
					handle,
					srvHandle,
					srvProxy,
					rtvHandle,
					rtvProxy,
					dsvHandle,
					dsvProxy) )
				{
				}

				if( srvProxy ) {
					outProxies[srvHandle] = AnyProxy(srvProxy);

					DependerTreeNode srvResourceNode;
					srvResourceNode.resourceHandle = srvHandle;
					resourceNode.children.push_back(srvResourceNode);
				}

				if( rtvProxy ) {
					outProxies[rtvHandle] = AnyProxy(rtvProxy);

					DependerTreeNode rtvResourceNode;
					rtvResourceNode.resourceHandle = rtvHandle;
					resourceNode.children.push_back(rtvResourceNode);
				}

				if( dsvProxy ) {
					outProxies[dsvHandle] = AnyProxy(dsvProxy);

					DependerTreeNode dsvResourceNode;
					dsvResourceNode.resourceHandle = dsvHandle;
					resourceNode.children.push_back(dsvResourceNode);
				}

				outResourceHierarchy.push_back(resourceNode);
			}
		};

		template <>
		class ProxyTreeCreator<Texture3D> {
		public:
			static const constexpr EResourceType    resource_type    = EResourceType::TEXTURE;
			static const constexpr EResourceSubType resource_subtype = EResourceSubType::TEXTURE_3D;

			using binding_type = TextureNDResourceBinding;
			using request_type = ResourceCreationRequest<Texture3D>;

			static bool create(
				const Ptr<ResourceProxyFactory> &proxyFactory,
				const request_type              &request,
				ResourceHandleList              &inDependencyHandles,
				binding_type                    &outBinding,
				ResourceProxyMap                &outProxies,
				DependerTreeNodeList            &outResourceHierarchy)
			{
				const Texture3DDescriptor& t3DDesc = static_cast<const Texture3DDescriptor&>(request.resourceDescriptor());
				Ptr<IResourceProxy<Texture3D>> proxy
					= proxyFactory->create<Texture3D>(EProxyType::Dynamic, t3DDesc, inDependencyHandles);

				ResourceHandle handle(t3DDesc.name, resource_type, resource_subtype);
				outProxies[handle] = AnyProxy(proxy);

				DependerTreeNode resourceNode;
				resourceNode.resourceHandle = handle;

				bool isCubeMap      = false; // Not possible for 3D textures
				bool isCubeMapArray = false; // Not possible for 3D textures

				ResourceHandle       srvHandle;
				TextureNDSRVProxyPtr srvProxy = nullptr;
				ResourceHandle       rtvHandle;
				TextureNDRTVProxyPtr rtvProxy = nullptr;
				ResourceHandle       dsvHandle;
				TextureNDDSVProxyPtr dsvProxy = nullptr;

				if( !createTextureNDDependerProxies<3>(
					t3DDesc.name,
					t3DDesc.gpuBinding,
					t3DDesc.dimensionNb,
					t3DDesc.dimensions,
					t3DDesc.array,
					t3DDesc.mipMap,
					t3DDesc.textureFormat,
					isCubeMap,
					handle,
					srvHandle,
					srvProxy,
					rtvHandle,
					rtvProxy,
					dsvHandle,
					dsvProxy) )
				{
				}

				if( srvProxy ) {
					outProxies[srvHandle] = AnyProxy(srvProxy);

					DependerTreeNode srvResourceNode;
					srvResourceNode.resourceHandle = srvHandle;
					resourceNode.children.push_back(srvResourceNode);
				}

				if( rtvProxy ) {
					outProxies[rtvHandle] = AnyProxy(rtvProxy);

					DependerTreeNode rtvResourceNode;
					rtvResourceNode.resourceHandle = rtvHandle;
					resourceNode.children.push_back(rtvResourceNode);
				}

				if( dsvProxy ) {
					outProxies[dsvHandle] = AnyProxy(dsvProxy);

					DependerTreeNode dsvResourceNode;
					dsvResourceNode.resourceHandle = dsvHandle;
					resourceNode.children.push_back(dsvResourceNode);
				}

				outResourceHierarchy.push_back(resourceNode);
			}
		};

	}

}

#endif