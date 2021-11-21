#ifndef __SHIRABE_VULKAN_RENDERPASS_RESOURCE_H__
#define __SHIRABE_VULKAN_RENDERPASS_RESOURCE_H__

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <base/declaration.h>
#include "rhi/resource_management/resourcetypes.h"
#include "rhi/resource_management/extensibility.h"
#include "rhi/vulkan_resources/resources/ivkglobalcontext.h"
#include "rhi/vulkan_core/vulkandevicecapabilities.h"

namespace engine
{
    namespace vulkan
    {
        struct SVulkanRHIRenderPassResource;
    }

    namespace rhi
    {
        template <> struct SRHIResourceMap<SRHIRenderPass> { using TMappedRHIResource = vulkan::SVulkanRHIRenderPassResource; };
    }

    namespace vulkan
    {
        using namespace rhi;

        /**
         * The SVulkanRHIImageResource struct describes the relevant data to deal
         * with textures inside the vulkan API.
         */
        struct SVulkanRHIRenderPassResource
        {
            struct Handles_t
            {
                VkRenderPass handle;
            };

            template <typename TResourceManager>
            static EEngineStatus initialize(SRHIRenderPassDescription const &aDescription
                                            , Handles_t                     &aGpuApiHandles
                                            , TResourceManager              *aResourceManager
                                            , IVkGlobalContext              *aVulkanEnvironment);

            template <typename TResourceManager>
            static EEngineStatus deinitialize(SRHIRenderPassDescription const &aDescription
                                              , Handles_t                     &aGpuApiHandles
                                              , TResourceManager              *aResourceManager
                                              , IVkGlobalContext              *aVulkanEnvironment);
        };

        using RHIRenderPassResourceState_t = SRHIResourceState<SRHIRenderPass>;

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanRHIRenderPassResource::initialize(SRHIRenderPassDescription const &aDescription
                                                               , Handles_t                     &aGpuApiHandles
                                                               , TResourceManager              *aResourceManager
                                                               , IVkGlobalContext              *aVulkanEnvironment)
        {

            /// CLog::Debug(logTag(), "Creating texture w/ name {}", aDescription.name);

            VkDevice const         &vkLogicalDevice  = aVulkanEnvironment->getLogicalDevice();
            VkPhysicalDevice const &vkPhysicalDevice = aVulkanEnvironment->getPhysicalDevice();

            std::vector<VkSubpassDescription>    vkSubpassDescriptions{};
            std::vector<VkAttachmentDescription> vkAttachmentDescriptions{};

            for(auto const &attachmentDesc : aDescription.attachmentDescriptions)
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

            for(auto const &subpassDesc : aDescription.subpassDescriptions)
            {
                std::vector<VkAttachmentReference> inputAttachmentReferences(0);
                std::vector<VkAttachmentReference> colorAttachmentReferences(0);
                std::vector<VkAttachmentReference> depthAttachmentReferences(0);

                for(SRHIAttachmentReference const &ref : subpassDesc.inputAttachments)
                {
                    VkAttachmentReference vkAttachmentReference {};
                    vkAttachmentReference.attachment = ref.attachment;
                    vkAttachmentReference.layout     = static_cast<VkImageLayout>(ref.layout);

                    inputAttachmentReferences.push_back(vkAttachmentReference);
                }

                for(SRHIAttachmentReference const &ref : subpassDesc.colorAttachments)
                {
                    VkAttachmentReference vkAttachmentReference {};
                    vkAttachmentReference.attachment = ref.attachment;
                    vkAttachmentReference.layout     = static_cast<VkImageLayout>(ref.layout);
                    colorAttachmentReferences.push_back(vkAttachmentReference);
                }

                for(SRHIAttachmentReference const &ref : subpassDesc.depthStencilAttachments)
                {
                    VkAttachmentReference vkAttachmentReference {};
                    vkAttachmentReference.attachment = ref.attachment;
                    vkAttachmentReference.layout     = static_cast<VkImageLayout>(ref.layout);

                    depthAttachmentReferences.push_back(vkAttachmentReference);
                }

                if(not inputAttachmentReferences.empty())
                {
                    inputAttachmentReferenceList.push_back(inputAttachmentReferences);
                }
                if(not colorAttachmentReferences.empty())
                {
                    colorAttachmentReferenceList.push_back(colorAttachmentReferences);
                }
                if(not depthAttachmentReferences.empty())
                {
                    depthAttachmentReferenceList.push_back(depthAttachmentReferences);
                }

                VkSubpassDescription vkSubpassDesc{};
                vkSubpassDesc.pipelineBindPoint       = VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS;
                vkSubpassDesc.pInputAttachments       = nullptr;
                vkSubpassDesc.inputAttachmentCount    = 0;
                vkSubpassDesc.pColorAttachments       = nullptr;
                vkSubpassDesc.colorAttachmentCount    = 0;
                vkSubpassDesc.pDepthStencilAttachment = nullptr;
                vkSubpassDesc.flags                   = 0;

                if(not inputAttachmentReferences.empty())
                {
                    vkSubpassDesc.pInputAttachments    = inputAttachmentReferenceList.back().data();
                    vkSubpassDesc.inputAttachmentCount = static_cast<uint32_t>(inputAttachmentReferenceList.back().size());
                }
                if(not colorAttachmentReferences.empty())
                {
                    vkSubpassDesc.pColorAttachments    = colorAttachmentReferenceList.back().data();
                    vkSubpassDesc.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentReferenceList.back().size());
                }
                if(not depthAttachmentReferences.empty())
                {
                    vkSubpassDesc.pDepthStencilAttachment = depthAttachmentReferenceList.back().data();
                }

                vkSubpassDescriptions.push_back(vkSubpassDesc);
            }

            std::vector<VkSubpassDependency> subpassDependencies {};
            subpassDependencies.resize(aDescription.subpassDependencies.size());
            for(std::size_t k=0; k<subpassDependencies.size(); ++k)
            {
                SRHISubpassDependency const &dep = aDescription.subpassDependencies[k];

                VkSubpassDependency subpassDependency {};
                subpassDependency.srcSubpass      = dep.srcPass;
                subpassDependency.srcStageMask    = dep.srcStage;
                subpassDependency.srcAccessMask   = dep.srcAccess;
                subpassDependency.dstSubpass      = dep.dstPass;
                subpassDependency.dstStageMask    = dep.dstStage;
                subpassDependency.dstAccessMask   = dep.dstAccess;
                subpassDependency.dependencyFlags = dep.dependencyFlags;
                subpassDependencies[k] = subpassDependency;
            }

            VkRenderPassCreateInfo vkRenderPassCreateInfo ={ };
            vkRenderPassCreateInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            vkRenderPassCreateInfo.pNext           = nullptr;
            vkRenderPassCreateInfo.pSubpasses      = vkSubpassDescriptions.data();
            vkRenderPassCreateInfo.subpassCount    = static_cast<uint32_t>(vkSubpassDescriptions.size());
            vkRenderPassCreateInfo.pAttachments    = vkAttachmentDescriptions.data();
            vkRenderPassCreateInfo.attachmentCount = static_cast<uint32_t>(vkAttachmentDescriptions.size());
            vkRenderPassCreateInfo.pDependencies   = subpassDependencies.data();
            vkRenderPassCreateInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());
            vkRenderPassCreateInfo.flags           = 0;

            VkRenderPass vkRenderPass = VK_NULL_HANDLE;

            VkResult result = vkCreateRenderPass(aVulkanEnvironment->getLogicalDevice(), &vkRenderPassCreateInfo, nullptr, &vkRenderPass);
            if(VkResult::VK_SUCCESS != result)
            {
                CLog::Error("CVulkanRenderPassResource::initialize", StaticStringHelpers::format("Failed to create render target view. Vulkan error: {}", result));
                return EEngineStatus::Error;
            }

            aGpuApiHandles.handle = vkRenderPass;

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanRHIRenderPassResource::deinitialize(SRHIRenderPassDescription const &aDescription
                                                                 , Handles_t                     &aGpuApiHandles
                                                                 , TResourceManager              *aResourceManager
                                                                 , IVkGlobalContext              *aVulkanEnvironment)
        {
            vkDestroyRenderPass(aVulkanEnvironment->getLogicalDevice(), aGpuApiHandles.handle, nullptr);

            return EEngineStatus::Ok;
        }
    }
}


#endif // VULKANRENDERPASSRESOURCE_H
