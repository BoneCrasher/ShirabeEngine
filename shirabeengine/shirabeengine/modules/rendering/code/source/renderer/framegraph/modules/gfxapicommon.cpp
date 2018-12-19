#include "renderer/framegraph/modules/gfxapicommon.h"

namespace engine
{
    namespace framegraph
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CFrameGraphModule<SGraphicsAPICommonModuleTag_t>::SPrePassExportData
        CFrameGraphModule<SGraphicsAPICommonModuleTag_t>::addPrePass(
                std::string        const &aPassName,
                CGraphBuilder            &aGraphBuilder,
                uint32_t           const &aWidth,
                uint32_t           const &aHeight,
                FrameGraphFormat_t const &aFormat)
        {
            /**
             * The SState struct is the internal state of the Pre pass.
             */
            struct SState
            {
                SFrameGraphResource backBufferId;
            };

            /**
             * The SPassData struct declares the externally managed pass data
             * for the pass to be created.
             */
            struct SPrePassData
            {
                SPrePassImportData importData;
                SPrePassExportData exportData;

                SState state;
            };

            auto const setup = [&] (
                    CPassBuilder       &aBuilder,
                    SPrePassData &aOutPassData) -> bool
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
                backBufferTextureDesc.permittedUsage = EFrameGraphResourceUsage::ColorAttachment | EFrameGraphResourceUsage::Unused;

                aOutPassData.importData.backBufferInput = aBuilder.importTexture(backBufferTextureDesc.readableName, backBufferTextureDesc);

                SFrameGraphResourceFlags flags{};
                flags.requiredFormat = FrameGraphFormat_t::Automatic;

                aOutPassData.exportData.backbuffer = aBuilder.forwardTexture(aOutPassData.importData.backBufferInput, flags);

                return true;
            };

            auto const execute = [=] (
                    SPrePassData                              const &aPassData,
                    CFrameGraphResources                      const &aFrameGraphResources,
                    CStdSharedPtr_t<IFrameGraphRenderContext>
                    &aContext) -> bool
            {
                using namespace engine::rendering;

                CLog::Verbose(logTag(), "PrePass");

                aContext->bindSwapChain(aPassData.importData.backBufferInput);
                aContext->bindCommandBuffer();

                return true;
            };

            auto pass = aGraphBuilder.spawnPass<CallbackPass<SPrePassData>>(aPassName, setup, execute);
            return pass->passData().exportData;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CFrameGraphModule<SGraphicsAPICommonModuleTag_t>::SPresentPassExportData
        CFrameGraphModule<SGraphicsAPICommonModuleTag_t>::addPresentPass(
                std::string         const &aPassName,
                CGraphBuilder             &aGraphBuilder,
                SFrameGraphResource const &aOutput)
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
                SFrameGraphReadTextureFlags readFlags{ };
                readFlags.requiredFormat = FrameGraphFormat_t::Automatic;
                readFlags.source         = EFrameGraphReadSource::Color;

                aOutPassData.importData.finalOutputId = aBuilder.readTexture(aOutput, readFlags, CRange(0, 1), CRange(0, 1));

                return true;
            };

            auto const execute = [=] (
                    SPresentPassData                          const&aPassData,
                    CFrameGraphResources                      const&aFrameGraphResources,
                    CStdSharedPtr_t<IFrameGraphRenderContext>      &aContext) -> bool
            {
                using namespace engine::rendering;

                CLog::Verbose(logTag(), "SwapChainPass");

                aContext->commitCommandBuffer();
                aContext->present();

                return true;
            };

            auto pass = aGraphBuilder.spawnPass<CallbackPass<SPresentPassData>>(aPassName, setup, execute);
            return pass->passData().exportData;
        }
        //<-----------------------------------------------------------------------------
    }
}
