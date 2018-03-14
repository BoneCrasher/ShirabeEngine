#ifndef __SHIRABE_FRAMEGRAPH_TEST_H___
#define __SHIRABE_FRAMEGRAPH_TEST_H___

#include <vector>

#include "Renderer/FrameGraph/GraphBuilder.h"
#include "Renderer/FrameGraph/PassLinker.h"

#include "Resources/Core/IResourceManager.h"
#include "GFXAPI/Types/All.h"

namespace Engine {
  namespace FrameGraph {
    using namespace Engine::Resources;
    using namespace Engine::GFXAPI;

    // NOTE:
    //   FrameGraphResourceId_t are just an index into a resource array.
    //   The important part about this is 
    std::vector<uint64_t> Range(uint64_t from_inclusive, uint64_t to_exclusive) {
      std::vector<uint64_t> r;
      for (uint64_t k = from_inclusive; k < to_exclusive; ++k)
        r.push_back(k);
      return r;
    }

    class GBufferPass {
    public:

      GBufferPass();
      ~GBufferPass();

      struct InputData {
        Map<FrameGraphResourceId_t, Vector<Ptr<IRenderable>>>
          renderables;
      };

      struct OutputData {
        FrameGraphRenderTarget
          gbuffer0,
          gbuffer1,
          gbuffer2,
          gbuffer3;
      };

      struct State {
        FrameGraphResourceId_t
          renderTargetTextureArray;
        std::vector<FrameGraphResourceId_t>
          renderTargetOutputs;
        FrameGraphResourceId_t
          renderableQueryId;
      };

      bool setup(
        PassLinker<GBufferPass>&passBuilder,
        InputData              &input,
        OutputData             &output);
      bool execute(
        Ptr<IRenderContext>      &context,
        InputData           const&input,
        OutputData          const&output);

    private:
      State m_state;
    };

    GBufferPass::GBufferPass()
      : m_state()
    { }

    GBufferPass::~GBufferPass()
    { }

    bool
      GBufferPass::setup(
        PassLinker<GBufferPass>&passLinker,
        InputData              &input,
        OutputData             &output)
    {
      Texture2D::Descriptor renderTargetTextureDescriptor = {};
      renderTargetTextureDescriptor.array.size = 4;

      // Basic underlying output buffer to be linked
      m_state.renderTargetTextureArray = passLinker.createResource<Texture2D>(renderTargetTextureDescriptor);

      // This will create a list of render targets for the texutre array to render to.
      // They'll be internally created and managed.
      m_state.renderTargetOutputs.resize(renderTargetTextureDescriptor.array.size);
      for (uint64_t const&k : Range(0, renderTargetTextureDescriptor.array.size))
        m_state.renderTargetOutputs[k] = passLinker.bindRenderTarget(m_state.renderTargetTextureArray, k);

      // Import renderable objects based on selector, flags, or whatever should be supported...
      m_state.renderableQueryId = passLinker.importRenderables();

      return true;
    }

    bool
      GBufferPass::execute(
        Ptr<IRenderContext>     &context,
        InputData          const&input,
        OutputData         const&output)
    {
      Vector<Ptr<IRenderable>> const&renderables = input.renderables.at(m_state.renderableQueryId);
      for (Ptr<IRenderable> const&r : renderables)
        context->render(r);

      return true;
    }

    class LightingPass {
    public:
      struct InputData {
        FrameGraphResource
          gbuffer0,
          gbuffer1,
          gbuffer2,
          gbuffer3;
      };

      struct OutputData {
        FrameGraphRenderTarget
          lightAccumulationBuffer;
      };

      bool setup(
        PassLinker<LightingPass>&passBuilder,
        InputData               &input,
        OutputData              &output) {
        return true;
      }

      bool execute(
        Ptr<IRenderContext>     &context,
        InputData          const&input,
        OutputData         const&output) {
        return true;
      }
    };

    static void foo() {
      Ptr<IRenderContext> context = nullptr;

      GraphBuilder graph;
      graph.initialize();
      graph.importPersistentResource("SwapChain", 1337);

      Ptr<Pass<GBufferPass>>  gbufferPass  = graph.spawnPass<GBufferPass>("GBufferPass");
      Ptr<Pass<LightingPass>> lightingPass = graph.spawnPass<LightingPass>("LightingPass");

      UniquePtr<FrameGraph> frameGraph = graph.compile();
      if (frameGraph)
        frameGraph->execute();
    }

  }
}

#endif