#include "vulkan_integration/rendering/vulkanrendercontext.h"
#include "vulkan_integration/resources/types/vulkantextureresource.h"
#include "vulkan_integration/resources/types/vulkanframebufferresource.h"
#include "vulkan_integration/resources/types/vulkanrenderpassresource.h"
#include "vulkan_integration/resources/types/vulkanmaterialpipelineresource.h"

#include <renderer/framegraph/framegraphrendercontext.h>
#include <resources/cresourcemanager.h>

#include <thread>
#include <base/string.h>

namespace engine
{
    namespace vulkan
    {
        using namespace framegraph;

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        EEngineStatus transferBufferData(VkDevice aDevice, ByteBuffer const &aDataSource, VkDeviceMemory const &aBufferMemory)
        {
            VkDeviceSize     const offset = 0;
            VkDeviceSize     const size   = aDataSource.size();
            VkMemoryMapFlags const flags  = 0;

            void *data = nullptr;
            VkResult const result = vkMapMemory(aDevice, aBufferMemory, offset, size, flags, &data);
            if(VkResult::VK_SUCCESS != result)
            {
                CLog::Error(logTag(), "Failed to map vulkan buffer w/ handle {}", aGpuBufferHandle);
                return EEngineStatus::Error;
            }

            if(nullptr != data) {
                auto input = aDataSource.data();
                memcpy(data, (void *) input, size);
                vkUnmapMemory(aDevice, aBufferMemory);
            }

            return EEngineStatus::Ok;
        }

        namespace local
        {
            SHIRABE_DECLARE_LOG_TAG(VulkanFrameGraphRenderContext);
        }

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        framegraph::SFrameGraphRenderContext CreateRenderContextForVulkan(Shared<CVulkanEnvironment> aVulkanEnvironment
                                                                        , Shared<CResourceManager>   aResourceManager)
        {
            using namespace local;

            framegraph::SFrameGraphRenderContext context {};

            context.clearAttachments = [=] (std::string const &aRenderPassId, uint32_t const &aCurrentSubpassIndex) -> EEngineStatus
            {
                SVulkanState    &state        = aVulkanEnvironment->getState();
                VkCommandBuffer commandBuffer = aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer();

                std::optional<SResourceState<SRenderPass>> resource = aResourceManager->getResource<SRenderPass>(aRenderPassId);
                if(not resource.has_value())
                {
                    return EEngineStatus::Error;
                }

                SRenderPass            const &renderPass  = resource->logicalResource;
                SRenderPassDescription const &description = renderPass.getDescription();

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
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.beginPass = [=] () -> EEngineStatus
            {
                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.endPass = [=] () -> EEngineStatus
            {
                SVulkanState    &state        = aVulkanEnvironment->getState();
                VkCommandBuffer commandBuffer = aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer();

                vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);

                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.copyImage = [=] (SFrameGraphTexture const &aSourceImageId,
                                     SFrameGraphTexture const &aTargetImageId) -> EEngineStatus
            {
                SHIRABE_UNUSED(aSourceImageId);
                SHIRABE_UNUSED(aTargetImageId);

                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.performImageLayoutTransfer = [=] (  SFrameGraphTexture const &aImageHandle
                                                      , CRange             const &aArrayRange
                                                      , CRange             const &aMipRange
                                                      , VkImageAspectFlags const &aAspectFlags
                                                      , VkImageLayout      const &aSourceLayout
                                                      , VkImageLayout      const &aTargetLayout) -> EEngineStatus
            {
                SVulkanState &state = aVulkanEnvironment->getState();

                VkCommandBuffer commandBuffer  = aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer();
                std::optional<SResourceState<STexture>> resource = aResourceManager->getResource<STexture>(aImageHandle.readableName);
                if(not resource.has_value())
                {
                    return EEngineStatus::Error;
                }

                STexture const &texture = resource->logicalResource;
                if(not resource->gpuApiResource)
                {
                    return EEngineStatus::Error;
                }

                VkImage image = std::static_pointer_cast<CVulkanTextureResource>(resource->gpuApiResource)->imageHandle;

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
        context.copyImageToBackBuffer = [=] (SFrameGraphTexture const &aSourceImageId) -> EEngineStatus
        {
            SVulkanState &state = aVulkanEnvironment->getState();

            VkCommandBuffer commandBuffer  = aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer();
            VkImage         swapChainImage = state.swapChain.swapChainImages.at(state.swapChain.currentSwapChainImageIndex);

            std::optional<SResourceState<STexture>> resource = aResourceManager->getResource<STexture>(aSourceImageId.readableName);
            if(not resource.has_value())
            {
                return EEngineStatus::Error;
            }

            STexture const &texture = resource->logicalResource;
            if(not resource->gpuApiResource)
            {
                return EEngineStatus::Error;
            }
            VkImage image = std::static_pointer_cast<CVulkanTextureResource>(resource->gpuApiResource)->imageHandle;

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
        //
        //<-----------------------------------------------------------------------------
        context.updateMaterial = [=] (  SFrameGraphMaterial                 const &aMaterialHandle
                                      , std::vector<SFrameGraphBuffer>      const &aGpuBufferHandles
                                      , std::vector<SFrameGraphTextureView> const &aGpuInputAttachmentTextureViewHandles
                                      , std::vector<SSampledImageBinding>   const &aGpuTextureViewHandles)
        {
            VkDevice device = aVulkanEnvironment->getLogicalDevice();

            SVulkanState &state = aVulkanEnvironment->getState();

            VkCommandBuffer commandBuffer  = aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer();

            std::optional<SResourceState<SPipeline>> resource = aResourceManager->getResource<SPipeline>(aMaterialHandle.readableName);
            if(not resource.has_value())
            {
                return EEngineStatus::Error;
            }

            if(not resource->gpuApiResource)
            {
                return EEngineStatus::Error;
            }

            SPipeline                       const &pipeline           = resource->logicalResource;
            Shared<CVulkanPipelineResource> const  gpuPipeline        = std::static_pointer_cast<CVulkanPipelineResource>(resource->gpuApiResource);
            SMaterialPipelineDescriptor     const &pipelineDescriptor = pipeline.getDescription();

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
                                auto const *const buffer = aVulkanEnvironment->getResourceStorage()->extract<CVulkanBufferResource>(aGpuBufferHandles[bufferCounter]);

                                VkDescriptorBufferInfo bufferInfo = {};
                                bufferInfo.buffer = buffer->handle;
                                bufferInfo.offset = 0;
                                bufferInfo.range  = buffer->getCurrentDescriptor()->createInfo.size;
                                descriptorSetWriteBufferInfos[bufferCounter] = bufferInfo;

                                VkWriteDescriptorSet descriptorWrite = {};
                                descriptorWrite.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                                descriptorWrite.pNext            = nullptr;
                                descriptorWrite.descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                                descriptorWrite.dstSet           = gpuPipeline->descriptorSets[startSetIndex + k];
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
                                auto const *const view = aVulkanEnvironment->getResourceStorage()->extract<CVulkanTextureViewResource>(aGpuInputAttachmentTextureViewHandles[inputAttachmentCounter]);

                                VkDescriptorImageInfo imageInfo {};
                                imageInfo.imageView   = view->handle;
                                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                                imageInfo.sampler     = VK_NULL_HANDLE;
                                descriptorSetWriteAttachmentImageInfos[inputAttachmentCounter] = imageInfo;

                                VkWriteDescriptorSet descriptorWrite = {};
                                descriptorWrite.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                                descriptorWrite.pNext            = nullptr;
                                descriptorWrite.descriptorType   = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
                                descriptorWrite.dstSet           = gpuPipeline->descriptorSets[startSetIndex + k];
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
                                if(aGpuTextureViewHandles.size() <= inputImageCounter)
                                {
                                    continue;
                                }

                                auto const &imageBinding = aGpuTextureViewHandles[inputImageCounter];

                                if(   (resources::GpuApiHandle_t)0 == imageBinding.imageView
                                   || (resources::GpuApiHandle_t)0 == imageBinding.image)
                                {
                                    continue;
                                }

                                auto const *const view  = aVulkanEnvironment->getResourceStorage()->extract<CVulkanTextureViewResource>(imageBinding.imageView);
                                auto const *const image = aVulkanEnvironment->getResourceStorage()->extract<CVulkanTextureResource>    (imageBinding.image);
                                if(nullptr == view || nullptr == image)
                                {
                                    continue;
                                }

                                VkDescriptorImageInfo imageInfo {};
                                imageInfo.imageView   = view->handle;
                                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                                imageInfo.sampler     = image->attachedSampler;
                                descriptorSetWriteImageInfos[inputImageCounter] = imageInfo;

                                VkWriteDescriptorSet descriptorWrite = {};
                                descriptorWrite.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                                descriptorWrite.pNext            = nullptr;
                                descriptorWrite.descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                                descriptorWrite.dstSet           = gpuPipeline->descriptorSets[startSetIndex + k];
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
        //<
        //<-----------------------------------------------------------------------------
        context.beginFrameCommandBuffers = [=] () -> EEngineStatus
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
        context.endFrameCommandBuffers = [=] () -> EEngineStatus
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
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        context.bindRenderPass = [=] (std::string                     const &aRenderPassId,
                                      std::string                     const &aFrameBufferId,
                                      std::vector<PassUID_t>          const &aPassExecutionOrder,
                                      SFrameGraphAttachmentCollection const &aAttachmentInfo,
                                      CFrameGraphMutableResources     const &aFrameGraphResources ) -> EEngineStatus
        {
            auto const *const renderPass  = aVulkanEnvironment->getResourceStorage()->extract<CVulkanRenderPassResource>(aRenderPassId);
            auto const *const frameBuffer = aVulkanEnvironment->getResourceStorage()->extract<CVulkanFrameBufferResource>(aFrameBufferId);

            //<-----------------------------------------------------------------------------
            // Helper function to find attachment indices in index lists.
            //<-----------------------------------------------------------------------------
            auto const findAttachmentRelationFn = [] (Vector<FrameGraphResourceId_t> const &aResourceIdIndex,
                                                      Vector<uint64_t>               const &aRelationIndices,
                                                      uint64_t                       const &aIndex)            -> bool
            {
                auto const predicate = [&] (uint64_t const &aTestIndex) -> bool
                {
                    return ( (aResourceIdIndex.size() > aTestIndex) and (aIndex == aResourceIdIndex.at(aTestIndex)) );
                };

                auto const &iterator = std::find_if(aRelationIndices.begin(), aRelationIndices.end(), predicate);

                return (aRelationIndices.end() != iterator);
            };
            //<-----------------------------------------------------------------------------

            auto const addIfNotAddedYet = [] (Vector<SSubpassDependency> &aDependencies, SSubpassDependency const &aToBeInserted) -> void
            {
                for(auto const &dep : aDependencies)
                {
                    if(   dep.srcPass      == aToBeInserted.srcPass
                          && dep.srcStage  == aToBeInserted.srcStage
                          && dep.srcAccess == aToBeInserted.srcAccess
                          && dep.dstPass   == aToBeInserted.dstPass
                          && dep.dstStage  == aToBeInserted.dstStage
                          && dep.dstAccess == aToBeInserted.dstAccess)
                    {
                        return;
                    }
                }

                aDependencies.push_back(aToBeInserted);
            };

            // Each element in the frame buffer is required to have the same dimensions.
            // These variables will store the first sizes encountered and will validate
            // against them for any subsequent size, to make sure that the attachments
            // to be bound are valid.
            int32_t width  = -1,
                    height = -1,
                    layers = -1;

            // This list will store the readable names of the texture views created upfront, so that the
            // framebuffer can bind to it.
            // std::vector<std::string> textureViewIds = {};

            //
            // The derivation of whether something is an input/color/depth attachment or not is most likely broken.

            //<-----------------------------------------------------------------------------
            // Begin the render pass derivation
            //<-----------------------------------------------------------------------------

            auto const &imageResourceIdList   = aAttachmentInfo.getAttachementImageResourceIds();
            auto const &viewResourceIdList    = aAttachmentInfo.getAttachementImageViewResourceIds();
            auto const &viewToImageAssignment = aAttachmentInfo.getAttachmentViewToImageAssignment();
            auto const &passToViewAssignment  = aAttachmentInfo.getAttachmentPassToViewAssignment();

            uint64_t alreadyProcessedAttachmentsFlags = 0;

            SRenderPassDescription renderPassDesc = {};
            renderPassDesc.name = aRenderPassId;
            renderPassDesc.attachmentDescriptions.resize(imageResourceIdList.size());

            std::array<SSubpassDependency, 2> initialDependencies = {};
            initialDependencies[0].srcPass         = VK_SUBPASS_EXTERNAL;
            initialDependencies[0].dstPass         = 0;
            initialDependencies[0].srcStage        = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            initialDependencies[0].srcAccess       = VK_ACCESS_MEMORY_READ_BIT;
            initialDependencies[0].dstStage        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            initialDependencies[0].dstAccess       = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
                                                   | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            initialDependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
            initialDependencies[1].srcPass         = VK_SUBPASS_EXTERNAL;
            initialDependencies[1].dstPass         = 0;
            initialDependencies[1].srcStage        = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            initialDependencies[1].srcAccess       = VK_ACCESS_MEMORY_READ_BIT;
            initialDependencies[1].dstStage        = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            initialDependencies[1].dstAccess       = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
                                                   | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
            initialDependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
            addIfNotAddedYet(renderPassDesc.subpassDependencies, initialDependencies[0]);
            addIfNotAddedYet(renderPassDesc.subpassDependencies, initialDependencies[1]);

            for(std::size_t k=0; k<aPassExecutionOrder.size(); ++k)
            {
                PassUID_t const passUid = aPassExecutionOrder[k];

                std::vector<uint64_t> const &attachmentResourceIndexList = passToViewAssignment.at(passUid);

                SSubpassDescription subpassDesc = {};

                for(auto const &index : attachmentResourceIndexList)
                {
                    FrameGraphResourceId_t const &resourceId = viewResourceIdList.at(index);

                    CEngineResult<Shared<SFrameGraphTextureView> const> const &textureViewFetch = aFrameGraphResources.get<SFrameGraphTextureView>(resourceId);
                    if(not textureViewFetch.successful())
                    {
                        CLog::Error(logTag(), CString::format("Fetching texture view w/ id {} failed.", resourceId));
                        return EEngineStatus::ResourceError_NotFound;
                    }

                    SFrameGraphTextureView const &textureView = *(textureViewFetch.data());

                    CEngineResult<Shared<SFrameGraphTextureView> const> const &parentTextureViewFetch = aFrameGraphResources.get<SFrameGraphTextureView>(textureView.parentResource);
                    if(not parentTextureViewFetch.successful())
                    {
                        CLog::Error(logTag(), CString::format("Fetching parent texture view  w/ id {} failed.", textureView.parentResource));
                        return EEngineStatus::ResourceError_NotFound;
                    }

                    // If the parent texture view is null, the parent is a texture object.
                    Shared<SFrameGraphTextureView> const &parentTextureView = (parentTextureViewFetch.data());

                    CEngineResult<Shared<SFrameGraphTexture> const> const &textureFetch = aFrameGraphResources.get<SFrameGraphTexture>(textureView.subjacentResource);
                    if(not textureFetch.successful())
                    {
                        CLog::Error(logTag(), CString::format("Fetching texture w/ id {} failed.", textureView.subjacentResource));
                        return EEngineStatus::ResourceError_NotFound;
                    }

                    SFrameGraphTexture const &texture = *(textureFetch.data());

                    // Validation first!
                    bool dimensionsValid = true;
                    if(0 > width)
                    {
                        width  = static_cast<int32_t>(texture.width);
                        height = static_cast<int32_t>(texture.height);
                        layers = textureView.arraySliceRange.length;

                        dimensionsValid = (0 < width and 0 < height and 0 < layers);
                    }
                    else
                    {
                        bool const validWidth  = (width  == static_cast<int32_t>(texture.width));
                        bool const validHeight = (height == static_cast<int32_t>(texture.height));
                        bool const validLayers = (layers == static_cast<int32_t>(textureView.arraySliceRange.length));

                        dimensionsValid = (validWidth and validHeight and validLayers);
                    }

                    if(not dimensionsValid)
                    {
                        EngineStatusPrintOnError(EEngineStatus::FrameGraph_RenderContext_AttachmentDimensionsInvalid, logTag(), "Invalid image view dimensions for frame buffer creation.");
                        return { EEngineStatus::Error };
                    }

                    // The texture view's dimensions are valid. Register it for the frame buffer texture view id list.
                    // textureViewIds.push_back(textureView.readableName);

                    uint32_t const attachmentIndex = viewToImageAssignment.at(textureView.resourceId);

                    SAttachmentReference attachmentReference {};
                    attachmentReference.attachment = static_cast<uint32_t>(attachmentIndex);

                    SSubpassDependency dependency = {};
                    dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
                    dependency.srcPass         = (k - 1);
                    dependency.dstPass         = k;

                    bool const isColorAttachment = findAttachmentRelationFn(aAttachmentInfo.getAttachementImageViewResourceIds(), aAttachmentInfo.getColorAttachments(), textureView.resourceId);
                    bool const isDepthAttachment = findAttachmentRelationFn(aAttachmentInfo.getAttachementImageViewResourceIds(), aAttachmentInfo.getDepthAttachments(), textureView.resourceId);
                    bool const isInputAttachment = findAttachmentRelationFn(aAttachmentInfo.getAttachementImageViewResourceIds(), aAttachmentInfo.getInputAttachments(), textureView.resourceId);

                    if(nullptr != parentTextureView && EFrameGraphResourceType::TextureView == parentTextureView->type)
                    {
                        bool const isParentColorAttachment = findAttachmentRelationFn(aAttachmentInfo.getAttachementImageViewResourceIds(), aAttachmentInfo.getColorAttachments(), parentTextureView->resourceId);
                        bool const isParentDepthAttachment = findAttachmentRelationFn(aAttachmentInfo.getAttachementImageViewResourceIds(), aAttachmentInfo.getDepthAttachments(), parentTextureView->resourceId);
                        bool const isParentInputAttachment = findAttachmentRelationFn(aAttachmentInfo.getAttachementImageViewResourceIds(), aAttachmentInfo.getInputAttachments(), parentTextureView->resourceId);

                        dependency.srcPass = std::distance(aPassExecutionOrder.begin(), std::find_if( aPassExecutionOrder.begin()
                                                                                                      , aPassExecutionOrder.end()
                                                                                                      , [&parentTextureView] (PassUID_t const &aUid) -> bool
                                { return (aUid == parentTextureView->assignedPassUID); }));

                        if( 0 < k )
                        {
                            if(isParentColorAttachment)
                            {
                                dependency.srcStage  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                                dependency.srcAccess = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                            }
                            else if(isParentDepthAttachment)
                            {
                                dependency.srcStage  = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                                dependency.srcAccess = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                            }
                            else if(isParentInputAttachment)
                            {
                                dependency.srcStage  = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                                dependency.srcAccess = VK_ACCESS_SHADER_WRITE_BIT;
                            }
                            else
                            {
                                // We hit a texture parent...
                                dependency.srcStage  = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
                                dependency.srcAccess = VK_ACCESS_MEMORY_READ_BIT;
                            }
                        }
                    }
                    else if(nullptr != parentTextureView && EFrameGraphResourceType::Texture == parentTextureView->type)
                    {
                        dependency.srcPass   = VK_SUBPASS_EXTERNAL;
                        dependency.srcStage  = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                        dependency.srcAccess = VK_ACCESS_MEMORY_READ_BIT;
                    }

                    if(isColorAttachment)
                    {
                        attachmentReference.layout = EImageLayout::COLOR_ATTACHMENT_OPTIMAL;
                        dependency.dstStage        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                        dependency.dstAccess       = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                        subpassDesc.colorAttachments.push_back(attachmentReference);
                    }
                    else if(isDepthAttachment)
                    {
                        if(textureView.mode.check(EFrameGraphViewAccessMode::Read))
                        {
                            attachmentReference.layout = EImageLayout::DEPTH_STENCIL_READ_ONLY_OPTIMAL;
                        }
                        else
                        {
                            attachmentReference.layout = EImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                        }

                        dependency.dstStage  = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                        dependency.dstAccess = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                        subpassDesc.depthStencilAttachments.push_back(attachmentReference);
                    }
                    else if(isInputAttachment)
                    {
                        attachmentReference.layout = EImageLayout::SHADER_READ_ONLY_OPTIMAL;
                        dependency.dstStage  = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                        dependency.dstAccess = VK_ACCESS_SHADER_READ_BIT;
                        subpassDesc.inputAttachments.push_back(attachmentReference);
                    }

                    // Easy way to handle 64 simultaneous image resources.
                    if(0 == (alreadyProcessedAttachmentsFlags & (1u << attachmentIndex)))
                    {
                        // For the choice of image layouts, check: https://www.saschawillems.de/?p=3055
                        SAttachmentDescription attachmentDesc = {};
                        attachmentDesc.loadOp         = EAttachmentLoadOp ::CLEAR;
                        attachmentDesc.storeOp        = EAttachmentStoreOp::DONT_CARE;
                        attachmentDesc.stencilLoadOp  = EAttachmentLoadOp ::CLEAR;
                        attachmentDesc.stencilStoreOp = EAttachmentStoreOp::DONT_CARE;
                        attachmentDesc.initialLayout  = EImageLayout::UNDEFINED;
                        attachmentDesc.finalLayout    = EImageLayout::TRANSFER_SRC_OPTIMAL; // For now we just assume everything to be presentable...
                        attachmentDesc.format         = texture.format;

                        if(isColorAttachment)
                        {
                            attachmentDesc.storeOp          = EAttachmentStoreOp::STORE;
                            attachmentDesc.clearColor.color = {0.0f, 0.0f, 0.0f, 1.0f};
                        }
                        else if(isDepthAttachment)
                        {
                            attachmentDesc.clearColor.depthStencil = {1.0f, 0};
                        }
                        else
                        {
                            attachmentDesc.loadOp        = EAttachmentLoadOp::LOAD;
                            attachmentDesc.initialLayout = EImageLayout::SHADER_READ_ONLY_OPTIMAL;
                        }

                        renderPassDesc.attachmentDescriptions[attachmentIndex] = attachmentDesc;
                    }

                    if(0 < k)
                    {
                        addIfNotAddedYet(renderPassDesc.subpassDependencies, dependency);
                    }
                }

                renderPassDesc.subpassDescriptions.push_back(subpassDesc);
            }

            std::array<SSubpassDependency, 2> finalDependencies = {};
            finalDependencies[0].srcPass   = (aPassExecutionOrder.size() - 1);
            finalDependencies[0].dstPass   = VK_SUBPASS_EXTERNAL;
            finalDependencies[0].srcStage  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            finalDependencies[0].srcAccess = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
                                             | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            finalDependencies[0].dstStage  = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            finalDependencies[0].dstAccess = VK_ACCESS_MEMORY_READ_BIT;
            finalDependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
            finalDependencies[1].srcPass   = (aPassExecutionOrder.size() - 1);
            finalDependencies[1].dstPass   = VK_SUBPASS_EXTERNAL;
            finalDependencies[1].srcStage  = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            finalDependencies[1].srcAccess = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
                                             | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
            finalDependencies[1].dstStage  = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            finalDependencies[1].dstAccess = VK_ACCESS_MEMORY_READ_BIT;
            finalDependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

            addIfNotAddedYet(renderPassDesc.subpassDependencies, finalDependencies[0]);
            addIfNotAddedYet(renderPassDesc.subpassDependencies, finalDependencies[1]);

            renderPassDesc.attachmentExtent.width  = width;
            renderPassDesc.attachmentExtent.height = height;
            renderPassDesc.attachmentExtent.depth  = layers;
            // renderPassDesc.attachmentTextureViews  = textureViewIds;

            {
                CEngineResult<Shared<ILogicalResourceObject>> renderPassObject = mResourceManager->useDynamicResource<SRenderPass>(renderPassDesc.name, renderPassDesc);
                if(EEngineStatus::ResourceManager_ResourceAlreadyCreated == renderPassObject.result())
                {
                    return {EEngineStatus::Ok};
                }
                else if( not (EEngineStatus::Ok==renderPassObject.result()))
                {
                    EngineStatusPrintOnError(renderPassObject.result(), logTag(), "Failed to create render pass.");
                    return {renderPassObject.result()};
                }

                registerUsedResource(renderPassDesc.name, renderPassObject.data());

                mCurrentRenderPassHandle = renderPassDesc.name;
            }

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

            vkCmdBeginRenderPass(mVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer(), &vkRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

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

            vkCmdEndRenderPass(mVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer());

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanRenderContext::beginGraphicsFrame()
        {
            return mVulkanEnvironment->beginGraphicsFrame().result();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanRenderContext::endGraphicsFrame()
        {
            return mVulkanEnvironment->endGraphicsFrame().result();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanRenderContext::present()
        {
            SVulkanState &vkState = mVulkanEnvironment->getState();

            VkQueue transferQueue = mVulkanEnvironment->getTransferQueue();
            VkQueue graphicsQueue = mVulkanEnvironment->getGraphicsQueue();
            VkQueue presentQueue  = mVulkanEnvironment->getPresentQueue();

            Shared<IVkFrameContext> const &frameContext = mVulkanEnvironment->getVkCurrentFrameContext();

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
                        mVulkanEnvironment->recreateSwapChain();
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
        EEngineStatus CVulkanRenderContext::bindAttributeAndIndexBuffers(GpuApiHandle_t const &aAttributeBufferId, GpuApiHandle_t const &aIndexBufferId, Vector<VkDeviceSize> aOffsets)
        {
            SVulkanState     &vkState        = mVulkanEnvironment->getState();
            VkCommandBuffer  vkCommandBuffer = mVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer();

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
            VkCommandBuffer  vkCommandBuffer = mVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer(); // The commandbuffers and swapchain count currently match

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
            VkCommandBuffer  vkCommandBuffer = mVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer(); // The commandbuffers and swapchain count currently match

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
            VkCommandBuffer  vkCommandBuffer = mVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer(); // The commandbuffers and swapchain count currently match

            vkCmdDraw(vkCommandBuffer, 6, 1, 0, 0);

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------
    }
}
