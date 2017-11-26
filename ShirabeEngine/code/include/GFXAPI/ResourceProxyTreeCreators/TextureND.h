#ifndef __SHIRABE_DX11TEXTURENDPROXYCREATOR_H__
#define __SHIRABE_DX11TEXTURENDPROXYCREATOR_H__

#include "Core/EngineStatus.h"
#include "Log/Log.h"

#include "Resources/System/Core/Handle.h"
#include "Resources/System/Core/ResourceDomainTransfer.h"
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
      // Input
      const std::string                   &textureName,
      BufferBindingFlags_t                 bindFlags,
      const unsigned int                   dimensionNb,
      const VecND<uint32_t, N>            &dimensions,
      const TextureArrayDescriptor        &arrayDesc,
      const TextureMipMapDescriptor       &mipMapDesc,
      const Format                        &textureFormat,
      bool                                 isCube,
      const ResourceHandle                &textureNDProxyHandle,
      const Ptr<ResourceProxyFactory>     &proxyFactory,
      // Output
      ResourceBinding<ShaderResourceView> &outSRVBinding,
      ResourceProxyMap                    &outSRVProxy,
      DependerTreeNode                    &outSRVHierarchy)
    {
      if((bindFlags & (std::underlying_type_t<BufferBinding>)BufferBinding::ShaderResource)) {
        ShaderResourceViewDescriptor srvDesc;
        srvDesc.name                                        = String::format("%0_SRV", textureName);
        srvDesc.format                                      = textureFormat;
        srvDesc.shaderResourceDimension.texture.dimensionNb = dimensionNb;
        srvDesc.shaderResourceDimension.texture.dimensions  = dimensions; // Will truncate or expand...
        srvDesc.shaderResourceDimension.texture.array       = arrayDesc;
        srvDesc.shaderResourceDimension.texture.isCube      = isCube;
        srvDesc.shaderResourceDimension.texture.mipMap      = mipMapDesc;

        ShaderResourceViewCreationRequest srvCreationRequest(srvDesc);

        ShaderResourceViewBinding binding;
        ResourceProxyMap          proxies;
        DependerTreeNodeList      hierarchy;

        ResourceHandleList dependencyInjection ={textureNDProxyHandle};

        // There will only be exactly one proxy, which is the SRV root. 
        // SRVs in DirectX can basically handle any combination of Textures, Arrays, MipMaps etc.
        // Vulkan is much more complex regarding textures and binding to the Pipeline.
        // OpenGL doesn't really GAF anyway, but is a bit more complex regarding texture binding
        // and update and needs wrapper classes to handle those 5 million calls in OGL.
        // These will basically be the SRV and RTV implementations on the respective platforms.
        TextureNDSRVProxyPtr srvProxy
          = ProxyTreeCreator<ShaderResourceView>::create(proxyFactory, srvCreationRequest, dependencyInjection, binding, proxies, hierarchy);

        // Verify?

        if(!srvProxy) {
          // TODO: Log
          return false;
        }
        else {
          ResourceHandle handle(srvDesc.name, EResourceType::GAPI_VIEW, EResourceSubType::SHADER_RESOURCE_VIEW);
          outSRVBinding   = binding;

          outSRVProxy[handle] = srvProxy;
          for(ResourceProxyMap::value_type const& p : proxies)
            outSRVProxy[p.first] = p.second;

          outSRVHierarchy = hierarchy.at(0);
        }
      }

      return true;
    }

    template <std::size_t N>
    static bool createTextureNDRTVProxy(
      const std::string                 &textureName,
      BufferBindingFlags_t               bindFlags,
      const unsigned int                 dimensionNb,
      const TextureArrayDescriptor      &arrayDesc,
      const TextureMipMapDescriptor     &mipMapDesc,
      const Format                      &textureFormat,
      const ResourceHandle              &textureNDProxyHandle,
      const Ptr<ResourceProxyFactory>   &proxyFactory,
      ResourceBinding<RenderTargetView> &outRTVBinding,
      ResourceProxyMap                  &outRTVProxy,
      DependerTreeNode                  &outRTVHierarchy)
    {
      if((bindFlags & (std::underlying_type_t<BufferBinding>)BufferBinding::ShaderOutput_RenderTarget)) {
        RenderTargetViewDescriptor rtvDesc ={0};
        rtvDesc.name          = String::format("%0_RTV", textureName);
        rtvDesc.textureFormat = textureFormat;
        rtvDesc.dimensionNb   = dimensionNb;
        rtvDesc.array         = arrayDesc;
        rtvDesc.mipMap        = mipMapDesc;

        RenderTargetViewCreationRequest rtvCreationRequest(rtvDesc);

        ResourceBinding<RenderTargetView> binding;
        ResourceProxyMap                  proxies;
        DependerTreeNodeList              hierarchy;

        ResourceHandleList dependencyInjection ={textureNDProxyHandle};

        TextureNDRTVProxyPtr rtvProxy
          = ProxyTreeCreator<RenderTargetView>::create(proxyFactory, rtvCreationRequest, dependencyInjection, binding, proxies, hierarchy);
        if(!rtvProxy) {
          // TODO: Log
          return false;
        }
        else {
          ResourceHandle handle(rtvDesc.name, EResourceType::GAPI_VIEW, EResourceSubType::RENDER_TARGET_VIEW);
          outRTVBinding = binding;

          outRTVProxy[handle] = rtvProxy;
          for(ResourceProxyMap::value_type const& p : proxies)
            outRTVProxy[p.first] = p.second;

          outRTVHierarchy = hierarchy.at(0);
        }
      }

      return true;
    }

    template <std::size_t N>
    static bool createTextureNDDSVProxy(
      const std::string                 &textureName,
      BufferBindingFlags_t               bindFlags,
      const unsigned int                 dimensionNb,
      const VecND<uint32_t, N>          &dimensions,
      const TextureArrayDescriptor      &arrayDesc,
      const TextureMipMapDescriptor     &mipMapDesc,
      const Format                      &textureFormat,
      bool                               isCube,
      const ResourceHandle              &textureNDProxyHandle,
      const Ptr<ResourceProxyFactory>   &proxyFactory,
      ResourceBinding<DepthStencilView> &outDSVBinding,
      ResourceProxyMap                  &outDSVProxy,
      DependerTreeNode                  &outDSVHierarchy)
    {
      if((bindFlags & (std::underlying_type_t<BufferBinding>)BufferBinding::ShaderOutput_DepthStencil)) {
        DepthStencilViewDescriptor dsvDesc;
        dsvDesc.name                = String::format("%0_DSV", textureName);
        dsvDesc.format              = textureFormat;
        dsvDesc.texture.dimensionNb = dimensionNb;
        dsvDesc.texture.array       = arrayDesc;
        dsvDesc.texture.mipMap      = mipMapDesc;

        DepthStencilViewCreationRequest dsvCreationRequest(dsvDesc);

        ResourceBinding<DepthStencilView> binding;
        ResourceProxyMap                  proxies;
        DependerTreeNodeList              hierarchy;

        ResourceHandleList dependencyInjection ={textureNDProxyHandle};

        TextureNDDSVProxyPtr dsvProxy
          = ProxyTreeCreator<DepthStencilView>::create(proxyFactory, dsvCreationRequest, dependencyInjection, binding, proxies, hierarchy);
        if(!dsvProxy) {
          // TODO: Log
          return false;
        }
        else {
          ResourceHandle handle(dsvDesc.name, EResourceType::GAPI_VIEW, EResourceSubType::DEPTH_STENCIL_VIEW);
          outDSVBinding = binding;

          outDSVProxy[handle] = dsvProxy;
          for(ResourceProxyMap::value_type const& p : proxies)
            outDSVProxy[p.first] = p.second;

          outDSVHierarchy = hierarchy.at(0);
        }
      }

      return true;
    }

    template <std::size_t N>
    static bool createTextureNDDependerProxies(
      const std::string               &textureName,
      BufferBindingFlags_t             bindFlags,
      const unsigned int               dimensionNb,
      const VecND<uint32_t, N>        &dimensions,
      const TextureArrayDescriptor    &arrayDesc,
      const TextureMipMapDescriptor   &mipMapDesc,
      const Format                    &textureFormat,
      bool                             isCube,
      const ResourceHandle            &textureNDProxyHandle,
      const Ptr<ResourceProxyFactory> &proxyFactory,
      TextureNDResourceBinding        &inOutBinding,
      ResourceProxyMap                &inOutProxies,
      DependerTreeNode                &inOutHierarchyRoot)
    {
      bool result = true;

      ResourceBinding<ShaderResourceView> srvBinding;
      ResourceProxyMap                    srvProxy;
      DependerTreeNode                    srvHierarchy;

      ResourceBinding<RenderTargetView> rtvBinding;
      ResourceProxyMap                  rtvProxy;
      DependerTreeNode                  rtvHierarchy;

      ResourceBinding<DepthStencilView> dsvBinding;
      ResourceProxyMap                  dsvProxy;
      DependerTreeNode                  dsvHierarchy;

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
        proxyFactory,
        srvBinding,
        srvProxy,
        srvHierarchy);

      if(result
         && srvBinding.handle.valid()
         && !srvProxy.empty())
      {
        inOutProxies[srvBinding.handle] = AnyProxy(srvProxy);
        inOutBinding.srvBinding         = srvBinding;
        inOutHierarchyRoot.children.push_back(srvHierarchy);
      }

      // If the parent resource should be bound as a render target view, create a descriptor and proxy for it.
      result |= createTextureNDRTVProxy<N>(
        textureName,
        bindFlags,
        dimensionNb,
        arrayDesc,
        mipMapDesc,
        textureFormat,
        textureNDProxyHandle,
        proxyFactory,
        rtvBinding,
        rtvProxy,
        rtvHierarchy);

      if(result
         && rtvBinding.handle.valid()
         && !rtvProxy.empty())
      {
        inOutProxies[rtvBinding.handle] = AnyProxy(rtvProxy);
        inOutBinding.rtvBinding         = rtvBinding;
        inOutHierarchyRoot.children.push_back(rtvHierarchy);
      }

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
        proxyFactory,
        dsvBinding,
        dsvProxy,
        dsvHierarchy);

      if(result
         && dsvBinding.handle.valid()
         && !dsvProxy.empty())
      {
        inOutProxies[dsvBinding.handle] = AnyProxy(dsvProxy);
        inOutBinding.dsvBinding         = dsvBinding;
        inOutHierarchyRoot.children.push_back(dsvHierarchy);
      }
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
          = proxyFactory->create<Texture1D>(EProxyType::Dynamic, request, inDependencyHandles);

        ResourceHandle handle(t1DDesc.name, EResourceType::TEXTURE, EResourceSubType::TEXTURE_1D);
        outProxies[handle] = AnyProxy(proxy);

        DependerTreeNode resourceNode;
        resourceNode.resourceHandle = handle;

        bool isCubeMap      = false; // Not possible for 1D textures
        bool isCubeMapArray = false; // Not possible for 1D textures

        TextureNDResourceBinding binding;
        binding.handle = handle;

        outResourceHierarchy.push_back(resourceNode);

        if(!createTextureNDDependerProxies<1>(
          t1DDesc.name,
          t1DDesc.gpuBinding,
          t1DDesc.dimensionNb,
          t1DDesc.dimensions,
          t1DDesc.array,
          t1DDesc.mipMap,
          t1DDesc.textureFormat,
          isCubeMap,
          handle,
          proxyFactory,
          binding,
          outProxies,
          resourceNode))
        {
        }

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

        TextureNDResourceBinding binding;
        binding.handle = handle;

        outResourceHierarchy.push_back(resourceNode);

        if(!createTextureNDDependerProxies<2>(
          t2DDesc.name,
          t2DDesc.gpuBinding,
          t2DDesc.dimensionNb,
          t2DDesc.dimensions,
          t2DDesc.array,
          t2DDesc.mipMap,
          t2DDesc.textureFormat,
          isCubeMap,
          handle,
          proxyFactory,
          binding,
          outProxies,
          resourceNode))
        {
        }
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
          = proxyFactory->create<Texture3D>(EProxyType::Dynamic, request, inDependencyHandles);

        ResourceHandle handle(t3DDesc.name, resource_type, resource_subtype);
        outProxies[handle] = AnyProxy(proxy);

        DependerTreeNode resourceNode;
        resourceNode.resourceHandle = handle;

        bool isCubeMap      = false; // Not possible for 3D textures
        bool isCubeMapArray = false; // Not possible for 3D textures

        TextureNDResourceBinding binding;
        binding.handle = handle;

        outResourceHierarchy.push_back(resourceNode);

        if(!createTextureNDDependerProxies<3>(
          t3DDesc.name,
          t3DDesc.gpuBinding,
          t3DDesc.dimensionNb,
          t3DDesc.dimensions,
          t3DDesc.array,
          t3DDesc.mipMap,
          t3DDesc.textureFormat,
          isCubeMap,
          handle,
          proxyFactory,
          binding,
          outProxies,
          resourceNode))
        {
        }
      }
    };

  }

}

#endif