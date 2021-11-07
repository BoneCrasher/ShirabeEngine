//
// Created by dottideveloper on 29.10.19.
//
#include "rhi/vulkan_resources/resources/types/vulkanbufferresource.h"
#include "rhi/vulkan_resources/resources/types/vulkanimageresource.h"
#include "rhi/vulkan_core/vulkandevicecapabilities.h"

namespace engine::vulkan
{
    auto __performImageLayoutTransfer(IVkGlobalContext                          *aVulkanEnvironment
                                      , SVulkanRHIImageResource::Handles_t const &aTexture
                                      , CRange                            const &aArrayRange
                                      , CRange                            const &aMipRange
                                      , VkImageAspectFlags                const &aAspectFlags
                                      , VkImageLayout                     const &aSourceLayout
                                      , VkImageLayout                     const &aTargetLayout) -> EEngineStatus
    {
        VkCommandBuffer commandBuffer = aVulkanEnvironment->getVkCurrentFrameContext()->getTransferCommandBuffer();

        VkImage image = aTexture.imageHandle;

        VkImageMemoryBarrier vkImageMemoryBarrier {};
        vkImageMemoryBarrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        vkImageMemoryBarrier.pNext                           = nullptr;
        vkImageMemoryBarrier.srcAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;
        vkImageMemoryBarrier.dstAccessMask                   = 0;
        vkImageMemoryBarrier.oldLayout                       = aSourceLayout;
        vkImageMemoryBarrier.newLayout                       = aTargetLayout;
        vkImageMemoryBarrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        vkImageMemoryBarrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        vkImageMemoryBarrier.image                           = image;
        vkImageMemoryBarrier.subresourceRange.aspectMask     = aAspectFlags;
        vkImageMemoryBarrier.subresourceRange.baseMipLevel   = aMipRange.offset;
        vkImageMemoryBarrier.subresourceRange.levelCount     = aMipRange.length;
        vkImageMemoryBarrier.subresourceRange.baseArrayLayer = aArrayRange.offset;
        vkImageMemoryBarrier.subresourceRange.layerCount     = aArrayRange.length;

        // Create pipeline barrier on swap chain image to move it to correct format.
        vkCmdPipelineBarrier(commandBuffer,
            VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
            VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
            VkDependencyFlagBits   ::VK_DEPENDENCY_BY_REGION_BIT,
            0, nullptr,
            0, nullptr,
            1, &vkImageMemoryBarrier);

        return EEngineStatus::Ok;
    }
}
