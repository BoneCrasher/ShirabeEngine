#ifndef __SHIRABE_FRAMEGRAPH_PASSES_LIGHTING_H__
#define __SHIRABE_FRAMEGRAPH_PASSES_LIGHTING_H__


#include "Renderer/FrameGraph/GraphBuilder.h"
#include "Renderer/FrameGraph/PassLinker.h"

#include "Resources/Core/IResourceManager.h"
#include "GFXAPI/Types/All.h"

namespace Engine {
	namespace FrameGraph {

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
				FrameGraphResource
					lightAccumulationBuffer;
			};

			struct State {
				FrameGraphResource
					renderTargetBinding;
			};

			LightingPass();
			~LightingPass();

			bool setup(
				PassLinker<LightingPass>&passBuilder,
				InputData               &input,
				OutputData              &output,
				FrameGraphResource const&gbuffer0,
				FrameGraphResource const&gbuffer1,
				FrameGraphResource const&gbuffer2,
				FrameGraphResource const&gbuffer3);

			bool execute(
				Ptr<IRenderContext>     &context,
				InputData          const&input,
				OutputData         const&output);

		private:
			State m_state;
		};

	}
}

#endif