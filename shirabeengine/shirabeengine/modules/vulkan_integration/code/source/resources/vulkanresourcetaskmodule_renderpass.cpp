#include <graphicsapi/resources/types/renderpass.h>
#include <graphicsapi/resources/gfxapiresourcebackend.h>
#include "vulkan/vulkandevicecapabilities.h"
#include "vulkan/resources/vulkanresourcetaskbackend.h"
#include "vulkan/resources/types/vulkanrenderpassresource.h"

#include <vulkan/vulkan.h>

namespace engine
{
    namespace vulkan
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanResourceTaskBackend::fnRenderPassCreationTask(
                CRenderPass::CCreationRequest  const &aRequest,
                ResolvedDependencyCollection_t const &aDepencies,
                ResourceTaskFn_t                     &aOutTask)
        {
            EEngineStatus status = EEngineStatus::Ok;

            CRenderPass::SDescriptor const &desc = aRequest.resourceDescriptor();

            aOutTask = [=] () -> SGFXAPIResourceHandleAssignment
            {
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

                    VkSubpassDescription vkSubpassDesc{};
                    vkSubpassDesc.pipelineBindPoint       = VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS;
                    vkSubpassDesc.pInputAttachments       = inputAttachmentReferences.data();
                    vkSubpassDesc.inputAttachmentCount    = inputAttachmentReferences.size();
                    vkSubpassDesc.pColorAttachments       = colorAttachmentReferences.data();
                    vkSubpassDesc.colorAttachmentCount    = colorAttachmentReferences.size();
                    vkSubpassDesc.pDepthStencilAttachment = depthAttachmentReferences.data();
                    vkSubpassDesc.flags = 0;

                    vkSubpassDescriptions.push_back(vkSubpassDesc);
                }

                VkRenderPassCreateInfo vkRenderPassCreateInfo ={ };
                vkRenderPassCreateInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
                vkRenderPassCreateInfo.pNext           = nullptr;
                vkRenderPassCreateInfo.pSubpasses      = vkSubpassDescriptions.data();
                vkRenderPassCreateInfo.subpassCount    = vkSubpassDescriptions.size();
                vkRenderPassCreateInfo.pAttachments    = vkAttachmentDescriptions.data();
                vkRenderPassCreateInfo.attachmentCount = vkAttachmentDescriptions.size();
                vkRenderPassCreateInfo.pDependencies   = nullptr;
                vkRenderPassCreateInfo.dependencyCount = 0;
                vkRenderPassCreateInfo.flags           = 0;

                VkRenderPass vkRenderPass = VK_NULL_HANDLE;

                VkResult result = vkCreateRenderPass(mVulkanEnvironment->getState().selectedLogicalDevice, &vkRenderPassCreateInfo, nullptr, &vkRenderPass);
                if(VkResult::VK_SUCCESS != result)
                {
                    throw CVulkanError("Failed to create render target view.", result);
                }

                SVulkanRenderPassResource *renderPassResource = new SVulkanRenderPassResource();
                renderPassResource->handle = vkRenderPass;

                SGFXAPIResourceHandleAssignment assignment ={ };

                auto const renderPassDeleter = [] (SVulkanRenderPassResource const *aPointer)
                {
                    if(aPointer)
                    {
                        delete aPointer;
                    }
                };

                assignment.publicResourceHandle   = desc.name; // Just abuse the pointer target address of the handle...
                assignment.internalResourceHandle = CStdSharedPtr_t<SVulkanRenderPassResource>(renderPassResource, renderPassDeleter);

                return assignment;
            };

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanResourceTaskBackend::fnRenderPassUpdateTask(
                CRenderPass::CUpdateRequest     const &aRequest,
                SGFXAPIResourceHandleAssignment const &aAssignment,
                ResolvedDependencyCollection_t  const &aDepencies,
                ResourceTaskFn_t                      &aOutTask)
        {
            EEngineStatus status = EEngineStatus::Ok;

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanResourceTaskBackend::fnRenderPassDestructionTask(
                CRenderPass::CDestructionRequest const &aRequest,
                SGFXAPIResourceHandleAssignment  const &aAssignment,
                ResolvedDependencyCollection_t   const &aDepencies,
                ResourceTaskFn_t                       &aOutTask)
        {
            EEngineStatus status = EEngineStatus::Ok;

            aOutTask = [=] () -> SGFXAPIResourceHandleAssignment
            {
                CStdSharedPtr_t<SVulkanRenderPassResource> renderPass = std::static_pointer_cast<SVulkanRenderPassResource>(aAssignment.internalResourceHandle);
                if(!renderPass)
                {
                    throw CVulkanError("Invalid internal data provided for render pass destruction.", VkResult::VK_ERROR_INVALID_EXTERNAL_HANDLE);
                }

                VkRenderPass vkRenderPass    = renderPass->handle;
                VkDevice     vkLogicalDevice = mVulkanEnvironment->getState().selectedLogicalDevice;

                vkDestroyRenderPass(vkLogicalDevice, vkRenderPass, nullptr);

                SGFXAPIResourceHandleAssignment assignment = aAssignment;
                assignment.internalResourceHandle = nullptr;

                return assignment;
            };

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanResourceTaskBackend::fnRenderPassQueryTask(
                CRenderPass::CQuery             const &aRequest,
                SGFXAPIResourceHandleAssignment const &aAssignment,
                ResourceTaskFn_t                      &aOutTask)
        {
            EEngineStatus status = EEngineStatus::Ok;

            return status;
        }
        //<-----------------------------------------------------------------------------
    }
}
