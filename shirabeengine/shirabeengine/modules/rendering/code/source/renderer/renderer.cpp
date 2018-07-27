#include "Renderer/Renderer.h"

#include "Renderer/FrameGraph/GraphBuilder.h"
#include "Renderer/FrameGraph/PassBuilder.h"
#include "Renderer/FrameGraph/Modules/GBufferGeneration.h"
#include "Renderer/FrameGraph/Modules/Lighting.h"
#include "Renderer/FrameGraph/Modules/Compositing.h"
         
#include "Renderer/FrameGraph/FrameGraphRenderContext.h"
#include "Renderer/FrameGraph/FrameGraphSerialization.h"

namespace Engine {
  namespace Rendering {

    Renderer::Renderer()
      : m_configuration()
      , m_appEnvironment()
      , m_frameGraphRenderContext(nullptr)
      , m_paused(true)
    {}

    Renderer::~Renderer() {
    }

    EEngineStatus 
      Renderer::initialize(
        CStdSharedPtr_t<ApplicationEnvironment>   const&environment,
        RendererConfiguration         const&configuration,
        CStdSharedPtr_t<IFrameGraphRenderContext>      &frameGraphRenderContext)
    {
      assert(nullptr != environment);
      assert(nullptr != frameGraphRenderContext);

      m_configuration           = configuration;
      m_appEnvironment          = environment;
      m_frameGraphRenderContext = frameGraphRenderContext;

      return EEngineStatus::Ok;
    }

    EEngineStatus 
      Renderer::deinitialize()
    {
      return EEngineStatus::Ok;
    }

    EEngineStatus 
      Renderer::reinitialize()
    {
      return EEngineStatus::Ok;
    }
    EEngineStatus 
      Renderer::pause()
    {
      return EEngineStatus::Ok;
    }

    EEngineStatus 
      Renderer::resume()
    {
      return EEngineStatus::Ok;
    }

    bool          
      Renderer::isPaused() const
    {
      return m_paused.load();
    }
    
    EEngineStatus
      Renderer::renderScene()
    {
      using namespace Engine;
      using namespace Engine::FrameGraph;
            
      OSDisplayDescriptor const&displayDesc = m_appEnvironment->primaryDisplay();

      uint32_t
        width  = displayDesc.bounds.size.x(),
        height = displayDesc.bounds.size.y();

      GraphBuilder graphBuilder{ };
      graphBuilder.initialize(m_appEnvironment);

      FrameGraphTexture backBufferTextureDesc{ };
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
      FrameGraphModule<GBufferModuleTag_t> gbufferModule{ };
      FrameGraphModule<GBufferModuleTag_t>::GBufferGenerationExportData gbufferExportData{ };
      gbufferExportData = gbufferModule.addGBufferGenerationPass(
        graphBuilder,
        renderables);

      // Lighting
      FrameGraphModule<LightingModuleTag_t> lightingModule{ };
      FrameGraphModule<LightingModuleTag_t>::LightingExportData lightingExportData{ };
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

      UniqueCStdSharedPtr_t<Engine::FrameGraph::Graph> frameGraph = graphBuilder.compile();

      CStdSharedPtr_t<FrameGraphGraphVizSerializer> serializer = std::make_shared<FrameGraphGraphVizSerializer>();
      serializer->initialize();

      frameGraph->acceptSerializer(serializer);
      serializer->writeToFile("FrameGraphTest");

      serializer->deinitialize();
      serializer = nullptr;

      system("makeGraphPNG.bat");

      // Renderer will call.
      if(frameGraph)
        frameGraph->execute(m_frameGraphRenderContext);

      return EEngineStatus::Ok;
    }

  }
}