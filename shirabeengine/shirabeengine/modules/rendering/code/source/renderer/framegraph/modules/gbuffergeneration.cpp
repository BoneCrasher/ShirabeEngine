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
            struct SStaticPassData
            {
                SGBufferGenerationImportData importData;
                SGBufferGenerationExportData exportData;

                SState state;
            };

            struct SDynamicPassData
            { };

            // ----------------------------------------------------------------------------------
            // Setup
            // ----------------------------------------------------------------------------------
            auto const staticSetup =
                           [&] (CPassStaticBuilder &aBuilder
                                , SStaticPassData  &aOutPassData) -> CEngineResult<>
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

                                   SFrameGraphTexture gbuffer0Desc ={ };
                                   gbuffer0Desc.width              = width;
                                   gbuffer0Desc.height             = height;
                                   gbuffer0Desc.depth              = 1;
                                   gbuffer0Desc.format             = FrameGraphFormat_t::R32G32B32A32_FLOAT;
                                   gbuffer0Desc.initialState       = EFrameGraphResourceInitState::Clear;
                                   gbuffer0Desc.arraySize          = 1;
                                   gbuffer0Desc.mipLevels          = 1;
                                   gbuffer0Desc.permittedUsage     =   EFrameGraphResourceUsage::InputAttachment
                                                                       | EFrameGraphResourceUsage::ColorAttachment;
                                   SFrameGraphTexture gbuffer1Desc ={ };
                                   gbuffer1Desc.width              = width;
                                   gbuffer1Desc.height             = height;
                                   gbuffer1Desc.depth              = 1;
                                   gbuffer1Desc.format             = FrameGraphFormat_t::R32G32B32A32_FLOAT;
                                   gbuffer1Desc.initialState       = EFrameGraphResourceInitState::Clear;
                                   gbuffer1Desc.arraySize          = 1;
                                   gbuffer1Desc.mipLevels          = 1;
                                   gbuffer1Desc.permittedUsage     =   EFrameGraphResourceUsage::InputAttachment
                                                                       | EFrameGraphResourceUsage::ColorAttachment;
                                   SFrameGraphTexture gbuffer2And3Desc ={ };
                                   gbuffer2And3Desc.width          = width;
                                   gbuffer2And3Desc.height         = height;
                                   gbuffer2And3Desc.depth          = 1;
                                   gbuffer2And3Desc.format         = FrameGraphFormat_t::B8G8R8A8_UNORM;
                                   gbuffer2And3Desc.initialState   = EFrameGraphResourceInitState::Clear;
                                   gbuffer2And3Desc.arraySize      = 2;
                                   gbuffer2And3Desc.mipLevels      = 1;
                                   gbuffer2And3Desc.permittedUsage =   EFrameGraphResourceUsage::InputAttachment
                                                                       | EFrameGraphResourceUsage::ColorAttachment;

                                   SFrameGraphTexture depthStencilDesc ={ };
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
            auto const dynamicSetup =
                           [&] (CPassDynamicBuilder           &aBuilder
                                , SFrameGraphDataSource const &aDataSource
                                , SDynamicPassData            &aOutPassData) -> CEngineResult<>
                               {
                                   std::vector<SFrameGraphRenderableResources> renderables = aDataSource.fetchRenderables({});
                                   std::vector<SFrameGraphMesh>                validMeshes;

                                   // Render-Loop
                                   for(auto const &renderableResources : renderables)
                                   {
                                        auto [result, mesh] = aBuilder.useMesh(renderableResources.mesh); // Will trace down the component hierarchies...

                                       for(std::size_t k=0; k<renderableResources.mesh.materials.size(); ++k)
                                       {
                                           auto [result, material] = aBuilder.useMaterial(renderableResources.mesh.materials[k]);

                                           for(auto const &buffer : renderableResources.mesh.materials[k].uniformBuffers)
                                           {
                                               auto const [result, bufferResource] = aBuilder.readBuffer(buffer);
                                               material.uniformBuffers.push_back(bufferResource);
                                           }

                                           for(auto const &texture : renderableResources.mesh.materials[k].textures)
                                           {
                                               SFrameGraphTextureResourceFlags flags;
                                               flags.arraySliceRange = CRange(0, 1);
                                               flags.mipSliceRange   = CRange(0, 1);
                                               flags.requiredFormat  = EFormat::Automatic;
                                               auto const [result, textureResource] = aBuilder.readTexture(texture, flags);
                                               material.textures.push_back(textureResource);
                                           }

                                           auto const [pipelineResult, pipeline] = aBuilder.usePipeline(material.basePipeline, {});

                                           mesh.materials[k] = material;
                                       }

                                       validMeshes.push_back(mesh);
                                   }
                               };
            // ----------------------------------------------------------------------------------
            // Execution
            // ----------------------------------------------------------------------------------
            auto const execute = [=] (SStaticPassData          const &aStaticPassData,
                                      SDynamicPassData         const &aDynamicPassData,
                                      SFrameGraphDataSource    const &aDataSource,
                                      CFrameGraphResources     const &aFrameGraphResources,
                                      SFrameGraphRenderContextState  &aRenderContextState,
                                      SFrameGraphResourceContext     &aResourceContext,
                                      SFrameGraphRenderContext       &aRenderContext) -> CEngineResult<>
            {
                using namespace engine::rendering;

                CLog::Verbose(logTag(), "GBufferGeneration");

                std::vector<SFrameGraphRenderableResources> renderables = aDataSource.fetchRenderables({});

                // Render-Loop
                for(SFrameGraphRenderableResources const &renderableResources : renderables)
                {
                    EEngineStatus const meshInitState = aResourceContext.initializeMesh(renderableResources.meshResource);
                    if(EEngineStatus::Ok != meshInitState) { continue; }
                    EEngineStatus const meshLoadState = aResourceContext.updateMesh(renderableResources.meshResource);
                    if(EEngineStatus::Ok != meshLoadState) { continue; }

                    aRenderContext.useMesh(aRenderContextState, renderableResources.meshResource);

                    for(SFrameGraphMaterial const &material : renderableResources.meshResource.materials)
                    {
                        EEngineStatus const pipelineInitState = aResourceContext.createPipeline(material.basePipeline);
                        if(EEngineStatus::Ok != pipelineInitState) { continue; }

                        EEngineStatus const materialInitState = aResourceContext.initializeMaterial(material);
                        if(EEngineStatus::Ok != materialInitState) { continue; }
                        EEngineStatus const materialLoadState = aResourceContext.updateMaterial(material);
                        if(EEngineStatus::Ok != materialLoadState) { continue; }

                        aRenderContext.drawIndexed(aRenderContextState, renderableResources.meshResource.indexCount);
                    }
                }

                return { EEngineStatus::Ok };
            };

            // ----------------------------------------------------------------------------------
            // Spawn the pass
            // ----------------------------------------------------------------------------------
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
