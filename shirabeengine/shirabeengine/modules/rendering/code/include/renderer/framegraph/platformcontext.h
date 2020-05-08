//
// Created by dottideveloper on 08.05.20.
//

#ifndef __SHIRABEDEVELOPMENT_PLATFORMCONTEXT_H__
#define __SHIRABEDEVELOPMENT_PLATFORMCONTEXT_H__

#include "framegraphdata.h"

namespace engine::framegraph
{
    struct SFrameGraphPlatformContext
    {
        Shared<os::SApplicationEnvironment> applicationEnvironment;
        Shared<wsi::CWSIDisplay>            display;
    };
}

#endif //__SHIRABEDEVELOPMENT_SCENEDATASOURCE_H__
