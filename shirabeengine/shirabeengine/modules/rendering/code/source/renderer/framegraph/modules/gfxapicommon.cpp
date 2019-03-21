#include "renderer/framegraph/modules/gfxapicommon.h"

namespace engine
{
    namespace framegraph
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult
        <
            CFrameGraphModule<SGraphicsAPICommonModuleTag_t>::SPrePassExportData
        >
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
                    CPassBuilder &aBuilder,
                    SPrePassData &aOutPassData)
                    -> CEngineResult<>
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

                CEngineResult<SFrameGraphResource> backBufferImport = aBuilder.importTexture(backBufferTextureDesc.readableName, backBufferTextureDesc);
#ifdef SHIRABE_DEBUG
                if(not backBufferImport.successful())
                {
                    CLog::Error(logTag(), CString::format("Failed to import back buffer texture '%1'", backBufferTextureDesc.readableName));
                    return { backBufferImport.result() };
                }
#endif
                aOutPassData.importData.backBufferInput = backBufferImport.data();

                SFrameGraphResourceFlags flags{};
                flags.requiredFormat = FrameGraphFormat_t::Automatic;

                CEngineResult<SFrameGraphResource> backBufferForward = aBuilder.forwardTexture(aOutPassData.importData.backBufferInput, flags);
#ifdef SHIRABE_DEBUG
                if(not backBufferForward.successful())
                {
                    CLog::Error(logTag(), CString::format("Failed to forward back buffer texture w/ id '%1'", aOutPassData.importData.backBufferInput));
                    return { backBufferForward.result() };
                }
#endif

                aOutPassData.exportData.backbuffer = backBufferForward.data();

                return { EEngineStatus::Ok };
            };

            auto const execute = [=] (
                    SPrePassData                              const &aPassData,
                    CFrameGraphResources                      const &aFrameGraphResources,
                    CStdSharedPtr_t<IFrameGraphRenderContext>
                    &aContext)
                    -> CEngineResult<>
            {
                SHIRABE_UNUSED(aFrameGraphResources);

                using namespace engine::rendering;

                CLog::Verbose(logTag(), "PrePass");

                static constexpr char const *sRenderPassResourceId  = "DefaultRenderPass";
                static constexpr char const *sFrameBufferResourceId = "DefaultFrameBuffer";

                aContext->bindSwapChain(aPassData.importData.backBufferInput);

                aContext->beginCommandBuffer();
                aContext->bindFrameBufferAndRenderPass(sFrameBufferResourceId, sRenderPassResourceId);

                return { EEngineStatus::Ok };
            };

            CEngineResult<CStdSharedPtr_t<CallbackPass<SPrePassData>>> passFetch = aGraphBuilder.spawnPass<CallbackPass<SPrePassData>>(aPassName, setup, execute);
            if(not passFetch.successful())
            {
                return { EEngineStatus::Error };
            }
            else
            {
                return { EEngineStatus::Ok, passFetch.data()->passData().exportData };
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult
        <
            CFrameGraphModule<SGraphicsAPICommonModuleTag_t>::SPresentPassExportData
        >
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
                    SPresentPassData &aOutPassData)
                    -> CEngineResult<>
            {
                SFrameGraphReadTextureFlags readFlags{ };
                readFlags.requiredFormat = FrameGraphFormat_t::Automatic;
                readFlags.source         = EFrameGraphReadSource::Color;

                aOutPassData.importData.finalOutputId = aBuilder.readTexture(aOutput, readFlags, CRange(0, 1), CRange(0, 1)).data();

                return { EEngineStatus::Ok };
            };

            auto const execute = [=] (
                    SPresentPassData                          const&aPassData,
                    CFrameGraphResources                      const&aFrameGraphResources,
                    CStdSharedPtr_t<IFrameGraphRenderContext>      &aContext)
                    -> CEngineResult<>
            {
                SHIRABE_UNUSED(aPassData);
                SHIRABE_UNUSED(aFrameGraphResources);

                using namespace engine::rendering;

                CLog::Verbose(logTag(), "PresentPass");

                static constexpr char const *sRenderPassResourceId  = "DefaultRenderPass";
                static constexpr char const *sFrameBufferResourceId = "DefaultFrameBuffer";


                // Important: The whole copyToBackBuffer-stuff may not be called from within a render pass.
                CEngineResult<CStdSharedPtr_t<SFrameGraphTextureView>> const viewFetch = aFrameGraphResources.get<SFrameGraphTextureView>(aPassData.importData.finalOutputId);
                if(not viewFetch.successful())
                {
                    CLog::Error(logTag(), "Failed to fetch source image texture view resource.");
                }

                SFrameGraphTextureView const &view = *viewFetch.data();

                CEngineResult<CStdSharedPtr_t<SFrameGraphTexture>> const textureFetch = aFrameGraphResources.get<SFrameGraphTexture>(view.subjacentResource);
                if(not textureFetch.successful())
                {
                    CLog::Error(logTag(), "Failed to fetch source image texture resource.");
                }

                SFrameGraphTexture const &texture = *textureFetch.data();

                aContext->unbindFrameBufferAndRenderPass(sFrameBufferResourceId, sRenderPassResourceId);
                aContext->copyToBackBuffer(texture);
                aContext->commitCommandBuffer();
                aContext->present();

                return { EEngineStatus::Ok };
            };

            CEngineResult<CStdSharedPtr_t<CallbackPass<SPresentPassData>>> spawn = aGraphBuilder.spawnPass<CallbackPass<SPresentPassData>>(aPassName, setup, execute);
            if(not spawn.successful())
            {
                return { EEngineStatus::Error };
            }
            else
            {
                CStdSharedPtr_t<CallbackPass<SPresentPassData>> pass = spawn.data();
                if(nullptr == pass)
                {
                    return { EEngineStatus::NullPointer };
                }
                else
                {
                    return { EEngineStatus::Ok, pass->passData().exportData };
                }
            }
        }
        //<-----------------------------------------------------------------------------
    }
}
