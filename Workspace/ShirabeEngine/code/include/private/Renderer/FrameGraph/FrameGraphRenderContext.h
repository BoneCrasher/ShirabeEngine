#ifndef __SHIRABE_FRAMEGRAPH_RENDERCONTEXT_H__
#define __SHIRABE_FRAMEGRAPH_RENDERCONTEXT_H__

#include "Core/EngineTypeHelper.h"
#include "Log/Log.h"

#include "Asset/AssetStorage.h"
#include "Resources/Core/IResourceManager.h"

#include "Renderer/IRenderer.h"
#include "Renderer/FrameGraph/FrameGraphData.h"

namespace Engine {
  namespace FrameGraph {
    using Engine::Asset::AssetId_t;
    using Engine::Asset::IAssetStorage;
    using namespace Engine::Renderer;

    DeclareInterface(IFrameGraphRenderContext);

    virtual EEngineStatus createTexture(
      FrameGraphResourceId_t const&id,
      FrameGraphResource     const&resource,
      FrameGraphTexture      const&texture) = 0;
    virtual EEngineStatus createTextureView(
      FrameGraphResourceId_t const&id,
      FrameGraphResource     const&resource,
      FrameGraphTexture      const&texture,
      FrameGraphTextureView  const&view) = 0;
    virtual EEngineStatus createBuffer(FrameGraphResourceId_t      const&, FrameGraphResource const&, FrameGraphBuffer      const&) = 0;
    virtual EEngineStatus createBufferView(FrameGraphResourceId_t  const&, FrameGraphResource const&, FrameGraphBufferView  const&) = 0;

    virtual EEngineStatus loadTextureAsset(AssetId_t const&) = 0;
    virtual EEngineStatus loadBufferAsset(AssetId_t  const&) = 0;
    virtual EEngineStatus loadMeshAsset(AssetId_t    const&) = 0;

    virtual EEngineStatus bindTextureView(
      FrameGraphResourceId_t const&id) = 0;
    virtual EEngineStatus bindBufferView(FrameGraphResourceId_t  const&) = 0;
    virtual EEngineStatus bindMesh(AssetId_t const&)                     = 0;

    virtual EEngineStatus unbindTextureView(
      FrameGraphResourceId_t const&id) = 0;
    virtual EEngineStatus unbindBufferView(FrameGraphResourceId_t  const&) = 0;
    virtual EEngineStatus unbindMesh(AssetId_t const&)                     = 0;

    virtual EEngineStatus unloadTextureAsset(AssetId_t const&) = 0;
    virtual EEngineStatus unloadBufferAsset(AssetId_t  const&) = 0;
    virtual EEngineStatus unloadMeshAsset(AssetId_t    const&) = 0;

    virtual EEngineStatus destroyTexture(
      FrameGraphResourceId_t const&) = 0;
    virtual EEngineStatus destroyTextureView(
      FrameGraphResourceId_t const&id,
      FrameGraphTextureView  const&view) = 0;
    virtual EEngineStatus destroyBuffer(FrameGraphResourceId_t      const&) = 0;
    virtual EEngineStatus destroyBufferView(FrameGraphResourceId_t  const&) = 0;

    virtual EEngineStatus render(Renderable const&renderable) = 0;

    DeclareInterfaceEnd(IFrameGraphRenderContext);

    class FrameGraphRenderContext
      : public IFrameGraphRenderContext
    {
      DeclareLogTag(FrameGraphRenderContext);
    public:
      static Ptr<FrameGraphRenderContext> create(
        Ptr<IAssetStorage>    assetStorage,
        Ptr<IResourceManager> resourceManager,
        Ptr<IRenderContext>   renderer);

      EEngineStatus createTexture(
        FrameGraphResourceId_t const&id,
        FrameGraphResource     const&resource,
        FrameGraphTexture      const&texture);
      EEngineStatus createTextureView(
        FrameGraphResourceId_t const&id,
        FrameGraphResource     const&resource,
        FrameGraphTexture      const&texture,
        FrameGraphTextureView  const&view);
      EEngineStatus createBuffer(FrameGraphResourceId_t const&, FrameGraphResource const&, FrameGraphBuffer      const&);
      EEngineStatus createBufferView(FrameGraphResourceId_t const&, FrameGraphResource const&, FrameGraphBufferView  const&);

      EEngineStatus loadTextureAsset(AssetId_t const&);
      EEngineStatus loadBufferAsset(AssetId_t  const&);
      EEngineStatus loadMeshAsset(AssetId_t    const&);

      EEngineStatus bindTextureView(
        FrameGraphResourceId_t const&id);
      EEngineStatus bindBufferView(FrameGraphResourceId_t  const&);
      EEngineStatus bindMesh(AssetId_t const&);

      EEngineStatus unbindTextureView(
        FrameGraphResourceId_t const&id);
      EEngineStatus unbindBufferView(FrameGraphResourceId_t  const&);
      EEngineStatus unbindMesh(AssetId_t const&);

      EEngineStatus unloadTextureAsset(AssetId_t const&);
      EEngineStatus unloadBufferAsset(AssetId_t  const&);
      EEngineStatus unloadMeshAsset(AssetId_t    const&);

      EEngineStatus destroyTexture(
        FrameGraphResourceId_t const&);
      EEngineStatus destroyTextureView(
        FrameGraphResourceId_t const&id,
        FrameGraphTextureView  const&view);
      EEngineStatus destroyBuffer(FrameGraphResourceId_t      const&);
      EEngineStatus destroyBufferView(FrameGraphResourceId_t  const&);

      EEngineStatus render(Renderable const&renderable);

    private:
      FrameGraphRenderContext(
        Ptr<IAssetStorage>    assetStorage,
        Ptr<IResourceManager> resourceManager,
        Ptr<IRenderContext>   renderer);

      EEngineStatus createShaderResourceView(
        FrameGraphResourceId_t const&id,
        FrameGraphResource     const&resource,
        FrameGraphTexture      const&texture,
        FrameGraphTextureView  const&view);
      EEngineStatus createRenderTargetView(
        FrameGraphResourceId_t const&id,
        FrameGraphResource     const&resource,
        FrameGraphTexture      const&texture,
        FrameGraphTextureView  const&view);
      EEngineStatus createDepthStencilView(
        FrameGraphResourceId_t const&id,
        FrameGraphResource     const&resource,
        FrameGraphTexture      const&texture,
        FrameGraphTextureView  const&view);

      void
        mapFrameGraphToInternalResource(
          FrameGraphResourceId_t const&,
          PublicResourceId_t     const&);
      Vector<PublicResourceId_t>
        getMappedInternalResourceIds(
          FrameGraphResourceId_t const&) const;
      void
        removeMappedInternalResourceIds(
          FrameGraphResourceId_t const&);

      Ptr<IAssetStorage>    m_assetStorage;
      Ptr<IResourceManager> m_resourceManager;
      Ptr<IRenderContext>   m_platformRendercontext;

      Map<FrameGraphResourceId_t, Vector<PublicResourceId_t>> m_resourceMap;
    };

  }
}

#endif