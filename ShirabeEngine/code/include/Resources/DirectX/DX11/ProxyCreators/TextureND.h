#ifndef __SHIRABE_DX11TEXTURENDPROXYCREATOR_H__
#define __SHIRABE_DX11TEXTURENDPROXYCREATOR_H__

#include "Core/EngineStatus.h"
#include "Log/Log.h"

#include "Resources/System/Core/Handle.h"
#include "Resources/System/Core/IResourceProxy.h"

#include "Resources/Types/TextureND.h"
#include "Resources/Types/ShaderResource.h"
#include "Resources/Types/RenderTarget.h"
#include "Resources/Types/DepthStencilView.h"

#include "Resources/DirectX/DX11/ProxyCreators/ShaderResourceView.h"
#include "Resources/DirectX/DX11/ProxyCreators/RenderTargetView.h"

namespace Engine {
	namespace Resources {

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
			ResourceProxyMap              &outProxies)
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

				Ptr<IResourceProxy<EResourceType::GAPI_VIEW, EResourceSubType::SHADER_RESOURCE_VIEW>> srvProxy
					= ProxyCreator<EResourceType::GAPI_VIEW, EResourceSubType::SHADER_RESOURCE_VIEW>::create(srvDesc, outProxies);

				if( !srvProxy ) {
					// TODO: Log
					return false;
				}
				else {
					ResourceHandle handle(srvDesc.name, EResourceType::GAPI_VIEW, EResourceSubType::SHADER_RESOURCE_VIEW);
					outProxies[handle] = srvProxy;
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
			ResourceProxyMap              &outProxies)
		{
			if( (bindFlags.gpuBinding & (std::underlying_type_t<BufferBinding>)BufferBinding::ShaderOutput_RenderTarget) ) {
				RenderTargetDescriptor rtvDesc;
				rtvDesc.name          = String::format("%0_RTV", textureName);
				rtvDesc.textureFormat = textureFormat;
				rtvDesc.dimensionNb   = dimensionNb;
				rtvDesc.array         = arrayDesc;
				rtvDesc.mipMap        = mipMapDesc;

				Ptr<IResourceProxy<EResourceType::GAPI_VIEW, EResourceSubType::RENDER_TARGET_VIEW>> rtvProxy
					= ProxyCreator<EResourceType::GAPI_VIEW, EResourceSubType::RENDER_TARGET_VIEW>::create(rtvDesc, outProxies);
				if( !rtvProxy ) {
					// TODO: Log
					return false;
				}
				else {
					ResourceHandle handle(rtvDesc.name, EResourceType::GAPI_VIEW, EResourceSubType::RENDER_TARGET_VIEW);
					outProxies[handle] = rtvProxy;
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
			ResourceProxyMap              &outProxies)
		{
			if( (bindFlags.gpuBinding & (std::underlying_type_t<BufferBinding>)BufferBinding::ShaderOutput_DepthStencil) ) {
				DepthStencilViewDescriptor dsvDesc;
				dsvDesc.name          = String::format("%0_DSV", textureName);
				dsvDesc.textureFormat = textureFormat;
				dsvDesc.dimensionNb   = dimensionNb;
				dsvDesc.array         = arrayDesc;
				dsvDesc.mipMap        = mipMapDesc;

				Ptr<IResourceProxy<EResourceType::GAPI_VIEW, EResourceSubType::DEPTH_STENCIL_VIEW>> dsvProxy
					= ProxyCreator<EResourceType::GAPI_VIEW, EResourceSubType::DEPTH_STENCIL_VIEW>::create(dsvDesc, outProxies);
				if( !dsvProxy ) {
					// TODO: Log
					return false;
				}
				else {
					ResourceHandle handle(dsvDesc.name, EResourceType::GAPI_VIEW, EResourceSubType::DEPTH_STENCIL_VIEW);
					outProxies[handle] = dsvProxy;
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
			ResourceProxyMap              &outProxies)
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
				outProxies);

			// If the parent resource should be bound as a render target view, create a descriptor and proxy for it.
			result |= createTextureNDRTVProxy<N>(
				textureName,
				bindFlags,
				dimensionNb,
				arrayDesc,
				mipMapDesc,
				textureFormat,
				outProxies);

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
				outProxies);
		}
		
		template <>
		class ProxyCreator<EResourceType::TEXTURE, EResourceSubType::TEXTURE_1D> {
		public:
			using Descriptor = ResourceDescriptor<EResourceType::TEXTURE, EResourceSubType::TEXTURE_1D>;

			static ResourceHandle create(
				const Descriptor &desc,
				ResourceProxyMap &outProxies/*,
												 ResourceHierarchyNode &outResourceHierarchy*/)
			{
				Texture1DDescriptor t1DDesc = (Texture1DDescriptor)desc;
				Ptr<GFXAPIProxy> t1DProxy = Ptr<GFXAPIProxy>(new GFXAPIProxy(desc));

				bool isCubeMap      = false; // Not possible for 1D textures
				bool isCubeMapArray = false; // Not possible for 1D textures

				if( !createTextureNDDependerProxies<1>(
					t1DDesc.name,
					t1DDesc.gpuBinding,
					t1DDesc.dimensionNb,
					t1DDesc.dimensions,
					t1DDesc.array,
					t1DDesc.mipMap,
					t1DDesc.textureFormat,
					isCubeMap,
					outProxies) )
				{
				}
			}
		};

		template <>
		class ProxyCreator<EResourceType::TEXTURE, EResourceSubType::TEXTURE_2D> {
		public:
			using Descriptor = ResourceDescriptor<EResourceType::TEXTURE, EResourceSubType::TEXTURE_2D>;

			static ResourceHandle create(
				const Descriptor &desc,
				ResourceProxyMap &outProxies/*,
												 ResourceHierarchyNode &outResourceHierarchy*/)
			{
				Texture2DDescriptor t2DDesc = (Texture2DDescriptor)desc;

				bool isCubeMap      = (t2DDesc.array.isTextureArray && (t2DDesc.array.size % 6) == 0);
				bool isCubeMapArray = isCubeMap && ((t2DDesc.array.size / 6) > 1);

				if( !createTextureNDDependerProxies<1>(
					t2DDesc.name,
					t2DDesc.gpuBinding,
					t2DDesc.dimensionNb,
					t2DDesc.dimensions,
					t2DDesc.array,
					t2DDesc.mipMap,
					t2DDesc.textureFormat,
					isCubeMap,
					outProxies) )
				{
				}
			}
		};

		template <>
		class ProxyCreator<EResourceType::TEXTURE, EResourceSubType::TEXTURE_3D> {
		public:
			using Descriptor = ResourceDescriptor<EResourceType::TEXTURE, EResourceSubType::TEXTURE_3D>;

			static ResourceHandle create(
				const Descriptor &desc,
				ResourceProxyMap &outProxies/*,
												 ResourceHierarchyNode &outResourceHierarchy*/)
			{
				Texture3DDescriptor t3DDesc = (Texture3DDescriptor)desc;

				bool isCubeMap      = false; // Not possible for 3D textures
				bool isCubeMapArray = false; // Not possible for 3D textures

				if( !createTextureNDDependerProxies<1>(
					t3DDesc.name,
					t3DDesc.gpuBinding,
					t3DDesc.dimensionNb,
					t3DDesc.dimensions,
					t3DDesc.array,
					t3DDesc.mipMap,
					t3DDesc.textureFormat,
					isCubeMap,
					outProxies) )
				{
				}
			}
		};

	}

}

#endif