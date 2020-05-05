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
            CFrameGraphModule<SLightingModuleTag_t>::SLightingExportData
        >
        CFrameGraphModule<SLightingModuleTag_t>::addLightingPass(
                std::string         const &aPassName,
                CGraphBuilder             &aGraphBuilder,
                SFrameGraphResource const &aGbuffer0,
                SFrameGraphResource const &aGbuffer1,
                SFrameGraphResource const &aGbuffer2,
                SFrameGraphResource const &aGbuffer3,
                SFrameGraphResource const &aDepthStencil)
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
            struct SStaticPassData
            {
                SLightingImportData importData;
                SLightingExportData exportData;

                SState state;
            };

            struct SDynamicPassData
            { };

            /**
             * Implement the setup function
             */
            auto const staticSetup =
                           [&] (
                               CPassStaticBuilder &aBuilder,
                               SStaticPassData    &aOutPassData) -> CEngineResult<>
                               {
                                   auto gbufferTextureFetch = aGraphBuilder.getResources()
                                                                           .getResource<SFrameGraphTexture>(aGbuffer0.subjacentResource);
                                   if(not gbufferTextureFetch.successful())
                                   {
                                       CLog::Error(logTag(), "Failed to fetch gbuffer texture.");
                                       return { EEngineStatus::Error };
                                   }

                                   SFrameGraphTexture gbufferTexture = *(gbufferTextureFetch.data());

                                   SFrameGraphTexture lightAccBufferDesc ={ };
                                   lightAccBufferDesc.width          = gbufferTexture.width;
                                   lightAccBufferDesc.height         = gbufferTexture.height;
                                   lightAccBufferDesc.depth          = 1;
                                   lightAccBufferDesc.format         = FrameGraphFormat_t::R32G32B32A32_FLOAT;
                                   lightAccBufferDesc.mipLevels      = 1;
                                   lightAccBufferDesc.arraySize      = 1;
                                   lightAccBufferDesc.initialState   = EFrameGraphResourceInitState::Clear;
                                   lightAccBufferDesc.permittedUsage = EFrameGraphResourceUsage::InputAttachment | EFrameGraphResourceUsage::ColorAttachment;

                                   aOutPassData.state.lightAccumulationBufferTextureId = aBuilder.createTexture("Light Accumulation Buffer", lightAccBufferDesc).data();

                                   SFrameGraphReadTextureFlags readFlags{ };
                                   readFlags.requiredFormat  = FrameGraphFormat_t::Automatic;
                                   readFlags.readSource      = EFrameGraphReadSource::Color;
                                   readFlags.arraySliceRange = CRange(0, 1);
                                   readFlags.mipSliceRange   = CRange(0, 1);

                                   SFrameGraphReadTextureFlags depthReadFlags{ };
                                   depthReadFlags.requiredFormat  = FrameGraphFormat_t::Automatic;
                                   depthReadFlags.readSource      = EFrameGraphReadSource::Depth;
                                   depthReadFlags.arraySliceRange = CRange(0, 1);
                                   depthReadFlags.mipSliceRange   = CRange(0, 1);

                                   aOutPassData.importData.gbuffer0 = aBuilder.readAttachment(aGbuffer0,     readFlags     ).data();
                                   aOutPassData.importData.gbuffer1 = aBuilder.readAttachment(aGbuffer1,     readFlags     ).data();
                                   aOutPassData.importData.gbuffer2 = aBuilder.readAttachment(aGbuffer2,     readFlags     ).data();
                                   aOutPassData.importData.gbuffer3 = aBuilder.readAttachment(aGbuffer3,     readFlags     ).data();
                                   aOutPassData.importData.depth    = aBuilder.readAttachment(aDepthStencil, depthReadFlags).data();

                                   SFrameGraphWriteTextureFlags writeFlags{ };
                                   writeFlags.requiredFormat  = FrameGraphFormat_t::Automatic;
                                   writeFlags.writeTarget     = EFrameGraphWriteTarget::Color;
                                   writeFlags.arraySliceRange = CRange(0, 1);
                                   writeFlags.mipSliceRange   = CRange(0, 1);

                                   aOutPassData.exportData.lightAccumulationBuffer = aBuilder.writeAttachment(aOutPassData.state.lightAccumulationBufferTextureId, writeFlags).data();

                                   SFrameGraphPipelineConfig pipelineConfig {};
                                   SFrameGraphMaterial const &material = aBuilder.useMaterial("phong_lighting", util::crc32FromString("materials/deferred/phong/phong_lighting.material.meta"), pipelineConfig).data();
                                   aOutPassData.importData.material = material;

                                   return { EEngineStatus::Ok };
                               };

            auto const dynamicSetup =
                           [&] (CPassDynamicBuilder           &aBuilder
                                , SFrameGraphDataSource const &aDataSource
                                , SDynamicPassData            &aOutPassData) -> CEngineResult<>
                               {
                                   return EEngineStatus::Ok;
                               };

            /**
             * Implement the execute function
             */
            auto const execute = [=] (
                    SStaticPassData          const &aStaticPassData,
                    SDynamicPassData         const &aDynamicPassData,
                    SFrameGraphDataSource    const &aDataSource,
                    CFrameGraphResources     const &aFrameGraphResources,
                    SFrameGraphRenderContextState  &aRenderContextState,
                    SFrameGraphResourceContext     &aResourceContext,
                    SFrameGraphRenderContext       &aRenderContext)
                    -> CEngineResult<>
            {
                CLog::Verbose(logTag(), "Lighting");

                aRenderContext.drawFullscreenQuadWithMaterial(aRenderContextState, aStaticPassData.importData.material);

                return { EEngineStatus::Ok };
            };

            auto passFetch = aGraphBuilder.spawnPass<CallbackPass<SStaticPassData, SDynamicPassData>>(aPassName, staticSetup, dynamicSetup, execute);
            if(not passFetch.successful())
            {
                return { EEngineStatus::Error };
            }
            else
            {
                Shared<CallbackPass<SStaticPassData, SDynamicPassData>> pass = passFetch.data();
                if(nullptr == pass)
                {
                    return { EEngineStatus::NullPointer };
                }
                else
                {
                    return { EEngineStatus::Ok, passFetch.data()->staticPassData().exportData };
                }
            }
        }
        //<-----------------------------------------------------------------------------
    }
}
