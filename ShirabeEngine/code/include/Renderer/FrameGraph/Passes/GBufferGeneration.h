#ifndef __SHIRABE_FRAMEGRAPH_PASSES_GBUFFER_H__
#define __SHIRABE_FRAMEGRAPH_PASSES_GBUFFER_H__


#include "Renderer/FrameGraph/GraphBuilder.h"
#include "Renderer/FrameGraph/PassLinker.h"

#include "Resources/Core/IResourceManager.h"
#include "GFXAPI/Types/All.h"

namespace Engine {
	namespace FrameGraph {
		
		class GBufferPass {
		public:

			GBufferPass();
			~GBufferPass();

			struct InputData {
				FrameGraphResource
					renderableQueryId;
			};

			struct OutputData {
				FrameGraphResource
					gbuffer0,
					gbuffer1,
					gbuffer2,
					gbuffer3;
			};

			struct State {
				std::vector<FrameGraphResource>
					renderTargetBindings;
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

	}
}

#endif