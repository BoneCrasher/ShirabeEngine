//
// Created by dottideveloper on 28.12.21.
//

#include "rhi/vulkan_core/vulkanenvironment.h"
#include "rhi/vulkan_core/vulkanenvironmenttypes.h"
#include "rhi/vulkan_resources/resources/ivkglobalcontext.h"
#include "rhi/vulkan_rendering/rendering/vulkan_rendercontext.h"

namespace engine::rhi
{
    using namespace vulkan;

    auto clearAttachments(Shared<rhi::IVkGlobalContext>         aVulkanEnvironment
                          , Shared<CRHIResourceManager>       aResourceManager
                          , Shared<asset::CAssetStorage>   aAssetStorage
                          , std::string             const &aRenderPassId) -> EEngineStatus
    {
        SVulkanState    &state        = aVulkanEnvironment->getState();
        VkCommandBuffer commandBuffer = aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer();

        auto const [success, resource] = aResourceManager->getResource<RHIRenderPassResourceState_t>(aRenderPassId, aVulkanEnvironment);
        if(CheckEngineError(success))
        {
            return EEngineStatus::Error;
        }

        RHIRenderPassResourceState_t    &renderPass  = *resource;
        SRHIRenderPassDescription const &description = renderPass.rhiCreateDesc;

        std::vector<VkClearRect>       clearRects       {};
        std::vector<VkClearAttachment> clearAttachments {};

        SRHISubpassDescription const &subpassDesc = description.subpassDescriptions.at(aState.currentSubpassIndex);
        for(std::size_t k=0; k<subpassDesc.colorAttachments.size(); ++k)
        {
            SRHIAttachmentReference   const &ref  = subpassDesc.colorAttachments[k];
            SRHIAttachmentDescription const &desc = description.attachmentDescriptions[ref.attachment];

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
            SRHIAttachmentReference   const &ref  = subpassDesc.depthStencilAttachments[k];
            SRHIAttachmentDescription const &desc = description.attachmentDescriptions[ref.attachment];

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
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    auto nextSubpass(Shared<CVulkanEnvironment>     aVulkanEnvironment
                     , Shared<CRHIResourceManager>       aResourceManager
                     , Shared<asset::CAssetStorage>   aAssetStorage
                     , SRenderGraphRenderContextState &aState) -> EEngineStatus
    {
        SVulkanState    &state        = aVulkanEnvironment->getState();
        VkCommandBuffer commandBuffer = aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer();

        vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);
        ++(aState.currentSubpassIndex);

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    auto copyImage(Shared<CVulkanEnvironment>       aVulkanEnvironment
                   , Shared<CRHIResourceManager>         aResourceManager
                   , Shared<asset::CAssetStorage>     aAssetStorage
                   , SRenderGraphRenderContextState   &aState
                   , SRenderGraphImage const &aSourceImageId
                   , SRenderGraphImage const &aTargetImageId) -> EEngineStatus
    {
        SHIRABE_UNUSED(aSourceImageId);
        SHIRABE_UNUSED(aTargetImageId);

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    auto performImageLayoutTransfer(Shared<CVulkanEnvironment>       aVulkanEnvironment
                                    , Shared<CRHIResourceManager>         aResourceManager
                                    , Shared<asset::CAssetStorage>     aAssetStorage
                                    , SRenderGraphRenderContextState   &aState
                                    , SRenderGraphImage const &aTexture
                                    , CRange                    const &aArrayRange
                                    , CRange                    const &aMipRange
                                    , VkImageAspectFlags        const &aAspectFlags
                                    , VkImageLayout             const &aSourceLayout
                                    , VkImageLayout             const &aTargetLayout) -> EEngineStatus
    {
        VkCommandBuffer commandBuffer = aVulkanEnvironment->getVkCurrentFrameContext()->getTransferCommandBuffer();

        OptRef_t<RHIImageResourceState_t> sampledImageOpt{};
        {
            auto const [success, resource] = aResourceManager->getResource<RHIImageResourceState_t>(aTexture.readableName, aVulkanEnvironment);
            if(CheckEngineError(success))
            {
                return EEngineStatus::Ok;
            }
            sampledImageOpt = resource;
        }
        RHIImageResourceState_t &sampledImage = *sampledImageOpt;

        return __performImageLayoutTransfer(aVulkanEnvironment.get()
                                            , sampledImage.rhiHandles
                                            , aArrayRange
                                            , aMipRange
                                            , aAspectFlags
                                            , aSourceLayout
                                            , aTargetLayout);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    auto copyImageToBackBuffer(Shared<CVulkanEnvironment>           aVulkanEnvironment
                               , Shared<CRHIResourceManager>             aResourceManager
                               , Shared<asset::CAssetStorage>         aAssetStorage
                               , SRenderGraphRenderContextState const &aState
                               , SRenderGraphImage     const &aSourceImageId) -> EEngineStatus
    {
        SVulkanState &state = aVulkanEnvironment->getState();

        VkCommandBuffer commandBuffer  = aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer();
        VkImage         swapChainImage = state.swapChain.swapChainImages.at(state.swapChain.currentSwapChainImageIndex);

        auto const [success, resource] = aResourceManager->getResource<RHIImageResourceState_t>(aSourceImageId.readableName, aVulkanEnvironment);
        if(CheckEngineError(success))
        {
            return EEngineStatus::Error;
        }

        RHIImageResourceState_t &texture = *resource;

        VkImage image = texture.rhiHandles.imageHandle;

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
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    auto beginFrameCommandBuffers(Shared<CVulkanEnvironment>           aVulkanEnvironment
                                  , Shared<CRHIResourceManager>             aResourceManager
                                  , Shared<asset::CAssetStorage>         aAssetStorage
                                  , SRenderGraphRenderContextState const &aState) -> EEngineStatus
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
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    auto endFrameCommandBuffers(Shared<CVulkanEnvironment>           aVulkanEnvironment
                                , Shared<CRHIResourceManager>             aResourceManager
                                , Shared<asset::CAssetStorage>         aAssetStorage
                                , SRenderGraphRenderContextState const &aState) -> EEngineStatus
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
                        , Shared<CRHIResourceManager>             aResourceManager
                        , Shared<asset::CAssetStorage>         aAssetStorage
                        , SRenderGraphRenderContextState const &aRenderContextState
                        , ResourceId_t                  const &aRenderPassId
                        , ResourceId_t                  const &aFrameBufferId) -> EEngineStatus
    {
        OptRef_t<RHIRenderPassResourceState_t>  renderPassResource {};
        OptRef_t<RHIFrameBufferResourceState_t> frameBufferResource {};

        {
            auto const [success, resource] = aResourceManager->getResource<RHIRenderPassResourceState_t>(aRenderPassId, aVulkanEnvironment);
            if(CheckEngineError(success))
            {
                return EEngineStatus::Error;
            }
            renderPassResource = resource;
        }

        {
            auto const [success, resource] = aResourceManager->getResource<RHIFrameBufferResourceState_t>(aFrameBufferId, aVulkanEnvironment);
            if(CheckEngineError(success))
            {
                return EEngineStatus::Error;
            }
            frameBufferResource = resource;
        }

        RHIRenderPassResourceState_t  renderPass  = *renderPassResource;
        RHIFrameBufferResourceState_t frameBuffer = *frameBufferResource;

        SVulkanState                    &state                 = aVulkanEnvironment->getState();
        SRHIRenderPassDescription const &renderPassDescription = renderPass.rhiCreateDesc;

        std::vector<VkClearValue> clearValues {};
        clearValues.resize(renderPassDescription.attachmentDescriptions.size());
        for(std::size_t k=0; k<renderPassDescription.attachmentDescriptions.size(); ++k)
        {
            SRHIAttachmentDescription const &desc = renderPassDescription.attachmentDescriptions[k];
            clearValues.push_back(desc.clearColor);
        }

        VkRenderPassBeginInfo vkRenderPassBeginInfo {};
        vkRenderPassBeginInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        vkRenderPassBeginInfo.pNext             = nullptr;
        vkRenderPassBeginInfo.renderPass        = renderPass.rhiHandles.handle;
        vkRenderPassBeginInfo.framebuffer       = frameBuffer.rhiHandles.handle;
        vkRenderPassBeginInfo.renderArea.offset = { 0, 0 };
        vkRenderPassBeginInfo.renderArea.extent = state.swapChain.selectedExtents;
        vkRenderPassBeginInfo.clearValueCount   = clearValues.size();
        vkRenderPassBeginInfo.pClearValues      = clearValues.data();

        vkCmdBeginRenderPass(aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer(), &vkRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    auto unbindRenderPass(Shared<CVulkanEnvironment>     aVulkanEnvironment
                          , Shared<CRHIResourceManager>       aResourceManager
                          , Shared<asset::CAssetStorage>   aAssetStorage
                          , SRenderGraphRenderContextState &aState
                          , ResourceId_t            const &aFrameBufferId
                          , ResourceId_t            const &aRenderPassId) -> EEngineStatus
    {

        SHIRABE_UNUSED(aFrameBufferId);
        SHIRABE_UNUSED(aRenderPassId);

        vkCmdEndRenderPass(aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer());

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    auto beginGraphicsFrame(Shared<CVulkanEnvironment>     aVulkanEnvironment
                            , Shared<CRHIResourceManager>       aResourceManager
                            , Shared<asset::CAssetStorage>   aAssetStorage
                            , SRenderGraphRenderContextState &aState) -> EEngineStatus
    {
        return aVulkanEnvironment->beginGraphicsFrame().result();
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    auto endGraphicsFrame(Shared<CVulkanEnvironment>     aVulkanEnvironment
                          , Shared<CRHIResourceManager>       aResourceManager
                          , Shared<asset::CAssetStorage>   aAssetStorage
                          , SRenderGraphRenderContextState &aState) -> EEngineStatus
    {
        return aVulkanEnvironment->endGraphicsFrame().result();
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    auto present(Shared<CVulkanEnvironment>     aVulkanEnvironment
                 , Shared<CRHIResourceManager>       aResourceManager
                 , Shared<asset::CAssetStorage>   aAssetStorage
                 , SRenderGraphRenderContextState &aState) -> EEngineStatus
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
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    auto useMesh(Shared<CVulkanEnvironment>           aVulkanEnvironment
                 , Shared<CRHIResourceManager>             aResourceManager
                 , Shared<asset::CAssetStorage>         aAssetStorage
                 , SRenderGraphRenderContextState       &aState
                 , SRenderGraphMesh               const &aMesh) -> EEngineStatus
    {
        SVulkanState     &vkState        = aVulkanEnvironment->getState();
        VkCommandBuffer  vkCommandBuffer = aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer();

        OptRef_t<MeshResourceState_t> meshOpt {};
        {
            auto [success, resource] = aResourceManager->getResource<MeshResourceState_t>(aMesh.readableName, aVulkanEnvironment);
            if(CheckEngineError(success))
            {
                return EEngineStatus::Ok;
            }
            meshOpt = resource;
        }
        MeshResourceState_t &mesh = *meshOpt;

        std::string const dataBufferId         = mesh.rhiCreateDesc.name;
        VkDeviceSize      firstIndexByteOffset = mesh.rhiCreateDesc.meshInfo.firstIndexOffset;

        auto const &[attrBufferGet, attributeBufferResource] = aResourceManager->getResource<RHIBufferResourceState_t>(dataBufferId, aVulkanEnvironment);
        EngineStatusPrintOnError(attrBufferGet, logTag(), "Failed to get data buffer.");
        SHIRABE_RETURN_RESULT_ON_ERROR(attrBufferGet);

        RHIBufferResourceState_t const &dataBuffer = *attributeBufferResource;

        std::vector<VkBuffer> buffers = { dataBuffer.rhiHandles.buffer
                                          , dataBuffer.rhiHandles.buffer
                                          , dataBuffer.rhiHandles.buffer
                                          , dataBuffer.rhiHandles.buffer };

        vkCmdBindVertexBuffers(vkCommandBuffer, 0, buffers.size(), buffers.data(), mesh.rhiCreateDesc.meshInfo.offsets.data());
        vkCmdBindIndexBuffer(vkCommandBuffer, dataBuffer.rhiHandles.buffer, firstIndexByteOffset, VkIndexType::VK_INDEX_TYPE_UINT16);

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    auto useMaterialWithPipeline(Shared<CVulkanEnvironment>       aVulkanEnvironment
                                 , Shared<CRHIResourceManager>         aResourceManager
                                 , Shared<asset::CAssetStorage>     aAssetStorage
                                 , SRenderGraphRenderContextState   &aState
                                 , SRenderGraphMaterial       const &aMaterial
                                 , SRenderGraphPipeline       const &aPipeline) -> EEngineStatus
    {
        VkCommandBuffer vkCommandBuffer = aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer();

        auto const [pipelineLayoutSuccess, pipelineLayoutOpt] = aResourceManager->getResource<RHIPipelineLayoutResourceState_t>(aPipeline.pipelineResourceId, aVulkanEnvironment);
        if(CheckEngineError(pipelineLayoutSuccess))
        {
            return EEngineStatus::Error;
        }
        RHIPipelineLayoutResourceState_t &pipelineLayout = *pipelineLayoutOpt;

        auto const [pipelineSuccess, pipelineOpt] = aResourceManager->getResource<RHIPipelineResourceState_t>(aPipeline.readableName, aVulkanEnvironment);
        if(CheckEngineError(pipelineSuccess))
        {
            return EEngineStatus::Error;
        }
        RHIPipelineResourceState_t &pipeline = *pipelineOpt;

        auto const [materialSuccess, materialOpt] = aResourceManager->getResource<MaterialResourceState_t>(aMaterial.readableName, aVulkanEnvironment);
        if(CheckEngineError(materialSuccess))
        {
            return EEngineStatus::Error;
        }
        MaterialResourceState_t &material = *materialOpt;

        vkCmdBindDescriptorSets(vkCommandBuffer
                                , VK_PIPELINE_BIND_POINT_GRAPHICS
                                , pipelineLayout.rhiHandles.pipelineLayout
                                , 0
                                , material.rhiHandles.descriptorSets.size()
                                , material.rhiHandles.descriptorSets.data()
                                , 0, nullptr);

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    auto bindPipeline(Shared<CVulkanEnvironment>     aVulkanEnvironment
                      , Shared<CRHIResourceManager>       aResourceManager
                      , Shared<asset::CAssetStorage>   aAssetStorage
                      , SRenderGraphRenderContextState &aState
                      , SRenderGraphPipeline     const &aPipeline) -> EEngineStatus
    {
        SVulkanState     &vkState        = aVulkanEnvironment->getState();
        VkCommandBuffer  vkCommandBuffer = aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer(); // The commandbuffers and swapchain count currently match

        auto const [success, resource] = aResourceManager->getResource<RHIPipelineResourceState_t>(aPipeline.readableName, aVulkanEnvironment);
        if(CheckEngineError(success))
        {
            return EEngineStatus::Error;
        }
        RHIPipelineResourceState_t &pipeline = *resource;

        vkCmdBindPipeline(vkCommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.rhiHandles.pipeline);

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    auto drawIndexed(Shared<CVulkanEnvironment>           aVulkanEnvironment
                     , Shared<CRHIResourceManager>             aResourceManager
                     , Shared<asset::CAssetStorage>         aAssetStorage
                     , SRenderGraphRenderContextState       &aState
                     , VkDeviceSize                  const  aIndexCount) -> EEngineStatus
    {
        SVulkanState     &vkState        = aVulkanEnvironment->getState();
        VkCommandBuffer  vkCommandBuffer = aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer(); // The commandbuffers and swapchain count currently match

        vkCmdDrawIndexed(vkCommandBuffer, aIndexCount, 1, 0, 0, 0);

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    auto drawQuad(Shared<CVulkanEnvironment>     aVulkanEnvironment
                  , Shared<CRHIResourceManager>       aResourceManager
                  , Shared<asset::CAssetStorage>   aAssetStorage
                  , SRenderGraphRenderContextState &aState) -> EEngineStatus
    {
        SVulkanState     &vkState        = aVulkanEnvironment->getState();
        VkCommandBuffer  vkCommandBuffer = aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer(); // The commandbuffers and swapchain count currently match

        vkCmdDraw(vkCommandBuffer, 6, 1, 0, 0);

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    auto drawFullscreenQuadWithMaterial(Shared<CVulkanEnvironment>           aVulkanEnvironment
                                        , Shared<CRHIResourceManager>             aResourceManager
                                        , Shared<asset::CAssetStorage>         aAssetStorage
                                        , SRenderGraphRenderContextState       &aState
                                        , SRenderGraphMaterial           const &aMaterial) -> EEngineStatus
    {
        //context.bindMaterial(aState, aMaterial);
        return detail::drawQuad(aVulkanEnvironment, aResourceManager, aAssetStorage, aState);
    }
}
