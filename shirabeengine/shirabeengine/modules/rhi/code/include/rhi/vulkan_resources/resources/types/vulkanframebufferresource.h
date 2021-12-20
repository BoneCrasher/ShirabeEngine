#ifndef __SHIRABE_VULKAN_FRAMEBUFFER_RESOURCE_H__
#define __SHIRABE_VULKAN_FRAMEBUFFER_RESOURCE_H__

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <base/declaration.h>
#include "rhi/resource_management/resourcetypes.h"
#include "rhi/resource_management/extensibility.h"
#include "rhi/vulkan_resources/resources/ivkglobalcontext.h"
#include "rhi/vulkan_resources/resources/types/vulkanrenderpassresource.h"
#include "rhi/vulkan_core/vulkandevicecapabilities.h"

namespace engine
{
    SHIRABE_DECLARE_VULKAN_RHI_RESOURCE(FrameBuffer)

    namespace vulkan
    {
        using namespace rhi;

        /**
         * The SVulkanRHIImageResource struct describes the relevant data to deal
         * with textures inside the vulkan API.
         */
        struct SVulkanRHIFrameBuffer
        {
            struct Handles_t
            {
                VkFramebuffer handle;
            };

            template <typename TResourceManager>
            static EEngineStatus initialize(SRHIFrameBufferDescription const &aDescription
                                            , Handles_t                      &aGpuApiHandles
                                            , TResourceManager               *aResourceManager
                                            , IVkGlobalContext               *aVulkanEnvironment);

            template <typename TResourceManager>
            static EEngineStatus deinitialize(SRHIFrameBufferDescription const &aDescription
                                              , Handles_t                      &aGpuApiHandles
                                              , TResourceManager               *aResourceManager
                                              , IVkGlobalContext               *aVulkanEnvironment);
        };
        using RHIFrameBufferResourceState_t = SRHIResourceState<SRHIFrameBuffer>;


        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanRHIFrameBuffer::initialize(SRHIFrameBufferDescription const &aDescription
                                                        , Handles_t                      &aGpuApiHandles
                                                        , TResourceManager               *aResourceManager
                                                        , IVkGlobalContext               *aVulkanEnvironment)
        {
            auto const &[success, renderPassOptRef] = aResourceManager->template getResource<RHIRenderPassResourceState_t>(aDescription.renderPassResourceId);
            if(CheckEngineError(success))
            {
                return success;
            }
            RHIRenderPassResourceState_t const &renderPass = *renderPassOptRef;

            std::vector<VkImageView> textureViews {};
            for(auto const &id : renderPass.rhiCreateDesc.attachmentTextureViews)
            {
                auto const &[success, textureViewOptRef] = aResourceManager->template getResource<RHIImageViewResourceState_t>(id, aVulkanEnvironment);
                if(CheckEngineError(success))
                {
                    return success;
                }
                RHIImageViewResourceState_t const &textureView = *textureViewOptRef;

                textureViews.push_back(textureView.rhiHandles.handle);
            }

            VkFramebufferCreateInfo vkFrameBufferCreateInfo {};
            vkFrameBufferCreateInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            vkFrameBufferCreateInfo.pNext           = nullptr;
            vkFrameBufferCreateInfo.pAttachments    = textureViews.data();
            vkFrameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(textureViews.size());
            vkFrameBufferCreateInfo.renderPass      = renderPass.rhiHandles.handle;
            vkFrameBufferCreateInfo.width           = renderPass.rhiCreateDesc.attachmentExtent.width;
            vkFrameBufferCreateInfo.height          = renderPass.rhiCreateDesc.attachmentExtent.height;
            vkFrameBufferCreateInfo.layers          = renderPass.rhiCreateDesc.attachmentExtent.depth;
            vkFrameBufferCreateInfo.flags           = 0;

            VkFramebuffer vkFrameBuffer = VK_NULL_HANDLE;

            VkResult result = vkCreateFramebuffer(aVulkanEnvironment->getLogicalDevice(), &vkFrameBufferCreateInfo, nullptr, &vkFrameBuffer);
            if(VkResult::VK_SUCCESS != result)
            {
                CLog::Error("SVulkanRHIFrameBufferResource::initialize", StaticStringHelpers::format("Failed to create frame buffer instance. Vulkan result: {}", result));
                return EEngineStatus::Error;
            }

            aGpuApiHandles.handle = vkFrameBuffer;

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanRHIFrameBuffer::deinitialize(SRHIFrameBufferDescription const &aDescription
                                                          , Handles_t                      &aGpuApiHandles
                                                          , TResourceManager               *aResourceManager
                                                          , IVkGlobalContext               *aVulkanEnvironment)
        {
            vkDestroyFramebuffer(aVulkanEnvironment->getLogicalDevice(), aGpuApiHandles.handle, nullptr);

            return EEngineStatus::Ok;
        }
    }
}

#endif // VULKANFRAMEBUFFERRESOURCE_H
