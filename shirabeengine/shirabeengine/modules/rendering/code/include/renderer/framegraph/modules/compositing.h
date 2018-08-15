#ifndef __SHIRABE_FRAMEGRAPH_MODULE_COMPOSITING_H__
#define __SHIRABE_FRAMEGRAPH_MODULE_COMPOSITING_H__

#include "Log/Log.h"

#include "Renderer/FrameGraph/GraphBuilder.h"
#include "Renderer/FrameGraph/PassBuilder.h"
#include "Renderer/FrameGraph/Modules/Module.h"

#include "Resources/Core/ResourceManager.h"
#include "GraphicsAPI/Resources/Types/All.h"

namespace engine {
	namespace framegraph {

    struct CompositingModuleTag_t {};

    template<>
    class SHIRABE_TEST_EXPORT FrameGraphModule<CompositingModuleTag_t> {
      SHIRABE_DECLARE_LOG_TAG(FrameGraphModule<CompositingModuleTag_t>);
    public:
      struct ImportData {
        FrameGraphResource
          gbuffer0,
          gbuffer1,
          gbuffer2,
          gbuffer3,
          lightAccumulationBuffer;
      };

      struct ExportData {
        FrameGraphResource
          output;
      };

      ExportData addDefaultCompositingPass(
        GraphBuilder            &graphBuilder,
        FrameGraphResource const&gbuffer0,
        FrameGraphResource const&gbuffer1,
        FrameGraphResource const&gbuffer2,
        FrameGraphResource const&gbuffer3,
        FrameGraphResource const&lightAccumulationBuffer,
        FrameGraphResource const&backBuffer);

    private:
    };

	}
}

#endif