#include "Renderer/FrameGraph/Modules/GBufferGeneration.h"

namespace Engine {
	namespace FrameGraph {

    FrameGraphModule<GBufferModuleTag_t>::GBufferGenerationExportData
      FrameGraphModule<GBufferModuleTag_t>::addGBufferGenerationPass(
        GraphBuilder &graphBuilder)
    {
      struct State {
        FrameGraphResource
          gbufferTextureArrayId;
      };

      struct PassData {
        GBufferGenerationImportData importData;
        GBufferGenerationExportData exportData;

        State state;
      };

      auto pass = graphBuilder.spawnPass<CallbackPass<PassData>>(
        "GBufferGeneration",
        [&] (PassBuilder&builder, PassData&passData) -> bool
      {
        uint32_t
          width  = 1920,
          height = 1080;

        Ptr<ApplicationEnvironment> environment = graphBuilder.getApplicationEnvironment();
        if(environment) {
          OSDisplayDescriptor const&displayDesc = environment->primaryDisplay();
          width  = displayDesc.bounds.size.x();
          height = displayDesc.bounds.size.y();
        }

        FrameGraphTexture gbufferDesc={ };
        gbufferDesc.width          = width;
        gbufferDesc.height         = height;
        gbufferDesc.depth          = 1;
        gbufferDesc.format         = FrameGraphFormat::R8G8B8A8_UNORM;
        gbufferDesc.initialState   = FrameGraphResourceInitState::Clear;
        gbufferDesc.arraySize      = 4;
        gbufferDesc.mipLevels      = 1;
        gbufferDesc.permittedUsage = FrameGraphResourceUsage::ImageResource | FrameGraphResourceUsage::RenderTarget;

        // Basic underlying output buffer to be linked
        passData.state.gbufferTextureArrayId = builder.createTexture("GBuffer Array Texture", gbufferDesc);

        // This will create a list of render targets for the texutre array to render to.
        // They'll be internally created and managed.
        FrameGraphWriteTextureFlags flags{ };
        flags.requiredFormat = gbufferDesc.format;
        flags.writeTarget    = FrameGraphWriteTarget::Color;

        passData.exportData.gbuffer0 = builder.writeTexture(passData.state.gbufferTextureArrayId, flags, Range(0, 1), Range(0, 1));
        passData.exportData.gbuffer1 = builder.writeTexture(passData.state.gbufferTextureArrayId, flags, Range(1, 1), Range(0, 1));
        passData.exportData.gbuffer2 = builder.writeTexture(passData.state.gbufferTextureArrayId, flags, Range(2, 1), Range(0, 1));
        passData.exportData.gbuffer3 = builder.writeTexture(passData.state.gbufferTextureArrayId, flags, Range(3, 1), Range(0, 1));

        // Import renderable objects based on selector, flags, or whatever should be supported...
        passData.importData.renderableQueryId = builder.importRenderables();

        return true;
      },
        [=] (Ptr<IRenderContext>&) -> bool
      {
        return true;
      });

      return pass->passData().exportData;
    }

	}
}