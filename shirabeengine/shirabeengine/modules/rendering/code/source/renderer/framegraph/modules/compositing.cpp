#include "renderer/framegraph/modules/compositing.h"

namespace engine
{
    namespace framegraph
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CFrameGraphModule<SCompositingModuleTag_t>::SExportData
        CFrameGraphModule<SCompositingModuleTag_t>::addDefaultCompositingPass(
                CGraphBuilder             &aGraphBuilder,
                SFrameGraphResource const &aGbuffer0,
                SFrameGraphResource const &aGbuffer1,
                SFrameGraphResource const &aGbuffer2,
                SFrameGraphResource const &aGbuffer3,
                SFrameGraphResource const &aLightAccumulationBuffer,
                SFrameGraphResource const &aBackBuffer)
        {
            /**
             * The SState struct is the internal state of the compositing pass.
             */
            struct SState
            {
            };

            /**
             * The SPassData struct declares the externally managed pass data
             * for the pass to be created.
             */
            struct SPassData
            {
                SImportData importData;
                SExportData exportData;

                SState state;
            };

            auto const setup = [&] (
                    CPassBuilder &aBuilder,
                    SPassData    &aOutPassData) -> bool
            {
                SFrameGraphReadTextureFlags readFlags{ };
                readFlags.requiredFormat = FrameGraphFormat_t::Automatic;
                readFlags.source         = EFrameGraphReadSource::Color;

                aOutPassData.importData.gbuffer0                = aBuilder.readTexture(aGbuffer0,                readFlags, CRange(0, 1), CRange(0, 1));
                aOutPassData.importData.gbuffer1                = aBuilder.readTexture(aGbuffer1,                readFlags, CRange(0, 1), CRange(0, 1));
                aOutPassData.importData.gbuffer2                = aBuilder.readTexture(aGbuffer2,                readFlags, CRange(0, 1), CRange(0, 1));
                aOutPassData.importData.gbuffer3                = aBuilder.readTexture(aGbuffer3,                readFlags, CRange(0, 1), CRange(0, 1));
                aOutPassData.importData.lightAccumulationBuffer = aBuilder.readTexture(aLightAccumulationBuffer, readFlags, CRange(0, 1), CRange(0, 1));

                SFrameGraphWriteTextureFlags writeFlags{ };
                writeFlags.requiredFormat = FrameGraphFormat_t::Automatic;
                writeFlags.writeTarget    = EFrameGraphWriteTarget::Color;

                aOutPassData.exportData.output = aBuilder.writeTexture(aBackBuffer, writeFlags, CRange(0, 1), CRange(0, 1));

                return true;
            };

            auto const execute = [=] (
                    SPassData                                 const&aPassData,
                    CFrameGraphResources                      const&aFrameGraphResources,
                    CStdSharedPtr_t<IFrameGraphRenderContext>      &aContext) -> bool
            {
                CLog::Verbose(logTag(), "Compositing");

                return true;
            };

            auto pass = aGraphBuilder.spawnPass<CallbackPass<SPassData>>("Compositing", setup, execute);
            return pass->passData().exportData;
        }
        //<-----------------------------------------------------------------------------
    }
}
