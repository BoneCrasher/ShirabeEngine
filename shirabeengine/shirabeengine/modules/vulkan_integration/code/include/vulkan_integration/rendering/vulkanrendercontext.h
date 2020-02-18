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
        using TextureResourceState_t      = SResourceState<STexture,      SVulkanTextureResource>;
        using TextureViewResourceState_t  = SResourceState<STextureView,  SVulkanTextureViewResource>;
        using BufferResourceState_t       = SResourceState<SBuffer,       SVulkanBufferResource>;
        using BufferViewResourceState_t   = SResourceState<SBufferView,   CVulkanBufferViewResource>;
        using RenderPassResourceState_t   = SResourceState<SRenderPass,   CVulkanRenderPassResource>;
        using FrameBufferResourceState_t  = SResourceState<SFrameBuffer,  CVulkanFrameBufferResource>;
        using ShaderModuleResourceState_t = SResourceState<SShaderModule, CVulkanShaderModuleResource>;
        using PipelineResourceState_t     = SResourceState<SPipeline,     SVulkanPipelineResource>;

        using VulkanResourceManager_t =
                CResourceManager<
                    // List of supported resource states...
                      TextureResourceState_t
                    , TextureViewResourceState_t
                    , BufferResourceState_t
                    , BufferViewResourceState_t
                    , RenderPassResourceState_t
                    , FrameBufferResourceState_t
                    , ShaderModuleResourceState_t
                    , PipelineResourceState_t
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
