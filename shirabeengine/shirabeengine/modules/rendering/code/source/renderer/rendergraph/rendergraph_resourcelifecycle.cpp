#include "renderer/resource_management/resourcetypes.h"
#include "renderer/rendergraph/rendergraph.h"

namespace engine
{
    namespace framegraph
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        bool CGraph::initializeGraphGlobalResources(SRenderGraphResourceContext &aResourceContext)
        {
            // This function will initialize renderpass independent, thus graph global resources including
            // imported textures, meshes and materials.
            //
            // Important:
            //   Do not change order, unless the change ensures proper initialization order of resource dependency trees.

            CRenderGraphResourceReferences_t &resources = mResourceData;
            for(auto const &imageId : resources.images())
            {
                auto const &[fetchStatus, resourcePtr] = mResourceData.getResource<SRenderGraphImage>(imageId);
                if(EEngineStatus::Ok != fetchStatus)
                {
                    CLog::Error(logTag(), "Failed to fetch render graph image w/ id %d", imageId);
                    continue;
                }

                if(false == resourcePtr->description.isDynamicImage)
                {
                    EEngineStatus const initState = aResourceContext.initializePersistentImage(*resourcePtr);
                    if(EEngineStatus::Ok != initState)
                    {
                        CLog::Error(logTag(), "Failed to initialize persistent image w/ id %d", imageId);
                        continue;
                    }

                    EEngineStatus const updateState = aResourceContext.updatePersistentTexture(*resourcePtr);
                    if(EEngineStatus::Ok != updateState)
                    {
                        CLog::Error(logTag(), "Failed to update persistent image w/ id %d", imageId);
                        continue;
                    }
                }
            }

            for(auto const &imageViewId : resources.imageViews())
            {
                auto const &[fetchStatus, resourcePtr] = mResourceData.getResource<SRenderGraphImageView>(imageViewId);
                if(EEngineStatus::Ok != fetchStatus)
                {
                    CLog::Error(logTag(), "Failed to fetch render graph image view w/ id %d", imageViewId);
                    continue;
                }

                auto const &[subjacentFetchStatus, subjacentResourcePtr] = mResourceData.getResource<SRenderGraphImage>(resourcePtr->subjacentResource);
                if(EEngineStatus::Ok != subjacentFetchStatus)
                {
                    CLog::Error(logTag(), "Failed to fetch subjacent render graph image w/ id %d", resourcePtr->subjacentResource);
                    continue;
                }

                EEngineStatus const state = aResourceContext.createTextureView(*subjacentResourcePtr, *resourcePtr);
                if(EEngineStatus::Ok != state)
                {
                    CLog::Error(logTag(), "Failed to create image view w/ id %d", imageViewId);
                    continue;
                }
            }

            for(auto const &bufferId : resources.buffers())
            {
                auto const &[fetchStatus, resourcePtr] = mResourceData.getResource<SRenderGraphBuffer>(bufferId);
                if(EEngineStatus::Ok != fetchStatus)
                {
                    CLog::Error(logTag(), "Failed to fetch render graph buffer w/ id %d", bufferId);
                    continue;
                }

                if(false == resourcePtr->description.isDynamicBuffer)
                {
                    EEngineStatus const initState = aResourceContext.initializePersistentBuffer(*resourcePtr);
                    if(EEngineStatus::Ok != initState)
                    {
                        CLog::Error(logTag(), "Failed to initialize buffer w/ id %d", bufferId);
                        continue;
                    }

                    EEngineStatus const updateState = aResourceContext.updatePersistentBuffer(*resourcePtr);
                    if(EEngineStatus::Ok != updateState)
                    {
                        CLog::Error(logTag(), "Failed to update buffer w/ id %d", bufferId);
                        continue;
                    }
                }
            }

            for(auto const &bufferViewId : resources.bufferViews())
            {
                auto const &[fetchStatus, resourcePtr] = mResourceData.getResource<SRenderGraphBufferView>(bufferViewId);
                if(EEngineStatus::Ok != fetchStatus)
                {
                    CLog::Error(logTag(), "Failed to fetch render graph buffer view w/ id %d", bufferViewId);
                    continue;
                }
            }

            for(auto const &meshId : resources.meshes())
            {
                auto const &[fetchStatus, resourcePtr] = mResourceData.getResource<SRenderGraphMesh>(meshId);
                if(EEngineStatus::Ok != fetchStatus)
                {
                    CLog::Error(logTag(), "Failed to fetch render graph mesh w/ id %d", meshId);
                    continue;
                }

                EEngineStatus const initState = aResourceContext.initializeMesh(*resourcePtr);
                if(EEngineStatus::Ok != initState)
                {
                    CLog::Error(logTag(), "Failed to initialize mesh w/ id %d", meshId);
                    continue;
                }

                EEngineStatus const updateState = aResourceContext.updateMesh(*resourcePtr);
                if(EEngineStatus::Ok != updateState)
                {
                    CLog::Error(logTag(), "Failed to update mesh w/ id %d", meshId);
                    continue;
                }
            }

            for(auto const &materialId : resources.meshes())
            {
                auto const &[fetchStatus, resourcePtr] = mResourceData.getResource<SRenderGraphMaterial>(materialId);
                if(EEngineStatus::Ok != fetchStatus)
                {
                    CLog::Error(logTag(), "Failed to fetch render graph material w/ id %d", materialId);
                    continue;
                }

                EEngineStatus const initState = aResourceContext.initializeMaterial(*resourcePtr);
                if(EEngineStatus::Ok != initState)
                {
                    CLog::Error(logTag(), "Failed to initialize material w/ id %d", materialId);
                    continue;
                }

                EEngineStatus const updateState = aResourceContext.updateMaterial(*resourcePtr);
                if(EEngineStatus::Ok != updateState)
                {
                    CLog::Error(logTag(), "Failed to update material w/ id %d", materialId);
                    continue;
                }
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        bool CGraph::deinitializeGraphGlobalResources(SRenderGraphResourceContext &aResourceContext)
        {

        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        bool CGraph::initializeRenderPassResources(SRenderGraphResourceContext &aResourceContext, Shared<CRenderPass> aRenderPass)
        {
            // This function will initialize renderpass dependent resources, which don't belong to a specific subpass though.
            // Usually, these are attachments only.

            SRenderGraphAttachmentCollection const &attachments = aRenderPass->attachments();
            RenderGraphResourceIdList const attachmentImageIds     = attachments.getAttachmentImageResourceIds();
            RenderGraphResourceIdList const attachmentImageViewIds = attachments.getAttachmentImageViewResourceIds();

            for(auto const &attachmentImageId : attachmentImageIds)
            {
                auto const &[result, image] = mResourceData.getResourceMutable<SRenderGraphImage>(attachmentImageId);
                if(EEngineStatus::Ok != result)
                {
                    CLog::Error(logTag(), "Failed to fetch attachment image w/ id %d", attachmentImageId);
                    continue;
                }

                aResourceContext.createTransientTexture(*image);
            }

            for(auto const &attachmentImageViewId : attachmentImageViewIds)
            {
                auto const &[result, resourcePtr] = mResourceData.getResourceMutable<SRenderGraphImageView>(attachmentImageViewId);
                if(EEngineStatus::Ok != result)
                {
                    CLog::Error(logTag(), "Failed to fetch attachment image view w/ id %d", attachmentImageViewId);
                    continue;
                }

                auto const &[subjacentFetchStatus, subjacentResourcePtr] = mResourceData.getResource<SRenderGraphImage>(resourcePtr->subjacentResource);
                if(EEngineStatus::Ok != subjacentFetchStatus)
                {
                    CLog::Error(logTag(), "Failed to fetch subjacent attachment image view w/ id %d", resourcePtr->subjacentResource);
                    continue;
                }

                aResourceContext.createTextureView(*subjacentResourcePtr, *resourcePtr);
            }
        }

        bool CGraph::deinitializeRenderPassResources(SRenderGraphResourceContext &aResourceContext, Shared<CRenderPass> aRenderPass)
        {
            SRenderGraphAttachmentCollection const &attachments = aRenderPass->attachments();
            RenderGraphResourceIdList const attachmentImageIds     = attachments.getAttachmentImageResourceIds();
            RenderGraphResourceIdList const attachmentImageViewIds = attachments.getAttachmentImageViewResourceIds();

            for(auto const &attachmentImageViewId : attachmentImageViewIds)
            {
                auto const &[result, resourcePtr] = mResourceData.getResourceMutable<SRenderGraphImageView>(attachmentImageViewId);
                if(EEngineStatus::Ok != result)
                {
                    CLog::Error(logTag(), "Failed to fetch attachment image view w/ id %d", attachmentImageViewId);
                    continue;
                }

                aResourceContext.destroyTextureView(*resourcePtr);
            }
        }

        bool CGraph::initializeSubpassResources(SRenderGraphResourceContext &aResourceContext
                                                , Shared<CRenderPass>        aRenderPass
                                                , Shared<CPassBase>          aSubpass
                                                , uint32_t const            &aSubpassIndex)
        {
            // This function will initialize all resources which are dependent on a renderpass and the specific subpass used in!
            // Mainly these will be specific pipeline instances.
            CRenderGraphResourceReferences_t resources = aSubpass->mutableResourceReferences();
            for(auto const &pipelineId : resources.pipelines())
            {
                auto const &[fetchStatus, resourcePtr] = mResourceData.getResource<SRenderGraphPipeline>(pipelineId);
                if(EEngineStatus::Ok != fetchStatus)
                {

                }

                EEngineStatus const state = aResourceContext.createPipeline(*resourcePtr, aRenderPass->getRenderPassName(), aSubpassIndex);
                if (EEngineStatus::Ok != state)
                {

                }
            }
        }

        bool CGraph::deinitializeSubpassResources(SRenderGraphResourceContext &aResourceContext
                                                  , Shared<CRenderPass>        aRenderPass
                                                  , Shared<CPassBase>          aSubpass
                                                  , uint32_t const            &aSubpassIndex)
        {

        }
        //<-----------------------------------------------------------------------------
    }
}
