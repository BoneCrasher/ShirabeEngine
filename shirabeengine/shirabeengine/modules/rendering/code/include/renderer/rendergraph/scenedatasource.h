//
// Created by dottideveloper on 08.05.20.
//

#ifndef __SHIRABEDEVELOPMENT_SCENEDATASOURCE_H__
#define __SHIRABEDEVELOPMENT_SCENEDATASOURCE_H__

#include "rendergraphdata.h"

namespace engine::framegraph
{
    struct SRenderGraphRenderableFetchFilter
    {};

    struct SRenderGraphDataSource
    {
        std::function<std::vector<SRenderGraphRenderable>(SRenderGraphRenderableFetchFilter)> fetchRenderables;
    };
}

#endif //__SHIRABEDEVELOPMENT_SCENEDATASOURCE_H__
