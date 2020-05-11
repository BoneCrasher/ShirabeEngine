#include "renderer/framegraph/modules/compositing.h"
#include <util/crc32.h>

namespace engine
{
    namespace framegraph
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult
        <
            CRenderGraphModule<SCompositingModuleTag_t>::SExportData
        >
        CRenderGraphModule<SCompositingModuleTag_t>::addDefaultCompositingPass(
            std::string const      &aPassName,
                CGraphBuilder          &aGraphBuilder,
                SRenderGraphImageView &aGbuffer0,
                SRenderGraphImageView &aGbuffer1,
                SRenderGraphImageView &aGbuffer2,
                SRenderGraphImageView &aGbuffer3,
                SRenderGraphImageView &aDepthStencil,
                SRenderGraphImageView &aLightAccumulationBuffer)
        {
            /**
             * The SState struct is the internal state of the compositing pass.
             */
            struct SState
            {
                SRenderGraphRenderTarget compositingBufferId;
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

            auto const setup = [&] (CPassBuilder                       &aBuilder
                                    , SPassData                        &aOutPassData
                                    , SRenderGraphPlatformContext const &aPlatformContext
                                    , SRenderGraphDataSource const      &aDataSource)
                    -> CEngineResult<>
            {
                auto gbufferTextureFetch = aGraphBuilder.getResources()
                                                        .getResource<SRenderGraphImage>(aGbuffer0.subjacentResource);
                if(not gbufferTextureFetch.successful())
                {
                    CLog::Error(logTag(), "Failed to fetch gbuffer texure.");
                    return { EEngineStatus::Error };
                }

                SRenderGraphImage gbufferTexture = *(gbufferTextureFetch.data());

                SRenderGraphRenderTarget compositingBufferDesc ={ };
                compositingBufferDesc.width          = gbufferTexture.width;
                compositingBufferDesc.height         = gbufferTexture.height;
                compositingBufferDesc.depth          = 1;
                compositingBufferDesc.format         = RenderGraphFormat_t::B8G8R8A8_UNORM;
                compositingBufferDesc.mipLevels      = 1;
                compositingBufferDesc.arraySize      = 1;
                compositingBufferDesc.initialState   = ERenderGraphResourceInitState::Clear;
                compositingBufferDesc.permittedUsage = ERenderGraphResourceUsage::InputAttachment | ERenderGraphResourceUsage::ColorAttachment;

                aOutPassData.state.compositingBufferId = aBuilder.createRenderTarget("Compositing Buffer", compositingBufferDesc).data();

                SRenderGraphReadTextureFlags readFlags{ };
                readFlags.requiredFormat  = RenderGraphFormat_t::Automatic;
                readFlags.readSource      = ERenderGraphReadSource::Color;
                readFlags.arraySliceRange = CRange(0, 1);
                readFlags.mipSliceRange   = CRange(0, 1);

                SRenderGraphReadTextureFlags depthReadFlags{ };
                depthReadFlags.requiredFormat  = RenderGraphFormat_t::Automatic;
                depthReadFlags.readSource      = ERenderGraphReadSource::Depth;
                depthReadFlags.arraySliceRange = CRange(0, 1);
                depthReadFlags.mipSliceRange   = CRange(0, 1);

                aOutPassData.importData.gbuffer0                = aBuilder.readAttachment(aGbuffer0,                readFlags     ).data();
                aOutPassData.importData.gbuffer1                = aBuilder.readAttachment(aGbuffer1,                readFlags     ).data();
                aOutPassData.importData.gbuffer2                = aBuilder.readAttachment(aGbuffer2,                readFlags     ).data();
                aOutPassData.importData.gbuffer3                = aBuilder.readAttachment(aGbuffer3,                readFlags     ).data();
                aOutPassData.importData.depth                   = aBuilder.readAttachment(aDepthStencil,            depthReadFlags).data();
                aOutPassData.importData.lightAccumulationBuffer = aBuilder.readAttachment(aLightAccumulationBuffer, readFlags     ).data();

                SRenderGraphWriteTextureFlags writeFlags{ };
                writeFlags.requiredFormat  = RenderGraphFormat_t::Automatic;
                writeFlags.writeTarget     = ERenderGraphWriteTarget::Color;
                writeFlags.arraySliceRange = CRange(0, 1);
                writeFlags.mipSliceRange   = CRange(0, 1);

                aOutPassData.exportData.output = aBuilder.writeAttachment(aOutPassData.state.compositingBufferId, writeFlags).data();

                SRenderGraphPipelineConfig pipelineConfig {};
                SRenderGraphMaterial const &material = aBuilder.useMaterial("compositing", util::crc32FromString("materials/deferred/compositing/compositing.material.meta"), pipelineConfig).data();
                aOutPassData.importData.material = material;

                return { EEngineStatus::Ok };
            };

            auto const execute = [=] (SPassData const                    &aPassData
                                      , SRenderGraphPlatformContext const &aPlatformContext
                                      , SRenderGraphDataSource const      &aDataSource
                                      , CRenderGraphResources const       &aRenderGraphResources
                                      , SRenderGraphRenderContextState    &aRenderContextState
                                      , SRenderGraphResourceContext       &aResourceContext
                                      , SRenderGraphRenderContext         &aRenderContext)
                    -> CEngineResult<>
            {
                CLog::Verbose(logTag(), "Compositing");

                aRenderContext.drawFullscreenQuadWithMaterial(aRenderContextState, aPassData.importData.material);

                return { EEngineStatus::Ok };
            };

            auto passFetch = aGraphBuilder.addSubpass<CallbackPass<SPassData>>(aPassName, setup, execute);
            if(not passFetch.successful())
            {
                return { EEngineStatus::Error };
            }
            else
            {
                Shared<CallbackPass<SPassData>> pass = passFetch.data();
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
