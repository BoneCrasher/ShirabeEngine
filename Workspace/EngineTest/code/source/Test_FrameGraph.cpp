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

#include <Renderer/FrameGraph/FrameGraphSerialization.h>

#include "Tests/Test_FrameGraph.h"

namespace Test {
  namespace FrameGraph {
    using namespace Engine;
    using namespace Engine::Renderer;

    class MockRenderer
      : public IRenderer
    {
      DeclareLogTag(MockRenderer);
    public:
      EEngineStatus initialize(
        const ApplicationEnvironment&,
        const RendererConfiguration&,
        const IResourceManagerPtr&);

      EEngineStatus deinitialize();
      EEngineStatus reinitialize();

      EEngineStatus pause();
      EEngineStatus resume();
      bool          isPaused() const;

      EEngineStatus render(Renderable const&);
    };

      EEngineStatus 
        MockRenderer::initialize(
          const ApplicationEnvironment&,
          const RendererConfiguration&,
          const IResourceManagerPtr&)
      {
        return EEngineStatus::Ok;
      }

      EEngineStatus
        MockRenderer::deinitialize()
      {
        return EEngineStatus::Ok;
      }

      EEngineStatus
        MockRenderer::reinitialize()
      {
        return EEngineStatus::Ok;
      }

      EEngineStatus
        MockRenderer::pause()
      {
        return EEngineStatus::Ok;
      }

      EEngineStatus
        MockRenderer::resume()
      { 
        return EEngineStatus::Ok;
      }

      bool
        MockRenderer::isPaused() const
      { 
        return false;
      }

      EEngineStatus
        MockRenderer::render(Renderable const&renderable)
      { 
        std::string message =
          String::format(
            "Renderable: %0\n"
            "  MeshId:     %1\n"
            "  MaterialId: %2\n",
            renderable.name,
            renderable.meshId,
            renderable.materialId);
        Log::Verbose(logTag(), message);

        return EEngineStatus::Ok;
      }
    

    class MockRenderContext
      : public IRenderContext
    {
      DeclareLogTag(MockRenderContext);
    public:
      static Ptr<IRenderContext> fromRenderer(Ptr<IRenderer> renderer) {
        assert(renderer != nullptr);

        Ptr<IRenderContext> context = Ptr<MockRenderContext>(new MockRenderContext(renderer));
        if(!context)
          Log::Error(logTag(), "Failed to create render context from renderer.");
        return context;
      }

      EEngineStatus render(Renderable const&renderable) {
          return m_renderer->render(renderable);
      }

    private:
      MockRenderContext(Ptr<IRenderer> renderer)
        : m_renderer(renderer)
      {}

      Ptr<IRenderer> m_renderer;
    };

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

      Ptr<IRenderer> renderer = MakeSharedPointerType<MockRenderer>();
      renderer->initialize(*appEnvironment, rendererConfiguration, nullptr);
      Ptr<IRenderContext> renderContext = MockRenderContext::fromRenderer(renderer);

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