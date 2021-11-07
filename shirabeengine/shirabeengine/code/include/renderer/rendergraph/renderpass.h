//
// Created by dottideveloper on 06.05.20.
//

#ifndef __SHIRABEDEVELOPMENT_RENDERPASS_H__
#define __SHIRABEDEVELOPMENT_RENDERPASS_H__

#include <core/datastructures/adjacencytree.h>
#include "renderer/rendergraph/subpass.h"

namespace engine::framegraph
{
    using datastructures::CAdjacencyTree;

    using RenderPassUID_t = uint64_t;

    class CRenderPass
    {
        SHIRABE_DECLARE_LOG_TAG(CRenderPass);

    public_constructors:
        CRenderPass(RenderPassUID_t const         &aRenderPassUid
                    , std::string const           &aRenderPassName
                    , RenderGraphResourceIdList    &aAccumulatedResourceReferences
                    , CRenderGraphMutableResources &aAccumulatedResourceData)
            : mRenderPassUid(aRenderPassUid)
            , mRenderPassName(aRenderPassName)
            , mSubpassAdjacency()
            , mSubpasses()
            , mAccumulatedResourceData(aAccumulatedResourceData)
            , mAccumulatedResourceReferences(aAccumulatedResourceReferences)
        {};

    public_destructors:

    public_methods:
        SHIRABE_INLINE RenderPassUID_t const &getRenderPassUid() const { return mRenderPassUid; }
        SHIRABE_INLINE std::string     const &getRenderPassName() const { return mRenderPassName; }

        SHIRABE_INLINE SRenderGraphAttachmentCollection const &attachments() const { return mAttachments; }
        SHIRABE_INLINE SRenderGraphAttachmentCollection &attachments() { return mAttachments; }

        void addSubpass(Shared<CPassBase> const &aPass);

        void collectSubpasses(class CGraphBuilder &aGraphBuilder);

        std::vector<PassUID_t> getTopologicallySortedSubpassList();

    private_members:
        RenderPassUID_t const     mRenderPassUid;
        std::string const         mRenderPassName;

        PassMap                   mSubpasses;
        CAdjacencyTree<PassUID_t> mSubpassAdjacency;

        CRenderGraphMutableResources &mAccumulatedResourceData;
        RenderGraphResourceIdList    &mAccumulatedResourceReferences;

        SRenderGraphAttachmentCollection mAttachments;
    };

    SHIRABE_DECLARE_LIST_OF_TYPE(Shared<CRenderPass>, CRenderPassPass)
    SHIRABE_DECLARE_MAP_OF_TYPES(RenderPassUID_t, Shared<CRenderPass>, RenderPass)
}

#endif //__SHIRABEDEVELOPMENT_RENDERPASS_H__
