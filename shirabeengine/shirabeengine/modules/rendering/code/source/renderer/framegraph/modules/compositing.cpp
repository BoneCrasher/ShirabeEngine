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
            CFrameGraphModule<SCompositingModuleTag_t>::SExportData
        >
        CFrameGraphModule<SCompositingModuleTag_t>::addDefaultCompositingPass(
                std::string         const &aPassName,
                CGraphBuilder             &aGraphBuilder,
                SFrameGraphResource const &aGbuffer0,
                SFrameGraphResource const &aGbuffer1,
                SFrameGraphResource const &aGbuffer2,
                SFrameGraphResource const &aGbuffer3,
                SFrameGraphResource const &aDepthStencil,
                SFrameGraphResource const &aLightAccumulationBuffer)
        {
            /**
             * The SState struct is the internal state of the compositing pass.
             */
            struct SState
            {
                SFrameGraphResource compositingBufferId;
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
                    SPassData    &aOutPassData)
                    -> CEngineResult<>
            {
                auto gbufferTextureFetch = aGraphBuilder.getResources()
                                                        .getResource<SFrameGraphTexture>(aGbuffer0.subjacentResource);
                if(not gbufferTextureFetch.successful())
                {
                    CLog::Error(logTag(), "Failed to fetch gbuffer texure.");
                    return { EEngineStatus::Error };
                }

                SFrameGraphTexture gbufferTexture = *(gbufferTextureFetch.data());

                SFrameGraphTexture compositingBufferDesc ={ };
                compositingBufferDesc.width          = gbufferTexture.width;
                compositingBufferDesc.height         = gbufferTexture.height;
                compositingBufferDesc.depth          = 1;
                compositingBufferDesc.format         = FrameGraphFormat_t::B8G8R8A8_UNORM;
                compositingBufferDesc.mipLevels      = 1;
                compositingBufferDesc.arraySize      = 1;
                compositingBufferDesc.initialState   = EFrameGraphResourceInitState::Clear;
                compositingBufferDesc.permittedUsage = EFrameGraphResourceUsage::InputAttachment | EFrameGraphResourceUsage::ColorAttachment;

                aOutPassData.state.compositingBufferId = aBuilder.createTexture("Compositing Buffer", compositingBufferDesc).data();

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

                aOutPassData.importData.gbuffer0                = aBuilder.readAttachment(aGbuffer0,                readFlags     ).data();
                aOutPassData.importData.gbuffer1                = aBuilder.readAttachment(aGbuffer1,                readFlags     ).data();
                aOutPassData.importData.gbuffer2                = aBuilder.readAttachment(aGbuffer2,                readFlags     ).data();
                aOutPassData.importData.gbuffer3                = aBuilder.readAttachment(aGbuffer3,                readFlags     ).data();
                aOutPassData.importData.depth                   = aBuilder.readAttachment(aDepthStencil,            depthReadFlags).data();
                aOutPassData.importData.lightAccumulationBuffer = aBuilder.readAttachment(aLightAccumulationBuffer, readFlags     ).data();

                SFrameGraphWriteTextureFlags writeFlags{ };
                writeFlags.requiredFormat  = FrameGraphFormat_t::Automatic;
                writeFlags.writeTarget     = EFrameGraphWriteTarget::Color;
                writeFlags.arraySliceRange = CRange(0, 1);
                writeFlags.mipSliceRange   = CRange(0, 1);

                aOutPassData.exportData.output = aBuilder.writeAttachment(aOutPassData.state.compositingBufferId, writeFlags).data();

                SFrameGraphPipelineConfig pipelineConfig {};
                SFrameGraphMaterial const &material = aBuilder.useMaterial("compositing", util::crc32FromString("materials/deferred/compositing/compositing.material.meta"), pipelineConfig).data();
                aOutPassData.importData.material = material;

                return { EEngineStatus::Ok };
            };

            auto const execute = [=] (
                    SPassData                const &aPassData,
                    SFrameGraphDataSource    const &aDataSource,
                    CFrameGraphResources     const &aFrameGraphResources,
                    SFrameGraphRenderContextState  &aRenderContextState,
                    SFrameGraphResourceContext     &aResourceContext,
                    SFrameGraphRenderContext       &aRenderContext)
                    -> CEngineResult<>
            {
                CLog::Verbose(logTag(), "Compositing");

                aRenderContext.drawFullscreenQuadWithMaterial(aRenderContextState, aPassData.importData.material);

                return { EEngineStatus::Ok };
            };

            auto passFetch = aGraphBuilder.spawnPass<CallbackPass<SPassData>>(aPassName, setup, execute);
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
