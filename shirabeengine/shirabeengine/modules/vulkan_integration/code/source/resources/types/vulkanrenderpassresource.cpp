//
// Created by dottideveloper on 29.10.19.
//
#include "vulkan_integration/resources/types/vulkanrenderpassresource.h"
#include "vulkan_integration/vulkandevicecapabilities.h"

namespace engine::vulkan
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanRenderPassResource::create(GpuApiResourceDependencies_t const &aDependencies)
    {
        SHIRABE_UNUSED(aDependencies);

        SRenderPassDescription const &desc = getDescription();

        std::vector<VkSubpassDescription>    vkSubpassDescriptions{};
        std::vector<VkAttachmentDescription> vkAttachmentDescriptions{};

        for(auto const &attachmentDesc : desc.attachmentDescriptions)
        {
            VkAttachmentDescription vkAttachmentDesc{};
            vkAttachmentDesc.loadOp         = static_cast<VkAttachmentLoadOp> (attachmentDesc.loadOp);
            vkAttachmentDesc.stencilLoadOp  = static_cast<VkAttachmentLoadOp> (attachmentDesc.stencilLoadOp);
            vkAttachmentDesc.storeOp        = static_cast<VkAttachmentStoreOp>(attachmentDesc.storeOp);
            vkAttachmentDesc.stencilStoreOp = static_cast<VkAttachmentStoreOp>(attachmentDesc.stencilStoreOp);
            vkAttachmentDesc.initialLayout  = static_cast<VkImageLayout>      (attachmentDesc.initialLayout);
            vkAttachmentDesc.finalLayout    = static_cast<VkImageLayout>      (attachmentDesc.finalLayout);
            vkAttachmentDesc.format         = CVulkanDeviceCapsHelper::convertFormatToVk(attachmentDesc.format);
            vkAttachmentDesc.samples        = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
            vkAttachmentDesc.flags          = 0;

            vkAttachmentDescriptions.push_back(vkAttachmentDesc);
        }

        std::vector<std::vector<VkAttachmentReference>> inputAttachmentReferenceList(0);
        std::vector<std::vector<VkAttachmentReference>> colorAttachmentReferenceList(0);
        std::vector<std::vector<VkAttachmentReference>> depthAttachmentReferenceList(0);

        for(auto const &subpassDesc : desc.subpassDescriptions)
        {
            std::vector<VkAttachmentReference> inputAttachmentReferences(0);
            std::vector<VkAttachmentReference> colorAttachmentReferences(0);
            std::vector<VkAttachmentReference> depthAttachmentReferences(0);

            for(SAttachmentReference const &ref : subpassDesc.inputAttachments)
            {
                VkAttachmentReference vkAttachmentReference {};
                vkAttachmentReference.attachment = ref.attachment;
                vkAttachmentReference.layout     = static_cast<VkImageLayout>(ref.layout);

                inputAttachmentReferences.push_back(vkAttachmentReference);
            }

            for(SAttachmentReference const &ref : subpassDesc.colorAttachments)
            {
                VkAttachmentReference vkAttachmentReference {};
                vkAttachmentReference.attachment = ref.attachment;
                vkAttachmentReference.layout     = static_cast<VkImageLayout>(ref.layout);

                colorAttachmentReferences.push_back(vkAttachmentReference);
            }

            for(SAttachmentReference const &ref : subpassDesc.depthStencilAttachments)
            {
                VkAttachmentReference vkAttachmentReference {};
                vkAttachmentReference.attachment = ref.attachment;
                vkAttachmentReference.layout     = static_cast<VkImageLayout>(ref.layout);

                depthAttachmentReferences.push_back(vkAttachmentReference);
            }

            inputAttachmentReferenceList.push_back(inputAttachmentReferences);
            colorAttachmentReferenceList.push_back(colorAttachmentReferences);
            depthAttachmentReferenceList.push_back(depthAttachmentReferences);

            VkSubpassDescription vkSubpassDesc{};
            vkSubpassDesc.pipelineBindPoint       = VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS;
            vkSubpassDesc.pInputAttachments       = inputAttachmentReferenceList.back().data();
            vkSubpassDesc.inputAttachmentCount    = static_cast<uint32_t>(inputAttachmentReferenceList.back().size());
            vkSubpassDesc.pColorAttachments       = colorAttachmentReferenceList.back().data();
            vkSubpassDesc.colorAttachmentCount    = static_cast<uint32_t>(colorAttachmentReferenceList.back().size());
            vkSubpassDesc.pDepthStencilAttachment = depthAttachmentReferenceList.back().data();
            vkSubpassDesc.flags = 0;

            vkSubpassDescriptions.push_back(vkSubpassDesc);
        }

        VkRenderPassCreateInfo vkRenderPassCreateInfo ={ };
        vkRenderPassCreateInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        vkRenderPassCreateInfo.pNext           = nullptr;
        vkRenderPassCreateInfo.pSubpasses      = vkSubpassDescriptions.data();
        vkRenderPassCreateInfo.subpassCount    = static_cast<uint32_t>(vkSubpassDescriptions.size());
        vkRenderPassCreateInfo.pAttachments    = vkAttachmentDescriptions.data();
        vkRenderPassCreateInfo.attachmentCount = static_cast<uint32_t>(vkAttachmentDescriptions.size());
        vkRenderPassCreateInfo.pDependencies   = nullptr;
        vkRenderPassCreateInfo.dependencyCount = 0;
        vkRenderPassCreateInfo.flags           = 0;

        VkRenderPass vkRenderPass = VK_NULL_HANDLE;

        VkResult result = vkCreateRenderPass(getVkContext()->getLogicalDevice(), &vkRenderPassCreateInfo, nullptr, &vkRenderPass);
        if(VkResult::VK_SUCCESS != result)
        {
            CLog::Error(logTag(), CString::format("Failed to create render target view. Vulkan error: {}", result));
            return { EEngineStatus::Error };
        }

        this->handle = vkRenderPass;

        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanRenderPassResource::destroy()
    {
        vkDestroyRenderPass(getVkContext()->getLogicalDevice(), this->handle, nullptr);

        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------
}
