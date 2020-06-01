#include "renderer/rendergraph/modules/gbuffergeneration.h"

namespace engine
{
    namespace framegraph
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult
        <
            CRenderGraphModule<SGBufferModuleTag_t>::SGBufferGenerationExportData
        >
        CRenderGraphModule<SGBufferModuleTag_t>::addGBufferGenerationPass(std::string const                  &aPassName
                                                                          , CGraphBuilder                    &aGraphBuilder
                                                                          , SBufferGenerationInputData const &aInputData)
        {
            /**
             * The SState struct is the internal state of the gbuffer generation pass.
             */
            struct SState
            {
                SRenderGraphRenderTarget gbufferTexture0Id;
                SRenderGraphRenderTarget gbufferTexture1Id;
                SRenderGraphRenderTarget gbufferTexture2And3Id;
                SRenderGraphRenderTarget depthStencilTextureId;
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
            auto const setup =
                           [&] (CPassBuilder                       &aBuilder
                                , SPassData                        &aOutPassData
                                , SRenderGraphPlatformContext const &aPlatformContext
                                , SRenderGraphDataSource const      &aDataSource) -> CEngineResult<>
                               {
                                   // Default extents.
                                   uint32_t width  = 1920;
                                   uint32_t height = 1080;

                                   Shared<os::SApplicationEnvironment> environment = aPlatformContext.applicationEnvironment;
                                   if(environment)
                                   {
                                       Shared<wsi::CWSIDisplay> const &display     = aPlatformContext.display;
                                       os::SOSDisplayDescriptor const &displayDesc = display->screenInfo()[display->primaryScreenIndex()];
                                       width  = displayDesc.bounds.size.x();
                                       height = displayDesc.bounds.size.y();
                                   }

                                   SRenderGraphDynamicImageDescription gbuffer0Desc ={ };
                                   gbuffer0Desc.width              = width;
                                   gbuffer0Desc.height             = height;
                                   gbuffer0Desc.depth              = 1;
                                   gbuffer0Desc.format             = RenderGraphFormat_t::R32G32B32A32_FLOAT;
                                   gbuffer0Desc.initialState       = ERenderGraphResourceInitState::Clear;
                                   gbuffer0Desc.arraySize          = 1;
                                   gbuffer0Desc.mipLevels          = 1;
                                   gbuffer0Desc.permittedUsage     = ERenderGraphResourceUsage::InputAttachment
                                                                     | ERenderGraphResourceUsage::ColorAttachment;
                                   SRenderGraphDynamicImageDescription gbuffer1Desc ={ };
                                   gbuffer1Desc.width              = width;
                                   gbuffer1Desc.height             = height;
                                   gbuffer1Desc.depth              = 1;
                                   gbuffer1Desc.format             = RenderGraphFormat_t::R32G32B32A32_FLOAT;
                                   gbuffer1Desc.initialState       = ERenderGraphResourceInitState::Clear;
                                   gbuffer1Desc.arraySize          = 1;
                                   gbuffer1Desc.mipLevels          = 1;
                                   gbuffer1Desc.permittedUsage     = ERenderGraphResourceUsage::InputAttachment
                                                                     | ERenderGraphResourceUsage::ColorAttachment;
                                   SRenderGraphDynamicImageDescription gbuffer2And3Desc ={ };
                                   gbuffer2And3Desc.width          = width;
                                   gbuffer2And3Desc.height         = height;
                                   gbuffer2And3Desc.depth          = 1;
                                   gbuffer2And3Desc.format         = RenderGraphFormat_t::B8G8R8A8_UNORM;
                                   gbuffer2And3Desc.initialState   = ERenderGraphResourceInitState::Clear;
                                   gbuffer2And3Desc.arraySize      = 2;
                                   gbuffer2And3Desc.mipLevels      = 1;
                                   gbuffer2And3Desc.permittedUsage = ERenderGraphResourceUsage::InputAttachment
                                                                     | ERenderGraphResourceUsage::ColorAttachment;

                                   SRenderGraphDynamicImageDescription depthStencilDesc ={ };
                                   depthStencilDesc.width          = width;
                                   depthStencilDesc.height         = height;
                                   depthStencilDesc.depth          = 1;
                                   depthStencilDesc.format         = RenderGraphFormat_t::D24_UNORM_S8_UINT;
                                   depthStencilDesc.initialState   = ERenderGraphResourceInitState::Clear;
                                   depthStencilDesc.arraySize      = 1;
                                   depthStencilDesc.mipLevels      = 1;
                                   depthStencilDesc.permittedUsage = ERenderGraphResourceUsage::InputAttachment
                                                                     | ERenderGraphResourceUsage::DepthAttachment;

                                   // Basic underlying output buffer to be linked
                                   aOutPassData.state.gbufferTexture0Id     = aBuilder.createRenderTarget("GBuffer Array Texture - Positions", gbuffer0Desc).data();
                                   aOutPassData.state.gbufferTexture1Id     = aBuilder.createRenderTarget("GBuffer Array Texture - Normals",   gbuffer1Desc).data();
                                   aOutPassData.state.gbufferTexture2And3Id = aBuilder.createRenderTarget("GBuffer Array Texture - Other",     gbuffer2And3Desc).data();
                                   aOutPassData.state.depthStencilTextureId = aBuilder.createRenderTarget("DepthStencil Texture",              depthStencilDesc).data();

                                   // This will create a list of render targets for the texutre array to render to.
                                   // They'll be internally created and managed.
                                   SRenderGraphWriteTextureFlags baseFlags = {};
                                   baseFlags.requiredFormat  = EFormat::Undefined;
                                   baseFlags.writeTarget     = ERenderGraphWriteTarget::Color;
                                   baseFlags.arraySliceRange = CRange(0, 1);
                                   baseFlags.mipSliceRange   = CRange(0, 1);

                                   SRenderGraphWriteTextureFlags write0 = baseFlags,
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

                                   SRenderGraphWriteTextureFlags depthFlags = {};
                                   depthFlags.requiredFormat  = depthStencilDesc.format;
                                   depthFlags.writeTarget     = ERenderGraphWriteTarget::Depth;
                                   depthFlags.arraySliceRange = CRange(0, 1);
                                   depthFlags.mipSliceRange   = CRange(0, 1);

                                   aOutPassData.exportData.gbuffer0     = aBuilder.writeAttachment(aOutPassData.state.gbufferTexture0Id,     write0).data();
                                   aOutPassData.exportData.gbuffer1     = aBuilder.writeAttachment(aOutPassData.state.gbufferTexture1Id,     write1).data();
                                   aOutPassData.exportData.gbuffer2     = aBuilder.writeAttachment(aOutPassData.state.gbufferTexture2And3Id, write2).data();
                                   aOutPassData.exportData.gbuffer3     = aBuilder.writeAttachment(aOutPassData.state.gbufferTexture2And3Id, write3).data();
                                   aOutPassData.exportData.depthStencil = aBuilder.writeAttachment(aOutPassData.state.depthStencilTextureId, depthFlags).data();

                                   std::vector<SRenderGraphRenderable>   renderables = aDataSource.fetchRenderables({});
                                   std::vector<SRenderGraphRenderObject> renderObjects;
                                   // Configure the pipeline to be used for all the objects...
                                   SRenderGraphPipelineConfig config;

                                   for(auto const &renderableResources : renderables)
                                   {
                                       auto const [result, mesh] = aBuilder.useMesh(renderableResources.mesh); // Will trace down the component hierarchies...

                                       SRenderGraphRenderObject object;
                                       object.mesh = mesh;

                                       auto const materials = renderableResources.materials;
                                       for(std::size_t k=0; k<materials.size(); ++k)
                                       {
                                           auto const renderableMaterial = materials[k];

                                           auto const [result, material]         = aBuilder.useMaterial(renderableMaterial);
                                           auto const [pipelineResult, pipeline] = aBuilder.usePipeline(renderableMaterial.sharedMaterialResourceId, config);

                                           SRenderGraphRenderObjectMaterial objectMaterial;
                                           objectMaterial.material = material;
                                           objectMaterial.pipeline = pipeline;
                                           object.materials.push_back(objectMaterial);
                                           renderObjects.push_back(object);
                                       }
                                   }

                                   aOutPassData.importData.renderObjects = renderObjects;

                                   return { EEngineStatus::Ok };
                               };
            // ----------------------------------------------------------------------------------
            // Execution
            // ----------------------------------------------------------------------------------
            auto const execute = [=] (SPassData const                     &aPassData
                                      , SRenderGraphPlatformContext const &aPlatformContext
                                      , SRenderGraphDataSource const      &aDataSource
                                      , CRenderGraphResources const       &aRenderGraphResources
                                      , SRenderGraphRenderContextState    &aRenderContextState
                                      , SRenderGraphResourceContext       &aResourceContext
                                      , SRenderGraphRenderContext         &aRenderContext) -> CEngineResult<>
            {
                using namespace engine::rendering;

                CLog::Verbose(logTag(), "GBufferGeneration");

                for(auto const &renderObject : aPassData.importData.renderObjects)
                {
                    SRenderGraphMesh const &mesh = renderObject.mesh;

                    aRenderContext.useMesh(aRenderContextState, mesh);

                    for(uint32_t k=0; k<renderObject.materials.size(); ++k)
                    {
                        SRenderGraphRenderObjectMaterial const &material = renderObject.materials[k];

                        aRenderContext.useMaterialWithPipeline(aRenderContextState, material.material, material.pipeline);
                        aRenderContext.drawIndexed(aRenderContextState, mesh.meshInfo.indexCount);
                    }
                }

                return { EEngineStatus::Ok };
            };

            // ----------------------------------------------------------------------------------
            // Spawn the pass
            // ----------------------------------------------------------------------------------
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
