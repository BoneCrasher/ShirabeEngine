#include <vector>

#include "Resources/Core/IResourceManager.h"
#include "GFXAPI/Types/All.h"


#include "Renderer/FrameGraph/GraphBuilder.h"
#include "Renderer/FrameGraph/PassLinker.h"
#include "Renderer/FrameGraph/Passes/GBufferGeneration.h"
#include "Renderer/FrameGraph/Passes/Lighting.h"

#include "Test_FrameGraph.h"

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

      Ptr<IRenderContext> context = nullptr;

      Ptr<Platform::ApplicationEnvironment> appEnvironment = MakeSharedPointerType<Platform::ApplicationEnvironment>();
      appEnvironment->osDisplays = Platform::OSDisplay::GetDisplays();

      GraphBuilder graph;
      graph.initialize(appEnvironment);
      graph.importPersistentResource("BackBuffer", 1337);

      // GBuffer
      Ptr<Pass<GBufferGenerationPass>>  gbufferPass  = graph.spawnPass<GBufferGenerationPass>("GBufferPass");

      // Lighting
      GBufferGenerationPass::OutputData const& gbufferOutputData = gbufferPass->outputData();
      Ptr<Pass<LightingPass>> lightingPass
        = graph.spawnPass<LightingPass>(
          "LightingPass",
          gbufferOutputData.gbuffer0,
          gbufferOutputData.gbuffer1,
          gbufferOutputData.gbuffer2,
          gbufferOutputData.gbuffer3);

      UniquePtr<Engine::FrameGraph::FrameGraph> frameGraph = graph.compile();

      // Renderer will call.
      if(frameGraph)
        frameGraph->execute();

      return true;
    }

  }
}