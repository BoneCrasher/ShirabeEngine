#include "renderer/framegraph/modules/compositing.h"

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
                auto gbufferTextureFetch = aGraphBuilder.getResources().get<SFrameGraphTexture>(aGbuffer0.subjacentResource);
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
                compositingBufferDesc.format         = FrameGraphFormat_t::R8G8B8A8_UNORM;
                compositingBufferDesc.mipLevels      = 1;
                compositingBufferDesc.arraySize      = 1;
                compositingBufferDesc.initialState   = EFrameGraphResourceInitState::Clear;
                compositingBufferDesc.permittedUsage = EFrameGraphResourceUsage::InputAttachment | EFrameGraphResourceUsage::ColorAttachment;

                aOutPassData.state.compositingBufferId = aBuilder.createTexture("Compositing Buffer", compositingBufferDesc).data();

                SFrameGraphReadTextureFlags readFlags{ };
                readFlags.requiredFormat = FrameGraphFormat_t::Automatic;
                readFlags.source         = EFrameGraphReadSource::Color;

                aOutPassData.importData.gbuffer0                = aBuilder.readTexture(aGbuffer0,                readFlags, CRange(0, 1), CRange(0, 1)).data();
                aOutPassData.importData.gbuffer1                = aBuilder.readTexture(aGbuffer1,                readFlags, CRange(0, 1), CRange(0, 1)).data();
                aOutPassData.importData.gbuffer2                = aBuilder.readTexture(aGbuffer2,                readFlags, CRange(0, 1), CRange(0, 1)).data();
                aOutPassData.importData.gbuffer3                = aBuilder.readTexture(aGbuffer3,                readFlags, CRange(0, 1), CRange(0, 1)).data();
                aOutPassData.importData.lightAccumulationBuffer = aBuilder.readTexture(aLightAccumulationBuffer, readFlags, CRange(0, 1), CRange(0, 1)).data();

                SFrameGraphWriteTextureFlags writeFlags{ };
                writeFlags.requiredFormat = FrameGraphFormat_t::Automatic;
                writeFlags.writeTarget    = EFrameGraphWriteTarget::Color;

                aOutPassData.exportData.output = aBuilder.writeTexture(aOutPassData.state.compositingBufferId, writeFlags, CRange(0, 1), CRange(0, 1)).data();

                return { EEngineStatus::Ok };
            };

            auto const execute = [=] (
                    SPassData                                 const&aPassData,
                    CFrameGraphResources                      const&aFrameGraphResources,
                    CStdSharedPtr_t<IFrameGraphRenderContext>      &aContext)
                    -> CEngineResult<>
            {
                SHIRABE_UNUSED(aPassData);
                SHIRABE_UNUSED(aFrameGraphResources);
                SHIRABE_UNUSED(aContext);

                CLog::Verbose(logTag(), "Compositing");

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
