//
// Created by dottideveloper on 08.05.20.
//

#ifndef __SHIRABEDEVELOPMENT_SCENEDATASOURCE_H__
#define __SHIRABEDEVELOPMENT_SCENEDATASOURCE_H__

#include "framegraphdata.h"

namespace engine::framegraph
{
    struct SRenderGraphDataSource
    {
        std::function<std::vector<SRenderGraphRenderableResources>(SRenderGraphRenderableFetchFilter)> fetchRenderables;
    };
}

#endif //__SHIRABEDEVELOPMENT_SCENEDATASOURCE_H__
