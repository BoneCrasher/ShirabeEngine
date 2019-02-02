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
        CEngineResult<> CVulkanResourceTaskBackend::fnRenderPassCreationTask(
                CRenderPass::CCreationRequest  const &aRequest,
                ResolvedDependencyCollection_t const &aDependencies,
                ResourceTaskFn_t                     &aOutTask)
        {
            SHIRABE_UNUSED(aDependencies);

            EEngineStatus status = EEngineStatus::Ok;

            CRenderPass::SDescriptor const &desc = aRequest.resourceDescriptor();

            aOutTask = [=] () -> CEngineResult<SGFXAPIResourceHandleAssignment>
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

                VkResult result = vkCreateRenderPass(mVulkanEnvironment->getState().selectedLogicalDevice, &vkRenderPassCreateInfo, nullptr, &vkRenderPass);
                if(VkResult::VK_SUCCESS != result)
                {
                    CLog::Error(logTag(), CString::format("Failed to create render target view. Vulkan error: %0", result));
                    return { EEngineStatus::Error };
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

                return { EEngineStatus::Ok, assignment };
            };

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CVulkanResourceTaskBackend::fnRenderPassUpdateTask(
                CRenderPass::CUpdateRequest     const &aRequest,
                SGFXAPIResourceHandleAssignment const &aAssignment,
                ResolvedDependencyCollection_t  const &aDependencies,
                ResourceTaskFn_t                      &aOutTask)
        {
            SHIRABE_UNUSED(aRequest);
            SHIRABE_UNUSED(aAssignment);
            SHIRABE_UNUSED(aDependencies);
            SHIRABE_UNUSED(aOutTask);

            EEngineStatus status = EEngineStatus::Ok;

            return { status };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CVulkanResourceTaskBackend::fnRenderPassDestructionTask(
                CRenderPass::CDestructionRequest const &aRequest,
                SGFXAPIResourceHandleAssignment  const &aAssignment,
                ResolvedDependencyCollection_t   const &aDependencies,
                ResourceTaskFn_t                       &aOutTask)
        {
            SHIRABE_UNUSED(aRequest);
            SHIRABE_UNUSED(aDependencies);

            EEngineStatus status = EEngineStatus::Ok;

            aOutTask = [=] () -> CEngineResult<SGFXAPIResourceHandleAssignment>
            {
                CStdSharedPtr_t<SVulkanRenderPassResource> renderPass = std::static_pointer_cast<SVulkanRenderPassResource>(aAssignment.internalResourceHandle);
                if(nullptr == renderPass)
                {
                    CLog::Error(logTag(), CString::format("Invalid internal data provided for render pass destruction. Vulkan error: %0", VkResult::VK_ERROR_INVALID_EXTERNAL_HANDLE));
                    return { EEngineStatus::Error };
                }

                VkRenderPass vkRenderPass    = renderPass->handle;
                VkDevice     vkLogicalDevice = mVulkanEnvironment->getState().selectedLogicalDevice;

                vkDestroyRenderPass(vkLogicalDevice, vkRenderPass, nullptr);

                SGFXAPIResourceHandleAssignment assignment = aAssignment;
                assignment.internalResourceHandle = nullptr;

                return { EEngineStatus::Ok, assignment };
            };

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CVulkanResourceTaskBackend::fnRenderPassQueryTask(
                CRenderPass::CQuery             const &aRequest,
                SGFXAPIResourceHandleAssignment const &aAssignment,
                ResourceTaskFn_t                      &aOutTask)
        {
            SHIRABE_UNUSED(aRequest);
            SHIRABE_UNUSED(aAssignment);
            SHIRABE_UNUSED(aOutTask);

            EEngineStatus status = EEngineStatus::Ok;

            return { status };
        }
        //<-----------------------------------------------------------------------------
    }
}
