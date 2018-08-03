#include "Renderer/FrameGraph/Modules/Compositing.h"

namespace engine {
  namespace FrameGraph {

    FrameGraphModule<CompositingModuleTag_t>::ExportData
      FrameGraphModule<CompositingModuleTag_t>::addDefaultCompositingPass(
        GraphBuilder            &graphBuilder,
        FrameGraphResource const&gbuffer0,
        FrameGraphResource const&gbuffer1,
        FrameGraphResource const&gbuffer2,
        FrameGraphResource const&gbuffer3,
        FrameGraphResource const&lightAccumulationBuffer,
        FrameGraphResource const&backBuffer)
    {
      struct State {
      };

      struct PassData {
        ImportData importData;
        ExportData exportData;

        State state;
      };

      auto pass = graphBuilder.spawnPass<CallbackPass<PassData>>(
        "Compositing",
        [&] (PassBuilder&builder, PassData&passData) -> bool
      {
        FrameGraphReadTextureFlags readFlags{ };
        readFlags.requiredFormat = FrameGraphFormat::Automatic;
        readFlags.source         = FrameGraphReadSource::Color;

        passData.importData.gbuffer0                = builder.readTexture(gbuffer0, readFlags, Range(0, 1), Range(0, 1));
        passData.importData.gbuffer1                = builder.readTexture(gbuffer1, readFlags, Range(0, 1), Range(0, 1));
        passData.importData.gbuffer2                = builder.readTexture(gbuffer2, readFlags, Range(0, 1), Range(0, 1));
        passData.importData.gbuffer3                = builder.readTexture(gbuffer3, readFlags, Range(0, 1), Range(0, 1));
        passData.importData.lightAccumulationBuffer = builder.readTexture(lightAccumulationBuffer, readFlags, Range(0, 1), Range(0, 1));

        FrameGraphWriteTextureFlags writeFlags{ };
        writeFlags.requiredFormat = FrameGraphFormat::Automatic;
        writeFlags.writeTarget    = FrameGraphWriteTarget::Color;

        passData.exportData.output = builder.writeTexture(backBuffer, writeFlags, Range(0, 1), Range(0, 1));

        return true;
      },
        [=] (PassData const&passData, FrameGraphResources const&frameGraphResources, CStdSharedPtr_t<IFrameGraphRenderContext>&context) -> bool
      {
        Log::Verbose(logTag(), "Compositing");

        return true;
      });

      return pass->passData().exportData;
    }

  }
}