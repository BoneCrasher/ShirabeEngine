#include <vector>

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

#include "Tests/Test_FrameGraph.h"

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

    EEngineStatus MockRenderContext::bindResource(PublicResourceId_t const&) { return EEngineStatus::Ok; }
    EEngineStatus MockRenderContext::unbindResource(PublicResourceId_t const&) { return EEngineStatus::Ok; }
    
    EEngineStatus
      MockRenderContext::render(Renderable const&renderable)
    {
      std::string message =
        String::format(
          "operation -> render(Renderable const&):\n"
          "Renderable: %0\n"
          "  MeshId:     %1\n"
          "  MaterialId: %2\n",
          renderable.name,
          renderable.meshId,
          renderable.materialId);
      Log::Verbose(logTag(), message);

      return EEngineStatus::Ok;
    }


    class MockFrameGraphRenderContext
      : public IFrameGraphRenderContext
    {
      DeclareLogTag(MockFrameGraphRenderContext);
    public:
      static Ptr<IFrameGraphRenderContext> fromRenderer(Ptr<IRenderContext> renderer) {
        assert(renderer != nullptr);

        Ptr<FrameGraph::IFrameGraphRenderContext> context = Ptr<MockFrameGraphRenderContext>(new MockFrameGraphRenderContext(renderer));
        if(!context)
          Log::Error(logTag(), "Failed to create render context from renderer.");
        return context;
      }
      
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
      MockFrameGraphRenderContext::MockFrameGraphRenderContext(Ptr<IRenderContext> renderer)
        : m_renderer(renderer)
      {}

      Ptr<IRenderContext> m_renderer;
    };

    EEngineStatus MockFrameGraphRenderContext::importTexture(
      FrameGraphTexture const&texture)
    {
      std::cout << "ImportTexture(...):\n" << to_string(texture) << "\n";

      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::createTexture(
      FrameGraphTexture const&texture)
    {
        std::cout << "CreateTexture(...):\n" << to_string(texture) << "\n"; 

      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::createTextureView(
      FrameGraphTexture      const&texture,
      FrameGraphTextureView  const&view)
    {
      std::cout << "CreateTextureView(...):\n" << to_string(view) << "\n";
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::createBuffer(FrameGraphResourceId_t const&, FrameGraphResource const&, FrameGraphBuffer      const&buffer)
    {
      std::cout << "CreateBuffer(...):\n" << to_string(buffer) << "\n";
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::createBufferView(FrameGraphResourceId_t const&, FrameGraphResource const&, FrameGraphBufferView  const&view)
    {
      std::cout << "CreateBufferView(...):\n" << to_string(view) << "\n";
      return EEngineStatus::Ok;
    }


    EEngineStatus MockFrameGraphRenderContext::loadTextureAsset(AssetId_t const&)
    {
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::loadBufferAsset(AssetId_t  const&)
    {
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::loadMeshAsset(AssetId_t    const&)
    {
      return EEngineStatus::Ok;
    }


    EEngineStatus MockFrameGraphRenderContext::bindTextureView(
      FrameGraphTextureView const&view)
    {
      std::cout << "BindTextureView(...):\n" << to_string(view) << "\n";
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::bindBufferView(FrameGraphResourceId_t  const&)
    {
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::bindMesh(AssetId_t const&)
    {
      return EEngineStatus::Ok;
    }


    EEngineStatus MockFrameGraphRenderContext::unbindTextureView(
      FrameGraphTextureView const&view)
    {
      std::cout << "UnbindTextureView(...):\n" << to_string(view) << "\n";
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::unbindBufferView(FrameGraphResourceId_t  const&)
    {
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::unbindMesh(AssetId_t const&)
    {
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::unloadTextureAsset(AssetId_t const&)
    {
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::unloadBufferAsset(AssetId_t  const&)
    {
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::unloadMeshAsset(AssetId_t    const&)
    {
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::destroyTexture(
      FrameGraphTexture const&texture)
    {
      std::cout << "DestroyTexture(...):\n" << to_string(texture) << "\n";
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::destroyTextureView(
      FrameGraphTextureView  const&view)
    {
      std::cout << "DestroyTextureView(...):\n" << to_string(view) << "\n";
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::destroyBuffer(FrameGraphResourceId_t      const&)
    {
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::destroyBufferView(FrameGraphResourceId_t  const&)
    {
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::render(Renderable const&renderable)
    {
      std::cout << "Render(...):\n" << to_string(renderable) << "\n";
      return EEngineStatus::Ok;
    }

    bool
      Test__FrameGraph::testAll()
    {
      bool ok = true;

      ok |= testGraphBuilder();

      return ok;
    }

    bool
      Test__FrameGraph::testGraphBuilder()
    {
      using namespace Engine;
      using namespace Engine::Core;
      using namespace Engine::Resources;
      using namespace Engine::GFXAPI;
      using namespace Engine::FrameGraph;

      Ptr<Platform::ApplicationEnvironment> appEnvironment = MakeSharedPointerType<Platform::ApplicationEnvironment>();
      appEnvironment->osDisplays = Platform::OSDisplay::GetDisplays();

      RendererConfiguration rendererConfiguration{};

      Ptr<IRenderContext> renderer = MakeSharedPointerType<MockRenderContext>();
      // renderer->initialize(*appEnvironment, rendererConfiguration, nullptr);
      Ptr<IFrameGraphRenderContext> renderContext = MockFrameGraphRenderContext::fromRenderer(renderer);

      OSDisplayDescriptor const&displayDesc = appEnvironment->primaryDisplay();

      uint32_t
        width  = displayDesc.bounds.size.x(),
        height = displayDesc.bounds.size.y();

      GraphBuilder graphBuilder{};
      graphBuilder.initialize(appEnvironment);

      FrameGraphTexture backBufferTextureDesc{};
      backBufferTextureDesc.width          = width;
      backBufferTextureDesc.height         = height;
      backBufferTextureDesc.depth          = 1;
      backBufferTextureDesc.format         = FrameGraphFormat::R8G8B8A8_UNORM;
      backBufferTextureDesc.initialState   = FrameGraphResourceInitState::Clear;
      backBufferTextureDesc.arraySize      = 1;
      backBufferTextureDesc.mipLevels      = 1;
      backBufferTextureDesc.permittedUsage = FrameGraphResourceUsage::RenderTarget;

      FrameGraphResource backBuffer{ };
      backBuffer = graphBuilder.registerTexture("BackBuffer", backBufferTextureDesc);

      RenderableList renderableCollection ={
        { "Cube",    0, 0 },
        { "Sphere",  0, 0 },
        { "Pyramid", 0, 0 }
      };
      FrameGraphResource renderables{ };
      renderables = graphBuilder.registerRenderables("SceneRenderables", renderableCollection);

      // GBuffer
      FrameGraphModule<GBufferModuleTag_t> gbufferModule{};
      FrameGraphModule<GBufferModuleTag_t>::GBufferGenerationExportData gbufferExportData{};
      gbufferExportData = gbufferModule.addGBufferGenerationPass(
        graphBuilder,
        renderables);

      // Lighting
      FrameGraphModule<LightingModuleTag_t> lightingModule{};
      FrameGraphModule<LightingModuleTag_t>::LightingExportData lightingExportData{};
      lightingExportData = lightingModule.addLightingPass(
        graphBuilder,
        gbufferExportData.gbuffer0,
        gbufferExportData.gbuffer1,
        gbufferExportData.gbuffer2,
        gbufferExportData.gbuffer3);

      // Compositing
      FrameGraphModule<CompositingModuleTag_t> compositingModule{ };
      FrameGraphModule<CompositingModuleTag_t>::ExportData compositingExportData{ };
      compositingExportData = compositingModule.addDefaultCompositingPass(
        graphBuilder,
        gbufferExportData.gbuffer0,
        gbufferExportData.gbuffer1,
        gbufferExportData.gbuffer2,
        gbufferExportData.gbuffer3,
        lightingExportData.lightAccumulationBuffer,
        backBuffer);

      UniquePtr<Engine::FrameGraph::Graph> frameGraph = graphBuilder.compile();

      Ptr<FrameGraphGraphVizSerializer> serializer = std::make_shared<FrameGraphGraphVizSerializer>();
      serializer->initialize();

      frameGraph->acceptSerializer(serializer);
      serializer->writeToFile("FrameGraphTest");

      serializer->deinitialize();
      serializer = nullptr;

      system("makeGraphPNG.bat");

      // Renderer will call.
      if(frameGraph)
        frameGraph->execute(renderContext);

      return true;
    }

  }
}