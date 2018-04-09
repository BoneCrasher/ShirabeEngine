#include "Renderer/FrameGraph/Passes/GBufferGeneration.h"

namespace Engine {
	namespace FrameGraph {

		// NOTE:
		//   FrameGraphResourceId_t are just an index into a resource array.
		//   The important part about this is 
		std::vector<uint64_t> ValueRange(uint64_t from_inclusive, uint64_t to_exclusive) {
			std::vector<uint64_t> r;
			for(uint64_t k = from_inclusive; k < to_exclusive; ++k)
				r.push_back(k);
			return r;
		}

		GBufferGenerationPass::GBufferGenerationPass()
			: m_state()
		{ }

		GBufferGenerationPass::~GBufferGenerationPass()
		{ }

		bool
			GBufferGenerationPass::setup(
        Ptr<ApplicationEnvironment>       const&environment,
        PassLinker<GBufferGenerationPass>      &passLinker,
        InputData                              &input,
        OutputData                             &output)
		{
      uint32_t
        width  = 1920,
        height = 1080;

      if(environment) {
        OSDisplayDescriptor const&displayDesc = environment->primaryDisplay();
        width  = displayDesc.bounds.size.x();
        height = displayDesc.bounds.size.y();
      }

      FrameGraphTexture gbufferDesc={};
      gbufferDesc.width        = width;
      gbufferDesc.height       = height;
      gbufferDesc.depth        = 1;
      gbufferDesc.format       = FrameGraphFormat::R8G8B8A8_UNORM;
      gbufferDesc.initialState = FrameGraphResourceInitState::Clear;
      gbufferDesc.arraySize    = 4;
      gbufferDesc.mipLevels    = 1;

			// Basic underlying output buffer to be linked
			m_state.gbufferTextureArrayId = passLinker.createTexture(gbufferDesc);

			// This will create a list of render targets for the texutre array to render to.
			// They'll be internally created and managed.
      FrameGraphWriteTextureFlags flags{};
      flags.requiredFormat = gbufferDesc.format;
      flags.writeTarget    = FrameGraphWriteTarget::Color;

      output.gbuffer0 = passLinker.writeTexture(m_state.gbufferTextureArrayId, flags, Range(0, 1), Range(0, 1));
			output.gbuffer1 = passLinker.writeTexture(m_state.gbufferTextureArrayId, flags, Range(1, 1), Range(0, 1));
			output.gbuffer2 = passLinker.writeTexture(m_state.gbufferTextureArrayId, flags, Range(2, 1), Range(0, 1));
			output.gbuffer3 = passLinker.writeTexture(m_state.gbufferTextureArrayId, flags, Range(3, 1), Range(0, 1));

			// Import renderable objects based on selector, flags, or whatever should be supported...
			input.renderableQueryId = passLinker.importRenderables();

			return true;
		}

		bool
			GBufferGenerationPass::execute(
				Ptr<IRenderContext>     &context,
				InputData          const&input,
				OutputData         const&output)
		{
			// Vector<Ptr<IRenderable>> const&renderables = input.renderables.at(m_state.renderableQueryId);
			// for(Ptr<IRenderable> const&r : renderables)
			// 	context->render(r);

			return true;
		}

	}
}