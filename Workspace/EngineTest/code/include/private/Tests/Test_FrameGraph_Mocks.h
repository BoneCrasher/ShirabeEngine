#ifndef __SHIRABE_TEST_FRAMEGRAPH_MOCKS_H__
#define __SHIRABE_TEST_FRAMEGRAPH_MOCKS_H__

#include <Log/Log.h>

#include <Core/EngineTypeHelper.h>
#include <GFXAPI/Types/All.h>

#include <Resources/Core/IResourceManager.h>

#include <Renderer/IRenderer.h>
#include <Renderer/FrameGraph/GraphBuilder.h>
#include <Renderer/FrameGraph/PassBuilder.h>
#include <Renderer/FrameGraph/Modules/GBufferGeneration.h>
#include <Renderer/FrameGraph/Modules/Lighting.h>
#include <Renderer/FrameGraph/Modules/Compositing.h>

#include <Renderer/FrameGraph/FrameGraphRenderContext.h>
#include <Renderer/FrameGraph/FrameGraphSerialization.h>

namespace Test {
  namespace FrameGraph {
    using namespace Engine;
    using namespace Engine::Renderer;
    using namespace Engine::FrameGraph;

    class MockRenderContext
      : public IRenderContext
    {
      DeclareLogTag(MockRenderContext);
    public:

      EEngineStatus bindResource(PublicResourceId_t const&);
      EEngineStatus unbindResource(PublicResourceId_t const&);
      EEngineStatus render(Renderable const&);
    };

    class MockFrameGraphRenderContext
      : public IFrameGraphRenderContext
    {
      DeclareLogTag(MockFrameGraphRenderContext);
    public:
      static Ptr<IFrameGraphRenderContext> fromRenderer(Ptr<IRenderContext> renderer);

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
        FrameGraphTextureView const&view);
      EEngineStatus bindBufferView(FrameGraphResourceId_t  const&);
      EEngineStatus bindMesh(AssetId_t const&);

      EEngineStatus unbindTextureView(
        FrameGraphTextureView const&view);
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
      MockFrameGraphRenderContext(Ptr<IRenderContext> renderer);

      Ptr<IRenderContext> m_renderer;
    };

	 
    #define Mock_DeclareResourceMethods(resource)         \
                                                          \
      EEngineStatus create##resource(                     \
        resource::CreationRequest const&inRequest,        \
        PublicResourceId_t             &outId,            \
        bool                            deferLoad = false \
      );                                                  \
                                                          \
      EEngineStatus load##resource(                       \
        PublicResourceId_t const&inId                     \
      );                                                  \
                                                          \
      EEngineStatus update##resource(                     \
        PublicResourceId_t      const&inId,               \
        resource::UpdateRequest const&inRequest           \
      );                                                  \
                                                          \
      EEngineStatus unload##resource(                     \
        PublicResourceId_t const&inId                     \
      );                                                  \
                                                          \
      EEngineStatus destroy##resource(                    \
        PublicResourceId_t const&inId                     \
      );                                   

    class MockResourceManager
      : public IResourceManager
    {
      DeclareLogTag(MockResourceManager);
    public:
      bool clear();

      Ptr<BasicGFXAPIResourceBackend>& backend();

      Mock_DeclareResourceMethods(SwapChain);
      Mock_DeclareResourceMethods(Texture);
      Mock_DeclareResourceMethods(RenderTargetView);
      Mock_DeclareResourceMethods(ShaderResourceView);
      Mock_DeclareResourceMethods(DepthStencilView);
      Mock_DeclareResourceMethods(DepthStencilState);
      Mock_DeclareResourceMethods(RasterizerState);
    };
	
  }
}

#endif