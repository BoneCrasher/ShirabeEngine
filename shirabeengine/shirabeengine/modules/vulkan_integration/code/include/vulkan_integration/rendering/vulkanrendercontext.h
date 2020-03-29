#ifndef __SHIRABE_VULKAN_RENDERCONTEXT_H__
#define __SHIRABE_VULKAN_RENDERCONTEXT_H__

#include <log/log.h>
#include <resources/resourcetypes.h>
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
#include "vulkan_integration/resources/types/vulkanmeshresource.h"
#include "vulkan_integration/resources/types/vulkanmaterialresource.h"

namespace engine
{
    namespace resources
    {
        using RenderPassResourceState_t   = SResourceState<SRenderPass,   SVulkanRenderPassResource>;
        using FrameBufferResourceState_t  = SResourceState<SFrameBuffer,  SVulkanFrameBufferResource>;
        using ShaderModuleResourceState_t = SResourceState<SShaderModule, SVulkanShaderModuleResource>;
        using PipelineResourceState_t     = SResourceState<SPipeline,     SVulkanPipelineResource>;
        using MeshResourceState_t         = SResourceState<SMesh,         SVulkanMeshResource>;
        using MaterialResourceState_t     = SResourceState<SMaterial,     SVulkanMaterialResource>;

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
                    , MeshResourceState_t
                    , MaterialResourceState_t
                >;
    }

    namespace framegraph
    {
        struct SFrameGraphRenderContext;
        struct SFrameGraphResourceContext;
    }

    namespace vulkan
    {
        static framegraph::SFrameGraphRenderContext
            CreateRenderContextForVulkan(Shared<CVulkanEnvironment>        aVulkanEnvironment
                                       , Shared<VulkanResourceManager_t>   aResourceManager
                                       , Shared<asset::CAssetStorage>      aAssetStorage);

        static framegraph::SFrameGraphResourceContext
            CreateResourceContextForVulkan(Shared<CVulkanEnvironment>        aVulkanEnvironment
                                         , Shared<VulkanResourceManager_t>   aResourceManager
                                         , Shared<asset::CAssetStorage>      aAssetStorage);
    }
}

#endif
