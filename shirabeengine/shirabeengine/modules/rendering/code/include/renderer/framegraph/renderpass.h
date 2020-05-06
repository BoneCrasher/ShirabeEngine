//
// Created by dottideveloper on 06.05.20.
//

#ifndef __SHIRABEDEVELOPMENT_RENDERPASS_H__
#define __SHIRABEDEVELOPMENT_RENDERPASS_H__

#include "renderer/framegraph/subpass.h"

namespace engine::framegraph
{
    using RenderPassUID_t = uint64_t;

    class CRenderPass
    {
    public_constructors:
    public_destructors:

    public_methods:
        RenderPassUID_t const &getRenderPassUid() const { return mRenderPassUid; }

        void addSubpass(Shared<CPassBase> const &aPass);

    private_members:
        RenderPassUID_t mRenderPassUid;
        PassMap         mPasses;
    };

    SHIRABE_DECLARE_LIST_OF_TYPE(Shared<CRenderPass>, CRenderPassPass);
    SHIRABE_DECLARE_MAP_OF_TYPES(RenderPassUID_t, Shared<CRenderPass>, RenderPass);
}

#endif //__SHIRABEDEVELOPMENT_RENDERPASS_H__
