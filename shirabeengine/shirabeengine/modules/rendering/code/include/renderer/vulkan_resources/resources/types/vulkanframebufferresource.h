#ifndef __SHIRABE_VULKAN_FRAMEBUFFER_RESOURCE_H__
#define __SHIRABE_VULKAN_FRAMEBUFFER_RESOURCE_H__

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <base/declaration.h>
#include "renderer/resource_management/resourcetypes.h"
#include "renderer/resource_management/extensibility.h"
#include "renderer/vulkan_resources/resources/ivkglobalcontext.h"
#include "renderer/vulkan_resources/resources/types/vulkanrenderpassresource.h"
#include "renderer/vulkan_core/vulkandevicecapabilities.h"

namespace engine
{
    namespace vulkan
    {
        struct SVulkanFrameBufferResource;
    }

    namespace resources
    {
        template <> struct SLogicalToGpuApiResourceTypeMap<SFrameBuffer> { using TGpuApiResource = vulkan::SVulkanFrameBufferResource;  };
    }

    namespace vulkan
    {
        using namespace resources;

        /**
         * The SVulkanTextureResource struct describes the relevant data to deal
         * with textures inside the vulkan API.
         */
        struct SVulkanFrameBufferResource
        {
            static constexpr bool is_loadable      = false;
            static constexpr bool is_unloadable    = false;
            static constexpr bool is_transferrable = false;

            struct Handles_t
            {
                VkFramebuffer handle;
            };

            template <typename TResourceManager>
            static EEngineStatus initialize(SFrameBufferDescription const &aDescription
                                          , Handles_t                     &aGpuApiHandles
                                          , TResourceManager              *aResourceManager
                                          , IVkGlobalContext              *aVulkanEnvironment);

            template <typename TResourceManager>
            static EEngineStatus deinitialize(SFrameBufferDescription const &aDescription
                                            , Handles_t                     &aGpuApiHandles
                                           , TResourceManager               *aResourceManager
                                           , IVkGlobalContext               *aVulkanEnvironment);
        };
        using FrameBufferResourceState_t = SResourceState<SFrameBuffer>;


        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanFrameBufferResource::initialize(SFrameBufferDescription const &aDescription
                                                           , Handles_t                     &aGpuApiHandles
                                                           , TResourceManager              *aResourceManager
                                                           , IVkGlobalContext              *aVulkanEnvironment)
        {
            auto const &[success, renderPassOptRef] = aResourceManager->template getResource<RenderPassResourceState_t>(aDescription.renderPassResourceId);
            if(CheckEngineError(success))
            {
                return success;
            }
            RenderPassResourceState_t const &renderPass = *renderPassOptRef;

            std::vector<VkImageView> textureViews {};
            for(auto const &id : renderPass.description.attachmentTextureViews)
            {
                auto const &[success, textureViewOptRef] = aResourceManager->template getResource<TextureViewResourceState_t>(id, aVulkanEnvironment);
                if(CheckEngineError(success))
                {
                    return success;
                }
                TextureViewResourceState_t const &textureView = *textureViewOptRef;

                textureViews.push_back(textureView.gpuApiHandles.handle);
            }

            VkFramebufferCreateInfo vkFrameBufferCreateInfo {};
            vkFrameBufferCreateInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            vkFrameBufferCreateInfo.pNext           = nullptr;
            vkFrameBufferCreateInfo.pAttachments    = textureViews.data();
            vkFrameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(textureViews.size());
            vkFrameBufferCreateInfo.renderPass      = renderPass.gpuApiHandles.handle;
            vkFrameBufferCreateInfo.width           = renderPass.description.attachmentExtent.width;
            vkFrameBufferCreateInfo.height          = renderPass.description.attachmentExtent.height;
            vkFrameBufferCreateInfo.layers          = renderPass.description.attachmentExtent.depth;
            vkFrameBufferCreateInfo.flags           = 0;

            VkFramebuffer vkFrameBuffer = VK_NULL_HANDLE;

            VkResult result = vkCreateFramebuffer(aVulkanEnvironment->getLogicalDevice(), &vkFrameBufferCreateInfo, nullptr, &vkFrameBuffer);
            if(VkResult::VK_SUCCESS != result)
            {
                CLog::Error("SVulkanFrameBufferResource::initialize", CString::format("Failed to create frame buffer instance. Vulkan result: {}", result));
                return { EEngineStatus::Error };
            }

            aGpuApiHandles.handle = vkFrameBuffer;

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanFrameBufferResource::deinitialize(SFrameBufferDescription const &aDescription
                                                             , Handles_t                     &aGpuApiHandles
                                                             , TResourceManager              *aResourceManager
                                                             , IVkGlobalContext              *aVulkanEnvironment)
        {
            vkDestroyFramebuffer(aVulkanEnvironment->getLogicalDevice(), aGpuApiHandles.handle, nullptr);

            return { EEngineStatus::Ok };
        }
    }
}

#endif // VULKANFRAMEBUFFERRESOURCE_H
