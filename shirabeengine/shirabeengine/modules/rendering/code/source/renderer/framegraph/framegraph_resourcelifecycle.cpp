#include "renderer/resource_management/resourcetypes.h"
#include "renderer/framegraph/framegraph.h"

namespace engine
{
    namespace framegraph
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        bool CGraph::initializeAttachments(SRenderGraphResourceContext &aResourceContext, Shared<CRenderPass> aRenderPass)
        {
            SRenderGraphAttachmentCollection const &attachments = aRenderPass->attachments();
            RenderGraphResourceIdList const attachmentImageIds     = attachments.getAttachmentImageResourceIds();
            RenderGraphResourceIdList const attachmentImageViewIds = attachments.getAttachmentImageViewResourceIds();

            for(auto const &attachmentImageId : attachmentImageIds)
            {
                auto const &[result, image] = mResourceData.getResourceMutable<SRenderGraphImage>(attachmentImageId);

                aResourceContext.createTransientTexture(*image);
            }

            for(auto const &attachmentImageViewId : attachmentImageViewIds)
            {
                auto const &[result, imageView] = mResourceData.getResourceMutable<SRenderGraphImageView>(attachmentImageViewId);
            }
        }

        bool CGraph::deinitializeAttachments(SRenderGraphResourceContext &aResourceContext, Shared<CRenderPass> aRenderPass)
        {

        }

        bool CGraph::initializeSubpassResources(SRenderGraphResourceContext &aResourceContext, Shared<CPassBase> aSubpass)
        {
            // Order of initialization:
            //    1. Images
            //    2. Buffers
            //    3. Shared Materials
            //    4. Materials
            //    5. Pipelines
            //    6. Meshes
            std::vector<RenderGraphResourceId_t> resources = aSubpass->mutableResourceReferences();

        }

        bool CGraph::deinitializeSubpassResources(SRenderGraphResourceContext &aResourceContext, Shared<CPassBase> aSubpass)
        {

        }
        //<-----------------------------------------------------------------------------
    }
}
