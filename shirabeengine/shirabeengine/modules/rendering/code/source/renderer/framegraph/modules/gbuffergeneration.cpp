#include "renderer/framegraph/modules/gbuffergeneration.h"

namespace engine
{
    namespace framegraph
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult
        <
            CFrameGraphModule<SGBufferModuleTag_t>::SGBufferGenerationExportData
        >
        CFrameGraphModule<SGBufferModuleTag_t>::addGBufferGenerationPass(
                std::string         const &aPassName,
                CGraphBuilder             &aGraphBuilder,
                SFrameGraphResource const &aRenderableInput)
        {
            /**
             * The SState struct is the internal state of the gbuffer generation pass.
             */
            struct SState
            {
                SFrameGraphResource gbufferTextureArrayId;
            };

            /**
             * The SPassData struct declares the externally managed pass data
             * for the pass to be created.
             */
            struct SPassData
            {
                SGBufferGenerationImportData importData;
                SGBufferGenerationExportData exportData;

                SState state;
            };

            auto const setup = [&] (
                    CPassBuilder &aBuilder,
                    SPassData    &aOutPassData)
                    -> CEngineResult<>
            {
                uint32_t width  = 1920;
                uint32_t height = 1080;

                CStdSharedPtr_t<SApplicationEnvironment> environment = aGraphBuilder.applicationEnvironment();
                if(environment)
                {
                    CStdSharedPtr_t<wsi::CWSIDisplay> const &display = aGraphBuilder.display();

                    SOSDisplayDescriptor const &displayDesc = display->screenInfo()[display->primaryScreenIndex()];
                    width  = displayDesc.bounds.size.x();
                    height = displayDesc.bounds.size.y();
                }

                SFrameGraphTexture gbufferDesc={ };
                gbufferDesc.width          = width;
                gbufferDesc.height         = height;
                gbufferDesc.depth          = 1;
                gbufferDesc.format         = FrameGraphFormat_t::R8G8B8A8_UNORM;
                gbufferDesc.initialState   = EFrameGraphResourceInitState::Clear;
                gbufferDesc.arraySize      = 4;
                gbufferDesc.mipLevels      = 1;
                gbufferDesc.permittedUsage = EFrameGraphResourceUsage::InputAttachment | EFrameGraphResourceUsage::ColorAttachment;

                // Basic underlying output buffer to be linked
                aOutPassData.state.gbufferTextureArrayId = aBuilder.createTexture("GBuffer Array Texture", gbufferDesc).data();

                // This will create a list of render targets for the texutre array to render to.
                // They'll be internally created and managed.
                SFrameGraphWriteTextureFlags flags = {};
                flags.requiredFormat = gbufferDesc.format;
                flags.writeTarget    = EFrameGraphWriteTarget::Color;

                aOutPassData.exportData.gbuffer0 = aBuilder.writeTexture(aOutPassData.state.gbufferTextureArrayId, flags, CRange(0, 1), CRange(0, 1)).data();
                aOutPassData.exportData.gbuffer1 = aBuilder.writeTexture(aOutPassData.state.gbufferTextureArrayId, flags, CRange(1, 1), CRange(0, 1)).data();
                aOutPassData.exportData.gbuffer2 = aBuilder.writeTexture(aOutPassData.state.gbufferTextureArrayId, flags, CRange(2, 1), CRange(0, 1)).data();
                aOutPassData.exportData.gbuffer3 = aBuilder.writeTexture(aOutPassData.state.gbufferTextureArrayId, flags, CRange(3, 1), CRange(0, 1)).data();

                // Import renderable objects based on selector, flags, or whatever should be supported...
                aOutPassData.importData.renderableListView = aBuilder.importRenderables("SceneRenderables", aRenderableInput).data();

                return { EEngineStatus::Ok };
            };

            auto const execute = [=] (
                    SPassData                                 const&aPassData,
                    CFrameGraphResources                      const&aFrameGraphResources,
                    CStdSharedPtr_t<IFrameGraphRenderContext>      &aRenderContext)
                    -> CEngineResult<>
            {
                using namespace engine::rendering;

                CLog::Verbose(logTag(), "GBufferGeneration");

                CEngineResult<CStdSharedPtr_t<SFrameGraphRenderableListView> const> renderableViewFetch = aFrameGraphResources.get<SFrameGraphRenderableListView>(aPassData.importData.renderableListView.resourceId);
                if(not renderableViewFetch.successful())
                {
                    CLog::Error(logTag(), "Failed to fetch renderable view.");
                    return { renderableViewFetch.result() };
                }

                CStdSharedPtr_t<SFrameGraphRenderableListView> const &renderableView = renderableViewFetch.data();
#if defined SHIRABE_DEBUG || defined SHIRABE_TEST
                if(nullptr == renderableView)
                {
                    CLog::Error(logTag(), CString::format("Renderable view with id %0 not found.", aPassData.importData.renderableListView.resourceId));
                    return { renderableViewFetch.result() };
                }
#endif
                CEngineResult<CStdSharedPtr_t<SFrameGraphRenderableList> const> renderableListFetch =
                    aFrameGraphResources.get<SFrameGraphRenderableList>(aPassData.importData.renderableListView.subjacentResource);

                if(not renderableListFetch.successful())
                {
                    CLog::Error(logTag(), "Failed to fetch renderable list.");
                    return { EEngineStatus::NullPointer };
                }

                CStdSharedPtr_t<SFrameGraphRenderableList> const &renderableList = renderableListFetch.data();
#if defined SHIRABE_DEBUG || defined SHIRABE_TEST
                if(nullptr == renderableList)
                {
                    CLog::Error(logTag(), CString::format("Renderable list with id %0 not found.", aPassData.importData.renderableListView.subjacentResource));
                    return { EEngineStatus::NullPointer };
                }
#endif

                aRenderContext->beginPass();

                for(FrameGraphResourceId_t const &renderableId : renderableView->renderableRefIndices)
                {
                    FrameGraphRenderable_t const &renderable = renderableList->renderableList.at(renderableId);

                    CEngineResult<> renderCall = aRenderContext->render(renderable);
                    if(not renderCall.successful())
                    {
                        CLog::Error(logTag(), "Failed to render renderable.");
                        return { renderCall.result() };
                    }
                }

                aRenderContext->endPass();

                return { EEngineStatus::Ok };
            };

            auto passFetch = aGraphBuilder.spawnPass<CallbackPass<SPassData>>(aPassName, setup, execute);
            if(not passFetch.successful())
            {
                return { EEngineStatus::Error };
            }
            else
            {
                CStdSharedPtr_t<CallbackPass<SPassData>> pass = passFetch.data();
                if(nullptr == pass)
                {
                    return { EEngineStatus::NullPointer };
                }
                else
                {
                    return { EEngineStatus::Ok, passFetch.data()->passData().exportData };
                }
            }
        }
        //<-----------------------------------------------------------------------------
    }
}
