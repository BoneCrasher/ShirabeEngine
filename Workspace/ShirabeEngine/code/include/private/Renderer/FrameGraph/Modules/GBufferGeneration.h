#ifndef __SHIRABE_FRAMEGRAPH_MODULE_GBUFFERGENERATION_H__
#define __SHIRABE_FRAMEGRAPH_MODULE_GBUFFERGENERATION_H__

#include "Platform/Platform.h"

#include "Log/Log.h"

#include "Renderer/FrameGraph/GraphBuilder.h"
#include "Renderer/FrameGraph/PassBuilder.h"
#include "Renderer/FrameGraph/Modules/Module.h"

#include "Resources/Core/IResourceManager.h"
#include "GFXAPI/Types/All.h"

namespace Engine {
  namespace FrameGraph {

    struct GBufferModuleTag_t {};

    template<>
    class SHIRABE_TEST_EXPORT FrameGraphModule<GBufferModuleTag_t> {
      DeclareLogTag(FrameGraphModule<GBufferModuleTag_t>);
    public:
      struct GBufferGenerationImportData {
        FrameGraphResource
          renderableListView;
      };

      struct GBufferGenerationExportData {
        FrameGraphResource
          gbuffer0,
          gbuffer1,
          gbuffer2,
          gbuffer3;
      };

      GBufferGenerationExportData
        addGBufferGenerationPass(
          GraphBuilder            &graphBuilder,
          FrameGraphResource const&renderableInput);

    private:
    };

  }
}

#endif