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

        template <typename T>
        using OptRef_t = std::optional<std::reference_wrapper<T>>;

        template <typename TLogical, typename TGpuApi>
        using FetchResult_t = std::tuple<bool, OptRef_t<TLogical>, OptRef_t<TGpuApi>, core::CBitField<EGpuApiResourceState>>;

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TLogical, typename TGpuApi>
        FetchResult_t<TLogical, TGpuApi> fetchResource(Shared<CResourceManager> const &aResourceManager
                                                     , ResourceId_t             const &aResourceId
                                                     , bool                     const  aGpuResourceMayBeNull = true)
        {
            static FetchResult_t<TLogical, TGpuApi> const sInvalid = { false, {}, {}, EGpuApiResourceState::Error };

            std::optional<SResourceState<TLogical>> resource = aResourceManager->getResource<TLogical>(aResourceId);
            if(not resource.has_value())
            {
                return sInvalid;
            }

            if(nullptr == resource->gpuApiResource && aGpuResourceMayBeNull)
            {
                return { true, resource->logicalResource, {}, resource->state };
            }
            else
            {
                return sInvalid;
            }

            Shared<TGpuApi> gpuApiResource = std::static_pointer_cast<TGpuApi>(resource->gpuApiResource);
            if(nullptr == gpuApiResource)
            {
                return sInvalid;
            }

            return { true, resource->logicalResource, *(gpuApiResource.get()), resource->state };
        }

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
                // CLog::Error(logTag(), "Failed to map vulkan buffer w/ handle {}", aGpuBufferHandle);
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

                auto const [success, logical, gpu, gpuState] = fetchResource<SRenderPass, CVulkanRenderPassResource>(aResourceManager, aRenderPassId);
                if(not success)
                {
                    return EEngineStatus::Error;
                }

                SRenderPass            const &renderPass  = *logical;
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

                auto const [success, logical, gpu, gpuState] = fetchResource<STexture, CVulkanTextureResource>(aResourceManager, aImageHandle.readableName, false);
                if(not success)
                {
                    return EEngineStatus::Error;
                }

                STexture               const &logicalResource = *logical;
                CVulkanTextureResource const &textureResource = *gpu;

                VkImage image = textureResource.imageHandle;

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

                auto const [success, logical, gpu, gpuState] = fetchResource<STexture, CVulkanTextureResource>(aResourceManager, aSourceImageId.readableName, false);
                if(not success)
                {
                    return EEngineStatus::Error;
                }

                STexture               const &logicalResource = *logical;
                CVulkanTextureResource const &textureResource = *gpu;

                VkImage image = textureResource.imageHandle;

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

                auto const [success, logical, gpu, gpuState] = fetchResource<SPipeline, CVulkanPipelineResource>(aResourceManager, aMaterialHandle.readableName, false);
                if(not success)
                {
                    return EEngineStatus::Error;
                }

                SPipeline               const &logicalResource = *logical;
                CVulkanPipelineResource const &gpuApiResource  = *gpu;

                SMaterialPipelineDescriptor const &pipelineDescriptor = logicalResource.getDescription();

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
                                    auto const [success, logical, gpu, gpuState] = fetchResource<SBuffer, CVulkanBufferResource>(aResourceManager, aGpuBufferHandles[bufferCounter].readableName, false);
                                    if(not success)
                                    {
                                        return EEngineStatus::Error;
                                    }

                                    SBuffer               const &logicalBufferResource = *logical;
                                    CVulkanBufferResource const &gpuApiBufferResource  = *gpu;

                                    VkDescriptorBufferInfo bufferInfo = {};
                                    bufferInfo.buffer = gpuApiBufferResource.handle;
                                    bufferInfo.offset = 0;
                                    bufferInfo.range  = logicalBufferResource.getDescription().createInfo.size;
                                    descriptorSetWriteBufferInfos[bufferCounter] = bufferInfo;

                                    VkWriteDescriptorSet descriptorWrite = {};
                                    descriptorWrite.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                                    descriptorWrite.pNext            = nullptr;
                                    descriptorWrite.descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                                    descriptorWrite.dstSet           = gpuApiResource.descriptorSets[startSetIndex + k];
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
                                    auto const [success, logical, gpu, gpuState] = fetchResource<STextureView, CVulkanTextureViewResource>(aResourceManager, aGpuInputAttachmentTextureViewHandles[inputAttachmentCounter].readableName, false);
                                    if(not success)
                                    {
                                        return EEngineStatus::Error;
                                    }

                                    STextureView               const &logicalViewResource = *logical;
                                    CVulkanTextureViewResource const &gpuApiViewResource  = *gpu;

                                    VkDescriptorImageInfo imageInfo {};
                                    imageInfo.imageView   = gpuApiViewResource.handle;
                                    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                                    imageInfo.sampler     = VK_NULL_HANDLE;
                                    descriptorSetWriteAttachmentImageInfos[inputAttachmentCounter] = imageInfo;

                                    VkWriteDescriptorSet descriptorWrite = {};
                                    descriptorWrite.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                                    descriptorWrite.pNext            = nullptr;
                                    descriptorWrite.descriptorType   = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
                                    descriptorWrite.dstSet           = gpuApiResource.descriptorSets[startSetIndex + k];
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

                                    auto const [success, logical, gpu, gpuState] = fetchResource<STexture, CVulkanTextureResource>(aResourceManager, aGpuTextureViewHandles[inputImageCounter].image.readableName, false);
                                    if(not success)
                                    {
                                        return EEngineStatus::Error;
                                    }

                                    STexture               const &logicalTextureResource = *logical;
                                    CVulkanTextureResource const &gpuApiTextureResource  = *gpu;

                                    auto const [success2, logical2, gpu2, gpuState2] = fetchResource<STextureView, CVulkanTextureViewResource>(aResourceManager, aGpuTextureViewHandles[inputImageCounter].imageView.readableName, false);
                                    if(not success2)
                                    {
                                        return EEngineStatus::Error;
                                    }

                                    STextureView               const &logicalViewResource = *logical2;
                                    CVulkanTextureViewResource const &gpuApiViewResource  = *gpu2;

                                    VkDescriptorImageInfo imageInfo {};
                                    imageInfo.imageView   = gpuApiViewResource.handle;
                                    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                                    imageInfo.sampler     = gpuApiTextureResource.attachedSampler;
                                    descriptorSetWriteImageInfos[inputImageCounter] = imageInfo;

                                    VkWriteDescriptorSet descriptorWrite = {};
                                    descriptorWrite.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                                    descriptorWrite.pNext            = nullptr;
                                    descriptorWrite.descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                                    descriptorWrite.dstSet           = gpuApiResource.descriptorSets[startSetIndex + k];
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

            context.createRenderPass = [=] (
                    std::string                     const &aRenderPassId,
                    std::vector<PassUID_t>          const &aPassExecutionOrder,
                    SFrameGraphAttachmentCollection const &aAttachmentInfo,
                    CFrameGraphMutableResources     const &aFrameGraphResources) -> EEngineStatus
            {
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
                            return EEngineStatus::Error;
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
                finalDependencies[0].srcPass         = (aPassExecutionOrder.size() - 1);
                finalDependencies[0].dstPass         = VK_SUBPASS_EXTERNAL;
                finalDependencies[0].srcStage        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                finalDependencies[0].srcAccess       = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
                                                       | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                finalDependencies[0].dstStage        = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
                finalDependencies[0].dstAccess       = VK_ACCESS_MEMORY_READ_BIT;
                finalDependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
                finalDependencies[1].srcPass         = (aPassExecutionOrder.size() - 1);
                finalDependencies[1].dstPass         = VK_SUBPASS_EXTERNAL;
                finalDependencies[1].srcStage        = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                finalDependencies[1].srcAccess       = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
                                                       | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
                finalDependencies[1].dstStage        = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
                finalDependencies[1].dstAccess       = VK_ACCESS_MEMORY_READ_BIT;
                finalDependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

                addIfNotAddedYet(renderPassDesc.subpassDependencies, finalDependencies[0]);
                addIfNotAddedYet(renderPassDesc.subpassDependencies, finalDependencies[1]);

                renderPassDesc.attachmentExtent.width  = width;
                renderPassDesc.attachmentExtent.height = height;
                renderPassDesc.attachmentExtent.depth  = layers;
                // renderPassDesc.attachmentTextureViews  = textureViewIds;

                {
                    CEngineResult<Shared<ILogicalResourceObject>> renderPassObject = aResourceManager->useDynamicResource<SRenderPass>(renderPassDesc.name, renderPassDesc);
                    if(EEngineStatus::ResourceManager_ResourceAlreadyCreated == renderPassObject.result())
                    {
                        return EEngineStatus::Ok;
                    }
                    else if( not (EEngineStatus::Ok==renderPassObject.result()))
                    {
                        EngineStatusPrintOnError(renderPassObject.result(), logTag(), "Failed to create render pass.");
                        return renderPassObject.result();
                    }

                    //registerUsedResource(renderPassDesc.name, renderPassObject.data());
                    //mCurrentRenderPassHandle = renderPassDesc.name;
                }

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
                auto const [success, logical, gpu, gpuState] = fetchResource<SRenderPass, CVulkanRenderPassResource>(aResourceManager, aRenderPassId, false);
                if(not success)
                {
                    return EEngineStatus::Error;
                }

                SRenderPass               const &logicalRenderPassResource = *logical;
                CVulkanRenderPassResource const &gpuApiRenderPassResource  = *gpu;

                auto const [success2, logical2, gpu2, gpuState2] = fetchResource<SFrameBuffer, CVulkanFrameBufferResource>(aResourceManager, aRenderPassId, false);
                if(not success2)
                {
                    return EEngineStatus::Error;
                }

                SFrameBuffer               const &logicalFrameBufferResource = *logical2;
                CVulkanFrameBufferResource const &gpuApiFrameBufferResource  = *gpu2;

                SVulkanState &state = aVulkanEnvironment->getState();

                SRenderPassDescription const &renderPassDescription = logicalRenderPassResource.getDescription();

                SRenderPassDependencies renderPassDependencies {};

                FrameGraphResourceIdList const &attachmentResources = aAttachmentInfo.getAttachementImageViewResourceIds();

                auto const &assignment = aAttachmentInfo.getAttachmentPassToViewAssignment();
                for(auto const &passUid : aPassExecutionOrder)
                {
                    std::vector<uint64_t> const &attachmentResourceIndexList = assignment.at(passUid);

                    SSubpassDescription subpassDesc = {};
                    for(auto const &index : attachmentResourceIndexList)
                    {
                        FrameGraphResourceId_t const &resourceId = attachmentResources.at(index);

                        CEngineResult<Shared<SFrameGraphTextureView> const> const &textureViewFetch = aFrameGraphResources.get<SFrameGraphTextureView>(resourceId);
                        if(not textureViewFetch.successful())
                        {
                            CLog::Error(logTag(), CString::format("Fetching texture view w/ id {} failed.", resourceId));
                            return EEngineStatus::ResourceError_NotFound;
                        }

                        SFrameGraphTextureView const &textureView = *(textureViewFetch.data());

                        auto const predicate = [textureView] (std::string const &aViewId) -> bool { return (aViewId == textureView.readableName); };

                        // The texture view's dimensions are valid. Register it for the frame buffer texture view id list.
                        if(renderPassDependencies.attachmentTextureViews.end() == std::find_if( renderPassDependencies.attachmentTextureViews.begin()
                                                                                              , renderPassDependencies.attachmentTextureViews.end()
                                                                                              , predicate))
                        {
                            renderPassDependencies.attachmentTextureViews.push_back(textureView.readableName);
                        }
                    }
                }

                EEngineStatus const renderPassLoaded = logicalRenderPassResource.initialize(renderPassDependencies).result(); // Make sure the resource is loaded before it is used in a command...
                EngineStatusPrintOnError(renderPassLoaded, logTag(), "Failed to load renderpass in backend.");
                SHIRABE_RETURN_RESULT_ON_ERROR(renderPassLoaded);

                SFrameBufferDependencies frameBufferDependencies {};
                frameBufferDependencies.referenceRenderPassId  = aRenderPassId;
                frameBufferDependencies.attachmentExtent       = renderPassDescription.attachmentExtent;
                frameBufferDependencies.attachmentTextureViews = renderPassDependencies.attachmentTextureViews;

                EEngineStatus const frameBufferLoaded = logicalFrameBufferResource.initialize(frameBufferDependencies).result();
                EngineStatusPrintOnError(frameBufferLoaded, logTag(), "Failed to load framebuffer in backend.");
                SHIRABE_RETURN_RESULT_ON_ERROR(renderPassLoaded);

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
                vkRenderPassBeginInfo.renderPass        = gpuApiRenderPassResource.handle;
                vkRenderPassBeginInfo.framebuffer       = gpuApiFrameBufferResource.handle;
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
            context.unbindRenderPass = [=] (std::string const &aFrameBufferId,
                                            std::string const &aRenderPassId) -> EEngineStatus
            {
                SHIRABE_UNUSED(aFrameBufferId);
                SHIRABE_UNUSED(aRenderPassId);

                vkCmdEndRenderPass(aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer());

                return EEngineStatus::Ok;
            };

            context.destroyFrameBuffer = [=] (std::string const &aFrameBufferId) -> EEngineStatus
            {
                auto const [success2, logical2, gpu2, gpuState2] = fetchResource<SFrameBuffer, CVulkanFrameBufferResource>(aResourceManager, aFrameBufferId, false);
                if(not success2)
                {
                    return EEngineStatus::Error;
                }

                SFrameBuffer               const &logicalFrameBufferResource = *logical2;
                CVulkanFrameBufferResource const &gpuApiFrameBufferResource  = *gpu2;

                return logicalFrameBufferResource.deinitialize(*(logicalFrameBufferResource.getCurrentDependencies())).result();
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------F------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.destroyRenderPass = [=] (std::string const &aRenderPassId) -> EEngineStatus
            {
                auto const [success, logical, gpu, gpuState] = fetchResource<SRenderPass, CVulkanRenderPassResource>(aResourceManager, aRenderPassId, false);
                if(not success)
                {
                    return EEngineStatus::Error;
                }

                SRenderPass               const &logicalRenderPassResource = *logical;
                CVulkanRenderPassResource const &gpuApiRenderPassResource  = *gpu;

                return logicalRenderPassResource.deinitialize(*(logicalRenderPassResource.getCurrentDependencies())).result();
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            context.beginGraphicsFrame = [=] () -> EEngineStatus
            {
                return aVulkanEnvironment->beginGraphicsFrame().result();
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            context.endGraphicsFrame = [=] () -> EEngineStatus
            {
                return aVulkanEnvironment->endGraphicsFrame().result();
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            context.present = [=] () -> EEngineStatus
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
            context.createTexture = [=] (SFrameGraphTexture const &aTexture) -> EEngineStatus
            {
                STextureDescription desc = {};
                desc.name        = aTexture.readableName;
                desc.textureInfo = static_cast<graphicsapi::STextureInfo>(aTexture);
                // Always set those...
                desc.gpuBinding.set(EBufferBinding::CopySource);
                desc.gpuBinding.set(EBufferBinding::CopyTarget);

                if(aTexture.requestedUsage.check(EFrameGraphResourceUsage::ColorAttachment))
                {
                    desc.gpuBinding.set(EBufferBinding::ColorAttachment);
                }

                if(aTexture.requestedUsage.check(EFrameGraphResourceUsage::DepthAttachment))
                {
                    desc.gpuBinding.set(EBufferBinding::DepthAttachment);
                }

                if(aTexture.requestedUsage.check(EFrameGraphResourceUsage::InputAttachment))
                {
                    desc.gpuBinding.set(EBufferBinding::InputAttachment);
                }

                desc.cpuGpuUsage = EResourceUsage::CPU_None_GPU_ReadWrite;

                {
                    CEngineResult<Shared<ILogicalResourceObject>> textureObject = aResourceManager->useDynamicResource<STexture>(desc.name, desc);
                    if( EEngineStatus::ResourceManager_ResourceAlreadyCreated == textureObject.result())
                    {
                        return EEngineStatus::Ok;
                    }
                    else
                    {
                        EngineStatusPrintOnError(textureObject.result(), logTag(), "Failed to create texture.");
                    }
                }
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.destroyTexture = [=] (SFrameGraphTexture const &aTexture) -> EEngineStatus
            {
                auto const [success, logical, gpu, gpuState] = fetchResource<STexture, CVulkanTextureResource>(aResourceManager, aTexture.readableName, false);
                if(not success)
                {
                    return EEngineStatus::Error;
                }

                STexture               const &logicalResource = *logical;
                CVulkanTextureResource const &gpuApiResource  = *gpu;

                logicalResource.unload();
                return logicalResource.deinitialize(*(logicalResource.getCurrentDependencies())).result();
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.createTextureView = [=] (SFrameGraphTexture     const &aTexture,
                                             SFrameGraphTextureView const &aView) -> EEngineStatus
            {

                STextureViewDescription desc = { };
                desc.name                 = aView.readableName;
                desc.textureFormat        = aView.format;
                desc.subjacentTextureInfo = static_cast<graphicsapi::STextureInfo>(aTexture);
                desc.arraySlices          = aView.arraySliceRange;
                desc.mipMapSlices         = aView.mipSliceRange;

                CEngineResult<Shared<ILogicalResourceObject>> textureViewObject = aResourceManager->useDynamicResource<STextureView>(desc.name, desc);
                EngineStatusPrintOnError(textureViewObject.result(), logTag(), "Failed to create texture.");

                auto const [success, logical, gpu, gpuState] = fetchResource<STexture, CVulkanTextureResource>(aResourceManager, aTexture.readableName, false);
                if(not success)
                {
                    return EEngineStatus::Error;
                }

                STexture               const &logicalResource = *logical;
                CVulkanTextureResource const &gpuApiResource  = *gpu;

                auto const [success2, logical2, gpu2, gpuState2] = fetchResource<STextureView, CVulkanTextureViewResource>(aResourceManager, aTextureView.readableName, false);
                if(not success2)
                {
                    return EEngineStatus::Error;
                }

                STextureView               const &logicalViewResource = *logical2;
                CVulkanTextureViewResource const &gpuApiViewResource  = *gpu2;

                STextureViewDependencies dependencies {};
                dependencies.subjacentTextureId = aTexture.readableName;

                logicalResource    .initialize({}).result();
                logicalViewResource.initialize(dependencies).result();
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.destroyTextureView = [=] (SFrameGraphTextureView const &aTextureView) -> EEngineStatus
            {
                auto const [success, logical, gpu, gpuState] = fetchResource<STextureView, CVulkanTextureViewResource>(aResourceManager, aTextureView.readableName, false);
                if(not success)
                {
                    return EEngineStatus::Error;
                }

                STextureView               const &logicalResource = *logical;
                CVulkanTextureViewResource const &gpuApiResource  = *gpu;

                logicalResource.unload();
                return logicalResource.deinitialize(*(logicalResource.getCurrentDependencies())).result();
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.createBuffer = [=] (SFrameGraphBuffer const &aBuffer) -> EEngineStatus
            {
                SBufferDescription desc = { };
                desc.name = aBuffer.readableName;

                VkBufferCreateInfo &createInfo = desc.createInfo;
                createInfo.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                createInfo.pNext                 = nullptr;
                createInfo.flags                 = 0;
                createInfo.usage                 = aBuffer.bufferUsage;
                createInfo.size                  = aBuffer.sizeInBytes;
                // Determined in backend
                // createInfo.sharingMode           = ...;
                // createInfo.queueFamilyIndexCount = ...;
                // createInfo.pQueueFamilyIndices   = ...;

                CEngineResult<Shared<ILogicalResourceObject>> bufferObject = aResourceManager->useDynamicResource<SBuffer>(desc.name, desc);
                EngineStatusPrintOnError(bufferObject.result(), logTag(), "Failed to create buffer.");

                return bufferObject.result();
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.destroyBuffer = [=] (SFrameGraphBuffer const &aBuffer) -> EEngineStatus
            {
                auto const [success, logical, gpu, gpuState] = fetchResource<SBuffer, CVulkanBufferResource>(aResourceManager, aBuffer.readableName, false);
                if(not success)
                {
                    return EEngineStatus::Error;
                }

                SBuffer               const &logicalResource = *logical;
                CVulkanBufferResource const &gpuApiResource  = *gpu;

                logicalResource.unload();
                return logicalResource.deinitialize(*(logicalResource.getCurrentDependencies())).result();
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.createBufferView = [=] (SFrameGraphBuffer     const &aBuffer
                                          , SFrameGraphBufferView const &aView) -> EEngineStatus
            {
                SBufferViewDescription desc = { };
                desc.name = aBuffer.readableName;

                VkBufferViewCreateInfo &createInfo = desc.createInfo;
                createInfo.sType  = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
                createInfo.pNext  = nullptr;
                createInfo.flags  = 0;
                // createInfo.offset = "...";
                // createInfo.buffer = "...";
                // createInfo.format = "...";
                // createInfo.range  = "...";

                CEngineResult<Shared<ILogicalResourceObject>> bufferViewObject = aResourceManager->useDynamicResource<SBufferView>(desc.name, desc);
                EngineStatusPrintOnError(bufferViewObject.result(), logTag(), "Failed to create buffer view.");
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.destroyBufferView = [=] (SFrameGraphBufferView const &aView) -> EEngineStatus
            {
                auto const [success, logical, gpu, gpuState] = fetchResource<SBufferView, CVulkanBufferViewResource>(aResourceManager, aView.readableName, false);
                if(not success)
                {
                    return EEngineStatus::Error;
                }

                SBufferView               const &logicalResource = *logical;
                CVulkanBufferViewResource const &gpuApiResource  = *gpu;

                logicalResource.unload();
                return logicalResource.deinitialize(*(logicalResource.getCurrentDependencies())).result();
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.bindPipeline = [=] (ResourceId_t const &aPipelineUID) -> EEngineStatus
            {
                SVulkanState     &vkState        = aVulkanEnvironment->getState();
                VkCommandBuffer  vkCommandBuffer = aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer(); // The commandbuffers and swapchain count currently match

                auto const [success, logical, gpu, gpuState] = fetchResource<SPipeline, CVulkanPipelineResource>(aResourceManager, aPipelineUID, false);
                if(not success)
                {
                    return EEngineStatus::Error;
                }

                SPipeline               const &logicalResource = *logical;
                CVulkanPipelineResource const &gpuApiResource  = *gpu;

                vkCmdBindPipeline(vkCommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, gpuApiResource.pipeline);

                vkCmdBindDescriptorSets(vkCommandBuffer
                        , VK_PIPELINE_BIND_POINT_GRAPHICS
                        , gpuApiResource.pipelineLayout
                        , 0
                        , gpuApiResource.descriptorSets.size()
                        , gpuApiResource.descriptorSets.data()
                        , 0, nullptr);

                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.unbindPipeline = [=] (ResourceId_t const &aPipelineUID) -> EEngineStatus
            {
                SHIRABE_UNUSED(aPipelineUID);
                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            context.bindResource = [=] (ResourceId_t const &aId) -> EEngineStatus
            {
                CLog::Verbose(logTag(), CString::format("Binding resource with id {}", aId));
                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            context.unbindResource = [=] (ResourceId_t const &aId) -> EEngineStatus
            {
                CLog::Verbose(logTag(), CString::format("Unbinding resource with id {}", aId));
                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.readMeshAsset = [=] (SFrameGraphMesh const &aMesh) -> EEngineStatus
            {
                CEngineResult<Shared<ILogicalResourceObject>> meshObject = aResourceManager->useAssetResource<SMesh>(aMesh.readableName, aMesh.meshAssetId);
                if(CheckEngineError(meshObject.result()))
                {
                    CLog::Error(logTag(), "Cannot use material asset {} with id {}", aMesh.readableName, aMesh.meshAssetId);
                    return meshObject.result();
                }
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.bindMesh = [=] (SFrameGraphMesh const &aMesh) -> EEngineStatus
            {
                SVulkanState     &vkState        = aVulkanEnvironment->getState();
                VkCommandBuffer  vkCommandBuffer = aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer();

                auto const [success, logical, gpu, gpuState] = fetchResource<SMesh, CVkApiResource<SMesh>>(aResourceManager, aMesh.readableName, true);
                if(not success)
                {
                    return EEngineStatus::Error;
                }

                SMesh const &logicalMeshResource = *logical;

                SMeshDependencies dependencies {};
                EEngineStatus const status = logicalMeshResource.initialize(dependencies).result();

                auto const [success1, logical1, gpu1, gpuState1] = fetchResource<SBuffer, CVulkanBufferResource>(aResourceManager, logicalMeshResource.vertexDataBufferResource->getDescription().name, false);
                if(not success1)
                {
                    return EEngineStatus::Error;
                }

                SBuffer               const &logicalAttributeBufferResource = *logical1;
                CVulkanBufferResource const &gpuApiAttributeBufferResource  = *gpu1;

                auto const [success2, logical2, gpu2, gpuState2] = fetchResource<SBuffer, CVulkanBufferResource>(aResourceManager, logicalMeshResource.indexBufferResource->getDescription().name, false);
                if(not success2)
                {
                    return EEngineStatus::Error;
                }

                SBuffer               const &logicalIndexBufferResource = *logical2;
                CVulkanBufferResource const &gpuApiIndexBufferResource  = *gpu2;

                logicalAttributeBufferResource.initialize({});
                logicalIndexBufferResource    .initialize({});

                transferBufferData(aVulkanEnvironment->getLogicalDevice(), logicalAttributeBufferResource.getDescription().dataSource(), gpuApiAttributeBufferResource.attachedMemory);
                transferBufferData(aVulkanEnvironment->getLogicalDevice(), logicalIndexBufferResource    .getDescription().dataSource(), gpuApiIndexBufferResource.attachedMemory);

                std::vector<VkBuffer> buffers = {   gpuApiAttributeBufferResource.handle
                        , gpuApiAttributeBufferResource.handle
                        , gpuApiAttributeBufferResource.handle
                        , gpuApiAttributeBufferResource.handle };

                vkCmdBindVertexBuffers(vkCommandBuffer, 0, buffers.size(), buffers.data(), logicalMeshResource.getDescription().offsets.data());
                vkCmdBindIndexBuffer(vkCommandBuffer, gpuApiIndexBufferResource.handle, 0, VkIndexType::VK_INDEX_TYPE_UINT16);

                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.unbindMesh = [=] (SFrameGraphMesh const &aMesh) -> EEngineStatus
            {
                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.readMaterialAsset = [=] (SFrameGraphMaterial const &aMaterial) -> EEngineStatus
            {
                CEngineResult<Shared<ILogicalResourceObject>> materialObject = aResourceManager->useAssetResource<SMaterial>(aMaterial.readableName, aMaterial.materialAssetId);
                if(CheckEngineError(materialObject.result()))
                {
                    CLog::Error(logTag(), "Cannot use material asset {} with id {}", aMaterial.readableName, aMaterial.materialAssetId);
                    return materialObject.result();
                }

                return materialObject.result();
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.bindMaterial = [=] (SFrameGraphMaterial const &aMaterial) -> EEngineStatus
            {
                auto const [success1, logical1, gpu1, gpuState1] = fetchResource<SMaterial, CVkApiResource<SMaterial>>(aResourceManager, aMaterial.readableName, false);
                if(not success1)
                {
                    return EEngineStatus::Error;
                }

                SMaterial             const &logicalMaterialResource = *logical1;
                SMaterialDependencies const &materialDependencies    = *(logicalMaterialResource.getCurrentDependencies());

                SMaterialDependencies dependencies {};
                dependencies.pipelineDependencies.systemUBOPipelineId   = "Core_pipeline";
                dependencies.pipelineDependencies.referenceRenderPassId = materialDependencies.pipelineDependencies.referenceRenderPassId;
                dependencies.pipelineDependencies.subpass               = mCurrentSubpass;
                dependencies.pipelineDependencies.shaderModuleId        = material->shaderModuleResource->getDescription().name;

                for(auto const &buffer : material->bufferResources)
                {
                    buffer->initialize({});
                }
                material->shaderModuleResource->initialize({});
                material->pipelineResource    ->initialize(dependencies.pipelineDependencies);

                EEngineStatus const status = material->initialize(dependencies).result();

                std::vector<GpuApiHandle_t>       gpuBufferIds                     {};
                std::vector<GpuApiHandle_t>       gpuInputAttachmentTextureViewIds {};
                std::vector<SSampledImageBinding> gpuTextureViewIds                {};

                for(auto const &buffer : material->bufferResources)
                {
                    mGraphicsAPIRenderContext->transferBufferData(buffer->getDescription().dataSource(), buffer->getGpuApiResourceHandle());
                    gpuBufferIds.push_back(buffer->getGpuApiResourceHandle());
                }

                Shared<SRenderPass>             renderPass      = std::static_pointer_cast<SRenderPass>(getUsedResource(aRenderPassHandle));
                SRenderPassDescription   const &renderPassDesc  = renderPass->getDescription();
                SRenderPassDependencies  const &renderPassDeps = *(renderPass->getCurrentDependencies());

                SSubpassDescription const &subPassDesc = renderPassDesc.subpassDescriptions.at(mCurrentSubpass);
                for(auto const &inputAttachment : subPassDesc.inputAttachments)
                {
                    uint32_t     const &attachmentIndex           = inputAttachment.attachment;
                    ResourceId_t const &attachementResourceHandle = renderPassDeps.attachmentTextureViews.at(attachmentIndex);

                    Shared<STextureView> attachmentTextureView = std::static_pointer_cast<STextureView>(getUsedResource(attachementResourceHandle));
                    gpuInputAttachmentTextureViewIds.push_back(attachmentTextureView->getGpuApiResourceHandle());
                }

                for(auto const &sampledImageAssetId : material->getDescription().sampledImages)
                {
                    std::string const sampledImageResourceId = fmt::format("{}", sampledImageAssetId);

                    Shared<ILogicalResourceObject> logicalTexture      = mResourceManager->useAssetResource(sampledImageResourceId, sampledImageAssetId).data();
                    Shared<STexture>               sampledImageTexture = std::static_pointer_cast<STexture>(logicalTexture);
                    if(nullptr != sampledImageTexture)
                    {
                        sampledImageTexture->initialize({}); // No-Op if loaded already...
                        sampledImageTexture->load();

                        if(sampledImageTexture->getDescription().gpuBinding.check(EBufferBinding::TextureInput))
                        {
                            mGraphicsAPIRenderContext->performImageLayoutTransfer(
                                    sampledImageTexture->getGpuApiResourceHandle()
                                    , CRange(0, 1)
                                    , CRange(0, 1)
                                    , VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
                                    , VK_IMAGE_LAYOUT_UNDEFINED
                                    , VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
                        }
                        // TODO: Determine load state of texture resource and determine, whether transfer is needed.
                        sampledImageTexture->transfer();     // No-Op if transferred already...

                        if(sampledImageTexture->getDescription().gpuBinding.check(EBufferBinding::TextureInput))
                        {
                            mGraphicsAPIRenderContext->performImageLayoutTransfer(
                                    sampledImageTexture->getGpuApiResourceHandle()
                                    , CRange(0, 1)
                                    , CRange(0, 1)
                                    , VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
                                    , VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
                                    , VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                        }

                        STextureViewDescription desc {};
                        desc.name                 = fmt::format("{}_{}_view", material->getDescription().name, sampledImageTexture->getDescription().name);
                        desc.subjacentTextureInfo = sampledImageTexture->getDescription().textureInfo;
                        desc.arraySlices          = { 0, 1 };
                        desc.mipMapSlices         = { 0, 1 };
                        desc.textureFormat        = sampledImageTexture->getDescription().textureInfo.format;

                        auto const [result, viewData] = mResourceManager->useDynamicResource<STextureView>(desc.name, desc);
                        if(CheckEngineError(result))
                        {
                            // ...
                            break;
                        }

                        Shared<STextureView> view = std::static_pointer_cast<STextureView>(viewData);

                        STextureViewDependencies deps {};
                        deps.subjacentTextureId = sampledImageResourceId;
                        view->initialize(deps);

                        SSampledImageBinding binding {};
                        binding.image     = sampledImageTexture->getGpuApiResourceHandle();
                        binding.imageView = view->getGpuApiResourceHandle();

                        gpuTextureViewIds.push_back(binding);
                    }
                    else
                    {
                        gpuTextureViewIds.push_back( {}); // Fill gaps
                    }
                }

                mGraphicsAPIRenderContext->updateResourceBindings(  material->pipelineResource->getGpuApiResourceHandle()
                        , gpuBufferIds
                        , gpuInputAttachmentTextureViewIds
                        , gpuTextureViewIds);

                auto const result = mGraphicsAPIRenderContext->bindPipeline(material->pipelineResource->getGpuApiResourceHandle());
                return result;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            context.render = [=] (SFrameGraphMesh     const& /* aMesh */
                                , SFrameGraphMaterial const& /* aMaterial */) -> EEngineStatus
            {
                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.drawIndexed = [=] (uint32_t const aIndexCount) -> EEngineStatus
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
            context.drawQuad = [=] () -> EEngineStatus
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
            context.drawFullscreenQuadWithMaterial = [=] (SFrameGraphMaterial const &aMaterial) -> EEngineStatus
            {
                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------
        }
    }
}
