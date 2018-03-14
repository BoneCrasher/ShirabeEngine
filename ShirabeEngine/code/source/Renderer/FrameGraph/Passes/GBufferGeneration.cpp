#include "Renderer/FrameGraph/Passes/GBufferGeneration.h"

namespace Engine {
	namespace FrameGraph {

		// NOTE:
		//   FrameGraphResourceId_t are just an index into a resource array.
		//   The important part about this is 
		std::vector<uint64_t> Range(uint64_t from_inclusive, uint64_t to_exclusive) {
			std::vector<uint64_t> r;
			for(uint64_t k = from_inclusive; k < to_exclusive; ++k)
				r.push_back(k);
			return r;
		}

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
			Texture2D::Descriptor renderTargetTextureDescriptor ={ };
			// TODO: Fill desc
			renderTargetTextureDescriptor.array.size = 4;

			// Basic underlying output buffer to be linked
			output.gbuffer0 = passLinker.createResource<Texture2D>(renderTargetTextureDescriptor);
			output.gbuffer1 = passLinker.createResource<Texture2D>(renderTargetTextureDescriptor);
			output.gbuffer2 = passLinker.createResource<Texture2D>(renderTargetTextureDescriptor);
			output.gbuffer3 = passLinker.createResource<Texture2D>(renderTargetTextureDescriptor);

			// This will create a list of render targets for the texutre array to render to.
			// They'll be internally created and managed.
			m_state.renderTargetBindings.resize(renderTargetTextureDescriptor.array.size);
			m_state.renderTargetBindings[0] = passLinker.bindRenderTarget(output.gbuffer0, 0);
			m_state.renderTargetBindings[1] = passLinker.bindRenderTarget(output.gbuffer1, 1);
			m_state.renderTargetBindings[2] = passLinker.bindRenderTarget(output.gbuffer2, 2);
			m_state.renderTargetBindings[3] = passLinker.bindRenderTarget(output.gbuffer3, 3);

			// Import renderable objects based on selector, flags, or whatever should be supported...
			input.renderableQueryId = passLinker.importRenderables();

			return true;
		}

		bool
			GBufferPass::execute(
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