#include <resources/resourcetypes.h>

#include "renderer/framegraph/framegraph.h"

namespace engine
{
    namespace framegraph
    {
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
#if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION
        bool CGraph::acceptSerializer(IRenderGraphSerializer &aSerializer) const
        {
            aSerializer.serializeGraph(*this);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CGraph::acceptDeserializer(IRenderGraphDeserializer &aDeserializer)
        {
            aDeserializer.deserializeGraph(*this);
        }
#endif
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CGraph& CGraph::operator=(CGraph const &aOther)
        {
            if(&aOther == this)
            {
                return (*this);
            }

            mSubpasses                = aOther.mSubpasses;
            mRenderPasses             = aOther.mRenderPasses;
            mRenderpassExecutionOrder = aOther.mRenderpassExecutionOrder;
#if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION
            mResourceAdjacency       = aOther.mResourceAdjacency;
            mResourceOrder           = aOther.mResourceOrder;
            mPassToResourceAdjacency = aOther.mPassToResourceAdjacency;
#endif
            mGraphMode               = EGraphMode::Compute;
            mRenderToBackBuffer      = false;

            return (*this);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::execute(SRenderGraphDataSource    const &aDataSource
                                      , SRenderGraphResourceContext     &aResourceContext
                                      , SRenderGraphRenderContext       &aRenderContext)
        {
            SRenderGraphRenderContextState renderContextState {};

            if(EGraphMode::Graphics == mGraphMode)
            {
                aRenderContext.beginGraphicsFrame(renderContextState);
            }

            // In any case...
            aRenderContext.beginFrameCommandBuffers(renderContextState);

            // if(EGraphMode::Graphics == mGraphMode)
            // {
            //     aRenderContext.bindRenderPass(renderContextState, sRenderPassResourceId, sFrameBufferResourceId, mResourceData);
            // }

            for(std::size_t k=0; k < mRenderpassExecutionOrder.size(); ++k)
            {
                RenderPassUID_t     const renderpassUid = mRenderpassExecutionOrder.at(k);
                Shared<CRenderPass> const renderpass    = mRenderPasses.at(renderpassUid);
                std::vector<PassUID_t>    subpasses     = renderpass->getTopologicallySortedSubpassList();

                resources::ResourceId_t renderpassResourceId  = renderpass->getRenderPassName();
                resources::ResourceId_t framebufferResourceId = renderpassResourceId + "_framebuffer";

                initializeAttachments(aResourceContext, renderpass);

                aResourceContext.createRenderPass(renderpassResourceId, subpasses, renderpass->attachments(), mResourceData);
                aResourceContext.createFrameBuffer(framebufferResourceId, renderpassResourceId);
                aRenderContext.bindRenderPass(renderContextState, renderpassResourceId, framebufferResourceId);
                aRenderContext.clearAttachments(renderContextState, renderpassResourceId);

                for(std::size_t j=0; j < subpasses.size(); ++j)
                {
                    PassUID_t         const subpassUid = subpasses.at(j);
                    Shared<CPassBase> const subpass    = mSubpasses.at(subpassUid);

                    initializeSubpassResources(aResourceContext, subpass);

                    CEngineResult<> executed = subpass->execute(aDataSource, mResourceData, renderContextState, aResourceContext, aRenderContext);
                    if(not executed.successful())
                    {
                        CLog::Error(logTag(), CString::format("Failed to execute pass {}", subpass->getSubpassUid()));
                        break;
                    }

                    // deinitializeSubpassResources(subpass);

                    aRenderContext.nextSubpass(renderContextState);
                }

                aRenderContext.unbindRenderPass(renderContextState, renderpassResourceId, framebufferResourceId);
                // aResourceContext.destroyFrameBuffer(framebufferResourceId);
                // aResourceContext.destroyRenderPass(renderpassResourceId);

                // deinitializeAttachments(renderpass);
            }

            if(EGraphMode::Graphics == mGraphMode && mRenderToBackBuffer)
            {
                CEngineResult<Shared<SRenderGraphImageView>> const sourceResourceFetch = mResourceData.getResource<SRenderGraphImageView>(mOutputTextureResourceId);
                if(not sourceResourceFetch.successful())
                {
                    CLog::Error(logTag(), CString::format("Failed to copy pass chain output to backbuffer. Invalid texture view."));
                    return {sourceResourceFetch.result()};
                }

                CEngineResult<Shared<SRenderGraphImage>> const parentResourceFetch = mResourceData.getResource<SRenderGraphImage>(sourceResourceFetch.data()
                                                                                                                                                   ->subjacentResource);
                if(not parentResourceFetch.successful())
                {
                    CLog::Error(logTag(), CString::format("Failed to copy pass chain output to backbuffer. Invalid texture."));
                    return {sourceResourceFetch.result()};
                }

                aRenderContext.copyImageToBackBuffer(renderContextState, *(parentResourceFetch.data()));
            }

            // In any case...
            aRenderContext.endFrameCommandBuffers(renderContextState);

            if(EGraphMode::Graphics == mGraphMode)
            {
                aRenderContext.present(renderContextState);
                aRenderContext.endGraphicsFrame(renderContextState);
            }

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------
    }
}
