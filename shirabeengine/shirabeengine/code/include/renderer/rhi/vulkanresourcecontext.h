#ifndef __SHIRABE_VULKAN_RESOURCECONTEXT_H__
#define __SHIRABE_VULKAN_RESOURCECONTEXT_H__

#include <log/log.h>
#include <rhi/resource_management/resourcetypes.h>
#include "renderer/renderertypes.h"

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
        static framegraph::SRenderGraphResourceContext
            CreateResourceContextForVulkan(Shared<CVulkanEnvironment>   aVulkanEnvironment
                                         , Shared<CRHIResourceManager>     aResourceManager
                                         , Shared<asset::CAssetStorage> aAssetStorage);
    }
}

#endif
