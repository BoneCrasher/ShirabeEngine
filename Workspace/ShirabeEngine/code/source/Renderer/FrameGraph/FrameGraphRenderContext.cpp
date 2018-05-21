#include "Renderer/FrameGraph/FrameGraphRenderContext.h"

namespace Engine {
  namespace FrameGraph {

    Ptr<FrameGraphRenderContext>
      FrameGraphRenderContext::create(
        Ptr<IAssetStorage>    assetStorage,
        Ptr<IResourceManager> resourceManager,
        Ptr<IRenderContext>   renderer)
    {
      assert(assetStorage    != nullptr);
      assert(resourceManager != nullptr);
      assert(renderer        != nullptr);

      Ptr<FrameGraphRenderContext> context = Ptr<FrameGraphRenderContext>(new FrameGraphRenderContext(assetStorage, resourceManager, renderer));
      if(!context)
        Log::Error(logTag(), "Failed to create render context from renderer and resourcemanager.");
      return context;
    }

    FrameGraphRenderContext::FrameGraphRenderContext(
      Ptr<IAssetStorage>    assetStorage,
      Ptr<IResourceManager> resourceManager,
      Ptr<IRenderContext>   renderer)
      : m_assetStorage(assetStorage)
      , m_resourceManager(resourceManager)
      , m_platformRendercontext(renderer)
    {}

    void FrameGraphRenderContext::mapFrameGraphToInternalResource(
      FrameGraphResourceId_t const&fgid,
      PublicResourceId_t     const&pid)
    {
      m_resourceMap[fgid].push_back(pid);
    }

    Vector<PublicResourceId_t>
      FrameGraphRenderContext::getMappedInternalResourceIds(FrameGraphResourceId_t const&fgid) const
    {
      if(m_resourceMap.find(fgid) == m_resourceMap.end())
        return {};

      return m_resourceMap.at(fgid);
    }

    void
      FrameGraphRenderContext::removeMappedInternalResourceIds(FrameGraphResourceId_t const&fgid)
    {
      m_resourceMap.erase(fgid);
    }

    EEngineStatus FrameGraphRenderContext::createTexture(
      FrameGraphResourceId_t const&id,
      FrameGraphResource     const&resource,
      FrameGraphTexture      const&texture)
    {
      Texture::Descriptor desc{};
      desc.name        = resource.readableName;
      desc.textureInfo = texture;
      if(texture.requestedUsage.check(FrameGraphResourceUsage::RenderTarget))
        desc.gpuBinding.set(BufferBinding::ShaderOutput_RenderTarget);
      if(texture.requestedUsage.check(FrameGraphResourceUsage::DepthTarget))
        desc.gpuBinding.set(BufferBinding::ShaderOutput_DepthStencil);
      if(texture.requestedUsage.check(FrameGraphResourceUsage::ImageResource))
        desc.gpuBinding.set(BufferBinding::ShaderResource);
      desc.cpuGpuUsage = ResourceUsage::CPU_None_GPU_ReadWrite;

      Texture::CreationRequest request(desc);

      PublicResourceId_t pid = 0;
      EEngineStatus status = m_resourceManager->createTexture(request, pid, false);
      HandleEngineStatusError(status, "Failed to create texture.");

      mapFrameGraphToInternalResource(id, pid);

      return status;
    }

    EEngineStatus FrameGraphRenderContext::createTextureView(
      FrameGraphResourceId_t const&id,
      FrameGraphResource     const&resource,
      FrameGraphTexture      const&texture,
      FrameGraphTextureView  const&view)
    {
      if(view.source == FrameGraphViewSource::Color) {
        if(view.mode.check(FrameGraphViewAccessMode::Read)) {
          return createShaderResourceView(id, resource, texture, view);
        }
        else if(view.mode.check(FrameGraphViewAccessMode::Write)) {
          return createRenderTargetView(id, resource, texture, view);
        }
      }
      else if(view.source == FrameGraphViewSource::Depth) {
        return createDepthStencilView(id, resource, texture, view);
      }

      return EEngineStatus::Error;
    }

    EEngineStatus FrameGraphRenderContext::createShaderResourceView(
      FrameGraphResourceId_t const&id,
      FrameGraphResource     const&resource,
      FrameGraphTexture      const&texture,
      FrameGraphTextureView  const&view)
    {
      Vector<PublicResourceId_t> const&subjacentResources = getMappedInternalResourceIds(resource.subjacentResource);
      if(subjacentResources.empty())
        return EEngineStatus::Error;

      ShaderResourceView::Descriptor desc{};
      desc.name             = resource.readableName;
      desc.format           = view.format;
      desc.subjacentTexture = texture;
      desc.srvType = ShaderResourceView::Descriptor::EShaderResourceDimension::Texture;

      ShaderResourceView::Texture t{ };
      t.arraySlice = view.arraySliceRange;
      t.mipSlice   = view.mipSliceRange;
      desc.shaderResourceDimension = t;

      ShaderResourceView::CreationRequest request(desc, subjacentResources[0]);

      PublicResourceId_t pid = 0;
      EEngineStatus status = m_resourceManager->createShaderResourceView(request, pid, false);
      HandleEngineStatusError(status, "Failed to create texture.");

      mapFrameGraphToInternalResource(id, pid);

      return status;
    }

    EEngineStatus FrameGraphRenderContext::createRenderTargetView(
      FrameGraphResourceId_t const&id,
      FrameGraphResource     const&resource,
      FrameGraphTexture      const&texture,
      FrameGraphTextureView  const&view)
    {
      Vector<PublicResourceId_t> const&subjacentResources = getMappedInternalResourceIds(resource.subjacentResource);
      if(subjacentResources.empty())
        return EEngineStatus::Error;

      DepthStencilView::Descriptor desc{ };
      desc.name             = resource.readableName;
      desc.format           = view.format;
      desc.subjacentTexture = texture;

      desc.arraySlices = view.arraySliceRange;
      desc.mipSlices   = view.mipSliceRange;

      DepthStencilView::CreationRequest request(desc, subjacentResources[0]);

      PublicResourceId_t pid = 0;
      EEngineStatus status = m_resourceManager->createDepthStencilView(request, pid, false);
      HandleEngineStatusError(status, "Failed to create texture.");

      mapFrameGraphToInternalResource(id, pid);

      return status;
    }

    EEngineStatus FrameGraphRenderContext::createDepthStencilView(
      FrameGraphResourceId_t const&id,
      FrameGraphResource     const&resource,
      FrameGraphTexture      const&texture,
      FrameGraphTextureView  const&view)
    {
      Vector<PublicResourceId_t> const&subjacentResources = getMappedInternalResourceIds(resource.subjacentResource);
      if(subjacentResources.empty())
        return EEngineStatus::Error;

      RenderTargetView::Descriptor desc{ };
      desc.name             = resource.readableName;
      desc.textureFormat    = view.format;
      desc.subjacentTexture = texture;

      desc.arraySlices  = view.arraySliceRange;
      desc.mipMapSlices = view.mipSliceRange;

      RenderTargetView::CreationRequest request(desc, subjacentResources[0]);

      PublicResourceId_t pid = 0;
      EEngineStatus status = m_resourceManager->createRenderTargetView(request, pid, false);
      HandleEngineStatusError(status, "Failed to create texture.");

      mapFrameGraphToInternalResource(id, pid);

      return status;
    }

    EEngineStatus FrameGraphRenderContext::createBuffer(
      FrameGraphResourceId_t const&id,
      FrameGraphResource     const&resource,
      FrameGraphBuffer       const&buffer)
    {
      return EEngineStatus::Ok;
    }

    EEngineStatus FrameGraphRenderContext::createBufferView(
      FrameGraphResourceId_t const&id,
      FrameGraphResource     const&resource,
      FrameGraphBufferView   const&view)
    {
      return EEngineStatus::Ok;
    }

    EEngineStatus FrameGraphRenderContext::loadTextureAsset(AssetId_t const&) { return EEngineStatus::Ok; }
    EEngineStatus FrameGraphRenderContext::loadBufferAsset(AssetId_t const&) { return EEngineStatus::Ok; }
    EEngineStatus FrameGraphRenderContext::loadMeshAsset(AssetId_t const&) { return EEngineStatus::Ok; }

    EEngineStatus FrameGraphRenderContext::bindTextureView(
      FrameGraphResourceId_t const&id)
    {
      Vector<PublicResourceId_t> const&subjacentResources = getMappedInternalResourceIds(id);
      for(PublicResourceId_t const&pid : subjacentResources)
        m_platformRendercontext->unbindResource(pid);

      return EEngineStatus::Ok;
    }

    EEngineStatus FrameGraphRenderContext::bindBufferView(FrameGraphResourceId_t  const&) { return EEngineStatus::Ok; }
    EEngineStatus FrameGraphRenderContext::bindMesh(AssetId_t const&) { return EEngineStatus::Ok; }

    EEngineStatus FrameGraphRenderContext::unbindTextureView(
      FrameGraphResourceId_t const&id)
    {
      Vector<PublicResourceId_t> const&subjacentResources = getMappedInternalResourceIds(id);
      for(PublicResourceId_t const&pid : subjacentResources)
        m_platformRendercontext->unbindResource(pid);

      return EEngineStatus::Ok;
    }

    EEngineStatus FrameGraphRenderContext::unbindBufferView(FrameGraphResourceId_t  const&) { return EEngineStatus::Ok; }
    EEngineStatus FrameGraphRenderContext::unbindMesh(AssetId_t const&) { return EEngineStatus::Ok; }

    EEngineStatus FrameGraphRenderContext::unloadTextureAsset(AssetId_t const&) { return EEngineStatus::Ok; }
    EEngineStatus FrameGraphRenderContext::unloadBufferAsset(AssetId_t  const&) { return EEngineStatus::Ok; }
    EEngineStatus FrameGraphRenderContext::unloadMeshAsset(AssetId_t    const&) { return EEngineStatus::Ok; }

    EEngineStatus
      FrameGraphRenderContext::destroyTexture(FrameGraphResourceId_t const&id)
    {
      Vector<PublicResourceId_t> const&subjacentResources = getMappedInternalResourceIds(id);
      if(subjacentResources.empty())
        return EEngineStatus::Error;

      EEngineStatus status = m_resourceManager->destroyTexture(subjacentResources[0]);
      removeMappedInternalResourceIds(id);

      return status;
    }

    EEngineStatus
      FrameGraphRenderContext::destroyTextureView(
        FrameGraphResourceId_t const&id,
        FrameGraphTextureView  const&view)
    {
      Vector<PublicResourceId_t> const&subjacentResources = getMappedInternalResourceIds(id);
      if(subjacentResources.empty())
        return EEngineStatus::Error;

      EEngineStatus status = EEngineStatus::Ok;

      if(view.source == FrameGraphViewSource::Color) {
        if(view.mode.check(FrameGraphViewAccessMode::Read)) {
          status = m_resourceManager->destroyShaderResourceView(subjacentResources[0]);
        }
        else if(view.mode.check(FrameGraphViewAccessMode::Write)) {
          status = m_resourceManager->destroyRenderTargetView(subjacentResources[0]);
        }
      }
      else if(view.source == FrameGraphViewSource::Depth) {
        status = m_resourceManager->destroyDepthStencilView(subjacentResources[0]);
      }

      removeMappedInternalResourceIds(id);

      return status;
    }

    EEngineStatus
      FrameGraphRenderContext::destroyBuffer(FrameGraphResourceId_t const&id)
    {
      return EEngineStatus::Ok;
    }

    EEngineStatus
      FrameGraphRenderContext::destroyBufferView(FrameGraphResourceId_t const&id)
    {
      return EEngineStatus::Ok;
    }

    EEngineStatus FrameGraphRenderContext::render(Renderable const&renderable) { return EEngineStatus::Ok; }


  }
}