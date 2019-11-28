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
                    CLog::Error(logTag(), CString::format("Failed to import back buffer texture '{}'", backBufferTextureDesc.readableName));
                    return { backBufferImport.result() };
                }
#endif

                aOutPassData.importData.backBufferInput = backBufferImport.data();

                SFrameGraphTextureResourceFlags flags{};
                flags.requiredFormat = FrameGraphFormat_t::Automatic;

                CEngineResult<SFrameGraphResource> backBufferForward = aBuilder.forwardTexture(aOutPassData.importData.backBufferInput, flags);
#ifdef SHIRABE_DEBUG
                if(not backBufferForward.successful())
                {
                    CLog::Error(logTag(), CString::format("Failed to forward back buffer texture w/ id '{}'", aOutPassData.importData.backBufferInput));
                    return { backBufferForward.result() };
                }
#endif

                aOutPassData.exportData.backbuffer = backBufferForward.data();

                return { EEngineStatus::Ok };
            };

            auto const execute = [=] (
                    SPrePassData                              const &aPassData,
                    CFrameGraphResources                      const &aFrameGraphResources,
                    Shared<IFrameGraphRenderContext>
                    &aContext)
                    -> CEngineResult<>
            {
                SHIRABE_UNUSED(aFrameGraphResources);

                using namespace engine::rendering;

                CLog::Verbose(logTag(), "PrePass");

                // Fetch the next backbuffer image and make it available to the resource manager.
                // aContext->bindSwapChain(aPassData.importData.backBufferInput);
                // Begin a command buffer.
                aContext->beginCommandBuffer();
                // Begin a render pass w/ a given framebuffer.
                // aContext->bindRenderPass(sFrameBufferResourceId, sRenderPassResourceId);

                return { EEngineStatus::Ok };
            };

            CEngineResult<Shared<CallbackPass<SPrePassData>>> passFetch = aGraphBuilder.spawnPass<CallbackPass<SPrePassData>>(aPassName, setup, execute);
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
                readFlags.readSource     = EFrameGraphReadSource::Color;

                aOutPassData.importData.finalOutputId = aBuilder.readAttachment(aOutput, readFlags).data();

                return { EEngineStatus::Ok };
            };

            auto const execute = [=] (
                    SPresentPassData                          const&aPassData,
                    CFrameGraphResources                      const&aFrameGraphResources,
                    Shared<IFrameGraphRenderContext>      &aContext)
                    -> CEngineResult<>
            {
                SHIRABE_UNUSED(aPassData);
                SHIRABE_UNUSED(aFrameGraphResources);

                using namespace engine::rendering;

                CLog::Verbose(logTag(), "PresentPass");

                static constexpr char const *sRenderPassResourceId  = "DefaultRenderPass";
                static constexpr char const *sFrameBufferResourceId = "DefaultFrameBuffer";


                // Important: The whole copyToBackBuffer-stuff may not be called from within a render pass.
                CEngineResult<Shared<SFrameGraphTextureView>> const viewFetch = aFrameGraphResources.get<SFrameGraphTextureView>(aPassData.importData.finalOutputId.resourceId);
                if(not viewFetch.successful())
                {
                    CLog::Error(logTag(), "Failed to fetch source image texture view resource.");
                }

                SFrameGraphTextureView const &view = *viewFetch.data();

                CEngineResult<Shared<SFrameGraphTexture>> const textureFetch = aFrameGraphResources.get<SFrameGraphTexture>(view.subjacentResource);
                if(not textureFetch.successful())
                {
                    CLog::Error(logTag(), "Failed to fetch source image texture resource.");
                }

                SFrameGraphTexture const &texture = *textureFetch.data();

                // aContext->unbindRenderPass(sFrameBufferResourceId, sRenderPassResourceId);
                aContext->copyImageToBackBuffer(texture);
                aContext->commitCommandBuffer();
                aContext->present();

                return { EEngineStatus::Ok };
            };

            CEngineResult<Shared<CallbackPass<SPresentPassData>>> spawn = aGraphBuilder.spawnPass<CallbackPass<SPresentPassData>>(aPassName, setup, execute);
            if(not spawn.successful())
            {
                return { EEngineStatus::Error };
            }
            else
            {
                Shared<CallbackPass<SPresentPassData>> pass = spawn.data();
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
