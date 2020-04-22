//
// Created by dotti on 19.10.19.
//

#ifndef __SHIRABEDEVELOPMENT_CRESOURCEMANAGER_H__
#define __SHIRABEDEVELOPMENT_CRESOURCEMANAGER_H__

#include "resources/cresourcemanagerbase.h"
#include "resources/resourcetypes.h"

namespace engine::resources
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    using CResourceManager = CResourceManagerBase<SResourceState<STexture>
                                                , SResourceState<STextureView>
                                                , SResourceState<SBuffer>
                                                , SResourceState<SBufferView>
                                                , SResourceState<SShaderModule>
                                                , SResourceState<SRenderPass>
                                                , SResourceState<SFrameBuffer>
                                                , SResourceState<SPipeline>
                                                , SResourceState<SMesh>
                                                , SResourceState<SMaterial>>;
    //<-----------------------------------------------------------------------------
}


#endif //__SHIRABEDEVELOPMENT_CRESOURCEMANAGER_H__
