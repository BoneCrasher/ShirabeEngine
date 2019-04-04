#include "renderer/framegraph/modules/lighting.h"

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
                SFrameGraphResource const &aGbuffer3)
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
            struct SPassData
            {
                SLightingImportData importData;
                SLightingExportData exportData;

                SState state;
            };

            /**
             * Implement the setup function
             */
            auto const setup = [&] (
                    CPassBuilder &aBuilder,
                    SPassData    &aOutPassData)
                    -> CEngineResult<>
            {
                auto gbufferTextureFetch = aGraphBuilder.getResources().get<SFrameGraphTexture>(aGbuffer0.subjacentResource);
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
                lightAccBufferDesc.format         = FrameGraphFormat_t::R32_FLOAT;
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

                aOutPassData.importData.gbuffer0 = aBuilder.readAttachment(aGbuffer0, readFlags).data();
                aOutPassData.importData.gbuffer1 = aBuilder.readAttachment(aGbuffer1, readFlags).data();
                aOutPassData.importData.gbuffer2 = aBuilder.readAttachment(aGbuffer2, readFlags).data();
                aOutPassData.importData.gbuffer3 = aBuilder.readAttachment(aGbuffer3, readFlags).data();

                SFrameGraphWriteTextureFlags writeFlags{ };
                writeFlags.requiredFormat  = FrameGraphFormat_t::Automatic;
                writeFlags.writeTarget     = EFrameGraphWriteTarget::Color;
                writeFlags.arraySliceRange = CRange(0, 1);
                writeFlags.mipSliceRange   = CRange(0, 1);

                aOutPassData.exportData.lightAccumulationBuffer = aBuilder.writeAttachment(aOutPassData.state.lightAccumulationBufferTextureId, writeFlags).data();

                return { EEngineStatus::Ok };
            };

            /**
             * Implement the execute function
             */
            auto const execute = [=] (
                    SPassData                                 const&aPassData,
                    CFrameGraphResources                      const&aFrameGraphResources,
                    CStdSharedPtr_t<IFrameGraphRenderContext>      &aContext)
                    -> CEngineResult<>
            {
                SHIRABE_UNUSED(aPassData);
                SHIRABE_UNUSED(aFrameGraphResources);
                SHIRABE_UNUSED(aContext);

                CLog::Verbose(logTag(), "Lighting");


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
