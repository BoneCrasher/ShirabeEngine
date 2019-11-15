#include "vulkan_integration/rendering/vulkanrendercontext.h"
#include "vulkan_integration/resources/types/vulkantextureresource.h"
#include "vulkan_integration/resources/types/vulkanframebufferresource.h"
#include "vulkan_integration/resources/types/vulkanrenderpassresource.h"
#include "vulkan_integration/resources/types/vulkanpipelineresource.h"

#include <thread>
#include <base/string.h>

namespace engine
{
    namespace vulkan
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------        
        bool CVulkanRenderContext::initialize(Shared<CVulkanEnvironment>     const &aVulkanEnvironment
                                            , Shared<CGpuApiResourceStorage> const &aResourceStorage)
        {
            assert(nullptr != aVulkanEnvironment);
            assert(nullptr != aResourceStorage);

            mVulkanEnvironment = aVulkanEnvironment;
            mResourceStorage   = aResourceStorage;

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CVulkanRenderContext::deinitialize()
        {
            mVulkanEnvironment = nullptr;

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanRenderContext::beginSubpass()
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanRenderContext::endSubpass()
        {
            SVulkanState    &state        = mVulkanEnvironment->getState();
            VkCommandBuffer commandBuffer = state.commandBuffers.at(state.swapChain.currentSwapChainImageIndex); // The commandbuffers and swapchain count currently match

            vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------

        EEngineStatus CVulkanRenderContext::copyImage(GpuApiHandle_t const &aSourceImageId,
                                                      GpuApiHandle_t const &aTargetImageId)
        {
            SHIRABE_UNUSED(aSourceImageId);
            SHIRABE_UNUSED(aTargetImageId);

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanRenderContext::copyToBackBuffer(GpuApiHandle_t const &aSourceImageId)
        {
            auto const *const texture = mVulkanEnvironment->getResourceStorage()->extract<CVulkanTextureResource>(aSourceImageId);
            if(nullptr == texture)
            {
                CLog::Error(logTag(), "Failed to fetch copy source image '%0'.", aSourceImageId);
                return EEngineStatus::Error;
            }

            SVulkanState &state = mVulkanEnvironment->getState();

            VkCommandBuffer vkCommandBuffer = state.commandBuffers.at(state.swapChain.currentSwapChainImageIndex);
            VkImage         swapChainImage  = state.swapChain.swapChainImages.at(state.swapChain.currentSwapChainImageIndex);

            VkImageMemoryBarrier vkImageMemoryBarrier {};
            vkImageMemoryBarrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            vkImageMemoryBarrier.pNext                           = nullptr;
            vkImageMemoryBarrier.srcAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;
            vkImageMemoryBarrier.dstAccessMask                   = 0;
            vkImageMemoryBarrier.oldLayout                       = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
            vkImageMemoryBarrier.newLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            vkImageMemoryBarrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
            vkImageMemoryBarrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
            vkImageMemoryBarrier.image                           = swapChainImage;
            vkImageMemoryBarrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            vkImageMemoryBarrier.subresourceRange.baseMipLevel   = 0;
            vkImageMemoryBarrier.subresourceRange.levelCount     = 1;
            vkImageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
            vkImageMemoryBarrier.subresourceRange.layerCount     = 1;

            // Create pipeline barrier on swap chain image to move it to correct format.
            vkCmdPipelineBarrier(vkCommandBuffer,
                                 VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VkDependencyFlagBits   ::VK_DEPENDENCY_BY_REGION_BIT,
                                 0, nullptr,
                                 0, nullptr,
                                 1, &vkImageMemoryBarrier);

            VkImageAspectFlags const vkAspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;

            VkImageSubresourceLayers vkSubresourceLayers {};
            vkSubresourceLayers.baseArrayLayer = 0;
            vkSubresourceLayers.layerCount     = 1;
            vkSubresourceLayers.mipLevel       = 0;
            vkSubresourceLayers.aspectMask     = vkAspectMask;

            VkExtent3D vkExtent {};
            vkExtent.width  = state.swapChain.selectedExtents.width;
            vkExtent.height = state.swapChain.selectedExtents.height;
            vkExtent.depth  = 1;

            VkImageCopy vkRegion {};
            vkRegion.srcOffset      = { 0, 0, 0 };
            vkRegion.srcSubresource = vkSubresourceLayers;
            vkRegion.dstOffset      = { 0, 0, 0 };
            vkRegion.dstSubresource = vkSubresourceLayers;
            vkRegion.extent         = vkExtent;

            vkCmdCopyImage(vkCommandBuffer,
                           texture->imageHandle,
                           VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                           swapChainImage,
                           VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1,
                           &vkRegion);

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanRenderContext::beginGraphicsCommandBuffer()
        {
            SVulkanState &state = mVulkanEnvironment->getState();

            VkCommandBufferBeginInfo vkCommandBufferBeginInfo = {};
            vkCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            vkCommandBufferBeginInfo.pNext = nullptr;
            vkCommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

            VkCommandBuffer commandBuffer = state.commandBuffers.at(state.swapChain.currentSwapChainImageIndex); // The commandbuffers and swapchain count currently match

            VkResult const result = vkBeginCommandBuffer(commandBuffer, &vkCommandBufferBeginInfo); // The command structure potentially changes. Recreate always.
            if(VkResult::VK_SUCCESS != result)
            {
                throw CVulkanError("Failed to begin command buffer.", result);
            }

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanRenderContext::commitGraphicsCommandBuffer()
        {
            SVulkanState &vkState = mVulkanEnvironment->getState();
            VkCommandBuffer vkCommandBuffer = vkState.commandBuffers.at(vkState.swapChain.currentSwapChainImageIndex); // The commandbuffers and swapchain count currently match
            VkImage         swapChainImage  = vkState.swapChain.swapChainImages.at(vkState.swapChain.currentSwapChainImageIndex);

            //
            // Make sure the swap chain is presentable.
            //
            VkImageMemoryBarrier vkImageMemoryBarrier {};
            vkImageMemoryBarrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            vkImageMemoryBarrier.pNext                           = nullptr;
            vkImageMemoryBarrier.srcAccessMask                   = 0;
            vkImageMemoryBarrier.dstAccessMask                   = 0;
            vkImageMemoryBarrier.oldLayout                       = VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            vkImageMemoryBarrier.newLayout                       = VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            vkImageMemoryBarrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
            vkImageMemoryBarrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
            vkImageMemoryBarrier.image                           = swapChainImage;
            vkImageMemoryBarrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            vkImageMemoryBarrier.subresourceRange.baseMipLevel   = 0;
            vkImageMemoryBarrier.subresourceRange.levelCount     = 1;
            vkImageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
            vkImageMemoryBarrier.subresourceRange.layerCount     = 1;

            // Create pipeline barrier on swap chain image to move it to correct format.
            vkCmdPipelineBarrier(vkCommandBuffer,
                                 VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
                                 VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
                                 VkDependencyFlagBits   ::VK_DEPENDENCY_BY_REGION_BIT,
                                 0, nullptr,
                                 0, nullptr,
                                 1, &vkImageMemoryBarrier);

            VkResult const result = vkEndCommandBuffer(vkCommandBuffer); // The commandbuffers and swapchain count currently match
            if(VkResult::VK_SUCCESS != result)
            {
                throw CVulkanError("Failed to record and commit command buffer.", result);
            }

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanRenderContext::bindRenderPass(GpuApiHandle_t const &aRenderPassId,
                                                           GpuApiHandle_t const &aFrameBufferId)
        {
            auto const *const renderPass  = mVulkanEnvironment->getResourceStorage()->extract<CVulkanRenderPassResource>(aRenderPassId);
            auto const *const frameBuffer = mVulkanEnvironment->getResourceStorage()->extract<CVulkanFrameBufferResource>(aFrameBufferId);

            if(nullptr == frameBuffer)
            {
                CLog::Error(logTag(), "Failed to fetch frame buffer '%0'.", aFrameBufferId);
                return EEngineStatus::Error;
            }

            if(nullptr == renderPass)
            {
                CLog::Error(logTag(), "Failed to fetch render pass '%0'.", aRenderPassId);
                return EEngineStatus::Error;
            }

            SVulkanState &state = mVulkanEnvironment->getState();

            VkClearValue clearColor = { 0.0f, 0.5f, 0.5f, 1.0f };

            VkRenderPassBeginInfo vkRenderPassBeginInfo {};
            vkRenderPassBeginInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            vkRenderPassBeginInfo.pNext             = nullptr;
            vkRenderPassBeginInfo.renderPass        = renderPass->handle;
            vkRenderPassBeginInfo.framebuffer       = frameBuffer->handle;
            vkRenderPassBeginInfo.renderArea.offset = { 0, 0 };
            vkRenderPassBeginInfo.renderArea.extent = state.swapChain.selectedExtents;
            vkRenderPassBeginInfo.clearValueCount   = 1;
            vkRenderPassBeginInfo.pClearValues      = &clearColor;

            vkCmdBeginRenderPass(state.commandBuffers.at(state.swapChain.currentSwapChainImageIndex), &vkRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanRenderContext::unbindRenderPass(GpuApiHandle_t const &aFrameBufferId,
                                                             GpuApiHandle_t const &aRenderPassId)
        {
            SHIRABE_UNUSED(aFrameBufferId);
            SHIRABE_UNUSED(aRenderPassId);

            // CEngineResult<Shared<SVulkanFrameBufferResource>> frameBufferFetch = mGraphicsAPIResourceBackend->getResource<SVulkanFrameBufferResource>(aFrameBufferId);
            // if(not frameBufferFetch.successful())
            // {
            //     CLog::Error(logTag(), "Failed to fetch frame buffer '%0'.", aFrameBufferId);
            //     return frameBufferFetch.result();
            // }
            //
            // CEngineResult<Shared<SVulkanRenderPassResource>> renderPassFetch = mGraphicsAPIResourceBackend->getResource<SVulkanRenderPassResource>(aRenderPassId);
            // if(not renderPassFetch.successful())
            // {
            //     CLog::Error(logTag(), "Failed to fetch render pass '%0'.", aRenderPassId);
            //     return renderPassFetch.result();
            // }
            //
            // SVulkanFrameBufferResource const &frameBuffer = *frameBufferFetch.data();
            // SVulkanRenderPassResource  const &renderPass  = *renderPassFetch.data();

            SVulkanState &state = mVulkanEnvironment->getState();

            vkCmdEndRenderPass(state.commandBuffers.at(state.swapChain.currentSwapChainImageIndex));

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanRenderContext::bindSwapChain(GpuApiHandle_t const &aSwapChainResourceId)
        {
            SHIRABE_UNUSED(aSwapChainResourceId);

            SVulkanState     &vkState     = mVulkanEnvironment->getState();
            SVulkanSwapChain &vkSwapChain = vkState.swapChain;

            uint32_t nextImageIndex = 0;

            VkDevice       device    = vkState.selectedLogicalDevice;
            VkSwapchainKHR swapChain = vkState.swapChain.handle;
            VkSemaphore    semaphore = vkState.swapChain.imageAvailableSemaphore;
            uint64_t const timeout   =  std::numeric_limits<uint64_t>::max();

            VkResult result = VK_SUCCESS;

            do
            {
                result =
                    vkAcquireNextImageKHR(
                        device,
                        swapChain,
                        timeout,
                        semaphore,
                        VK_NULL_HANDLE,
                        &nextImageIndex);

                if(VkResult::VK_ERROR_OUT_OF_DATE_KHR == result)
                {
                    mVulkanEnvironment->recreateSwapChain();
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }

            }
            while(VkResult::VK_SUCCESS != result);

            // if(VkResult::VK_SUCCESS != result)
            // {
            //    CLog::Error(logTag(), CString::format("AquireNextImageKHR failed with VkResult: %0", result));
            //    throw CVulkanError("Failed to execute 'vkAcquireNextImageKHR'.", result);
            // }

            vkState.swapChain.currentSwapChainImageIndex = nextImageIndex;

            // VkImage      &image    = vkSwapChain.swapChainImages.at(static_cast<uint64_t>(nextImageIndex));
            // Shared<void>  resource = Shared<void>(static_cast<void*>(&image), [] (void*) {});

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanRenderContext::present()
        {
            SVulkanState &vkState = mVulkanEnvironment->getState();

            VkQueue presentQueue  = mVulkanEnvironment->getPresentQueue();
            VkQueue graphicsQueue = mVulkanEnvironment->getGraphicsQueue();

            VkCommandBuffer vkCommandBuffer = vkState.commandBuffers.at(vkState.swapChain.currentSwapChainImageIndex); // The commandbuffers and swapchain count currently match

            VkSwapchainKHR swapChains[]       = { vkState.swapChain.handle                   };
            VkSemaphore    waitSemaphores[]   = { vkState.swapChain.imageAvailableSemaphore  };
            VkSemaphore    signalSemaphores[] = { vkState.swapChain.renderCompletedSemaphore };

            VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT };

            VkSubmitInfo vkSubmitInfo {};
            vkSubmitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            vkSubmitInfo.pNext                = nullptr;
            vkSubmitInfo.waitSemaphoreCount   = 1;
            vkSubmitInfo.pWaitSemaphores      = waitSemaphores;
            vkSubmitInfo.pWaitDstStageMask    = waitStages;
            vkSubmitInfo.commandBufferCount   = 1;
            vkSubmitInfo.pCommandBuffers      = &vkCommandBuffer;
            vkSubmitInfo.signalSemaphoreCount = 1;
            vkSubmitInfo.pSignalSemaphores    = signalSemaphores;

            VkResult result = vkQueueSubmit(graphicsQueue, 1, &vkSubmitInfo, VK_NULL_HANDLE);
            if(VK_SUCCESS != result)
            {
                throw CVulkanError("Failed to execute 'vkQueueSubmit'", result);
            }

            VkPresentInfoKHR vkPresentInfo {};
            vkPresentInfo.sType              =  VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            vkPresentInfo.pNext              =  nullptr;
            vkPresentInfo.waitSemaphoreCount =  1;
            vkPresentInfo.pWaitSemaphores    =  signalSemaphores;
            vkPresentInfo.swapchainCount     =  1;
            vkPresentInfo.pSwapchains        =  swapChains;
            vkPresentInfo.pImageIndices      = &(vkState.swapChain.currentSwapChainImageIndex);
            vkPresentInfo.pResults           =  nullptr;

            result = vkQueuePresentKHR(presentQueue, &vkPresentInfo);
            if(VK_SUCCESS != result)
            {
                throw CVulkanError("Failed to execute 'vkQueuePresentKHR'", result);
            }

            // Temporary workaround to avoid memory depletion from GPU workloads using validation layers.
            // Implement better synchronization and throttling, once ready.
            result = vkQueueWaitIdle(presentQueue);
            if(VK_SUCCESS != result)
            {
                throw CVulkanError("Failed to execute 'vkQueueWaitIdle' for temporary synchronization implementation", result);
            }

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanRenderContext::bindPipeline(GpuApiHandle_t  const &aPipelineUID)
        {
            SVulkanState     &vkState        = mVulkanEnvironment->getState();
            VkCommandBuffer  vkCommandBuffer = vkState.commandBuffers.at(vkState.swapChain.currentSwapChainImageIndex); // The commandbuffers and swapchain count currently match

            auto const *const pipeline = mVulkanEnvironment->getResourceStorage()->extract<CVulkanPipelineResource>(aPipelineUID);
            if(nullptr == pipeline)
            {
                CLog::Error(logTag(), "Failed to fetch pipeline '%0'.", aPipelineUID);
                return EEngineStatus::Error;
            }

            vkCmdBindPipeline(vkCommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline);

            vkCmdDraw(vkCommandBuffer, 3, 1, 0, 0); // Single triangle for now.

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanRenderContext::unbindPipeline(GpuApiHandle_t const &aPipelineUID)
        {
            SHIRABE_UNUSED(aPipelineUID);
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanRenderContext::bindResource(GpuApiHandle_t const &aId)
        {
            CLog::Verbose(logTag(), CString::format("Binding resource with id %0", aId));
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanRenderContext::unbindResource(GpuApiHandle_t const &aId)
        {
            CLog::Verbose(logTag(), CString::format("Unbinding resource with id %0", aId));
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanRenderContext::render(SRenderable const &aRenderable)
        {
            CLog::Verbose(logTag(), CString::format("Rendering renderable: %0", convert_to_string(aRenderable)));
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------
    }
}
