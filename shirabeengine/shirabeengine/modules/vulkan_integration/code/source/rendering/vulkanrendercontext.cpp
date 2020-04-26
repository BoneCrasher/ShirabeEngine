#include "vulkan_integration/rendering/vulkanrendercontext.h"

#include <core/enginetypehelper.h>
#include <mesh/loader.h>
#include <mesh/declaration.h>
#include <material/loader.h>
#include <material/declaration.h>
#include <material/serialization.h>
#include <renderer/framegraph/framegraphrendercontext.h>

#include <base/string.h>

namespace engine
{
    namespace vulkan
    {
        using namespace framegraph;
        using engine::resources::CResourceManager;

        template <typename T>
        using OptRef_t = std::optional<std::reference_wrapper<T>>;

        template <typename TResource>
        using FetchResult_t = std::tuple<bool, OptRef_t<TResource>>;

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResource, typename... TArgs>
        FetchResult_t<TResource> fetchResource(ResourceId_t             const &aResourceId
                                             , Shared<CResourceManager>        aResourceManager
                                             , TArgs                      &&...aArgs)
        {
            static FetchResult_t<TResource> const sInvalid = { false, {} };

            auto const &[success, resource] = aResourceManager->getResource<TResource>(aResourceId, std::forward<TArgs>(aArgs)...);
            if(CheckEngineError(success))
            {
                return sInvalid;
            }

            return { true, resource };
        }

        namespace detail
        {
            auto clearAttachments(Shared<CVulkanEnvironment>     aVulkanEnvironment
                                , Shared<CResourceManager>       aResourceManager
                                , Shared<asset::CAssetStorage>   aAssetStorage
                                , SFrameGraphRenderContextState &aState
                                , std::string             const &aRenderPassId) -> EEngineStatus
            {
                SVulkanState    &state        = aVulkanEnvironment->getState();
                VkCommandBuffer commandBuffer = aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer();

                auto const [success, resource] = fetchResource<RenderPassResourceState_t>(aRenderPassId, aResourceManager, aVulkanEnvironment);
                if(not success)
                {
                    return EEngineStatus::Error;
                }

                RenderPassResourceState_t    &renderPass  = *resource;
                SRenderPassDescription const &description = renderPass.description;

                std::vector<VkClearRect>       clearRects       {};
                std::vector<VkClearAttachment> clearAttachments {};

                SSubpassDescription const &subpassDesc = description.subpassDescriptions.at(aState.currentSubpassIndex);
                for(std::size_t k=0; k<subpassDesc.colorAttachments.size(); ++k)
                {
                    SAttachmentReference   const &ref  = subpassDesc.colorAttachments[k];
                    SAttachmentDescription const &desc = description.attachmentDescriptions[ref.attachment];

                    VkClearAttachment clear {};
                    clear.colorAttachment = k;
                    clear.aspectMask      = VK_IMAGE_ASPECT_COLOR_BIT;
                    clear.clearValue      = desc.clearColor;

                    VkClearRect rect {};
                    rect.baseArrayLayer = 0;
                    rect.layerCount     = 1;
                    rect.rect.offset    = { 0, 0 };
                    rect.rect.extent    = { description.attachmentExtent.width,  description.attachmentExtent.height };

                    clearAttachments.push_back(clear);
                    clearRects      .push_back(rect);
                }

                for(std::size_t k=0; k<subpassDesc.depthStencilAttachments.size(); ++k)
                {
                    SAttachmentReference   const &ref  = subpassDesc.depthStencilAttachments[k];
                    SAttachmentDescription const &desc = description.attachmentDescriptions[ref.attachment];

                    VkClearAttachment clear {};
                    clear.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
                    clear.clearValue = desc.clearColor;

                    VkClearRect rect {};
                    rect.baseArrayLayer = 0;
                    rect.layerCount     = 1;
                    rect.rect.offset    = { 0, 0 };
                    rect.rect.extent    = { description.attachmentExtent.width,  description.attachmentExtent.height };

                    clearAttachments.push_back(clear);
                    clearRects      .push_back(rect);
                }

                vkCmdClearAttachments(commandBuffer, clearAttachments.size(), clearAttachments.data(), clearRects.size(), clearRects.data());
                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto endPass(Shared<CVulkanEnvironment>     aVulkanEnvironment
                       , Shared<CResourceManager>       aResourceManager
                       , Shared<asset::CAssetStorage>   aAssetStorage
                       , SFrameGraphRenderContextState &aState) -> EEngineStatus
            {
                SVulkanState    &state        = aVulkanEnvironment->getState();
                VkCommandBuffer commandBuffer = aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer();

                vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);
                ++(aState.currentSubpassIndex);

                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto copyImage(Shared<CVulkanEnvironment>       aVulkanEnvironment
                         , Shared<CResourceManager>         aResourceManager
                         , Shared<asset::CAssetStorage>     aAssetStorage
                         , SFrameGraphRenderContextState   &aState
                         , SFrameGraphDynamicTexture const &aSourceImageId
                         , SFrameGraphDynamicTexture const &aTargetImageId) -> EEngineStatus
            {
                SHIRABE_UNUSED(aSourceImageId);
                SHIRABE_UNUSED(aTargetImageId);

                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto performImageLayoutTransfer(Shared<CVulkanEnvironment>       aVulkanEnvironment
                                          , Shared<CResourceManager>         aResourceManager
                                          , Shared<asset::CAssetStorage>     aAssetStorage
                                          , SFrameGraphRenderContextState   &aState
                                          , SFrameGraphDynamicTexture const &aTexture
                                          , CRange                    const &aArrayRange
                                          , CRange                    const &aMipRange
                                          , VkImageAspectFlags        const &aAspectFlags
                                          , VkImageLayout             const &aSourceLayout
                                          , VkImageLayout             const &aTargetLayout) -> EEngineStatus
            {
                VkCommandBuffer commandBuffer = aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer();

                OptRef_t <TextureResourceState_t> sampledImageOpt{};
                {
                    auto[success, resource] = fetchResource<TextureResourceState_t>(aTexture.readableName, aResourceManager, aVulkanEnvironment);
                    if( not success )
                    {
                        return EEngineStatus::Ok;
                    }
                    sampledImageOpt = resource;
                }
                TextureResourceState_t &sampledImage = *sampledImageOpt;
                VkImage image = sampledImage.gpuApiHandles.imageHandle;

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
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            auto copyImageToBackBuffer(Shared<CVulkanEnvironment>           aVulkanEnvironment
                                     , Shared<CResourceManager>             aResourceManager
                                     , Shared<asset::CAssetStorage>         aAssetStorage
                                     , SFrameGraphRenderContextState const &aState
                                     , SFrameGraphDynamicTexture     const &aSourceImageId) -> EEngineStatus
            {
                SVulkanState &state = aVulkanEnvironment->getState();

                VkCommandBuffer commandBuffer  = aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer();
                VkImage         swapChainImage = state.swapChain.swapChainImages.at(state.swapChain.currentSwapChainImageIndex);

                auto const [success, resource] = fetchResource<TextureResourceState_t>(aSourceImageId.readableName, aResourceManager, aVulkanEnvironment);
                if(not success)
                {
                    return EEngineStatus::Error;
                }

                TextureResourceState_t &texture = *resource;

                VkImage image = texture.gpuApiHandles.imageHandle;

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
                vkCmdPipelineBarrier(commandBuffer,
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

                vkCmdCopyImage(commandBuffer,
                               image,
                               VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                               swapChainImage,
                               VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                               1,
                               &vkRegion);

                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            auto beginFrameCommandBuffers(Shared<CVulkanEnvironment>           aVulkanEnvironment
                                        , Shared<CResourceManager>             aResourceManager
                                        , Shared<asset::CAssetStorage>         aAssetStorage
                                        , SFrameGraphRenderContextState const &aState) -> EEngineStatus
            {
                SVulkanState &state = aVulkanEnvironment->getState();

                auto const begin = [&] (VkCommandBuffer const &buffer) -> void
                {
                    VkCommandBufferBeginInfo vkCommandBufferBeginInfo = {};
                    vkCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                    vkCommandBufferBeginInfo.pNext = nullptr;
                    vkCommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

                    VkResult const result = vkBeginCommandBuffer(buffer, &vkCommandBufferBeginInfo); // The command structure potentially changes. Recreate always.
                    if(VkResult::VK_SUCCESS != result)
                    {
                        throw CVulkanError("Failed to begin command buffer.", result);
                    }
                };

                VkCommandBuffer transferCommandBuffer = aVulkanEnvironment->getVkCurrentFrameContext()->getTransferCommandBuffer();
                VkCommandBuffer graphicsCommandBuffer = aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer();

                begin(transferCommandBuffer);
                begin(graphicsCommandBuffer);

                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            auto endFrameCommandBuffers(Shared<CVulkanEnvironment>           aVulkanEnvironment
                                      , Shared<CResourceManager>             aResourceManager
                                      , Shared<asset::CAssetStorage>         aAssetStorage
                                      , SFrameGraphRenderContextState const &aState) -> EEngineStatus
            {
                SVulkanState &vkState = aVulkanEnvironment->getState();

                VkCommandBuffer transferCommandBuffer = aVulkanEnvironment->getVkCurrentFrameContext()->getTransferCommandBuffer();
                VkCommandBuffer graphicsCommandBuffer = aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer();

                VkImage swapChainImage = vkState.swapChain.swapChainImages.at(vkState.swapChain.currentSwapChainImageIndex);

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
                vkCmdPipelineBarrier(graphicsCommandBuffer,
                                     VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
                                     VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
                                     VkDependencyFlagBits   ::VK_DEPENDENCY_BY_REGION_BIT,
                                     0, nullptr,
                                     0, nullptr,
                                     1, &vkImageMemoryBarrier);

                auto const end = [&] (VkCommandBuffer const &buffer) -> void
                {
                    VkResult const result = vkEndCommandBuffer(buffer); // The commandbuffers and swapchain count currently match
                    if(VkResult::VK_SUCCESS != result)
                    {
                        throw CVulkanError("Failed to record and commit command buffer.", result);
                    }
                };

                 end(graphicsCommandBuffer);
                 end(transferCommandBuffer);

                 return EEngineStatus::Ok;
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            auto bindRenderPass(Shared<CVulkanEnvironment>           aVulkanEnvironment
                              , Shared<CResourceManager>             aResourceManager
                              , Shared<asset::CAssetStorage>         aAssetStorage
                              , SFrameGraphRenderContextState const &aRenderContextState
                              , ResourceId_t                  const &aRenderPassId
                              , ResourceId_t                  const &aFrameBufferId
                              , CFrameGraphMutableResources   const &aFrameGraphResources ) -> EEngineStatus
            {
                OptRef_t<RenderPassResourceState_t>  renderPassResource {};
                OptRef_t<FrameBufferResourceState_t> frameBufferResource {};

                {
                    auto [success, resource] = fetchResource<RenderPassResourceState_t>(aRenderPassId, aResourceManager, aVulkanEnvironment);
                    if (not success)
                    {
                        return EEngineStatus::Error;
                    }
                    renderPassResource = resource;
                }

                {
                    auto [success, resource] = fetchResource<FrameBufferResourceState_t>(aFrameBufferId, aResourceManager, aVulkanEnvironment);
                    if(not success)
                    {
                        return EEngineStatus::Error;
                    }
                    frameBufferResource = resource;
                }

                RenderPassResourceState_t  renderPass  = *renderPassResource;
                FrameBufferResourceState_t frameBuffer = *frameBufferResource;

                SVulkanState                 &state                 = aVulkanEnvironment->getState();
                SRenderPassDescription const &renderPassDescription = renderPass.description;

                std::vector<VkClearValue> clearValues {};
                clearValues.resize(renderPassDescription.attachmentDescriptions.size());
                for(std::size_t k=0; k<renderPassDescription.attachmentDescriptions.size(); ++k)
                {
                    SAttachmentDescription const &desc = renderPassDescription.attachmentDescriptions[k];
                    clearValues.push_back(desc.clearColor);
                }

                VkRenderPassBeginInfo vkRenderPassBeginInfo {};
                vkRenderPassBeginInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                vkRenderPassBeginInfo.pNext             = nullptr;
                vkRenderPassBeginInfo.renderPass        = renderPass.gpuApiHandles.handle;
                vkRenderPassBeginInfo.framebuffer       = frameBuffer.gpuApiHandles.handle;
                vkRenderPassBeginInfo.renderArea.offset = { 0, 0 };
                vkRenderPassBeginInfo.renderArea.extent = state.swapChain.selectedExtents;
                vkRenderPassBeginInfo.clearValueCount   = clearValues.size();
                vkRenderPassBeginInfo.pClearValues      = clearValues.data();

                vkCmdBeginRenderPass(aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer(), &vkRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            auto unbindRenderPass(Shared<CVulkanEnvironment>     aVulkanEnvironment
                                , Shared<CResourceManager>       aResourceManager
                                , Shared<asset::CAssetStorage>   aAssetStorage
                                , SFrameGraphRenderContextState &aState
                                , ResourceId_t            const &aFrameBufferId
                                , ResourceId_t            const &aRenderPassId) -> EEngineStatus
            {

                SHIRABE_UNUSED(aFrameBufferId);
                SHIRABE_UNUSED(aRenderPassId);

                vkCmdEndRenderPass(aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer());

                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            auto beginGraphicsFrame(Shared<CVulkanEnvironment>     aVulkanEnvironment
                                  , Shared<CResourceManager>       aResourceManager
                                  , Shared<asset::CAssetStorage>   aAssetStorage
                                  , SFrameGraphRenderContextState &aState) -> EEngineStatus
            {
                return aVulkanEnvironment->beginGraphicsFrame().result();
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            auto endGraphicsFrame(Shared<CVulkanEnvironment>     aVulkanEnvironment
                                , Shared<CResourceManager>       aResourceManager
                                , Shared<asset::CAssetStorage>   aAssetStorage
                                , SFrameGraphRenderContextState &aState) -> EEngineStatus
            {
                return aVulkanEnvironment->endGraphicsFrame().result();
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            auto present(Shared<CVulkanEnvironment>     aVulkanEnvironment
                       , Shared<CResourceManager>       aResourceManager
                       , Shared<asset::CAssetStorage>   aAssetStorage
                       , SFrameGraphRenderContextState &aState) -> EEngineStatus
            {
                SVulkanState &vkState = aVulkanEnvironment->getState();

                VkQueue transferQueue = aVulkanEnvironment->getTransferQueue();
                VkQueue graphicsQueue = aVulkanEnvironment->getGraphicsQueue();
                VkQueue presentQueue  = aVulkanEnvironment->getPresentQueue();

                Shared<IVkFrameContext> const &frameContext = aVulkanEnvironment->getVkCurrentFrameContext();

                VkCommandBuffer const &transferCommandBuffer      = frameContext->getTransferCommandBuffer();
                VkCommandBuffer const &graphicsCommandBuffer      = frameContext->getGraphicsCommandBuffer();
                VkSemaphore     const &imageAvailableSemaphore    = frameContext->getImageAvailableSemaphore();
                VkSemaphore     const &transferCompletedSemaphore = frameContext->getTransferCompletedSemaphore();
                VkSemaphore     const &renderCompletedSemaphore   = frameContext->getRenderCompletedSemaphore();

                VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT };

                {
                    VkSemaphore    waitSemaphores[]   = { imageAvailableSemaphore };
                    VkSemaphore    signalSemaphores[] = { transferCompletedSemaphore };

                    VkSubmitInfo vkSubmitInfo {};
                    vkSubmitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                    vkSubmitInfo.pNext                = nullptr;
                    vkSubmitInfo.waitSemaphoreCount   = 1;
                    vkSubmitInfo.pWaitSemaphores      = waitSemaphores;
                    vkSubmitInfo.pWaitDstStageMask    = waitStages;
                    vkSubmitInfo.commandBufferCount   = 1;
                    vkSubmitInfo.pCommandBuffers      = &transferCommandBuffer;
                    vkSubmitInfo.signalSemaphoreCount = 1;
                    vkSubmitInfo.pSignalSemaphores    = signalSemaphores;

                    VkResult result = vkQueueSubmit(transferQueue, 1, &vkSubmitInfo, VK_NULL_HANDLE);
                    if(VkResult::VK_SUCCESS != result)
                    {
                        throw CVulkanError("Failed to execute 'vkQueueSubmit' on transfer queue", result);
                    }
                }

                {
                    VkSemaphore    waitSemaphores[]   = { transferCompletedSemaphore };
                    VkSemaphore    signalSemaphores[] = { renderCompletedSemaphore  };

                    VkSubmitInfo vkSubmitInfo {};
                    vkSubmitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                    vkSubmitInfo.pNext                = nullptr;
                    vkSubmitInfo.waitSemaphoreCount   = 1;
                    vkSubmitInfo.pWaitSemaphores      = waitSemaphores;
                    vkSubmitInfo.pWaitDstStageMask    = waitStages;
                    vkSubmitInfo.commandBufferCount   = 1;
                    vkSubmitInfo.pCommandBuffers      = &graphicsCommandBuffer;
                    vkSubmitInfo.signalSemaphoreCount = 1;
                    vkSubmitInfo.pSignalSemaphores    = signalSemaphores;

                    VkResult result = vkQueueSubmit(graphicsQueue, 1, &vkSubmitInfo, VK_NULL_HANDLE);
                    if(VkResult::VK_SUCCESS != result)
                    {
                        throw CVulkanError("Failed to execute 'vkQueueSubmit' on graphics queueu", result);
                    }
                }

                {
                    VkSwapchainKHR swapChains[]       = { vkState.swapChain.handle };
                    VkSemaphore    waitSemaphores[]   = { renderCompletedSemaphore };

                    VkPresentInfoKHR vkPresentInfo {};
                    vkPresentInfo.sType              =  VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
                    vkPresentInfo.pNext              =  nullptr;
                    vkPresentInfo.waitSemaphoreCount =  1;
                    vkPresentInfo.pWaitSemaphores    =  waitSemaphores;
                    vkPresentInfo.swapchainCount     =  1;
                    vkPresentInfo.pSwapchains        =  swapChains;
                    vkPresentInfo.pImageIndices      = &(vkState.swapChain.currentSwapChainImageIndex);
                    vkPresentInfo.pResults           =  nullptr;

                    VkResult result = vkQueuePresentKHR(presentQueue, &vkPresentInfo);
                    switch(result)
                    {
                        case VkResult::VK_ERROR_OUT_OF_DATE_KHR:
                        case VkResult::VK_SUBOPTIMAL_KHR:
                            aVulkanEnvironment->recreateSwapChain();
                            break;
                        case VkResult::VK_SUCCESS:
                            break;
                        default:
                            throw CVulkanError("Failed to execute 'vkQueuePresentKHR'", result);
                    }
                }

                // Temporary workaround to avoid memory depletion from GPU workloads using validation layers.
                // Implement better synchronization and throttling, once ready.
                VkResult result = vkQueueWaitIdle(presentQueue);
                if(VK_SUCCESS != result)
                {
                    // throw CVulkanError("Failed to execute 'vkQueueWaitIdle' for temporary synchronization implementation", result);
                }

                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto useMesh(Shared<CVulkanEnvironment>     aVulkanEnvironment
                       , Shared<CResourceManager>       aResourceManager
                       , Shared<asset::CAssetStorage>   aAssetStorage
                       , SFrameGraphRenderContextState       &aState
                       , SFrameGraphMesh               const &aMesh) -> EEngineStatus
            {
                SVulkanState     &vkState        = aVulkanEnvironment->getState();
                VkCommandBuffer  vkCommandBuffer = aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer();

                std::string const dataBufferId  = fmt::format("{}_{}", aMesh.readableName, "databuffer");
                std::string const indexBufferId = fmt::format("{}_{}", aMesh.readableName, "indexbuffer");

                OptRef_t<MeshResourceState_t> meshOpt {};
                {
                    auto [success, resource] = fetchResource<MeshResourceState_t>(aMesh.readableName, aResourceManager, aVulkanEnvironment);
                    if(not success)
                    {
                        return EEngineStatus::Ok;
                    }
                    meshOpt = resource;
                }
                MeshResourceState_t &mesh = *meshOpt;

                OptRef_t<BufferResourceState_t> dataBufferOpt {};
                {
                    auto [success, resource] = fetchResource<BufferResourceState_t>(dataBufferId, aResourceManager, aVulkanEnvironment);
                    if(not success)
                    {
                        return EEngineStatus::Ok;
                    }
                    dataBufferOpt = resource;
                }
                BufferResourceState_t & dataBuffer = *dataBufferOpt;

                OptRef_t<BufferResourceState_t> indexBufferOpt {};
                {
                    auto [success, resource] = fetchResource<BufferResourceState_t>(indexBufferId, aResourceManager, aVulkanEnvironment);
                    if(not success)
                    {
                        return EEngineStatus::Ok;
                    }
                    indexBufferOpt = resource;
                }
                BufferResourceState_t &indexBuffer = *indexBufferOpt;

                auto const &[attrBufferInit, attributeBufferResource] = aResourceManager->getResource<BufferResourceState_t>(dataBufferId, aVulkanEnvironment);
                EngineStatusPrintOnError(attrBufferInit, logTag(), "Failed to initialize attribute buffer.");
                SHIRABE_RETURN_RESULT_ON_ERROR(attrBufferInit);

                auto const &[indexBufferInit, indexBufferResource] = aResourceManager->getResource<BufferResourceState_t>(indexBufferId, aVulkanEnvironment);
                EngineStatusPrintOnError(indexBufferInit, logTag(), "Failed to initialize indexbuffer.");
                SHIRABE_RETURN_RESULT_ON_ERROR(indexBufferInit);

                transferBufferData(aVulkanEnvironment->getLogicalDevice(), dataBuffer.description.dataSource(),  dataBuffer.gpuApiHandles.attachedMemory);
                transferBufferData(aVulkanEnvironment->getLogicalDevice(), indexBuffer.description.dataSource(), indexBuffer.gpuApiHandles.attachedMemory);

                std::vector<VkBuffer> buffers = { dataBuffer.gpuApiHandles.handle
                                                  , dataBuffer.gpuApiHandles.handle
                                                  , dataBuffer.gpuApiHandles.handle
                                                  , dataBuffer.gpuApiHandles.handle };

                vkCmdBindVertexBuffers(vkCommandBuffer, 0, buffers.size(), buffers.data(), aOffsets.data());
                vkCmdBindIndexBuffer(vkCommandBuffer, indexBuffer.gpuApiHandles.handle, 0, VkIndexType::VK_INDEX_TYPE_UINT16);

                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto useMaterialWithPipeline(Shared<CVulkanEnvironment>     aVulkanEnvironment
                                       , Shared<CResourceManager>       aResourceManager
                                       , Shared<asset::CAssetStorage>   aAssetStorage
                                       , SFrameGraphRenderContextState   &aState
                                       , SFrameGraphMaterial       const &aMaterial
                                       , SFrameGraphPipelineConfig const &aPipelineConfig) -> EEngineStatus
            {
                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            struct SSampledImageBinding
            {
                OptRef_t<TextureViewResourceState_t> imageView;
                OptRef_t<TextureResourceState_t>     image;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto updateDescriptorSets(Shared<CVulkanEnvironment>                               aVulkanEnvironment
                                    , Shared<CResourceManager>                                 aResourceManager
                                    , Shared<asset::CAssetStorage>                             aAssetStorage
                                    , SFrameGraphRenderContextState                     const &aState
                                    , MaterialResourceState_t                           const &aMaterialHandle
                                    , std::vector<OptRef_t<BufferResourceState_t>>      const &aUniformBufferStates
                                    , std::vector<OptRef_t<TextureViewResourceState_t>> const &aInputAttachmentStates
                                    , std::vector<SSampledImageBinding>                 const &aTextureViewStates) -> EEngineStatus
            {


                VkDevice      device = aVulkanEnvironment->getLogicalDevice();
                SVulkanState &state  = aVulkanEnvironment->getState();

                VkCommandBuffer commandBuffer  = aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer();

                auto const [success, resource] = fetchResource<PipelineResourceState_t>(aMaterialHandle.description.pipelineDescriptor.name, aResourceManager, aVulkanEnvironment);
                if(not success)
                {
                    return EEngineStatus::Error;
                }

                PipelineResourceState_t &pipeline = *resource;

                SMaterialPipelineDescriptor const &pipelineDescriptor = pipeline.description;

                std::vector<VkWriteDescriptorSet>   descriptorSetWrites {};
                std::vector<VkDescriptorBufferInfo> descriptorSetWriteBufferInfos {};
                std::vector<VkDescriptorImageInfo>  descriptorSetWriteAttachmentImageInfos {};
                std::vector<VkDescriptorImageInfo>  descriptorSetWriteImageInfos {};

                descriptorSetWriteBufferInfos         .resize(aUniformBufferStates.size());
                descriptorSetWriteAttachmentImageInfos.resize(aInputAttachmentStates.size());
                descriptorSetWriteImageInfos          .resize(aTextureViewStates.size());

                uint64_t        writeCounter           = 0;
                uint64_t        bufferCounter          = 0;
                uint64_t        inputAttachmentCounter = 0;
                uint64_t        inputImageCounter      = 0;
                uint64_t const startSetIndex = (pipelineDescriptor.includesSystemBuffers ? 0 : 2); // Set 0 and 1 are system buffers...

                for(std::size_t k=0; k<pipelineDescriptor.descriptorSetLayoutBindings.size(); ++k)
                {
                    std::vector<VkDescriptorSetLayoutBinding> const setBindings  = pipelineDescriptor.descriptorSetLayoutBindings[k];
                    for(std::size_t j=0; j<setBindings.size(); ++j)
                    {
                        VkDescriptorSetLayoutBinding const binding = setBindings[j];

                        switch(binding.descriptorType)
                        {
                            case VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                            {
                                BufferResourceState_t buffer = *(aUniformBufferStates[bufferCounter]);

                                VkDescriptorBufferInfo bufferInfo = {};
                                bufferInfo.buffer = buffer.gpuApiHandles.handle;
                                bufferInfo.offset = 0;
                                bufferInfo.range  = buffer.description.createInfo.size;
                                descriptorSetWriteBufferInfos[bufferCounter] = bufferInfo;

                                VkWriteDescriptorSet descriptorWrite = {};
                                descriptorWrite.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                                descriptorWrite.pNext            = nullptr;
                                descriptorWrite.descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                                descriptorWrite.dstSet           = pipeline.gpuApiHandles.descriptorSets[startSetIndex + k];
                                descriptorWrite.dstBinding       = binding.binding;
                                descriptorWrite.dstArrayElement  = 0;
                                descriptorWrite.descriptorCount  = 1; // We only update one descriptor, i.e. pBufferInfo.count;
                                descriptorWrite.pBufferInfo      = &(descriptorSetWriteBufferInfos[bufferCounter++]);
                                descriptorWrite.pImageInfo       = nullptr; // Optional
                                descriptorWrite.pTexelBufferView = nullptr;

                                descriptorSetWrites.push_back(descriptorWrite);
                                //descriptorSetWrites[writeCounter++] = descriptorWrite;
                            }
                                break;
                            case VkDescriptorType::VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
                            {
                                TextureViewResourceState_t &textureView = *(aInputAttachmentStates[inputAttachmentCounter]);

                                VkDescriptorImageInfo imageInfo {};
                                imageInfo.imageView   = textureView.gpuApiHandles.handle;
                                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                                imageInfo.sampler     = VK_NULL_HANDLE;
                                descriptorSetWriteAttachmentImageInfos[inputAttachmentCounter] = imageInfo;

                                VkWriteDescriptorSet descriptorWrite = {};
                                descriptorWrite.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                                descriptorWrite.pNext            = nullptr;
                                descriptorWrite.descriptorType   = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
                                descriptorWrite.dstSet           = pipeline.gpuApiHandles.descriptorSets[startSetIndex + k];
                                descriptorWrite.dstBinding       = binding.binding;
                                descriptorWrite.dstArrayElement  = 0;
                                descriptorWrite.descriptorCount  = 1; // We only update one descriptor, i.e. pBufferInfo.count;
                                descriptorWrite.pBufferInfo      = nullptr;
                                descriptorWrite.pImageInfo       = &(descriptorSetWriteAttachmentImageInfos[inputAttachmentCounter++]); // Optional
                                descriptorWrite.pTexelBufferView = nullptr;
                                // descriptorSetWrites[writeCounter++] = descriptorWrite;
                                descriptorSetWrites.push_back(descriptorWrite);
                            }
                                break;
                            case VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
                            {
                                if(aTextureViewStates.size() <= inputImageCounter)
                                {
                                    continue;
                                }

                                auto const &imageBinding = aTextureViewStates[inputImageCounter];

                                if(not (imageBinding.image.has_value() && imageBinding.imageView.has_value()))
                                {
                                    continue;
                                }

                                TextureViewResourceState_t &textureView = *(imageBinding.imageView);
                                TextureResourceState_t     &texture     = *(imageBinding.image);

                                VkDescriptorImageInfo imageInfo {};
                                imageInfo.imageView   = textureView.gpuApiHandles.handle;
                                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                                imageInfo.sampler     = texture.gpuApiHandles.attachedSampler;
                                descriptorSetWriteImageInfos[inputImageCounter] = imageInfo;

                                VkWriteDescriptorSet descriptorWrite = {};
                                descriptorWrite.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                                descriptorWrite.pNext            = nullptr;
                                descriptorWrite.descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                                descriptorWrite.dstSet           = pipeline.gpuApiHandles.descriptorSets[startSetIndex + k];
                                descriptorWrite.dstBinding       = binding.binding;
                                descriptorWrite.dstArrayElement  = 0;
                                descriptorWrite.descriptorCount  = 1; // We only update one descriptor, i.e. pBufferInfo.count;
                                descriptorWrite.pBufferInfo      = nullptr;
                                descriptorWrite.pImageInfo       = &(descriptorSetWriteImageInfos[inputImageCounter++]); // Optional
                                descriptorWrite.pTexelBufferView = nullptr;
                                // descriptorSetWrites[writeCounter++] = descriptorWrite;
                                descriptorSetWrites.push_back(descriptorWrite);
                            }
                                break;
                            default:
                                break;
                        }
                    }
                }

                vkUpdateDescriptorSets(device, descriptorSetWrites.size(), descriptorSetWrites.data(), 0, nullptr);
                return EEngineStatus::Ok;
            };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        auto const bindPipeline = [=] (SFrameGraphRenderContextState &aState
                                       , PipelineResourceState_t       &aPipeline) -> EEngineStatus
            {


            SVulkanState     &vkState        = aVulkanEnvironment->getState();
            VkCommandBuffer  vkCommandBuffer = aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer(); // The commandbuffers and swapchain count currently match

            vkCmdBindPipeline(vkCommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, aPipeline.gpuApiHandles.pipeline);

            vkCmdBindDescriptorSets(vkCommandBuffer
                                    , VK_PIPELINE_BIND_POINT_GRAPHICS
                                    , aPipeline.gpuApiHandles.pipelineLayout
                                    , 0
                                    , aPipeline.gpuApiHandles.descriptorSets.size()
                                    , aPipeline.gpuApiHandles.descriptorSets.data()
                                    , 0, nullptr);

            return EEngineStatus::Ok;
            };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        context.unbindPipeline = [=] (SFrameGraphRenderContextState       &aState
                                      , SFrameGraphPipeline           const &aPipeline) -> EEngineStatus
            {
            SHIRABE_UNUSED(aPipeline);

            return EEngineStatus::Ok;
            };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        context.drawIndexed = [=] (SFrameGraphRenderContextState       &aState
                                   , VkDeviceSize                  const  aIndexCount) -> EEngineStatus
            {
            SVulkanState     &vkState        = aVulkanEnvironment->getState();
            VkCommandBuffer  vkCommandBuffer = aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer(); // The commandbuffers and swapchain count currently match

            vkCmdDrawIndexed(vkCommandBuffer, aIndexCount, 1, 0, 0, 0);

            return EEngineStatus::Ok;
            };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        context.drawQuad = [=] (SFrameGraphRenderContextState &aState) -> EEngineStatus
            {
            SVulkanState     &vkState        = aVulkanEnvironment->getState();
            VkCommandBuffer  vkCommandBuffer = aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer(); // The commandbuffers and swapchain count currently match

            vkCmdDraw(vkCommandBuffer, 6, 1, 0, 0);

            return EEngineStatus::Ok;
            };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        context.drawFullscreenQuadWithMaterial = [=] (SFrameGraphRenderContextState       &aState
                                                      , SFrameGraphMaterial           const &aMaterial) -> EEngineStatus
            {
            //context.bindMaterial(aState, aMaterial);
            context.drawQuad(aState);

            return EEngineStatus::Ok;
            };

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        framegraph::SFrameGraphRenderContext CreateRenderContextForVulkan(Shared<CVulkanEnvironment>   aVulkanEnvironment
                                                                        , Shared<CResourceManager>     aResourceManager
                                                                        , Shared<asset::CAssetStorage> aAssetStorage)
        {
            using namespace local;
            using namespace resources;

            framegraph::SFrameGraphRenderContext context {};

            context.clearAttachments  = std::bind([&] () -> EEngineStatus
                { return detail::clearAttachments(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                , std::placeholders::_1
                                                , std::placeholders::_2); });

            context.endPass = std::bind([&] () -> EEngineStatus
                { return detail::endPass(aVulkanEnvironment, aResourceManager, aAssetStorage
                                       , std::placeholders::_1); });

            context.copyImage = std::bind([&] () -> EEngineStatus
                { return detail::copyImage(aVulkanEnvironment, aResourceManager, aAssetStorage
                                         , std::placeholders::_1
                                         , std::placeholders::_2
                                         , std::placeholders::_3); });

            context.performImageLayoutTransfer = std::bind([&] () -> EEngineStatus
                { return detail::performImageLayoutTransfer(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                          , std::placeholders::_1
                                                          , std::placeholders::_2
                                                          , std::placeholders::_3
                                                          , std::placeholders::_4
                                                          , std::placeholders::_5
                                                          , std::placeholders::_6
                                                          , std::placeholders::_7); });

            context.copyImageToBackBuffer = std::bind([&] () -> EEngineStatus
                { return detail::copyImageToBackBuffer(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                     , std::placeholders::_1
                                                     , std::placeholders::_2); });

            context.beginFrameCommandBuffers = std::bind([&] () -> EEngineStatus
                { return detail::beginFrameCommandBuffers(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                        , std::placeholders::_1); });

            context.beginFrameCommandBuffers = std::bind([&] () -> EEngineStatus
                { return detail::endFrameCommandBuffers(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                      , std::placeholders::_1); });

            context.bindRenderPass = std::bind([&] () -> EEngineStatus
                { return detail::bindRenderPass(aVulkanEnvironment, aResourceManager, aAssetStorage
                                              , std::placeholders::_1
                                              , std::placeholders::_2
                                              , std::placeholders::_3
                                              , std::placeholders::_4); });

            context.unbindRenderPass = std::bind([&] () -> EEngineStatus
               { return detail::unbindRenderPass(aVulkanEnvironment, aResourceManager, aAssetStorage
                                               , std::placeholders::_1
                                               , std::placeholders::_2
                                               , std::placeholders::_3); });

            context.beginGraphicsFrame = std::bind([&] () -> EEngineStatus
               { return detail::beginGraphicsFrame(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                 , std::placeholders::_1); });

            context.endGraphicsFrame = std::bind([&] () -> EEngineStatus
               { return detail::endGraphicsFrame(aVulkanEnvironment, aResourceManager, aAssetStorage
                                               , std::placeholders::_1); });

            context.present = std::bind([&] () -> EEngineStatus
                { return detail::endGraphicsFrame(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                , std::placeholders::_1);

            context.useMesh = std::bind([&] () -> EEngineStatus
                { return detail::useMesh(aVulkanEnvironment, aResourceManager, aAssetStorage
                                       , std::placeholders::_1
                                       , std::placeholders::_2);

            context.useMaterialWithPipeline = std::bind([&] () -> EEngineStatus
                { return detail::useMaterialWithPipeline(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                       , std::placeholders::_1
                                                       , std::placeholders::_2
                                                       , std::placeholders::_3);
            //<-----------------------------------------------------------------------------
        }
        //<-----------------------------------------------------------------------------
    }
}
