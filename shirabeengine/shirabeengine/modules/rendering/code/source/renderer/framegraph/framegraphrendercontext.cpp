#include <assert.h>

#include "Renderer/IRenderer.h"
#include "Renderer/FrameGraph/FrameGraphRenderContext.h"

namespace engine {
  namespace framegraph {
    using namespace engine::Rendering;

    CStdSharedPtr_t<FrameGraphRenderContext>
      FrameGraphRenderContext::create(
        CStdSharedPtr_t<IAssetStorage>   assetStorage,
        CStdSharedPtr_t<ResourceManager> resourceManager,
        CStdSharedPtr_t<IRenderContext>  renderer)
    {
      assert(assetStorage    != nullptr);
      assert(resourceManager != nullptr);
      assert(renderer        != nullptr);

      CStdSharedPtr_t<FrameGraphRenderContext> context = CStdSharedPtr_t<FrameGraphRenderContext>(new FrameGraphRenderContext(assetStorage, resourceManager, renderer));
      if(!context)
        Log::Error(logTag(), "Failed to create render context from renderer and resourcemanager.");
      return context;
    }

    FrameGraphRenderContext::FrameGraphRenderContext(
      CStdSharedPtr_t<IAssetStorage>   assetStorage,
      CStdSharedPtr_t<ResourceManager> resourceManager,
      CStdSharedPtr_t<IRenderContext>  renderer)
      : m_assetStorage(assetStorage)
      , m_resourceManager(resourceManager)
      , m_platformRenderContext(renderer)
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

    EEngineStatus FrameGraphRenderContext::importTexture(
      FrameGraphTexture const&texture)
    {
      PublicResourceId_t pid = "";
      mapFrameGraphToInternalResource(texture.readableName, pid);
      return EEngineStatus::Ok;
    }

    EEngineStatus FrameGraphRenderContext::createTexture(
      FrameGraphTexture const&texture)
    {
      Texture::Descriptor desc{};
      desc.name        = texture.readableName;
      desc.textureInfo = texture;
      if(texture.requestedUsage.check(FrameGraphResourceUsage::RenderTarget))
        desc.gpuBinding.set(BufferBinding::ColorAttachement);
      if(texture.requestedUsage.check(FrameGraphResourceUsage::DepthTarget))
        desc.gpuBinding.set(BufferBinding::DepthAttachement);
      if(texture.requestedUsage.check(FrameGraphResourceUsage::ImageResource))
        desc.gpuBinding.set(BufferBinding::InputAttachement);

      desc.cpuGpuUsage = ResourceUsage::CPU_None_GPU_ReadWrite;

      Texture::CreationRequest request(desc);

      Log::Verbose(logTag(), String::format("Texture:\n%0", to_string(texture)));

      EEngineStatus status = m_resourceManager->createResource<Texture>(request, texture.readableName, false);
      if(EEngineStatus::ResourceManager_ResourceAlreadyCreated == status)
        return EEngineStatus::Ok;
      else 
        HandleEngineStatusError(status, "Failed to create texture.");

      return status;
    }

    EEngineStatus FrameGraphRenderContext::createTextureView(
      FrameGraphTexture      const&texture,
      FrameGraphTextureView  const&view)
    {
      Log::Verbose(logTag(), String::format("TextureView:\n%0", to_string(view)));
      
      TextureView::Descriptor desc{ };
      desc.name             = view.readableName;
      desc.textureFormat    = view.format;
      desc.subjacentTexture = texture;
      desc.arraySlices      = view.arraySliceRange;
      desc.mipMapSlices     = view.mipSliceRange;
      desc.dependencies.push_back(texture.readableName);

      TextureView::CreationRequest request(desc, texture.readableName);

      EEngineStatus status = m_resourceManager->createResource<TextureView>(request, view.readableName, false);
      HandleEngineStatusError(status, "Failed to create texture.");

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
      Log::Verbose(logTag(), String::format("TextureView:\n%0", to_string(view)));

      Vector<PublicResourceId_t> const&subjacentResources = getMappedInternalResourceIds(view.readableName);
      for(PublicResourceId_t const&pid : subjacentResources)
        m_platformRenderContext->bindResource(pid);

      return EEngineStatus::Ok;
    }

    EEngineStatus FrameGraphRenderContext::bindBufferView(FrameGraphResourceId_t  const&) { return EEngineStatus::Ok; }
    EEngineStatus FrameGraphRenderContext::bindMesh(AssetId_t const&) { return EEngineStatus::Ok; }

    EEngineStatus FrameGraphRenderContext::unbindTextureView(
     FrameGraphTextureView const&view)
    {
      Log::Verbose(logTag(), String::format("TextureView:\n%0", to_string(view)));

      Vector<PublicResourceId_t> const&subjacentResources = getMappedInternalResourceIds(view.readableName);
      for(PublicResourceId_t const&pid : subjacentResources)
        m_platformRenderContext->unbindResource(pid);

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
      Log::Verbose(logTag(), String::format("Texture:\n%0", to_string(texture)));

      EEngineStatus status = m_resourceManager->destroyResource<Texture>(texture.readableName);

      return status;
    }

    EEngineStatus
      FrameGraphRenderContext::destroyTextureView(
        FrameGraphTextureView  const&view)
    {
      Log::Verbose(logTag(), String::format("TextureView:\n%0", to_string(view)));

      EEngineStatus status = EEngineStatus::Ok;
      status = m_resourceManager->destroyResource<TextureView>(view.readableName);

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