//
// Created by dotti on 19.10.19.
//

#ifndef __SHIRABEDEVELOPMENT_CRESOURCEMANAGER_H__
#define __SHIRABEDEVELOPMENT_CRESOURCEMANAGER_H__

#include "rhi/resource_management/resourcetypes.h"
#include "rhi/vulkan_resources/resources/types/all.h"
#include "rhi/resource_management/cresourcemanagerbase.h"

namespace engine::rhi
{
    //<-----------------------------------------------------------------------------
    // Specialize the resource manager for the specific platform RHI types.
    // Currently there's only one, so this abstraction is retained for the future,
    // although currently unnecessary.
    //<-----------------------------------------------------------------------------
    template <typename... TResources>
    using CRHIResourceManagerRemap = CRHIResourceManagerBase<vulkan::IVkGlobalContext, SRHIResourceState<TResources>...>;

    using CRHIResourceManager = CRHIResourceManagerRemap<SRHIImage
                                                         , SRHIImageView
                                                         , SRHIBuffer
                                                         , SRHIBufferView
                                                         , SRHIMemory
                                                         , SRHIShaderModule
                                                         , SRHIRenderPass
                                                         , SRHIFrameBuffer
                                                         , SRHIPipelineLayout
                                                         , SRHIPipeline
                                                         , SRHIDescriptorPool>;
    //<-----------------------------------------------------------------------------
}


#endif //__SHIRABEDEVELOPMENT_CRESOURCEMANAGER_H__
