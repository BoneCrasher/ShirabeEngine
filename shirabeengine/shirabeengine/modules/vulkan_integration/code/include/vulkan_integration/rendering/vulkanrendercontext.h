#ifndef __SHIRABE_VULKAN_RENDERCONTEXT_H__
#define __SHIRABE_VULKAN_RENDERCONTEXT_H__

#include <core/enginetypehelper.h>
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
        static framegraph::SFrameGraphRenderContext
            CreateRenderContextForVulkan(Shared<CVulkanEnvironment>          aVulkanEnvironment
                                       , Shared<resources::CResourceManager> aResourceManager
                                       , Shared<asset::CAssetStorage> aAssetStorage);
    }
}

#endif
