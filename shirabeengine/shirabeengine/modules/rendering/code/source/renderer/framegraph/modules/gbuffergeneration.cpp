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
                std::string               const &aPassName,
                CGraphBuilder                   &aGraphBuilder)
        {
            /**
             * The SState struct is the internal state of the gbuffer generation pass.
             */
            struct SState
            {
                SFrameGraphResource gbufferTexture0Id;
                SFrameGraphResource gbufferTexture1Id;
                SFrameGraphResource gbufferTexture2And3Id;
                SFrameGraphResource depthStencilTextureId;
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

            // ----------------------------------------------------------------------------------
            // Setup
            // ----------------------------------------------------------------------------------
            auto const setup = [&] (CPassBuilder &aBuilder,
                                    SPassData    &aOutPassData) -> CEngineResult<>
            {
                // Default extents.
                uint32_t width  = 1920;
                uint32_t height = 1080;

                Shared<os::SApplicationEnvironment> environment = aGraphBuilder.applicationEnvironment();
                if(environment)
                {
                    Shared<wsi::CWSIDisplay> const &display     = aGraphBuilder.display();
                    os::SOSDisplayDescriptor const &displayDesc = display->screenInfo()[display->primaryScreenIndex()];
                    width  = displayDesc.bounds.size.x();
                    height = displayDesc.bounds.size.y();
                }

                SFrameGraphDynamicTexture gbuffer0Desc ={ };
                gbuffer0Desc.width              = width;
                gbuffer0Desc.height             = height;
                gbuffer0Desc.depth              = 1;
                gbuffer0Desc.format             = FrameGraphFormat_t::R32G32B32A32_FLOAT;
                gbuffer0Desc.initialState       = EFrameGraphResourceInitState::Clear;
                gbuffer0Desc.arraySize          = 1;
                gbuffer0Desc.mipLevels          = 1;
                gbuffer0Desc.permittedUsage     =   EFrameGraphResourceUsage::InputAttachment
                                                  | EFrameGraphResourceUsage::ColorAttachment;
                SFrameGraphDynamicTexture gbuffer1Desc ={ };
                gbuffer1Desc.width              = width;
                gbuffer1Desc.height             = height;
                gbuffer1Desc.depth              = 1;
                gbuffer1Desc.format             = FrameGraphFormat_t::R32G32B32A32_FLOAT;
                gbuffer1Desc.initialState       = EFrameGraphResourceInitState::Clear;
                gbuffer1Desc.arraySize          = 1;
                gbuffer1Desc.mipLevels          = 1;
                gbuffer1Desc.permittedUsage     =   EFrameGraphResourceUsage::InputAttachment
                                                  | EFrameGraphResourceUsage::ColorAttachment;
                SFrameGraphDynamicTexture gbuffer2And3Desc ={ };
                gbuffer2And3Desc.width          = width;
                gbuffer2And3Desc.height         = height;
                gbuffer2And3Desc.depth          = 1;
                gbuffer2And3Desc.format         = FrameGraphFormat_t::B8G8R8A8_UNORM;
                gbuffer2And3Desc.initialState   = EFrameGraphResourceInitState::Clear;
                gbuffer2And3Desc.arraySize      = 2;
                gbuffer2And3Desc.mipLevels      = 1;
                gbuffer2And3Desc.permittedUsage =   EFrameGraphResourceUsage::InputAttachment
                                                  | EFrameGraphResourceUsage::ColorAttachment;

                SFrameGraphDynamicTexture depthStencilDesc ={ };
                depthStencilDesc.width          = width;
                depthStencilDesc.height         = height;
                depthStencilDesc.depth          = 1;
                depthStencilDesc.format         = FrameGraphFormat_t::D24_UNORM_S8_UINT;
                depthStencilDesc.initialState   = EFrameGraphResourceInitState::Clear;
                depthStencilDesc.arraySize      = 1;
                depthStencilDesc.mipLevels      = 1;
                depthStencilDesc.permittedUsage =   EFrameGraphResourceUsage::InputAttachment
                                                  | EFrameGraphResourceUsage::DepthAttachment;

                // Basic underlying output buffer to be linked
                aOutPassData.state.gbufferTexture0Id     = aBuilder.createTexture("GBuffer Array Texture - Positions", gbuffer0Desc).data();
                aOutPassData.state.gbufferTexture1Id     = aBuilder.createTexture("GBuffer Array Texture - Normals",   gbuffer1Desc).data();
                aOutPassData.state.gbufferTexture2And3Id = aBuilder.createTexture("GBuffer Array Texture - Other",     gbuffer2And3Desc).data();
                aOutPassData.state.depthStencilTextureId = aBuilder.createTexture("DepthStencil Texture",              depthStencilDesc).data();

                // This will create a list of render targets for the texutre array to render to.
                // They'll be internally created and managed.
                SFrameGraphWriteTextureFlags baseFlags = {};
                baseFlags.requiredFormat  = EFormat::Undefined;
                baseFlags.writeTarget     = EFrameGraphWriteTarget::Color;
                baseFlags.arraySliceRange = CRange(0, 1);
                baseFlags.mipSliceRange   = CRange(0, 1);

                SFrameGraphWriteTextureFlags write0 = baseFlags,
                                             write1 = baseFlags,
                                             write2 = baseFlags,
                                             write3 = baseFlags;

                write0.arraySliceRange.offset = 0;
                write0.requiredFormat         = gbuffer0Desc.format;
                write1.arraySliceRange.offset = 0;
                write1.requiredFormat         = gbuffer1Desc.format;
                write2.arraySliceRange.offset = 0;
                write2.requiredFormat         = gbuffer2And3Desc.format;
                write3.arraySliceRange.offset = 1;
                write3.requiredFormat         = gbuffer2And3Desc.format;

                SFrameGraphWriteTextureFlags depthFlags = {};
                depthFlags.requiredFormat  = depthStencilDesc.format;
                depthFlags.writeTarget     = EFrameGraphWriteTarget::Depth;
                depthFlags.arraySliceRange = CRange(0, 1);
                depthFlags.mipSliceRange   = CRange(0, 1);

                aOutPassData.exportData.gbuffer0     = aBuilder.writeAttachment(aOutPassData.state.gbufferTexture0Id,     write0).data();
                aOutPassData.exportData.gbuffer1     = aBuilder.writeAttachment(aOutPassData.state.gbufferTexture1Id,     write1).data();
                aOutPassData.exportData.gbuffer2     = aBuilder.writeAttachment(aOutPassData.state.gbufferTexture2And3Id, write2).data();
                aOutPassData.exportData.gbuffer3     = aBuilder.writeAttachment(aOutPassData.state.gbufferTexture2And3Id, write3).data();
                aOutPassData.exportData.depthStencil = aBuilder.writeAttachment(aOutPassData.state.depthStencilTextureId, depthFlags).data();

                return { EEngineStatus::Ok };
            };

            // ----------------------------------------------------------------------------------
            // Execution
            // ----------------------------------------------------------------------------------
            auto const execute = [=] (
                    SPassData                const &aPassData,
                    SFrameGraphDataSource    const &aDataSource,
                    CFrameGraphResources     const &aFrameGraphResources,
                    SFrameGraphRenderContextState  &aRenderContextState,
                    SFrameGraphRenderContext       &aRenderContext)
                    -> CEngineResult<>
            {
                using namespace engine::rendering;

                CLog::Verbose(logTag(), "GBufferGeneration");

                std::vector<SFrameGraphRenderableResources> renderables = aDataSource.fetchRenderables({});

                for(SFrameGraphRenderableResources const &renderableResources : renderables)
                {
                    aRenderContext.useMesh    (aRenderContextState, renderableResources.meshResource);
                    aRenderContext.drawIndexed(aRenderContextState, renderableResources.meshResource.indexCount);
                }

                return { EEngineStatus::Ok };
            };

            // ----------------------------------------------------------------------------------
            // Spawn the pass
            // ----------------------------------------------------------------------------------
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
