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
      std::string        const&id,
      PublicResourceId_t const&pid)
    {
      m_resourceMap[id].push_back(pid);
    }

    Vector<PublicResourceId_t>
      FrameGraphRenderContext::getMappedInternalResourceIds(std::string const&id) const
    {
      if(m_resourceMap.find(id) == m_resourceMap.end())
        return {};

      return m_resourceMap.at(id);
    }

    void
      FrameGraphRenderContext::removeMappedInternalResourceIds(std::string const&id)
    {
      m_resourceMap.erase(id);
    }

    EEngineStatus FrameGraphRenderContext::createTexture(
      FrameGraphTexture const&texture)
    {
      Texture::Descriptor desc{};
      desc.name        = texture.readableName;
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

      mapFrameGraphToInternalResource(texture.readableName, pid);

      return status;
    }

    EEngineStatus FrameGraphRenderContext::createTextureView(
      FrameGraphTexture      const&texture,
      FrameGraphTextureView  const&view)
    {
      if(view.source == FrameGraphViewSource::Color) {
        if(view.mode.check(FrameGraphViewAccessMode::Read)) {
          return createShaderResourceView(texture, view);
        }
        else if(view.mode.check(FrameGraphViewAccessMode::Write)) {
          return createRenderTargetView(texture, view);
        }
      }
      else if(view.source == FrameGraphViewSource::Depth) {
        return createDepthStencilView(texture, view);
      }

      return EEngineStatus::Error;
    }

    EEngineStatus FrameGraphRenderContext::createShaderResourceView(
      FrameGraphTexture      const&texture,
      FrameGraphTextureView  const&view)
    {
      Vector<PublicResourceId_t> const&subjacentResources = getMappedInternalResourceIds(texture.readableName);
      if(subjacentResources.empty())
        return EEngineStatus::Error;

      ShaderResourceView::Descriptor desc{};
      desc.name             = view.readableName;
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

      mapFrameGraphToInternalResource(view.readableName, pid);

      return status;
    }

    EEngineStatus FrameGraphRenderContext::createRenderTargetView(
      FrameGraphTexture      const&texture,
      FrameGraphTextureView  const&view)
    {
      Vector<PublicResourceId_t> const&subjacentResources = getMappedInternalResourceIds(texture.readableName);
      if(subjacentResources.empty())
        return EEngineStatus::Error;

      DepthStencilView::Descriptor desc{ };
      desc.name             = view.readableName;
      desc.format           = view.format;
      desc.subjacentTexture = texture;

      desc.arraySlices = view.arraySliceRange;
      desc.mipSlices   = view.mipSliceRange;

      DepthStencilView::CreationRequest request(desc, subjacentResources[0]);

      PublicResourceId_t pid = 0;
      EEngineStatus status = m_resourceManager->createDepthStencilView(request, pid, false);
      HandleEngineStatusError(status, "Failed to create texture.");

      mapFrameGraphToInternalResource(view.readableName, pid);

      return status;
    }

    EEngineStatus FrameGraphRenderContext::createDepthStencilView(
      FrameGraphTexture      const&texture,
      FrameGraphTextureView  const&view)
    {
      Vector<PublicResourceId_t> const&subjacentResources = getMappedInternalResourceIds(texture.readableName);
      if(subjacentResources.empty())
        return EEngineStatus::Error;

      RenderTargetView::Descriptor desc{ };
      desc.name             = view.readableName;
      desc.textureFormat    = view.format;
      desc.subjacentTexture = texture;

      desc.arraySlices  = view.arraySliceRange;
      desc.mipMapSlices = view.mipSliceRange;

      RenderTargetView::CreationRequest request(desc, subjacentResources[0]);

      PublicResourceId_t pid = 0;
      EEngineStatus status = m_resourceManager->createRenderTargetView(request, pid, false);
      HandleEngineStatusError(status, "Failed to create texture.");

      mapFrameGraphToInternalResource(view.readableName, pid);

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
      FrameGraphTextureView const&view)
    {
      Vector<PublicResourceId_t> const&subjacentResources = getMappedInternalResourceIds(view.readableName);
      for(PublicResourceId_t const&pid : subjacentResources)
        m_platformRendercontext->unbindResource(pid);

      return EEngineStatus::Ok;
    }

    EEngineStatus FrameGraphRenderContext::bindBufferView(FrameGraphResourceId_t  const&) { return EEngineStatus::Ok; }
    EEngineStatus FrameGraphRenderContext::bindMesh(AssetId_t const&) { return EEngineStatus::Ok; }

    EEngineStatus FrameGraphRenderContext::unbindTextureView(
     FrameGraphTextureView const&view)
    {
      Vector<PublicResourceId_t> const&subjacentResources = getMappedInternalResourceIds(view.readableName);
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
      FrameGraphRenderContext::destroyTexture(FrameGraphTexture const&texture)
    {
      Vector<PublicResourceId_t> const&subjacentResources = getMappedInternalResourceIds(texture.readableName);
      if(subjacentResources.empty())
        return EEngineStatus::Error;

      EEngineStatus status = m_resourceManager->destroyTexture(subjacentResources[0]);
      removeMappedInternalResourceIds(texture.readableName);

      return status;
    }

    EEngineStatus
      FrameGraphRenderContext::destroyTextureView(
        FrameGraphTextureView  const&view)
    {
      Vector<PublicResourceId_t> const&subjacentResources = getMappedInternalResourceIds(view.readableName);
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

      removeMappedInternalResourceIds(view.readableName);

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