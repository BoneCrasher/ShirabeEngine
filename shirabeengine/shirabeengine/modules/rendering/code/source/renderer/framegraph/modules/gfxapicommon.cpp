#include "renderer/framegraph/modules/gfxapicommon.h"

namespace engine
{
    namespace framegraph
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CFrameGraphModule<SGraphicsAPICommonModuleTag_t>::SSwapChainPassExportData
        CFrameGraphModule<SGraphicsAPICommonModuleTag_t>::addSwapChainPass(
                CGraphBuilder            &aGraphBuilder,
                uint32_t           const &aWidth,
                uint32_t           const &aHeight,
                FrameGraphFormat_t const &aFormat)
        {
            /**
             * The SState struct is the internal state of the swapchain pass.
             */
            struct SState
            {
                SFrameGraphResource backBufferId;
            };

            /**
             * The SPassData struct declares the externally managed pass data
             * for the pass to be created.
             */
            struct SSwapChainPassData
            {
                SSwapChainPassImportData importData;
                SSwapChainPassExportData exportData;

                SState state;
            };

            auto const setup = [&] (
                    CPassBuilder       &aBuilder,
                    SSwapChainPassData &aOutPassData) -> bool
            {
                SFrameGraphTexture backBufferTextureDesc{ };
                backBufferTextureDesc.readableName   = "BackBuffer";
                backBufferTextureDesc.width          = aWidth;
                backBufferTextureDesc.height         = aHeight;
                backBufferTextureDesc.depth          = 1;
                backBufferTextureDesc.format         = aFormat;
                backBufferTextureDesc.initialState   = EFrameGraphResourceInitState::Clear;
                backBufferTextureDesc.arraySize      = 1;
                backBufferTextureDesc.mipLevels      = 1;
                backBufferTextureDesc.permittedUsage = EFrameGraphResourceUsage::RenderTarget | EFrameGraphResourceUsage::Unused;

                aOutPassData.importData.backBufferInput = aBuilder.importTexture(backBufferTextureDesc.readableName, backBufferTextureDesc);

                SFrameGraphResourceFlags flags{};
                flags.requiredFormat = FrameGraphFormat_t::Automatic;

                aOutPassData.exportData.backbuffer = aBuilder.forwardTexture(aOutPassData.importData.backBufferInput, flags);

                return true;
            };

            auto const execute = [=] (
                    SSwapChainPassData                        const&aPassData,
                    CFrameGraphResources                      const&aFrameGraphResources,
                    CStdSharedPtr_t<IFrameGraphRenderContext>      &aContext) -> bool
            {
                using namespace engine::rendering;

                CLog::Verbose(logTag(), "SwapChainPass");

                aContext->bindSwapChain(aPassData.importData.backBufferInput);

                return true;
            };

            auto pass = aGraphBuilder.spawnPass<CallbackPass<SSwapChainPassData>>("SwapChainPass", setup, execute);
            return pass->passData().exportData;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CFrameGraphModule<SGraphicsAPICommonModuleTag_t>::SPresentPassExportData
        CFrameGraphModule<SGraphicsAPICommonModuleTag_t>::addPresentPass(
                CGraphBuilder             &aGraphBuilder,
                SFrameGraphResource const &aRenderingResult)
        {
            /**
             * The SState struct is the internal state of the present generation pass.
             */
            struct SState
            {
            };

            /**
             * The SPassData struct declares the externally managed pass data
             * for the pass to be created.
             */
            struct SPresentPassData
            {
                SPresentPassImportData importData;
                SPresentPassExportData exportData;

                SState state;
            };

            auto const setup = [&] (
                    CPassBuilder     &aBuilder,
                    SPresentPassData &aOutPassData) -> bool
            {
                SFrameGraphResource const handle = aBuilder.acceptTexture(aRenderingResult);

                return true;
            };

            auto const execute = [=] (
                    SPresentPassData                          const&aPassData,
                    CFrameGraphResources                      const&aFrameGraphResources,
                    CStdSharedPtr_t<IFrameGraphRenderContext>      &aContext) -> bool
            {
                using namespace engine::rendering;

                CLog::Verbose(logTag(), "SwapChainPass");

                aContext->present();

                return true;
            };

            auto pass = aGraphBuilder.spawnPass<CallbackPass<SPresentPassData>>("PresentPass", setup, execute);
            return pass->passData().exportData;
        }
        //<-----------------------------------------------------------------------------
    }
}
