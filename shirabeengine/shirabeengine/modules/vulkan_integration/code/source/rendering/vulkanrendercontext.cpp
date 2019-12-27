#include "vulkan_integration/rendering/vulkanrendercontext.h"
#include "vulkan_integration/resources/types/vulkantextureresource.h"
#include "vulkan_integration/resources/types/vulkanframebufferresource.h"
#include "vulkan_integration/resources/types/vulkanrenderpassresource.h"
#include "vulkan_integration/resources/types/vulkanmaterialpipelineresource.h"

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
        //
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanRenderContext::clearAttachments(GpuApiHandle_t const &aRenderPassId, uint32_t const &aCurrentSubpassIndex)
        {
            SVulkanState    &state        = mVulkanEnvironment->getState();
            VkCommandBuffer commandBuffer = state.commandBuffers.at(state.swapChain.currentSwapChainImageIndex);

            auto                   const *const renderPass = mResourceStorage->extract<CVulkanRenderPassResource>(aRenderPassId);
            SRenderPassDescription const       &description = *(renderPass->getCurrentDescriptor());

            std::vector<VkClearRect>       clearRects       {};
            std::vector<VkClearAttachment> clearAttachments {};

            SSubpassDescription const &subpassDesc = description.subpassDescriptions.at(aCurrentSubpassIndex);
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
                CLog::Error(logTag(), "Failed to fetch copy source image '{}'.", aSourceImageId);
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
        //
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanRenderContext::transferBufferData(ByteBuffer const &aDataSource, GpuApiHandle_t const &aGpuBufferHandle)
        {
            auto const *const gpuBuffer = mVulkanEnvironment->getResourceStorage()->extract<CVulkanBufferResource>(aGpuBufferHandle);
            if(nullptr == gpuBuffer)
            {
                return EEngineStatus::Error;
            }

            VkDevice device = mVulkanEnvironment->getLogicalDevice();

            VkDeviceSize     const offset = 0;
            VkDeviceSize     const size   = aDataSource.size();
            VkMemoryMapFlags const flags  = 0;

            void *data = nullptr;
            VkResult const result = vkMapMemory(device, gpuBuffer->attachedMemory, offset, size, flags, &data);
            if(VkResult::VK_SUCCESS != result)
            {
                CLog::Error(logTag(), "Failed to map vulkan buffer w/ handle {}", aGpuBufferHandle);
                return EEngineStatus::Error;
            }

            auto input = aDataSource.data();
            memcpy(data, (void*)input, size);
            vkUnmapMemory(device, gpuBuffer->attachedMemory);


            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanRenderContext::transferImageData(GpuApiHandle_t const &aTextureResourceHandle)
        {
            auto *const texture = mResourceStorage->extract<CVulkanTextureResource>(aTextureResourceHandle);
            return texture->transfer().result();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanRenderContext::updateResourceBindings(  GpuApiHandle_t const                    &aGpuMaterialHandle
                                                                   , std::vector<GpuApiHandle_t>       const &aGpuBufferHandles
                                                                   , std::vector<GpuApiHandle_t>       const &aGpuInputAttachmentTextureViewHandles
                                                                   , std::vector<SSampledImageBinding> const &aGpuTextureViewHandles)
        {
            VkDevice device = mVulkanEnvironment->getLogicalDevice();

            auto                       *const pipeline           = mResourceStorage->extract<CVulkanPipelineResource>(aGpuMaterialHandle);
            SMaterialPipelineDescriptor const pipelineDescriptor = *(pipeline->getCurrentDescriptor());

            std::vector<VkWriteDescriptorSet>   descriptorSetWrites {};
            std::vector<VkDescriptorBufferInfo> descriptorSetWriteBufferInfos {};
            std::vector<VkDescriptorImageInfo>  descriptorSetWriteAttachmentImageInfos {};
            std::vector<VkDescriptorImageInfo>  descriptorSetWriteImageInfos {};

            descriptorSetWriteBufferInfos         .resize(aGpuBufferHandles.size());
            descriptorSetWriteAttachmentImageInfos.resize(aGpuInputAttachmentTextureViewHandles.size());
            descriptorSetWriteImageInfos          .resize(aGpuTextureViewHandles.size());

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
                                auto const *const buffer = mResourceStorage->extract<CVulkanBufferResource>(aGpuBufferHandles[bufferCounter]);

                                VkDescriptorBufferInfo bufferInfo = {};
                                bufferInfo.buffer = buffer->handle;
                                bufferInfo.offset = 0;
                                bufferInfo.range  = buffer->getCurrentDescriptor()->createInfo.size;
                                descriptorSetWriteBufferInfos[bufferCounter] = bufferInfo;

                                VkWriteDescriptorSet descriptorWrite = {};
                                descriptorWrite.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                                descriptorWrite.pNext            = nullptr;
                                descriptorWrite.descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                                descriptorWrite.dstSet           = pipeline->descriptorSets[startSetIndex + k];
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
                                auto const *const view = mResourceStorage->extract<CVulkanTextureViewResource>(aGpuInputAttachmentTextureViewHandles[inputAttachmentCounter]);

                                VkDescriptorImageInfo imageInfo {};
                                imageInfo.imageView   = view->handle;
                                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                                imageInfo.sampler     = VK_NULL_HANDLE;
                                descriptorSetWriteAttachmentImageInfos[inputAttachmentCounter] = imageInfo;

                                VkWriteDescriptorSet descriptorWrite = {};
                                descriptorWrite.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                                descriptorWrite.pNext            = nullptr;
                                descriptorWrite.descriptorType   = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
                                descriptorWrite.dstSet           = pipeline->descriptorSets[startSetIndex + k];
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
                                auto const &imageBinding = aGpuTextureViewHandles[inputImageCounter];
                                auto const *const view  = mResourceStorage->extract<CVulkanTextureViewResource>(imageBinding.imageView);
                                auto const *const image = mResourceStorage->extract<CVulkanTextureResource>    (imageBinding.image);

                                VkDescriptorImageInfo imageInfo {};
                                imageInfo.imageView   = view->handle;
                                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                                imageInfo.sampler     = image->attachedSampler;
                                descriptorSetWriteAttachmentImageInfos[inputImageCounter] = imageInfo;

                                VkWriteDescriptorSet descriptorWrite = {};
                                descriptorWrite.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                                descriptorWrite.pNext            = nullptr;
                                descriptorWrite.descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                                descriptorWrite.dstSet           = pipeline->descriptorSets[startSetIndex + k];
                                descriptorWrite.dstBinding       = binding.binding;
                                descriptorWrite.dstArrayElement  = 0;
                                descriptorWrite.descriptorCount  = 1; // We only update one descriptor, i.e. pBufferInfo.count;
                                descriptorWrite.pBufferInfo      = nullptr;
                                descriptorWrite.pImageInfo       = &(descriptorSetWriteAttachmentImageInfos[inputImageCounter++]); // Optional
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
                CLog::Error(logTag(), "Failed to fetch frame buffer '{}'.", aFrameBufferId);
                return EEngineStatus::Error;
            }

            if(nullptr == renderPass)
            {
                CLog::Error(logTag(), "Failed to fetch render pass '{}'.", aRenderPassId);
                return EEngineStatus::Error;
            }

            SVulkanState &state = mVulkanEnvironment->getState();

            std::vector<VkClearValue> clearValues {};
            clearValues.resize(renderPass->getCurrentDescriptor()->attachmentDescriptions.size());
            for(std::size_t k=0; k<renderPass->getCurrentDescriptor()->attachmentDescriptions.size(); ++k)
            {
                SAttachmentDescription const &desc = renderPass->getCurrentDescriptor()->attachmentDescriptions[k];
                clearValues.push_back(desc.clearColor);
            }

            VkRenderPassBeginInfo vkRenderPassBeginInfo {};
            vkRenderPassBeginInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            vkRenderPassBeginInfo.pNext             = nullptr;
            vkRenderPassBeginInfo.renderPass        = renderPass->handle;
            vkRenderPassBeginInfo.framebuffer       = frameBuffer->handle;
            vkRenderPassBeginInfo.renderArea.offset = { 0, 0 };
            vkRenderPassBeginInfo.renderArea.extent = state.swapChain.selectedExtents;
            vkRenderPassBeginInfo.clearValueCount   = clearValues.size();
            vkRenderPassBeginInfo.pClearValues      = clearValues.data();

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
            //     CLog::Error(logTag(), "Failed to fetch frame buffer '{}'.", aFrameBufferId);
            //     return frameBufferFetch.result();
            // }
            //
            // CEngineResult<Shared<SVulkanRenderPassResource>> renderPassFetch = mGraphicsAPIResourceBackend->getResource<SVulkanRenderPassResource>(aRenderPassId);
            // if(not renderPassFetch.successful())
            // {
            //     CLog::Error(logTag(), "Failed to fetch render pass '{}'.", aRenderPassId);
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

            SVulkanState &vkState = mVulkanEnvironment->getState();

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

                switch(result)
                {
                    case VkResult::VK_ERROR_OUT_OF_DATE_KHR:
                    case VkResult::VK_SUBOPTIMAL_KHR:
                        mVulkanEnvironment->recreateSwapChain();
                        swapChain = vkState.swapChain.handle;
                        semaphore = vkState.swapChain.imageAvailableSemaphore;
                        break;
                    case VkResult::VK_SUCCESS:
                        break;
                    default:
                        throw CVulkanError("Failed to execute 'vkAcquireNextImageKHR'", result);
                }

            }
            while(VkResult::VK_SUCCESS != result);

            // if(VkResult::VK_SUCCESS != result)
            // {
            //    CLog::Error(logTag(), CString::format("AquireNextImageKHR failed with VkResult: {}", result));
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
            if(VkResult::VK_SUCCESS != result)
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
            switch(result)
            {
                case VkResult::VK_ERROR_OUT_OF_DATE_KHR:
                case VkResult::VK_SUBOPTIMAL_KHR:
                    mVulkanEnvironment->recreateSwapChain();
                    break;
                case VkResult::VK_SUCCESS:
                    break;
                default:
                    throw CVulkanError("Failed to execute 'vkQueuePresentKHR'", result);
            }

            // Temporary workaround to avoid memory depletion from GPU workloads using validation layers.
            // Implement better synchronization and throttling, once ready.
            result = vkQueueWaitIdle(presentQueue);
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
        EEngineStatus CVulkanRenderContext::bindAttributeAndIndexBuffers(GpuApiHandle_t const &aAttributeBufferId, GpuApiHandle_t const &aIndexBufferId, Vector<VkDeviceSize> aOffsets)
        {
            SVulkanState     &vkState        = mVulkanEnvironment->getState();
            VkCommandBuffer  vkCommandBuffer = vkState.commandBuffers.at(vkState.swapChain.currentSwapChainImageIndex);

            auto const *const attributeBuffer = mVulkanEnvironment->getResourceStorage()->extract<CVulkanBufferResource>(aAttributeBufferId);
            auto const *const indexBuffer     = mVulkanEnvironment->getResourceStorage()->extract<CVulkanBufferResource>(aIndexBufferId);

            if(nullptr == attributeBuffer || nullptr == indexBuffer)
            {
                CLog::Error(logTag(), "Failed to fetch attribute or indexbuffer '{}/{}'.", aAttributeBufferId, aIndexBufferId);
                return EEngineStatus::Error;
            }

            std::vector<VkBuffer> buffers = { attributeBuffer->handle, attributeBuffer->handle, attributeBuffer->handle, attributeBuffer->handle };

            vkCmdBindVertexBuffers(vkCommandBuffer, 0, buffers.size(), buffers.data(), aOffsets.data());
            vkCmdBindIndexBuffer(vkCommandBuffer, indexBuffer->handle, 0, VkIndexType::VK_INDEX_TYPE_UINT16);

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanRenderContext::bindPipeline(GpuApiHandle_t const &aPipelineUID)
        {
            SVulkanState     &vkState        = mVulkanEnvironment->getState();
            VkCommandBuffer  vkCommandBuffer = vkState.commandBuffers.at(vkState.swapChain.currentSwapChainImageIndex); // The commandbuffers and swapchain count currently match

            auto const *const pipeline = mVulkanEnvironment->getResourceStorage()->extract<CVulkanPipelineResource>(aPipelineUID);
            if(nullptr == pipeline)
            {
                CLog::Error(logTag(), "Failed to fetch pipeline '{}'.", aPipelineUID);
                return EEngineStatus::Error;
            }

            vkCmdBindPipeline(vkCommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline);

            vkCmdBindDescriptorSets(vkCommandBuffer
                    , VK_PIPELINE_BIND_POINT_GRAPHICS
                    , pipeline->pipelineLayout
                    , 0
                    , pipeline->descriptorSets.size()
                    , pipeline->descriptorSets.data()
                    , 0, nullptr);

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
            CLog::Verbose(logTag(), CString::format("Binding resource with id {}", aId));
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanRenderContext::unbindResource(GpuApiHandle_t const &aId)
        {
            CLog::Verbose(logTag(), CString::format("Unbinding resource with id {}", aId));
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanRenderContext::render(SRenderable const &aRenderable)
        {
            CLog::Verbose(logTag(), CString::format("Rendering renderable: {}", convert_to_string(aRenderable)));
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanRenderContext::drawIndex(uint32_t const aIndexCount)
        {
            SVulkanState     &vkState        = mVulkanEnvironment->getState();
            VkCommandBuffer  vkCommandBuffer = vkState.commandBuffers.at(vkState.swapChain.currentSwapChainImageIndex); // The commandbuffers and swapchain count currently match

            vkCmdDrawIndexed(vkCommandBuffer, aIndexCount, 1, 0, 0, 0);

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanRenderContext::drawQuad()
        {
            SVulkanState     &vkState        = mVulkanEnvironment->getState();
            VkCommandBuffer  vkCommandBuffer = vkState.commandBuffers.at(vkState.swapChain.currentSwapChainImageIndex); // The commandbuffers and swapchain count currently match

            vkCmdDraw(vkCommandBuffer, 6, 1, 0, 0);

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------
    }
}
