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
    CEngineResult<> CVulkanFrameBufferResource::create(GpuApiResourceDependencies_t const &aDependencies)
    {
        SFrameBufferDescription const &desc = getDescription();

        auto                   const *const renderPass            = getVkContext()->getResourceStorage()->extract<CVulkanRenderPassResource>(aDependencies.at(desc.referenceRenderPassId));
        SRenderPassDescription const       &renderPassDescription = renderPass->getDescription();

        std::vector<VkImageView> textureViews {};

        for(auto const &id : renderPassDescription.attachmentTextureViews)
        {
            auto const *const textureView = getVkContext()->getResourceStorage()->extract<CVulkanTextureViewResource>(aDependencies.at(id));
            textureViews.push_back(textureView->handle);
        }

        VkFramebufferCreateInfo vkFrameBufferCreateInfo {};
        vkFrameBufferCreateInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        vkFrameBufferCreateInfo.pNext           = nullptr;
        vkFrameBufferCreateInfo.pAttachments    = textureViews.data();
        vkFrameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(textureViews.size());
        vkFrameBufferCreateInfo.renderPass      = renderPass->handle;
        vkFrameBufferCreateInfo.width           = renderPassDescription.attachmentExtent.width;
        vkFrameBufferCreateInfo.height          = renderPassDescription.attachmentExtent.height;
        vkFrameBufferCreateInfo.layers          = renderPassDescription.attachmentExtent.depth;
        vkFrameBufferCreateInfo.flags           = 0;

        VkFramebuffer vkFrameBuffer = VK_NULL_HANDLE;

        VkResult result = vkCreateFramebuffer(getVkContext()->getLogicalDevice(), &vkFrameBufferCreateInfo, nullptr, &vkFrameBuffer);
        if(VkResult::VK_SUCCESS != result)
        {
            CLog::Error(logTag(), CString::format("Failed to create frame buffer instance. Vulkan result: {}", result));
            return { EEngineStatus::Error };
        }

        this->handle = vkFrameBuffer;

        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanFrameBufferResource::destroy()
    {
        vkDestroyFramebuffer(getVkContext()->getLogicalDevice(), this->handle, nullptr);

        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------
}
