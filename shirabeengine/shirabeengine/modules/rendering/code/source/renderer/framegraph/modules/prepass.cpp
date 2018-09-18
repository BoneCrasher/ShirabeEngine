#include "renderer/framegraph/modules/prepass.h"

namespace engine
{
    namespace framegraph
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CFrameGraphModule<SPrepassModuleTag_t>::SPrepassExportData
        CFrameGraphModule<SPrepassModuleTag_t>::addPrepass(
                CGraphBuilder             &aGraphBuilder,
                SFrameGraphResource const &aBackBuffer)
        {
            /**
             * The SState struct is the internal state of the gbuffer generation pass.
             */
            struct SState
            {
                SFrameGraphResource backBufferId;
            };

            /**
             * The SPassData struct declares the externally managed pass data
             * for the pass to be created.
             */
            struct SPassData
            {
                SPrepassImportData importData;
                SPrepassExportData exportData;

                SState state;
            };

            auto const setup = [&] (
                    CPassBuilder &aBuilder,
                    SPassData    &aOutPassData) -> bool
            {
                aOutPassData.importData.backBufferInput = aBackBuffer;
                aOutPassData.exportData.backbuffer      = aBuilder.forwardResource(aBackBuffer.readableName, aBackBuffer);

                return true;
            };

            auto const execute = [=] (
                    SPassData                                 const&aPassData,
                    CFrameGraphResources                      const&aFrameGraphResources,
                    CStdSharedPtr_t<IFrameGraphRenderContext>      &aContext) -> bool
            {
                using namespace engine::rendering;

                CLog::Verbose(logTag(), "Prepass");

                return true;
            };

            auto pass = aGraphBuilder.spawnPass<CallbackPass<SPassData>>("Prepass", setup, execute);
            return pass->passData().exportData;
        }
        //<-----------------------------------------------------------------------------
    }
}
