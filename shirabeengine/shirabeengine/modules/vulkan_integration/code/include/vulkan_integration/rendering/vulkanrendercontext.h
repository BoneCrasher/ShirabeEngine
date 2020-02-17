#ifndef __SHIRABE_VULKAN_RENDERCONTEXT_H__
#define __SHIRABE_VULKAN_RENDERCONTEXT_H__

#include <log/log.h>
#include <resources/resourcetypes.h>
#include <renderer/irendercontext.h>
#include <renderer/renderertypes.h>
#include <resources/cresourcemanager.h>
#include "vulkan_integration/vulkanenvironment.h"
#include "vulkan_integration/resources/vulkanresourceoperations.h"

#include "vulkan_integration/resources/types/vulkanbufferresource.h"
#include "vulkan_integration/resources/types/vulkanbufferviewresource.h"
#include "vulkan_integration/resources/types/vulkantextureresource.h"
#include "vulkan_integration/resources/types/vulkantextureviewresource.h"
#include "vulkan_integration/resources/types/vulkanrenderpassresource.h"
#include "vulkan_integration/resources/types/vulkanframebufferresource.h"
#include "vulkan_integration/resources/types/vulkanmaterialpipelineresource.h"
#include "vulkan_integration/resources/types/vulkanshadermoduleresource.h"

namespace engine
{
    namespace resources
    {
        using VulkanResourceManager_t =
                CResourceManager<
                    // List of supported resource states...
                      SResourceState<STexture,      CVulkanTextureResource>
                    , SResourceState<STextureView,  CVulkanTextureViewResource>
                    , SResourceState<SBuffer,       CVulkanBufferResource>
                    , SResourceState<SBufferView,   CVulkanBufferViewResource>
                    , SResourceState<SRenderPass,   CVulkanRenderPassResource>
                    , SResourceState<SFrameBuffer,  CVulkanFrameBufferResource>
                    , SResourceState<SShaderModule, CVulkanShaderModuleResource>
                    , SResourceState<SPipeline,     CVulkanPipelineResource>
                >;
    }

    namespace framegraph
    {
        struct SFrameGraphRenderContext;
    }

    namespace vulkan
    {
        static framegraph::SFrameGraphRenderContext
            CreateRenderContextForVulkan(Shared<CVulkanEnvironment>     aVulkanEnvironment
                                       , Shared<CGpuApiResourceStorage> aResourceStorage);
    }
}

#endif
