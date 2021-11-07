//
// Created by dottideveloper on 07.05.20.
//

#include "renderer/rendergraph/graphbuilder.h"
#include "renderer/rendergraph/renderpass.h"

namespace engine::framegraph
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    void CRenderPass::addSubpass(Shared<CPassBase> const &aPass)
    {
        mSubpasses.insert({aPass->getSubpassUid(), aPass });
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    void CRenderPass::collectSubpasses(CGraphBuilder &aGraphBuilder)
    {
        for(auto const subpass : mSubpasses)
        {
            CRenderGraphResourceReferences_t const subpassResourceReferences = subpass.second->mutableResourceReferences();
            for(auto const &imageId : subpassResourceReferences.images())
            {
                auto const &[status, resourcePtr] = mAccumulatedResourceData.getResourceMutable<SRenderGraphImage>(imageId);
                if(CheckEngineError(status))
                {
                    CLog::Error(logTag(), "Could not fetch render graph image.");
                    return;
                }

                #if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION
                // Map the resources to it's pass appropriately
                        bool const alreadyRegistered = alreadyRegisteredFn<RenderGraphResourceId_t>(mPassToResourceAdjacency[aPass->passUID()], resource.resourceId);
                        if(!alreadyRegistered)
                        {
                            mPassToResourceAdjacency[aPass->passUID()].push_back(resource.resourceId);
                        }
                #endif

                mAccumulatedResourceReferences.push_back(imageId);
            }

            for(auto const &imageViewId : subpassResourceReferences.imageViews())
            {
                auto const &[status, resourcePtr] = mAccumulatedResourceData.getResourceMutable<SRenderGraphImageView>(imageViewId);
                if(CheckEngineError(status))
                {
                    CLog::Error(logTag(), "Could not fetch render graph image view.");
                    return;
                }

                {
                    auto const &[status, parentResourcePtr] = mAccumulatedResourceData.getResourceMutable<SRenderGraphResource>(resourcePtr->parentResource);
                    if(CheckEngineError(status))
                    {
                        CLog::Error(logTag(), "Could not fetch parent render graph resource.");
                        return;
                    }

                    // Create a pass dependency used for topologically sorting the graph
                    // to adapt the pass execution order.
                    RenderPassUID_t const &renderPassUid = getRenderPassUid();
                    if(parentResourcePtr->assignedRenderpassUID != renderPassUid)
                    {
                        auto const &[creationStatus] = aGraphBuilder.createRenderPassDependencyByUID(parentResourcePtr->assignedRenderpassUID, renderPassUid);
                        if(CheckEngineError(creationStatus))
                        {
                            CLog::Error(logTag(), "Failed to create subpass dependency.");
                            return;
                        }
                    }

                    // Map subpass adjacency information for intra-renderpass sorting.
                    PassUID_t const &passUid = subpass.second->getSubpassUid();
                    if(parentResourcePtr->assignedPassUID != passUid)
                    {
                        mSubpassAdjacency.connect(parentResourcePtr->assignedPassUID, passUid);
                    }

                    #if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION

                    // Do the same for the resources!
                    bool const resourceAdjacencyAlreadyRegistered = alreadyRegisteredFn<RenderGraphResourceId_t>(mResourceAdjacency[parentResource.resourceId], resource.resourceId);
                    if(!resourceAdjacencyAlreadyRegistered)
                    {
                        mResourceAdjacency[parentResource.resourceId].push_back(resource.resourceId);
                    }

                    // And map the resources to it's pass appropriately
                    bool const resourceEdgeAlreadyRegistered = alreadyRegisteredFn<RenderGraphResourceId_t>(mPassToResourceAdjacency[passUID], resource.resourceId);
                    if(!resourceEdgeAlreadyRegistered)
                    {
                        mPassToResourceAdjacency[passUID].push_back(resource.resourceId);
                    }
                    #endif

                    auto const &[subjacentStatus, subjacentPtr] = mAccumulatedResourceData.getResourceMutable<SRenderGraphImage>(resourcePtr->subjacentResource);
                    if(CheckEngineError(subjacentStatus))
                    {
                        CLog::Error(logTag(), "Could not fetch subjacent render graph resource.");
                        return;
                    }

                    // Auto adjust format if requested
                    if(RenderGraphFormat_t::Automatic == resourcePtr->description.format)
                    {
                        resourcePtr->description.format = subjacentPtr->description.dynamicImage.format;
                    }
                }

                mAccumulatedResourceReferences.push_back(imageViewId);
            }

            for(auto const &bufferId : subpassResourceReferences.buffers())
            {
                mAccumulatedResourceReferences.push_back(bufferId);
            }

            for(auto const &bufferViewId : subpassResourceReferences.bufferViews())
            {
                mAccumulatedResourceReferences.push_back(bufferViewId);
            }

            for(auto const &meshId : subpassResourceReferences.meshes())
            {
                mAccumulatedResourceReferences.push_back(meshId);
            }

            for(auto const &materialId : subpassResourceReferences.materials())
            {
                mAccumulatedResourceReferences.push_back(materialId);
            }

            for(auto const &pipelineId : subpassResourceReferences.pipelines())
            {
                mAccumulatedResourceReferences.push_back(pipelineId);
            }
        }
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    std::vector<PassUID_t> CRenderPass::getTopologicallySortedSubpassList()
    {
        return mSubpassAdjacency.topologicalSort();
    }
    //<-----------------------------------------------------------------------------
}
