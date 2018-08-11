#ifndef __SHIRABE_TEST_FRAMEGRAPH_MOCKS_H__
#define __SHIRABE_TEST_FRAMEGRAPH_MOCKS_H__

#include <Log/Log.h>

#include <Core/EngineTypeHelper.h>
#include <GFXAPI/Types/All.h>

#include <Resources/Core/IResourceManager.h>
#include <Resources/Core/ProxyBasedResourceManager.h>
#include <Resources/Core/ResourceProxyFactory.h>

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
      SHIRABE_DECLARE_LOG_TAG(MockRenderContext);
    public:

      EEngineStatus bindResource(PublicResourceId_t const&);
      EEngineStatus unbindResource(PublicResourceId_t const&);
      EEngineStatus render(Renderable const&);
    };

    class MockFrameGraphRenderContext
      : public IFrameGraphRenderContext
    {
      SHIRABE_DECLARE_LOG_TAG(MockFrameGraphRenderContext);
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
        resource::CUpdateRequest const&inRequest           \
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
      SHIRABE_DECLARE_LOG_TAG(MockResourceManager);
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
	
    #define Mock_DeclareTaskBuilderModule(Type)                                                                                                                  \
          EEngineStatus creationTask   (Type::CreationRequest    const&request, ResolvedDependencyCollection const&depencies, ResourceTaskFn_t &outTask); \
          EEngineStatus updateTask     (Type::CUpdateRequest      const&request, ResolvedDependencyCollection const&depencies, ResourceTaskFn_t &outTask); \
          EEngineStatus destructionTask(Type::CDestructionRequest const&request, ResolvedDependencyCollection const&depencies, ResourceTaskFn_t &outTask); \
          EEngineStatus queryTask      (Type::Query              const&request, ResourceTaskFn_t &outTask);     

    class MockGFXAPITaskBackend
      : public GFXAPIResourceTaskBackend<EngineTypes>
    {
      SHIRABE_DECLARE_LOG_TAG(MockGFXAPITaskBackend);
    public:
      Mock_DeclareTaskBuilderModule(Texture);
      Mock_DeclareTaskBuilderModule(ShaderResourceView);
      Mock_DeclareTaskBuilderModule(RenderTargetView);
      Mock_DeclareTaskBuilderModule(DepthStencilView);
      Mock_DeclareTaskBuilderModule(DepthStencilState);
      Mock_DeclareTaskBuilderModule(RasterizerState);
      Mock_DeclareTaskBuilderModule(SwapChain);
      Mock_DeclareTaskBuilderModule(SwapChainBuffer);
    };
  }
}

#endif