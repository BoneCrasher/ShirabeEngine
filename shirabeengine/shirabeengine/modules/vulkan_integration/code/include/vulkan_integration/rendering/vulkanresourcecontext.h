#ifndef __SHIRABE_VULKAN_RESOURCECONTEXT_H__
#define __SHIRABE_VULKAN_RESOURCECONTEXT_H__

#include <log/log.h>
#include <resources/resourcetypes.h>
#include <renderer/renderertypes.h>

#include "vulkan_integration/vulkanenvironment.h"
#include "vulkan_integration/resources/vulkanresourceoperations.h"

namespace engine
{
    namespace framegraph
    {
        struct SFrameGraphRenderContext;
        struct SFrameGraphResourceContext;
    }

    namespace vulkan
    {
        static framegraph::SFrameGraphResourceContext
            CreateResourceContextForVulkan(Shared<CVulkanEnvironment>   aVulkanEnvironment
                                         , Shared<CResourceManager>     aResourceManager
                                         , Shared<asset::CAssetStorage> aAssetStorage);
    }
}

#endif
