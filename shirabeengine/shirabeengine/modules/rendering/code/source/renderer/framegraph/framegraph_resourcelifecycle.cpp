#include <resources/resourcetypes.h>

#include "renderer/framegraph/framegraph.h"

namespace engine
{
    namespace framegraph
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        bool CGraph::initializeAttachments(SFrameGraphResourceContext &aResourceContext, Shared<CRenderPass> aRenderPass)
        {
            SFrameGraphAttachmentCollection const &attachments = aRenderPass->attachments();
            FrameGraphResourceIdList const attachmentImageIds     = attachments.getAttachmentImageResourceIds();
            FrameGraphResourceIdList const attachmentImageViewIds = attachments.getAttachmentImageViewResourceIds();

            for(auto const &attachmentImageId : attachmentImageIds)
            {
                auto const &[result, image] = mResourceData.getResourceMutable<SFrameGraphTransientTexture>(attachmentImageId);

                aResourceContext.createTransientTexture(image);
            }

            for(auto const &attachmentImageViewId : attachmentImageViewIds)
            {
                auto const &[result, imageView] = mResourceData.getResourceMutable<SFrameGraphTextureView>(attachmentImageViewId);
            }
        }

        bool CGraph::deinitializeAttachments(SFrameGraphResourceContext &aResourceContext, Shared<CRenderPass> aRenderPass)
        {

        }

        bool CGraph::initializeSubpassResources(SFrameGraphResourceContext &aResourceContext, Shared<CPassBase> aSubpass)
        {

        }

        bool CGraph::deinitializeSubpassResources(SFrameGraphResourceContext &aResourceContext, Shared<CPassBase> aSubpass)
        {

        }
        //<-----------------------------------------------------------------------------
    }
}
