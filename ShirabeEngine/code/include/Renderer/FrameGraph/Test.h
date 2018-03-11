#ifndef __SHIRABE_FRAMEGRAPH_TEST_H___
#define __SHIRABE_FRAMEGRAPH_TEST_H___

#include "Renderer/FrameGraph/GraphBuilder.h"

#include "Resources/Core/IResourceManager.h"
#include "GFXAPI/Types/All.h"

namespace Engine {
  namespace FrameGraph {
    using namespace Engine::Resources;
    using namespace Engine::GFXAPI;

    // NOTE:
    //   FrameGraphResourceId_t are just an index into a resource array.
    //   The important part about this is 

    class GBufferPass {
      GBufferPass();
      ~GBufferPass();

      struct InputData {

      };

      struct OutputData {
        FrameGraphResourceId_t 
          gbuffer0, 
          gbuffer1,
          gbuffer2,
          gbuffer3;
      };

      struct State {
        FrameGraphResourceId_t renderTargetTextureArray;
      };

      bool setup(PassBuilder<GBufferPass>&passBuilder);
      bool execute();

    private:
      State m_state;
    };

    GBufferPass::GBufferPass()
      : m_state()
    { }

    GBufferPass::~GBufferPass() 
    { }

    bool 
      GBufferPass::setup(PassBuilder<GBufferPass>&passBuilder) 
    {
      Texture2D::Descriptor renderTargetTextureDescriptor = {};
      renderTargetTextureDescriptor.array.size = 4;

      m_state.renderTargetTextureArray = passBuilder.createResource<Texture2D>(renderTargetTextureDescriptor);
      
      return true;
    }

    bool 
      GBufferPass::execute() 
    {
      return true;
    }

    class LightingPass {
      struct InputData {

      };

      struct OutputData {

      };

      bool setup(PassBuilder<LightingPass>&passBuilder) {
        return true;
      }

      bool execute() {
        return true;
      }
    };

    static void foo() {

      GraphBuilder graph;
      graph.initialize();
      graph.importPersistentResource("SwapChain", 1337);

      Ptr<Pass<GBufferPass>> gbufferPass = graph.spawnPass<GBufferPass>("GBufferPass");      
      PassBuilder<GBufferPass> gbufferPassBuilder(gbufferPass);
      gbufferPass->setup(gbufferPassBuilder);

      Ptr<Pass<LightingPass>> lightingPass = graph.spawnPass<LightingPass>("LightingPass");
      PassBuilder<LightingPass> lightingPassBuilder(lightingPass);
      lightingPass->setup(lightingPassBuilder);

      gbufferPass->execute();
      lightingPass->execute();

    }

  }
}

#endif