#include <vector>

#include <Core/EngineTypeHelper.h>
#include <GFXAPI/Types/All.h>

#include <Resources/Core/IResourceManager.h>
#include <Renderer/FrameGraph/GraphBuilder.h>
#include <Renderer/FrameGraph/PassBuilder.h>
#include <Renderer/FrameGraph/Modules/GBufferGeneration.h>
#include <Renderer/FrameGraph/Modules/Lighting.h>
#include <Renderer/FrameGraph/Modules/Compositing.h>

#include "Tests/Test_FrameGraph.h"

namespace Test {
  namespace FrameGraph {

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

      // Ptr<IRenderContext> context = nullptr;

      Ptr<Platform::ApplicationEnvironment> appEnvironment = MakeSharedPointerType<Platform::ApplicationEnvironment>();
      appEnvironment->osDisplays = Platform::OSDisplay::GetDisplays();

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

      // GBuffer
      FrameGraphModule<GBufferModuleTag_t> gbufferModule{};
      FrameGraphModule<GBufferModuleTag_t>::GBufferGenerationExportData gbufferExportData{};
      gbufferExportData = gbufferModule.addGBufferGenerationPass(graphBuilder);

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

      UniquePtr<Engine::FrameGraph::FrameGraph> frameGraph = graphBuilder.compile();

      // Renderer will call.
      if(frameGraph)
        frameGraph->execute();

      return true;
    }

  }
}