#ifndef __SHIRABE_VULKAN_RESOURCECONTEXT_H__
#define __SHIRABE_VULKAN_RESOURCECONTEXT_H__

#include <log/log.h>
#include "renderer/resource_management/resourcetypes.h"
#include "renderer/renderertypes.h"

#include "renderer/vulkan_resources/resources/vulkanresourceoperations.h"

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
                                         , Shared<CResourceManager>     aResourceManager
                                         , Shared<asset::CAssetStorage> aAssetStorage);
    }
}

#endif