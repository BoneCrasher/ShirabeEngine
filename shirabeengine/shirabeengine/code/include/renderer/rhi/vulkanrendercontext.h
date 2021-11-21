#ifndef __SHIRABE_VULKAN_RENDERCONTEXT_H__
#define __SHIRABE_VULKAN_RENDERCONTEXT_H__

#include <core/enginetypehelper.h>
#include <rhi/vulkan_resources/resources/vulkanresourceoperations.h>

namespace engine
{
    namespace framegraph
    {
        struct SRenderGraphRenderContext;
        struct SRenderGraphResourceContext;
    }

    namespace vulkan
    {
        static framegraph::SRenderGraphRenderContext
            CreateRenderContextForVulkan(Shared<CVulkanEnvironment>       aVulkanEnvironment
                                       , Shared<rhi::CRHIResourceManager> aResourceManager
                                       , Shared<asset::CAssetStorage>     aAssetStorage);
    }
}

#endif
