//
// Created by dottideveloper on 29.10.19.
//
#include "vulkan_integration/resources/types/vulkanpipelineresource.h"
#include "vulkan_integration/resources/types/vulkantextureviewresource.h"
#include "vulkan_integration/vulkandevicecapabilities.h"

namespace engine::vulkan
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanPipelineResource::create(GpuApiResourceDependencies_t const &aDependencies)
    {
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanPipelineResource::destroy()
    {
        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------
}
