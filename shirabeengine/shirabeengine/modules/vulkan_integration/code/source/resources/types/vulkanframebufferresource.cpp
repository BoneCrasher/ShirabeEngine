//
// Created by dottideveloper on 29.10.19.
//
#include "vulkan_integration/resources/types/vulkanframebufferresource.h"
#include "vulkan_integration/resources/types/vulkanrenderpassresource.h"
#include "vulkan_integration/resources/types/vulkantextureviewresource.h"
#include "vulkan_integration/vulkandevicecapabilities.h"

namespace engine::vulkan
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanFrameBufferResource::create(  SFrameBufferDescription      const &aDescription
                                                       , SFrameBufferDependencies     const &aDependencies
                                                       , GpuApiResourceDependencies_t const &aResolvedDependencies)
    {
        if(EGpuApiResourceState::Loaded == getResourceState()
           || EGpuApiResourceState::Loading == getResourceState())
        {
            return EEngineStatus::Ok;
        }

        setResourceState(EGpuApiResourceState::Loading);

        CVkApiResource<SFrameBuffer>::create(aDescription, aDependencies, aResolvedDependencies);

        auto const *const renderPass = getVkContext()->getResourceStorage()->extract<CVulkanRenderPassResource>(aResolvedDependencies.at(aDependencies.referenceRenderPassId));

        std::vector<VkImageView> textureViews {};
        for(auto const &id : aDependencies.attachmentTextureViews)
        {
            auto const *const textureView = getVkContext()->getResourceStorage()->extract<CVulkanTextureViewResource>(aResolvedDependencies.at(id));
            textureViews.push_back(textureView->handle);
        }

        VkFramebufferCreateInfo vkFrameBufferCreateInfo {};
        vkFrameBufferCreateInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        vkFrameBufferCreateInfo.pNext           = nullptr;
        vkFrameBufferCreateInfo.pAttachments    = textureViews.data();
        vkFrameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(textureViews.size());
        vkFrameBufferCreateInfo.renderPass      = renderPass->handle;
        vkFrameBufferCreateInfo.width           = aDependencies.attachmentExtent.width;
        vkFrameBufferCreateInfo.height          = aDependencies.attachmentExtent.height;
        vkFrameBufferCreateInfo.layers          = aDependencies.attachmentExtent.depth;
        vkFrameBufferCreateInfo.flags           = 0;

        VkFramebuffer vkFrameBuffer = VK_NULL_HANDLE;

        VkResult result = vkCreateFramebuffer(getVkContext()->getLogicalDevice(), &vkFrameBufferCreateInfo, nullptr, &vkFrameBuffer);
        if(VkResult::VK_SUCCESS != result)
        {
            CLog::Error(logTag(), CString::format("Failed to create frame buffer instance. Vulkan result: {}", result));
            return { EEngineStatus::Error };
        }

        this->handle = vkFrameBuffer;

        setResourceState(EGpuApiResourceState::Loaded);

        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanFrameBufferResource::destroy()
    {
        vkDestroyFramebuffer(getVkContext()->getLogicalDevice(), this->handle, nullptr);

        setResourceState(EGpuApiResourceState::Discarded);

        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------
}
