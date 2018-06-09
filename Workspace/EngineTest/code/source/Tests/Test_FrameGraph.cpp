#include <vector>

#include <Core/EngineTypeHelper.h>

#include <Asset/AssetIndex.h>
#include <Asset/AssetStorage.h>

#include <Resources/Core/ResourceManager.h>
#include <Resources/Core/ResourceProxyFactory.h>
#include <Resources/Subsystems/GFXAPI/Types/All.h>

#include <Renderer/IRenderer.h>
#include <Renderer/FrameGraph/GraphBuilder.h>
#include <Renderer/FrameGraph/PassBuilder.h>
#include <Renderer/FrameGraph/Modules/GBufferGeneration.h>
#include <Renderer/FrameGraph/Modules/Lighting.h>
#include <Renderer/FrameGraph/Modules/Compositing.h>

#include <Renderer/FrameGraph/FrameGraphRenderContext.h>
#include <Renderer/FrameGraph/FrameGraphSerialization.h>

#include "Tests/Test_FrameGraph.h"
#include "Tests/Test_FrameGraph_Mocks.h"

namespace Test {
  namespace FrameGraph {
    using namespace Engine;
    using namespace Engine::Renderer;
    using namespace Engine::FrameGraph;

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
      appEnvironment->instanceHandle         = 0;
      appEnvironment->previousInstanceHandle = 0;
      appEnvironment->osDisplays             = Platform::OSDisplay::GetDisplays(appEnvironment->primaryDisplayIndex);


      //
      // ASSET STORAGE
      // 
      namespace asset = Engine::Asset;
      asset::AssetRegistry<asset::Asset> registry = asset::AssetIndex::loadIndexById("Default");
      
      Ptr<asset::AssetStorage> assetStorage = MakeSharedPointerType<asset::AssetStorage>();
      assetStorage->readIndex(registry);

      //
      // RESOURCE MANAGEMENT
      // 
      Ptr<MockGFXAPITaskBackend> gfxApiResourceTaskBackend = MakeSharedPointerType<MockGFXAPITaskBackend>();
      gfxApiResourceTaskBackend->initialize();

      Ptr<GFXAPIResourceBackend> gfxApiResourceBackend = MakeSharedPointerType<GFXAPIResourceBackend>();
      gfxApiResourceBackend->setResourceTaskBackend(gfxApiResourceTaskBackend);

      Ptr<ResourceProxyFactory> resourceProxyFactory = MakeSharedPointerType<ResourceProxyFactory>(gfxApiResourceBackend);
      Ptr<ResourceManager>      proxyResourceManager = MakeSharedPointerType<ResourceManager>(resourceProxyFactory);
      proxyResourceManager->setResourceBackend(gfxApiResourceBackend);

      gfxApiResourceBackend->initialize();

      //
      // RENDERING
      // 
      RendererConfiguration rendererConfiguration{};
      Ptr<IRenderContext> renderer = MakeSharedPointerType<MockRenderContext>();
      // renderer->initialize(*appEnvironment, rendererConfiguration, nullptr);
      // 
      Ptr<IFrameGraphRenderContext> renderContext = FrameGraphRenderContext::create(assetStorage, proxyResourceManager, renderer);

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

      gfxApiResourceBackend->deinitialize();

      return true;
    }

  }
}