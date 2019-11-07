//
// Created by dotti on 06.11.19.
//

#ifndef __SHIRABEDEVELOPMENT_VULKANRESOURCEBINDERS_H__
#define __SHIRABEDEVELOPMENT_VULKANRESOURCEBINDERS_H__

#include <base/cpp_tools.h>
#include <resources/resourcetypes.h>
#include "vulkan_integration/resources/types/vulkanbufferresource.h"
#include "vulkan_integration/resources/types/vulkanbufferviewresource.h"
#include "vulkan_integration/resources/types/vulkantextureresource.h"
#include "vulkan_integration/resources/types/vulkantextureviewresource.h"

namespace engine::resources
{
    using namespace vulkan;

    class CVulkanFrameBufferResource;
    class CVulkanRenderPassResource;

    template <typename... TRelatedResourceTypes>
    struct SResourceBinder
    {
        CEngineResult<> operator()(TRelatedResourceTypes &&...aInstances)
        {
            // if constexpr (std::is_same_v<variadic_tuple_t<TRelatedResourceTypes...>
            //                            , variadic_tuple_t<CVulkanFrameBufferResource, CVulkanRenderPassResource>>)
            // {
            //
            // }

            return bindResourceImpl(std::forward<TRelatedResourceTypes>(aInstances)...);
        }
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename... TRelatedResourceTypes>
    struct SResourceUnbinder
    {
        CEngineResult<> operator()(TRelatedResourceTypes &&...aInstances)
        {
            return unbindResourceImpl(std::forward<TRelatedResourceTypes>(aInstances)...);
        }
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename... TRelatedResourceTypes>
    struct SResourceTransferrer
    {
        CEngineResult<> operator()(TRelatedResourceTypes &&...aInstances)
        {
            return transferResourceImpl(std::forward<TRelatedResourceTypes>(aInstances)...);
        }
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    EEngineStatus bindResourceImpl  (CVulkanFrameBufferResource const &aFrameBuffer, CVulkanRenderPassResource const &aRenderPass);
    EEngineStatus unbindResourceImpl(CVulkanRenderPassResource const &aRenderPass);
    //<-----------------------------------------------------------------------------
    EEngineStatus bindResourceImpl    (CVulkanTextureResource const &aTexture);
    EEngineStatus transferResourceImpl(CVulkanTextureResource const &aTexture);
    EEngineStatus unbindResourceImpl  (CVulkanTextureResource const &aTexture);
    //<-----------------------------------------------------------------------------
    EEngineStatus bindResourceImpl  (CVulkanTextureViewResource const &aTextureView);
    EEngineStatus unbindResourceImpl(CVulkanTextureViewResource const &aTextureView);
    //<-----------------------------------------------------------------------------
    EEngineStatus bindResourceImpl    (CVulkanBufferResource const &aBuffer);
    EEngineStatus transferResourceImpl(CVulkanBufferResource const &aBuffer);
    EEngineStatus unbindResourceImpl  (CVulkanBufferResource const &aBuffer);
    //<-----------------------------------------------------------------------------
    EEngineStatus bindResourceImpl  (CVulkanBufferViewResource const &aBufferView);
    EEngineStatus unbindResourceImpl(CVulkanBufferViewResource const &aBufferView);
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
}

#endif //__SHIRABEDEVELOPMENT_VULKANRESOURCEBINDERS_H__
