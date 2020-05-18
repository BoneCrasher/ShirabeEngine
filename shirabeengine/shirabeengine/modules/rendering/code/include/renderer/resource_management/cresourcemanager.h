//
// Created by dotti on 19.10.19.
//

#ifndef __SHIRABEDEVELOPMENT_CRESOURCEMANAGER_H__
#define __SHIRABEDEVELOPMENT_CRESOURCEMANAGER_H__

#include "renderer/resource_management/cresourcemanagerbase.h"
#include "renderer/resource_management/resourcetypes.h"

namespace engine::resources
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename... TResources>
    using CResourceManagerRemap = CResourceManagerBase<SResourceState<TResources>...>;

    using CResourceManager = CResourceManagerRemap<STexture
                                                 , STextureView
                                                 , SBuffer
                                                 , SBufferView
                                                 , SShaderModule
                                                 , SRenderPass
                                                 , SFrameBuffer
                                                 , SPipeline
                                                 , SMesh
                                                 , SMaterial>;
    //<-----------------------------------------------------------------------------
}


#endif //__SHIRABEDEVELOPMENT_CRESOURCEMANAGER_H__