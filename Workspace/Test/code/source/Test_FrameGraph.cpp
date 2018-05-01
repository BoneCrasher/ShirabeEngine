#include <vector>

#include <Core/EngineTypeHelper.h>
#include <GFXAPI/Types/All.h>

#include <Resources/Core/IResourceManager.h>
#include <Renderer/FrameGraph/GraphBuilder.h>
#include <Renderer/FrameGraph/PassBuilder.h>
#include <Renderer/FrameGraph/Modules/GBufferGeneration.h>
#include <Renderer/FrameGraph/Modules/Lighting.h>

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

      GraphBuilder graphBuilder{};
      graphBuilder.initialize(appEnvironment);
      graphBuilder.importPersistentResource("BackBuffer", 1337);

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

      UniquePtr<Engine::FrameGraph::FrameGraph> frameGraph = graphBuilder.compile();

      // Renderer will call.
      if(frameGraph)
        frameGraph->execute();

      return true;
    }

  }
}