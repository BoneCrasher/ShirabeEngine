//
// Created by dottideveloper on 07.05.20.
//

#include "renderer/framegraph/graphbuilder.h"
#include "renderer/framegraph/renderpass.h"

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
            FrameGraphResourceIdList const subpassResourceReferences = subpass.second->mutableResourceReferences();
            for(FrameGraphResourceId_t const &resourceId : subpassResourceReferences)
            {
                CEngineResult<Shared<SFrameGraphResource>> resourceFetch = mAccumulatedResourceData.getResourceMutable<SFrameGraphResource>(resourceId);
                if(not resourceFetch.successful())
                {
                    CLog::Error(logTag(), "Could not fetch subpass resource data.");
                    return;
                }
                SFrameGraphResource &resource = *(resourceFetch.data());

                // For each underlying OR imported resource (textures/buffers or whatever importable)
                if(SHIRABE_FRAMEGRAPH_UNDEFINED_RESOURCE == resource.parentResource)
                {
                    if(EFrameGraphResourceType::Texture == resource.type)
                    {
                        #if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION
                        // Map the resources to it's pass appropriately
                        bool const alreadyRegistered = alreadyRegisteredFn<FrameGraphResourceId_t>(mPassToResourceAdjacency[aPass->passUID()], resource.resourceId);
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
                    CEngineResult<Shared<SFrameGraphResource>> parentResourceFetch = mAccumulatedResourceData.getResource<SFrameGraphResource>(resource.parentResource);
                    if(not parentResourceFetch.successful())
                    {
                        CLog::Error(logTag(), "Could not fetch pass data.");
                        return;
                    }
                    SFrameGraphResource const &parentResource = *(parentResourceFetch.data());

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
                    bool const resourceAdjacencyAlreadyRegistered = alreadyRegisteredFn<FrameGraphResourceId_t>(mResourceAdjacency[parentResource.resourceId], resource.resourceId);
                    if(!resourceAdjacencyAlreadyRegistered)
                    {
                        mResourceAdjacency[parentResource.resourceId].push_back(resource.resourceId);
                    }

                    // And map the resources to it's pass appropriately
                    bool const resourceEdgeAlreadyRegistered = alreadyRegisteredFn<FrameGraphResourceId_t>(mPassToResourceAdjacency[passUID], resource.resourceId);
                    if(!resourceEdgeAlreadyRegistered)
                    {
                        mPassToResourceAdjacency[passUID].push_back(resource.resourceId);
                    }
                    #endif

                    if(EFrameGraphResourceType::TextureView == resource.type)
                    {
                        CEngineResult<Shared<SFrameGraphTexture>>     textureFetch     = mAccumulatedResourceData.getResourceMutable<SFrameGraphTexture>(resource.subjacentResource);
                        CEngineResult<Shared<SFrameGraphTextureView>> textureViewFetch = mAccumulatedResourceData.getResourceMutable<SFrameGraphTextureView>(resource.resourceId);
                        if(not (textureFetch.successful() and textureViewFetch.successful()))
                        {
                            CLog::Error(logTag(), "Could not fetch texture and/or texture view from resource registry.");
                            return;
                        }
                        SFrameGraphTexture     &texture     = *(textureFetch.data());
                        SFrameGraphTextureView &textureView = *(textureViewFetch.data());

                        // Auto adjust format if requested
                        if(FrameGraphFormat_t::Automatic == textureView.format)
                        {
                            textureView.format = texture.format;
                        }
                    }
                    else if(EFrameGraphResourceType::BufferView == resource.type)
                    {
                        // TODO
                    }
                }
            }

            // Now that the internal resource references were adjusted and duplicates removed, confirm the index in the graph builder
            for(FrameGraphResourceId_t const &id : subpass.second->resourceReferences())
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
