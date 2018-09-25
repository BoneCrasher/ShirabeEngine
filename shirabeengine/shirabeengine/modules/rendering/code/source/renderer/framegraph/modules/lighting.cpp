#include "renderer/framegraph/modules/lighting.h"

namespace engine
{
    namespace framegraph
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CFrameGraphModule<SLightingModuleTag_t>::SLightingExportData
        CFrameGraphModule<SLightingModuleTag_t>::addLightingPass(
                std::string         const &aPassName,
                CGraphBuilder             &aGraphBuilder,
                SFrameGraphResource const &aGbuffer0,
                SFrameGraphResource const &aGbuffer1,
                SFrameGraphResource const &aGbuffer2,
                SFrameGraphResource const &aGbuffer3)
        {
            /**
             * The SState struct is the internal state of the lighting pass.
             */
            struct SState
            {
                SFrameGraphResource lightAccumulationBufferTextureId;
            };

            /**
             * The SPassData struct declares the externally managed pass data
             * for the pass to be created.
             */
            struct SPassData
            {
                SLightingImportData importData;
                SLightingExportData exportData;

                SState state;
            };

            auto const setup = [&] (
                    CPassBuilder &aBuilder,
                    SPassData    &aOutPassData) -> bool
            {
                SFrameGraphTexture gbufferTexture = *aGraphBuilder.getResources().get<SFrameGraphTexture>(aGbuffer0.subjacentResource);

                SFrameGraphTexture lightAccBufferDesc ={ };
                lightAccBufferDesc.width          = gbufferTexture.width;
                lightAccBufferDesc.height         = gbufferTexture.height;
                lightAccBufferDesc.depth          = 1;
                lightAccBufferDesc.format         = FrameGraphFormat_t::R32_FLOAT;
                lightAccBufferDesc.mipLevels      = 1;
                lightAccBufferDesc.arraySize      = 1;
                lightAccBufferDesc.initialState   = EFrameGraphResourceInitState::Clear;
                lightAccBufferDesc.permittedUsage = EFrameGraphResourceUsage::ImageResource | EFrameGraphResourceUsage::RenderTarget;

                aOutPassData.state.lightAccumulationBufferTextureId = aBuilder.createTexture("Light Accumulation Buffer", lightAccBufferDesc);

                SFrameGraphReadTextureFlags readFlags{ };
                readFlags.requiredFormat = FrameGraphFormat_t::Automatic;
                readFlags.source         = EFrameGraphReadSource::Color;

                aOutPassData.importData.gbuffer0 = aBuilder.readTexture(aGbuffer0, readFlags, CRange(0, 1), CRange(0, 1));
                aOutPassData.importData.gbuffer1 = aBuilder.readTexture(aGbuffer1, readFlags, CRange(0, 1), CRange(0, 1));
                aOutPassData.importData.gbuffer2 = aBuilder.readTexture(aGbuffer2, readFlags, CRange(0, 1), CRange(0, 1));
                aOutPassData.importData.gbuffer3 = aBuilder.readTexture(aGbuffer3, readFlags, CRange(0, 1), CRange(0, 1));

                SFrameGraphWriteTextureFlags writeFlags{ };
                writeFlags.requiredFormat = FrameGraphFormat_t::Automatic;
                writeFlags.writeTarget    = EFrameGraphWriteTarget::Color;

                aOutPassData.exportData.lightAccumulationBuffer = aBuilder.writeTexture(aOutPassData.state.lightAccumulationBufferTextureId, writeFlags, CRange(0, 1), CRange(0, 1));

                return true;
            };

            auto const execute = [=] (
                    SPassData                                 const&aPassData,
                    CFrameGraphResources                      const&aFrameGraphResources,
                    CStdSharedPtr_t<IFrameGraphRenderContext>      &aContext) -> bool
            {
                CLog::Verbose(logTag(), "Lighting");

                return true;
            };

            auto pass = aGraphBuilder.spawnPass<CallbackPass<SPassData>>(aPassName, setup, execute);
            return pass->passData().exportData;
        }
        //<-----------------------------------------------------------------------------
    }
}
