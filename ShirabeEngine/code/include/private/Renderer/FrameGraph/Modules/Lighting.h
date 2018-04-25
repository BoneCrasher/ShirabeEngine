#ifndef __SHIRABE_FRAMEGRAPH_MODULE_LIGHTING_H__
#define __SHIRABE_FRAMEGRAPH_MODULE_LIGHTING_H__

#include "Renderer/FrameGraph/GraphBuilder.h"
#include "Renderer/FrameGraph/PassBuilder.h"
#include "Renderer/FrameGraph/Modules/Module.h"

#include "Resources/Core/IResourceManager.h"
#include "GFXAPI/Types/All.h"

namespace Engine {
	namespace FrameGraph {

    struct LightingModuleTag_t {};

    template<>
    class FrameGraphModule<LightingModuleTag_t> {
    public:
      struct LightingImportData {
        FrameGraphResource
          gbuffer0,
          gbuffer1,
          gbuffer2,
          gbuffer3;
      };

      struct LightingExportData {
        FrameGraphResource
          lightAccumulationBuffer;
      };

      LightingExportData addLightingPass(
        GraphBuilder            &graphBuilder,
        FrameGraphResource const&gbuffer0,
        FrameGraphResource const&gbuffer1,
        FrameGraphResource const&gbuffer2,
        FrameGraphResource const&gbuffer3);

    private:
    };

	}
}

#endif