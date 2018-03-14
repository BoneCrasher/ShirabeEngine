#include "Renderer/FrameGraph/Passes/Lighting.h"

namespace Engine {
	namespace FrameGraph {


		LightingPass::LightingPass()  {

		}

		LightingPass::~LightingPass() {}

		bool 
			LightingPass::setup(
				PassLinker<LightingPass>&passLinker,
				InputData               &input,
				OutputData              &output,
				FrameGraphResource const&gbuffer0,
				FrameGraphResource const&gbuffer1,
				FrameGraphResource const&gbuffer2,
				FrameGraphResource const&gbuffer3)
		{
			input.gbuffer0 = passLinker.bindInput(gbuffer0);
			input.gbuffer1 = passLinker.bindInput(gbuffer1);
			input.gbuffer2 = passLinker.bindInput(gbuffer2);
			input.gbuffer3 = passLinker.bindInput(gbuffer3);

			Texture2D::Descriptor lightAccBufferDesc ={ };
			// TODO: Fill desc
			output.lightAccumulationBuffer = passLinker.createResource<Texture2D>(lightAccBufferDesc);
			
			m_state.renderTargetBinding = passLinker.bindRenderTarget(output.lightAccumulationBuffer, 0, 1, 0);

			return true;
		}

		bool 
			LightingPass::execute(
				Ptr<IRenderContext>     &context,
				InputData          const&input, 
				OutputData         const&output)
		{
			return true;
		}
	}
}