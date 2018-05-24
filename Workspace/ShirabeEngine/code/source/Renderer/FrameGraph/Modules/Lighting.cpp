#include "Renderer/FrameGraph/Modules/Lighting.h"

namespace Engine {
  namespace FrameGraph {

    FrameGraphModule<LightingModuleTag_t>::LightingExportData
      FrameGraphModule<LightingModuleTag_t>::addLightingPass(
        GraphBuilder            &graphBuilder,
        FrameGraphResource const&gbuffer0,
        FrameGraphResource const&gbuffer1,
        FrameGraphResource const&gbuffer2,
        FrameGraphResource const&gbuffer3)
    {
      struct State {
        FrameGraphResource
          lightAccumulationBufferTextureId;
      };

      struct PassData {
        LightingImportData importData;
        LightingExportData exportData;

        State state;
      };

      auto pass = graphBuilder.spawnPass<CallbackPass<PassData>>(
        "LightAccumulation",
        [&] (PassBuilder&builder, PassData&passData) -> bool
      {
        FrameGraphTexture gbufferTexture = *graphBuilder.getResources().getTexture(gbuffer0.subjacentResource);

        FrameGraphTexture lightAccBufferDesc ={ };
        lightAccBufferDesc.width          = gbufferTexture.width;
        lightAccBufferDesc.height         = gbufferTexture.height;
        lightAccBufferDesc.depth          = 1;
        lightAccBufferDesc.format         = FrameGraphFormat::R32_FLOAT;
        lightAccBufferDesc.mipLevels      = 1;
        lightAccBufferDesc.arraySize      = 1;
        lightAccBufferDesc.initialState   = FrameGraphResourceInitState::Clear;
        lightAccBufferDesc.permittedUsage = FrameGraphResourceUsage::ImageResource | FrameGraphResourceUsage::RenderTarget;

        passData.state.lightAccumulationBufferTextureId = builder.createTexture("Light Accumulation Buffer", lightAccBufferDesc);

        FrameGraphReadTextureFlags readFlags{ };
        readFlags.requiredFormat = FrameGraphFormat::Automatic;
        readFlags.source         = FrameGraphReadSource::Color;

        passData.importData.gbuffer0 = builder.readTexture(gbuffer0, readFlags, Range(0, 1), Range(0, 1));
        passData.importData.gbuffer1 = builder.readTexture(gbuffer1, readFlags, Range(0, 1), Range(0, 1));
        passData.importData.gbuffer2 = builder.readTexture(gbuffer2, readFlags, Range(0, 1), Range(0, 1));
        passData.importData.gbuffer3 = builder.readTexture(gbuffer3, readFlags, Range(0, 1), Range(0, 1));

        FrameGraphWriteTextureFlags writeFlags{ };
        writeFlags.requiredFormat = FrameGraphFormat::Automatic;
        writeFlags.writeTarget    = FrameGraphWriteTarget::Color;

        passData.exportData.lightAccumulationBuffer = builder.writeTexture(passData.state.lightAccumulationBufferTextureId, writeFlags, Range(0, 1), Range(0, 1));

        return true;
      },
        [=] (PassData const&passData, FrameGraphResources const&frameGraphResources, Ptr<IFrameGraphRenderContext>&context) -> bool
      {
        Log::Verbose(logTag(), "Lighting");

        return true;
      });

      return pass->passData().exportData;
    }

  }
}