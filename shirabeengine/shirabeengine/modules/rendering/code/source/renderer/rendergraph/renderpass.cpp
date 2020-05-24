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
        {asdfasfdj;kl
            CRenderGraphResourceReferences_t const subpassResourceReferences = subpass.second->mutableResourceReferences();
            for(RenderGraphResourceId_t const &resourceId : subpassResourceReferences)
            {
                CEngineResult<Shared<SRenderGraphResource>> resourceFetch = mAccumulatedResourceData.getResourceMutable<SRenderGraphResource>(resourceId);
                if(not resourceFetch.successful())
                {
                    CLog::Error(logTag(), "Could not fetch subpass resource data.");
                    return;
                }
                SRenderGraphResource &resource = *(resourceFetch.data());

                // For each underlying OR imported resource (textures/buffers or whatever importable)
                if(SHIRABE_FRAMEGRAPH_UNDEFINED_RESOURCE == resource.parentResource)
                {
                    if(ERenderGraphResourceType::Image == resource.type)
                    {
                        #if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION
                        // Map the resources to it's pass appropriately
                        bool const alreadyRegistered = alreadyRegisteredFn<RenderGraphResourceId_t>(mPassToResourceAdjacency[aPass->passUID()], resource.resourceId);
                        if(!alreadyRegistered)
                        {
                            mPassToResourceAdjacency[aPass->passUID()].push_back(resource.resourceId);
                        }
                        #endif
                        continue;
                    }
                }
                    // For each derived resource (views)
                else
                {
                    // Avoid internal references for passes!
                    // If the edge from pass k to pass k+1 was not added yet.
                    // Create edge: Parent-->Source
                    CEngineResult<Shared<SRenderGraphResource>> parentResourceFetch = mAccumulatedResourceData.getResource<SRenderGraphResource>(resource.parentResource);
                    if(not parentResourceFetch.successful())
                    {
                        CLog::Error(logTag(), "Could not fetch pass data.");
                        return;
                    }
                    SRenderGraphResource const &parentResource = *(parentResourceFetch.data());

                    RenderPassUID_t const &renderPassUid = getRenderPassUid();
                    if(parentResource.assignedRenderpassUID != renderPassUid)
                    {
                        aGraphBuilder.createRenderPassDependencyByUID(parentResource.assignedRenderpassUID, renderPassUid);
                    }

                    // Create a pass dependency used for topologically sorting the graph
                    // to derive the pass execution order.
                    PassUID_t const &passUid = subpass.second->getSubpassUid();
                    if(parentResource.assignedPassUID != passUid)
                    {
                        mSubpassAdjacency.connect(parentResource.assignedPassUID, passUid);
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

                    if(ERenderGraphResourceType::ImageView == resource.type)
                    {
                        CEngineResult<Shared<SRenderGraphImage>>     textureFetch     = mAccumulatedResourceData.getResourceMutable<SRenderGraphImage>(resource.subjacentResource);
                        CEngineResult<Shared<SRenderGraphImageView>> textureViewFetch = mAccumulatedResourceData.getResourceMutable<SRenderGraphImageView>(resource.resourceId);
                        if(not (textureFetch.successful() and textureViewFetch.successful()))
                        {
                            CLog::Error(logTag(), "Could not fetch texture and/or texture view from resource registry.");
                            return;
                        }
                        SRenderGraphImage     &texture     = *(textureFetch.data());
                        SRenderGraphImageView &textureView = *(textureViewFetch.data());

                        // Auto adjust format if requested
                        if(RenderGraphFormat_t::Automatic == textureView.description.format)
                        {
                            textureView.description.format = texture.description.dynamicImage.format;
                        }
                    }
                    else if(ERenderGraphResourceType::BufferView == resource.type)
                    {
                        // TODO
                    }
                }
            }

            // Now that the internal resource references were adjusted and duplicates removed, confirm the index in the graph builder
            for(RenderGraphResourceId_t const &id : subpass.second->resourceReferences())
            {
                mAccumulatedResourceReferences.push_back(id);
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
