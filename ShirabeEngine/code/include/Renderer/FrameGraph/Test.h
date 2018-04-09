#ifndef __SHIRABE_FRAMEGRAPH_TEST_H___
#define __SHIRABE_FRAMEGRAPH_TEST_H___

#include <vector>

#include "Resources/Core/IResourceManager.h"
#include "GFXAPI/Types/All.h"


#include "Renderer/FrameGraph/GraphBuilder.h"
#include "Renderer/FrameGraph/PassLinker.h"
#include "Renderer/FrameGraph/Passes/GBufferGeneration.h"
#include "Renderer/FrameGraph/Passes/Lighting.h"

namespace Engine {
  namespace FrameGraph {
    using namespace Engine::Resources;
    using namespace Engine::GFXAPI;
    
    static void foo() {
      Ptr<IRenderContext>         context     = nullptr;
      Ptr<ApplicationEnvironment> environment = nullptr;

      GraphBuilder graph;
      graph.initialize(environment);
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

      UniquePtr<FrameGraph> frameGraph = graph.compile();

      // Renderer will call.
      if(frameGraph)
        frameGraph->execute();
    }

  }
}

#endif