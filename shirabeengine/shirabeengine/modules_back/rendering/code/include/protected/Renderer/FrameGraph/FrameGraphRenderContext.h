#ifndef __SHIRABE_FRAMEGRAPH_RENDERCONTEXT_H__
#define __SHIRABE_FRAMEGRAPH_RENDERCONTEXT_H__

#include "core/enginetypehelper.h"
#include "Log/Log.h"

#include "Asset/AssetStorage.h"
#include "Resources/Core/ResourceManager.h"

#include "Renderer/RendererTypes.h"
#include "Renderer/FrameGraph/FrameGraphData.h"

namespace Engine {

  namespace Rendering {
    class IRenderContext;
  }

  namespace FrameGraph {
    using Engine::Asset::AssetId_t;
    using Engine::Asset::IAssetStorage;
    using namespace Engine::Resources;
    using namespace Engine::GFXAPI;
    using namespace Engine::Rendering;

    DeclareInterface(IFrameGraphRenderContext);

    virtual EEngineStatus importTexture(
      FrameGraphTexture const&texture) = 0;

    virtual EEngineStatus createTexture(
      FrameGraphTexture const&texture) = 0;
    virtual EEngineStatus createTextureView(
      FrameGraphTexture     const&texture,
      FrameGraphTextureView const&view) = 0;
    virtual EEngineStatus createBuffer(FrameGraphResourceId_t      const&, FrameGraphResource const&, FrameGraphBuffer      const&) = 0;
    virtual EEngineStatus createBufferView(FrameGraphResourceId_t  const&, FrameGraphResource const&, FrameGraphBufferView  const&) = 0;

    virtual EEngineStatus loadTextureAsset(AssetId_t const&) = 0;
    virtual EEngineStatus loadBufferAsset(AssetId_t  const&) = 0;
    virtual EEngineStatus loadMeshAsset(AssetId_t    const&) = 0;

    virtual EEngineStatus bindTextureView(
      FrameGraphTextureView const&view) = 0;
    virtual EEngineStatus bindBufferView(FrameGraphResourceId_t  const&) = 0;
    virtual EEngineStatus bindMesh(AssetId_t const&)                     = 0;

    virtual EEngineStatus unbindTextureView(
      FrameGraphTextureView const&view) = 0;
    virtual EEngineStatus unbindBufferView(FrameGraphResourceId_t  const&) = 0;
    virtual EEngineStatus unbindMesh(AssetId_t const&)                     = 0;

    virtual EEngineStatus unloadTextureAsset(AssetId_t const&) = 0;
    virtual EEngineStatus unloadBufferAsset(AssetId_t  const&) = 0;
    virtual EEngineStatus unloadMeshAsset(AssetId_t    const&) = 0;

    virtual EEngineStatus destroyTexture(
      FrameGraphTexture const&texture) = 0;
    virtual EEngineStatus destroyTextureView(
      FrameGraphTextureView  const&view) = 0;
    virtual EEngineStatus destroyBuffer(FrameGraphResourceId_t      const&) = 0;
    virtual EEngineStatus destroyBufferView(FrameGraphResourceId_t  const&) = 0;

    virtual EEngineStatus render(Renderable const&renderable) = 0;

    DeclareInterfaceEnd(IFrameGraphRenderContext);

    class SHIRABE_TEST_EXPORT FrameGraphRenderContext
      : public IFrameGraphRenderContext
    {
      SHIRABE_DECLARE_LOG_TAG(FrameGraphRenderContext);
    public:
      static CStdSharedPtr_t<FrameGraphRenderContext> create(
        CStdSharedPtr_t<IAssetStorage>   assetStorage,
        CStdSharedPtr_t<ResourceManager> resourceManager,
        CStdSharedPtr_t<IRenderContext>  renderer);

      EEngineStatus importTexture(
        FrameGraphTexture const&texture);

      EEngineStatus createTexture(
        FrameGraphTexture      const&texture);
      EEngineStatus createTextureView(
        FrameGraphTexture      const&texture,
        FrameGraphTextureView  const&view);
      EEngineStatus createBuffer(FrameGraphResourceId_t const&, FrameGraphResource const&, FrameGraphBuffer      const&);
      EEngineStatus createBufferView(FrameGraphResourceId_t const&, FrameGraphResource const&, FrameGraphBufferView  const&);

      EEngineStatus loadTextureAsset(AssetId_t const&);
      EEngineStatus loadBufferAsset(AssetId_t  const&);
      EEngineStatus loadMeshAsset(AssetId_t    const&);

      EEngineStatus bindTextureView(
        FrameGraphTextureView  const&view);
      EEngineStatus bindBufferView(FrameGraphResourceId_t  const&);
      EEngineStatus bindMesh(AssetId_t const&);

      EEngineStatus unbindTextureView(
        FrameGraphTextureView  const&view);
      EEngineStatus unbindBufferView(FrameGraphResourceId_t  const&);
      EEngineStatus unbindMesh(AssetId_t const&);

      EEngineStatus unloadTextureAsset(AssetId_t const&);
      EEngineStatus unloadBufferAsset(AssetId_t  const&);
      EEngineStatus unloadMeshAsset(AssetId_t    const&);

      EEngineStatus destroyTexture(
        FrameGraphTexture const&texture);
      EEngineStatus destroyTextureView(
        FrameGraphTextureView  const&view);
      EEngineStatus destroyBuffer(FrameGraphResourceId_t      const&);
      EEngineStatus destroyBufferView(FrameGraphResourceId_t  const&);

      EEngineStatus render(Renderable const&renderable);

    private:
      FrameGraphRenderContext(
        CStdSharedPtr_t<IAssetStorage>   assetStorage,
        CStdSharedPtr_t<ResourceManager> resourceManager,
        CStdSharedPtr_t<IRenderContext>  renderer);

      void
        mapFrameGraphToInternalResource(
          std::string        const&,
          PublicResourceId_t const&);
      Vector<PublicResourceId_t>
        getMappedInternalResourceIds(
          std::string const&) const;
      void
        removeMappedInternalResourceIds(
          std::string const&);

      CStdSharedPtr_t<IAssetStorage>   m_assetStorage;
      CStdSharedPtr_t<ResourceManager> m_resourceManager;
      CStdSharedPtr_t<IRenderContext>  m_platformRenderContext;

      Map<std::string, Vector<PublicResourceId_t>> m_resourceMap;
    };

  }
}

#endif