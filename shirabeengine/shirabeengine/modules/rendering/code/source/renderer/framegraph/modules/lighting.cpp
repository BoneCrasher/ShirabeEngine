#include "renderer/framegraph/modules/lighting.h"
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
            CRenderGraphModule<SLightingModuleTag_t>::SLightingExportData
        >
        CRenderGraphModule<SLightingModuleTag_t>::addLightingPass(
            std::string const      &aPassName,
                CGraphBuilder          &aGraphBuilder,
                SRenderGraphImageView &aGbuffer0,
                SRenderGraphImageView &aGbuffer1,
                SRenderGraphImageView &aGbuffer2,
                SRenderGraphImageView &aGbuffer3,
                SRenderGraphImageView &aDepthStencil)
        {
            /**
             * The SState struct is the internal state of the lighting pass.
             */
            struct SState
            {
                SRenderGraphRenderTarget lightAccumulationBufferTextureId;
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

            /**
             * Implement the setup function
             */
            auto const setup =
                           [&] (
                               CPassBuilder                       &aBuilder
                               , SPassData                        &aOutPassData
                               , SRenderGraphPlatformContext const &aPlatformContext
                               , SRenderGraphDataSource const      &aDataSource
                               ) -> CEngineResult<>
                               {
                                   auto gbufferTextureFetch = aGraphBuilder.getResources()
                                                                           .getResource<SRenderGraphImage>(aGbuffer0.subjacentResource);
                                   if(not gbufferTextureFetch.successful())
                                   {
                                       CLog::Error(logTag(), "Failed to fetch gbuffer texture.");
                                       return { EEngineStatus::Error };
                                   }

                                   SRenderGraphImage gbufferTexture = *(gbufferTextureFetch.data());

                                   SRenderGraphRenderTarget lightAccBufferDesc ={ };
                                   lightAccBufferDesc.width          = gbufferTexture.width;
                                   lightAccBufferDesc.height         = gbufferTexture.height;
                                   lightAccBufferDesc.depth          = 1;
                                   lightAccBufferDesc.format         = RenderGraphFormat_t::R32G32B32A32_FLOAT;
                                   lightAccBufferDesc.mipLevels      = 1;
                                   lightAccBufferDesc.arraySize      = 1;
                                   lightAccBufferDesc.initialState   = ERenderGraphResourceInitState::Clear;
                                   lightAccBufferDesc.permittedUsage = ERenderGraphResourceUsage::InputAttachment | ERenderGraphResourceUsage::ColorAttachment;

                                   aOutPassData.state.lightAccumulationBufferTextureId = aBuilder.createRenderTarget("Light Accumulation Buffer", lightAccBufferDesc).data();

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

                                   aOutPassData.importData.gbuffer0 = aBuilder.readAttachment(aGbuffer0,     readFlags     ).data();
                                   aOutPassData.importData.gbuffer1 = aBuilder.readAttachment(aGbuffer1,     readFlags     ).data();
                                   aOutPassData.importData.gbuffer2 = aBuilder.readAttachment(aGbuffer2,     readFlags     ).data();
                                   aOutPassData.importData.gbuffer3 = aBuilder.readAttachment(aGbuffer3,     readFlags     ).data();
                                   aOutPassData.importData.depth    = aBuilder.readAttachment(aDepthStencil, depthReadFlags).data();

                                   SRenderGraphWriteTextureFlags writeFlags{ };
                                   writeFlags.requiredFormat  = RenderGraphFormat_t::Automatic;
                                   writeFlags.writeTarget     = ERenderGraphWriteTarget::Color;
                                   writeFlags.arraySliceRange = CRange(0, 1);
                                   writeFlags.mipSliceRange   = CRange(0, 1);

                                   aOutPassData.exportData.lightAccumulationBuffer = aBuilder.writeAttachment(aOutPassData.state.lightAccumulationBufferTextureId, writeFlags).data();

                                   SRenderGraphPipelineConfig pipelineConfig {};
                                   SRenderGraphMaterial const &material = aBuilder.useMaterial("phong_lighting", util::crc32FromString("materials/deferred/phong/phong_lighting.material.meta"), pipelineConfig).data();
                                   aOutPassData.importData.material = material;

                                   return { EEngineStatus::Ok };
                               };

            /**
             * Implement the execute function
             */
            auto const execute = [=] (SPassData const                    &aPassData
                                      , SRenderGraphPlatformContext const &aPlatformContext
                                      , SRenderGraphDataSource const      &aDataSource
                                      , CRenderGraphResources const       &aRenderGraphResources
                                      , SRenderGraphRenderContextState    &aRenderContextState
                                      , SRenderGraphResourceContext       &aResourceContext
                                      , SRenderGraphRenderContext         &aRenderContext)
                    -> CEngineResult<>
            {
                CLog::Verbose(logTag(), "Lighting");

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
