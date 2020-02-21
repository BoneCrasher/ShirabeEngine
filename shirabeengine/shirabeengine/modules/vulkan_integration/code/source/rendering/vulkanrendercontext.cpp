#include "vulkan_integration/rendering/vulkanrendercontext.h"
#include "vulkan_integration/resources/types/vulkantextureresource.h"
#include "vulkan_integration/resources/types/vulkanframebufferresource.h"
#include "vulkan_integration/resources/types/vulkanrenderpassresource.h"
#include "vulkan_integration/resources/types/vulkanmaterialpipelineresource.h"

#include <asset/assetstorage.h>
#include <mesh/loader.h>
#include <mesh/declaration.h>
#include <material/loader.h>
#include <material/declaration.h>
#include <material/serialization.h>
#include <renderer/framegraph/framegraphrendercontext.h>

#include <thread>
#include <base/string.h>

namespace engine
{
    namespace vulkan
    {
        using namespace framegraph;
        using engine::resources::VulkanResourceManager_t;

        template <typename T>
        using OptRef_t = std::optional<std::reference_wrapper<T>>;

        template <typename TResource>
        using FetchResult_t = std::tuple<bool, OptRef_t<TResource>>;

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        FetchResult_t<TResource> fetchResource(Shared<VulkanResourceManager_t> const &aResourceManager
                                             , ResourceId_t                    const &aResourceId)
        {
            static FetchResult_t<TResource> const sInvalid = { false, {} };

            OptRef_t<TResource> resource = aResourceManager->getResource<TResource>(aResourceId);
            if(not resource.has_value())
            {
                return sInvalid;
            }

            return { true, resource };
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
        framegraph::SFrameGraphRenderContext CreateRenderContextForVulkan(Shared<CVulkanEnvironment>        aVulkanEnvironment
                                                                        , Shared<VulkanResourceManager_t>   aResourceManager
                                                                        , Shared<asset::CAssetStorage>      aAssetStorage
                                                                        , Shared<mesh::CMeshLoader>         aMeshLoader
                                                                        , Shared<material::CMaterialLoader> aMaterialLoader)
        {
            using namespace local;
            using namespace resources;

            framegraph::SFrameGraphRenderContext context {};

            context.clearAttachments = [=] (SFrameGraphRenderContextState       &aState
                                          , std::string                   const &aRenderPassId
                                          , uint32_t                      const &aCurrentSubpassIndex) -> EEngineStatus
            {
                SHIRABE_UNUSED(aState);

                SVulkanState    &state        = aVulkanEnvironment->getState();
                VkCommandBuffer commandBuffer = aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer();

                auto const [success, resource] = fetchResource<RenderPassResourceState_t>(aResourceManager, aRenderPassId);
                if(not success)
                {
                    return EEngineStatus::Error;
                }

                RenderPassResourceState_t    &renderPass  = *resource;
                SRenderPassDescription const &description = renderPass.description;

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
            context.beginPass = [=] (SFrameGraphRenderContextState &aState) -> EEngineStatus
            {
                SHIRABE_UNUSED(aState);

                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.endPass = [=] (SFrameGraphRenderContextState &aState) -> EEngineStatus
            {
                SHIRABE_UNUSED(aState);

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
            context.copyImage = [=](SFrameGraphRenderContextState       &aState,
                                     SFrameGraphDynamicTexture            const &aSourceImageId,
                                     SFrameGraphDynamicTexture            const &aTargetImageId) -> EEngineStatus
            {
                SHIRABE_UNUSED(aState);
                SHIRABE_UNUSED(aSourceImageId);
                SHIRABE_UNUSED(aTargetImageId);

                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.performImageLayoutTransfer = [=] (SFrameGraphRenderContextState       &aState
                                                    , SFrameGraphDynamicTexture            const &aImageHandle
                                                    , CRange                        const &aArrayRange
                                                    , CRange                        const &aMipRange
                                                    , VkImageAspectFlags            const &aAspectFlags
                                                    , VkImageLayout                 const &aSourceLayout
                                                    , VkImageLayout                 const &aTargetLayout) -> EEngineStatus
            {
                SHIRABE_UNUSED(aState);

                SVulkanState &state = aVulkanEnvironment->getState();

                VkCommandBuffer commandBuffer  = aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer();

                auto const [success, resource] = fetchResource<TextureResourceState_t>(aResourceManager, aImageHandle.readableName);
                if(not success)
                {
                    return EEngineStatus::Error;
                }

                TextureResourceState_t const &texture = *resource;

                VkImage image = texture.gpuApiHandles.imageHandle;

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
            context.copyImageToBackBuffer = [=](SFrameGraphRenderContextState const &aState
                                               , SFrameGraphDynamicTexture            const &aSourceImageId) -> EEngineStatus
            {
                SHIRABE_UNUSED(aState);

                SVulkanState &state = aVulkanEnvironment->getState();

                VkCommandBuffer commandBuffer  = aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer();
                VkImage         swapChainImage = state.swapChain.swapChainImages.at(state.swapChain.currentSwapChainImageIndex);

                auto const [success, resource] = fetchResource<TextureResourceState_t>(aResourceManager, aSourceImageId.readableName);
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
            context.beginFrameCommandBuffers = [=] (SFrameGraphRenderContextState const &aState) -> EEngineStatus
            {
                SHIRABE_UNUSED(aState);

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
            context.endFrameCommandBuffers = [=] (SFrameGraphRenderContextState const &aState) -> EEngineStatus
            {
                SHIRABE_UNUSED(aState);

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

            context.createRenderPass = [=] (SFrameGraphRenderContextState   const &aState,
                                            std::string                     const &aRenderPassId,
                                            std::vector<PassUID_t>          const &aPassExecutionOrder,
                                            SFrameGraphAttachmentCollection const &aAttachmentInfo,
                                            CFrameGraphMutableResources     const &aFrameGraphResources) -> EEngineStatus
            {
                SHIRABE_UNUSED(aState);

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

                        CEngineResult < Shared < SFrameGraphDynamicTexture > const> const &textureFetch = aFrameGraphResources.get<SFrameGraphDynamicTexture>(textureView.subjacentResource);
                        if(not textureFetch.successful())
                        {
                            CLog::Error(logTag(), CString::format("Fetching texture w/ id {} failed.", textureView.subjacentResource));
                            return EEngineStatus::ResourceError_NotFound;
                        }

                        SFrameGraphDynamicTexture const &texture = *(textureFetch.data());

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
                }

                return EEngineStatus::Ok;
            };

            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            context.bindRenderPass = [=] (SFrameGraphRenderContextState   const &aState,
                                          std::string                     const &aRenderPassId,
                                          std::string                     const &aFrameBufferId,
                                          std::vector<PassUID_t>          const &aPassExecutionOrder,
                                          SFrameGraphAttachmentCollection const &aAttachmentInfo,
                                          CFrameGraphMutableResources     const &aFrameGraphResources ) -> EEngineStatus
            {
                SHIRABE_UNUSED(aState);

                OptRef_t<RenderPassResourceState_t>  renderPassResource {};
                OptRef_t<FrameBufferResourceState_t> frameBufferResource {};

                {
                    auto [success, resource] = fetchResource<RenderPassResourceState_t>(aResourceManager, aRenderPassId);
                    if (not success)
                    {
                        return EEngineStatus::Error;
                    }
                    renderPassResource = resource;
                }

                {
                    auto [success, resource] = fetchResource<FrameBufferResourceState_t>(aResourceManager, aFrameBufferId);
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
                SRenderPassDependencies       renderPassDependencies {};

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

                CEngineResult<> const renderPassInitialized = aResourceManager->initializeResource<RenderPassResourceState_t>(aRenderPassId, renderPassDependencies, aVulkanEnvironment);
                EngineStatusPrintOnError(renderPassInitialized.result(), logTag(), "Failed to load renderpass in backend.");
                SHIRABE_RETURN_RESULT_ON_ERROR(renderPassInitialized.result());

                SFrameBufferDependencies frameBufferDependencies {};
                frameBufferDependencies.referenceRenderPassId  = aRenderPassId;
                frameBufferDependencies.attachmentExtent       = renderPassDescription.attachmentExtent;
                frameBufferDependencies.attachmentTextureViews = renderPassDependencies.attachmentTextureViews;

                CEngineResult<> const frameBufferInitialized = aResourceManager->initializeResource<FrameBufferResourceState_t>(aFrameBufferId, frameBufferDependencies, aVulkanEnvironment);
                EngineStatusPrintOnError(frameBufferInitialized.result(), logTag(), "Failed to load framebuffer in backend.");
                SHIRABE_RETURN_RESULT_ON_ERROR(frameBufferInitialized);

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
            context.unbindRenderPass = [=] (SFrameGraphRenderContextState       &aState
                                          , std::string                 const &aFrameBufferId
                                          , std::string                 const &aRenderPassId) -> EEngineStatus
            {
                SHIRABE_UNUSED(aState);
                SHIRABE_UNUSED(aFrameBufferId);
                SHIRABE_UNUSED(aRenderPassId);

                vkCmdEndRenderPass(aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer());

                return EEngineStatus::Ok;
            };

            context.destroyFrameBuffer = [=] (SFrameGraphRenderContextState       &aState
                                            , std::string                   const &aFrameBufferId) -> EEngineStatus
            {
                SHIRABE_UNUSED(aState);

                auto [success, resource] = fetchResource<FrameBufferResourceState_t>(aResourceManager, aFrameBufferId);
                if(not success)
                {
                    return EEngineStatus::Error;
                }

                FrameBufferResourceState_t &frameBuffer = *resource;

                CEngineResult<> const deinitialized = aResourceManager->deinitializeResource<FrameBufferResourceState_t>(aFrameBufferId, frameBuffer.dependencies, aVulkanEnvironment);
                return deinitialized.result();
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------F------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.destroyRenderPass = [=] (SFrameGraphRenderContextState       &aState
                                           , std::string                   const &aRenderPassId) -> EEngineStatus
            {
                SHIRABE_UNUSED(aState);

                auto [success, resource] = fetchResource<RenderPassResourceState_t>(aResourceManager, aRenderPassId);
                if(not success)
                {
                    return EEngineStatus::Error;
                }

                RenderPassResourceState_t &renderPass = *resource;

                CEngineResult<> const deinitialized = aResourceManager->deinitializeResource<RenderPassResourceState_t>(aRenderPassId, renderPass.dependencies, aVulkanEnvironment);
                return deinitialized.result();
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            context.beginGraphicsFrame = [=] (SFrameGraphRenderContextState &aState) -> EEngineStatus
            {
                SHIRABE_UNUSED(aState);

                return aVulkanEnvironment->beginGraphicsFrame().result();
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            context.endGraphicsFrame = [=] (SFrameGraphRenderContextState &aState) -> EEngineStatus
            {
                SHIRABE_UNUSED(aState);

                return aVulkanEnvironment->endGraphicsFrame().result();
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            context.present = [=] (SFrameGraphRenderContextState &aState) -> EEngineStatus
            {
                SHIRABE_UNUSED(aState);

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
            context.createTexture = [=](SFrameGraphRenderContextState       &aState
                                       , SFrameGraphDynamicTexture            const &aTexture) -> EEngineStatus
            {
                SHIRABE_UNUSED(aState);

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
            context.destroyTexture = [=](SFrameGraphRenderContextState       &aState
                                        , SFrameGraphDynamicTexture            const &aTexture) -> EEngineStatus
            {
                SHIRABE_UNUSED(aState);

                OptRef_t<TextureResourceState_t> resourceOpt {};
                {
                    auto [success, resource] = fetchResource<TextureResourceState_t>(aResourceManager, aTexture.readableName);
                    if(not success)
                    {
                        return EEngineStatus::Ok;
                    }
                    resourceOpt = resource;
                }
                TextureResourceState_t &texture = *resourceOpt;

                CEngineResult<> const deinitialized = aResourceManager->deinitializeResource<TextureResourceState_t>(aTexture.readableName, texture.dependencies, aVulkanEnvironment);
                return deinitialized.result();
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.createTextureView = [=] (SFrameGraphRenderContextState       &aState
                                           , SFrameGraphDynamicTexture            const &aTexture
                                           , SFrameGraphTextureView        const &aView) -> EEngineStatus
            {
                STextureViewDescription desc = { };
                desc.name                 = aView.readableName;
                desc.textureFormat        = aView.format;
                desc.subjacentTextureInfo = static_cast<graphicsapi::STextureInfo>(aTexture);
                desc.arraySlices          = aView.arraySliceRange;
                desc.mipMapSlices         = aView.mipSliceRange;

                CEngineResult<Shared<ILogicalResourceObject>> textureViewObject = aResourceManager->useDynamicResource<STextureView>(desc.name, desc);
                EngineStatusPrintOnError(textureViewObject.result(), logTag(), "Failed to create texture.");
                SHIRABE_RETURN_RESULT_ON_ERROR(textureViewObject.result());

                OptRef_t<TextureResourceState_t> textureOpt {} ;
                {
                    auto [success, resource] = fetchResource<TextureResourceState_t>(aResourceManager, aTexture.readableName);
                    if(not success)
                    {
                        return EEngineStatus::Ok;
                    }
                    textureOpt = resource;
                }
                TextureResourceState_t &texture = *textureOpt;

                OptRef_t<TextureViewResourceState_t> textureViewOpt {};
                {
                    auto [success, resource] = fetchResource<TextureViewResourceState_t>(aResourceManager, aView.readableName);
                    if(not success)
                    {
                        return EEngineStatus::Ok;
                    }
                    textureViewOpt = resource;
                }
                TextureViewResourceState_t &textureView = *textureViewOpt;

                STextureViewDependencies textureViewDependencies {};
                textureViewDependencies.subjacentTextureId = aTexture.readableName;

                CEngineResult<> const textureInitialized = aResourceManager->initializeResource<TextureResourceState_t>    (aTexture.readableName, {}, aVulkanEnvironment);
                EngineStatusPrintOnError(textureInitialized.result(), logTag(), "Failed to initialize texture.");
                SHIRABE_RETURN_RESULT_ON_ERROR(textureInitialized.result());

                CEngineResult<> const textureViewInitialized = aResourceManager->initializeResource<TextureViewResourceState_t>(aView.readableName, textureViewDependencies, aVulkanEnvironment);
                EngineStatusPrintOnError(textureViewInitialized.result(), logTag(), "Failed to initialize texture view.");
                SHIRABE_RETURN_RESULT_ON_ERROR(textureViewInitialized.result());

                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.destroyTextureView = [=] (SFrameGraphRenderContextState       &aState
                                            , SFrameGraphTextureView        const &aTextureView) -> EEngineStatus
            {
                SHIRABE_UNUSED(aState);

                OptRef_t<TextureViewResourceState_t> resourceOpt {};
                {
                    auto [success, resource] = fetchResource<TextureViewResourceState_t>(aResourceManager, aTextureView.readableName);
                    if(not success)
                    {
                        return EEngineStatus::Ok;
                    }
                    resourceOpt = resource;
                }
                TextureViewResourceState_t &textureView = *resourceOpt;

                CEngineResult<> const deinitialization = aResourceManager->deinitializeResource<TextureViewResourceState_t>(aTextureView.readableName, textureView.dependencies, aVulkanEnvironment);
                return deinitialization.result();
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.createBuffer = [=] (SFrameGraphRenderContextState       &aState
                                      , SFrameGraphBuffer             const &aBuffer) -> EEngineStatus
            {
                SHIRABE_UNUSED(aState);

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
            context.destroyBuffer = [=] (SFrameGraphRenderContextState       &aState
                                       , SFrameGraphBuffer             const &aBuffer) -> EEngineStatus
            {
                SHIRABE_UNUSED(aState);

                OptRef_t<BufferResourceState_t> resourceOpt {};
                {
                    auto [success, resource] = fetchResource<BufferResourceState_t>(aResourceManager, aBuffer.readableName);
                    if(not success)
                    {
                        return EEngineStatus::Ok;
                    }
                    resourceOpt = resource;
                }
                BufferResourceState_t &buffer = *resourceOpt;

                CEngineResult<> const deinitialization = aResourceManager->deinitializeResource<BufferResourceState_t>(aBuffer.readableName, buffer.dependencies, aVulkanEnvironment);
                EngineStatusPrintOnError(deinitialization.result(), logTag(), "Failed to deinitialize buffer.");
                SHIRABE_RETURN_RESULT_ON_ERROR(deinitialization.result());
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.createBufferView = [=] (SFrameGraphRenderContextState       &aState
                                          , SFrameGraphBuffer             const &aBuffer
                                          , SFrameGraphBufferView         const &aView) -> EEngineStatus
            {
                SHIRABE_UNUSED(aState);

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

                auto const &[success, resource] = aResourceManager->useDynamicResource<BufferViewResourceState_t>(desc.name, desc);
                EngineStatusPrintOnError(success, logTag(), "Failed to create buffer view.");
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.destroyBufferView = [=] (SFrameGraphRenderContextState       &aState
                                           , SFrameGraphBufferView         const &aView) -> EEngineStatus
            {
                SHIRABE_UNUSED(aState);

                OptRef_t<BufferViewResourceState_t> resourceOpt {};
                {
                    auto [success, resource] = fetchResource<BufferViewResourceState_t>(aResourceManager, aView.readableName);
                    if(not success)
                    {
                        return EEngineStatus::Ok;
                    }
                    resourceOpt = resource;
                }
                BufferViewResourceState_t &bufferView = *resourceOpt;

                CEngineResult<> const deinitialization = aResourceManager->deinitializeResource<BufferViewResourceState_t>(aView.readableName, bufferView.dependencies, aVulkanEnvironment);
                EngineStatusPrintOnError(deinitialization.result(), logTag(), "Failed to deinitialize buffer.");
                SHIRABE_RETURN_RESULT_ON_ERROR(deinitialization.result());
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.bindPipeline = [=] (SFrameGraphRenderContextState       &aState
                                      , ResourceId_t                  const &aPipelineUID) -> EEngineStatus
            {
                SHIRABE_UNUSED(aState);

                SVulkanState     &vkState        = aVulkanEnvironment->getState();
                VkCommandBuffer  vkCommandBuffer = aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer(); // The commandbuffers and swapchain count currently match

                OptRef_t<PipelineResourceState_t> resourceOpt {};
                {
                    auto [success, resource] = fetchResource<PipelineResourceState_t>(aResourceManager, aPipelineUID);
                    if(not success)
                    {
                        return EEngineStatus::Ok;
                    }
                    resourceOpt = resource;
                }
                PipelineResourceState_t &pipeline = *resourceOpt;

                vkCmdBindPipeline(vkCommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.gpuApiHandles.pipeline);

                vkCmdBindDescriptorSets(vkCommandBuffer
                        , VK_PIPELINE_BIND_POINT_GRAPHICS
                        , pipeline.gpuApiHandles.pipelineLayout
                        , 0
                        , pipeline.gpuApiHandles.descriptorSets.size()
                        , pipeline.gpuApiHandles.descriptorSets.data()
                        , 0, nullptr);

                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.unbindPipeline = [=] (SFrameGraphRenderContextState       &aState
                                        , ResourceId_t                  const &aPipelineUID) -> EEngineStatus
            {
                SHIRABE_UNUSED(aState);
                SHIRABE_UNUSED(aPipelineUID);

                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            context.bindResource = [=] (SFrameGraphRenderContextState       &aState
                                      , ResourceId_t                  const &aId) -> EEngineStatus
            {
                SHIRABE_UNUSED(aState);

                CLog::Verbose(logTag(), CString::format("Binding resource with id {}", aId));
                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            context.unbindResource = [=] (SFrameGraphRenderContextState       &aState
                                        , ResourceId_t                  const &aId) -> EEngineStatus
            {
                SHIRABE_UNUSED(aState);

                CLog::Verbose(logTag(), CString::format("Unbinding resource with id {}", aId));
                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.readMeshAsset = [=] (SFrameGraphRenderContextState       &aState
                                       , SFrameGraphMesh               const &aMesh) -> EEngineStatus
            {
                SHIRABE_UNUSED(aState);

                using namespace mesh;

                auto const &[result, instance] = aMeshLoader->loadMeshInstance(aMesh.readableName, aAssetStorage, aMesh.meshAssetId);
                if(CheckEngineError(result))
                {
                    return result;
                }

                SMeshDataFile const &dataFile = instance->dataFile();

                uint64_t accumulatedVertexDataSize = 0;
                for(auto const &attributeDescription : dataFile.attributes)
                {
                    accumulatedVertexDataSize += (attributeDescription.length * attributeDescription.bytesPerSample);
                }

                DataSourceAccessor_t dataAccessor = [=] () -> ByteBuffer
                {
                    asset::AssetID_t const assetUid = asset::assetIdFromUri(dataFile.dataBinaryFilename);
                    auto const [result, buffer] = aAssetStorage->loadAssetData(assetUid);
                    if(CheckEngineError(result))
                    {
                        CLog::Error("DataSourceAccessor_t::MeshBinaryData", "Failed to load binary data for mesh. Result: {}", result);
                        return {};
                    }

                    ByteBuffer bufferView = buffer.createView(0, accumulatedVertexDataSize);
                    return buffer;
                };

                SBufferDescription dataBufferDescription {};
                dataBufferDescription.name                             = fmt::format("{}_{}_", aMesh.readableName, "databuffer");
                dataBufferDescription.dataSource                       = dataAccessor;
                dataBufferDescription.createInfo.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                dataBufferDescription.createInfo.pNext                 = nullptr;
                dataBufferDescription.createInfo.flags                 = 0;
                dataBufferDescription.createInfo.size                  = accumulatedVertexDataSize;
                dataBufferDescription.createInfo.pQueueFamilyIndices   = nullptr;
                dataBufferDescription.createInfo.queueFamilyIndexCount = 0;
                dataBufferDescription.createInfo.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
                dataBufferDescription.createInfo.usage                 = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

                DataSourceAccessor_t indexDataAccessor = [=] () -> ByteBuffer
                {
                    asset::AssetID_t const assetUid = asset::assetIdFromUri(dataFile.indexBinaryFilename);
                    auto const [result, buffer] = aAssetStorage->loadAssetData(assetUid);
                    if(CheckEngineError(result))
                    {
                        CLog::Error("DataSourceAccessor_t::MeshBinaryData", "Failed to load binary data for mesh. Result: {}", result);
                        return {};
                    }

                    ByteBuffer bufferView = buffer.createView(dataFile.indices.offset, dataFile.indices.length);
                    return buffer;
                };

                SBufferDescription indexBufferDescription   {};
                indexBufferDescription.name                             = fmt::format("{}_{}", aMesh.readableName, "indexbuffer");
                indexBufferDescription.createInfo.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                indexBufferDescription.createInfo.pNext                 = nullptr;
                indexBufferDescription.createInfo.flags                 = 0;
                indexBufferDescription.dataSource                       = indexDataAccessor;
                indexBufferDescription.createInfo.size                  = (dataFile.indices.length * dataFile.indices.bytesPerSample);
                indexBufferDescription.createInfo.pQueueFamilyIndices   = nullptr;
                indexBufferDescription.createInfo.queueFamilyIndexCount = 0;
                indexBufferDescription.createInfo.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
                indexBufferDescription.createInfo.usage                 = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

                SMeshDescriptor meshDescription {};
                meshDescription.name                   = aMesh.readableName;
                meshDescription.dataBufferDescription  = dataBufferDescription;
                meshDescription.indexBufferDescription = indexBufferDescription;

                meshDescription.attributeCount   = dataFile.attributeSampleCount;
                meshDescription.indexSampleCount = dataFile.indexSampleCount;

                Vector<VkDeviceSize> offsets;
                offsets.resize(4);

                VkDeviceSize currentOffset = 0;
                for(uint64_t k=0; k<4; ++k)
                {
                    offsets[k] = currentOffset;
                    VkDeviceSize length = (dataFile.attributes[k].length * dataFile.attributes[k].bytesPerSample);
                    currentOffset += length;
                }
                meshDescription.offsets = offsets;

                CEngineResult<OptRef_t<MeshResourceState_t>>   meshResult            = aResourceManager->useDynamicResource<MeshResourceState_t>  (meshDescription.name,        meshDescription       );
                CEngineResult<OptRef_t<BufferResourceState_t>> attributeBufferResult = aResourceManager->useDynamicResource<BufferResourceState_t>(dataBufferDescription.name,  dataBufferDescription );
                CEngineResult<OptRef_t<BufferResourceState_t>> indexBufferResult     = aResourceManager->useDynamicResource<BufferResourceState_t>(indexBufferDescription.name, indexBufferDescription);

                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.bindMesh = [=] (SFrameGraphRenderContextState       &aState
                                  , SFrameGraphMesh               const &aMesh) -> EEngineStatus
            {
                SHIRABE_UNUSED(aState);

                SVulkanState     &vkState        = aVulkanEnvironment->getState();
                VkCommandBuffer  vkCommandBuffer = aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer();

                std::string const dataBufferId  = fmt::format("{}_{}", aMesh.readableName, "databuffer");
                std::string const indexBufferId = fmt::format("{}_{}", aMesh.readableName, "indexbuffer");

                OptRef_t<MeshResourceState_t> meshOpt {};
                {
                    auto [success, resource] = fetchResource<MeshResourceState_t>(aResourceManager, aMesh.readableName);
                    if(not success)
                    {
                        return EEngineStatus::Ok;
                    }
                    meshOpt = resource;
                }
                MeshResourceState_t &mesh = *meshOpt;

                OptRef_t<BufferResourceState_t> dataBufferOpt {};
                {
                    auto [success, resource] = fetchResource<BufferResourceState_t>(aResourceManager, dataBufferId);
                    if(not success)
                    {
                        return EEngineStatus::Ok;
                    }
                    dataBufferOpt = resource;
                }
                BufferResourceState_t & dataBuffer = *dataBufferOpt;

                OptRef_t<BufferResourceState_t> indexBufferOpt {};
                {
                    auto [success, resource] = fetchResource<BufferResourceState_t>(aResourceManager, indexBufferId);
                    if(not success)
                    {
                        return EEngineStatus::Ok;
                    }
                    indexBufferOpt = resource;
                }
                BufferResourceState_t &indexBuffer = *indexBufferOpt;

                CEngineResult<> const dataBufferInit = aResourceManager->initializeResource<BufferResourceState_t>(dataBufferId, dataBuffer.dependencies, aVulkanEnvironment);
                EngineStatusPrintOnError(dataBufferInit.result(), logTag(), "Failed to initialize databuffer.");
                SHIRABE_RETURN_RESULT_ON_ERROR(dataBufferInit.result());

                CEngineResult<> const indexBufferInit = aResourceManager->initializeResource<BufferResourceState_t>(indexBufferId, indexBuffer.dependencies, aVulkanEnvironment);
                EngineStatusPrintOnError(indexBufferInit.result(), logTag(), "Failed to initialize indexbuffer.");
                SHIRABE_RETURN_RESULT_ON_ERROR(indexBufferInit.result());

                transferBufferData(aVulkanEnvironment->getLogicalDevice(), dataBuffer.description.dataSource(),  dataBuffer.gpuApiHandles.attachedMemory);
                transferBufferData(aVulkanEnvironment->getLogicalDevice(), indexBuffer.description.dataSource(), indexBuffer.gpuApiHandles.attachedMemory);

                std::vector<VkBuffer> buffers = { dataBuffer.gpuApiHandles.handle
                                                , dataBuffer.gpuApiHandles.handle
                                                , dataBuffer.gpuApiHandles.handle
                                                , dataBuffer.gpuApiHandles.handle };

                vkCmdBindVertexBuffers(vkCommandBuffer, 0, buffers.size(), buffers.data(), mesh.description.offsets.data());
                vkCmdBindIndexBuffer  (vkCommandBuffer, indexBuffer.gpuApiHandles.handle, 0, VkIndexType::VK_INDEX_TYPE_UINT16);

                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.unbindMesh = [=] (SFrameGraphRenderContextState        &aState
                                    , SFrameGraphMesh                const &aMesh) -> EEngineStatus
            {
                SHIRABE_UNUSED(aState);
                SHIRABE_UNUSED(aMesh);

                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto const configureInputAssembly = [] (SMaterialPipelineDescriptor &aPipelineDescriptor) -> void
            {
                aPipelineDescriptor.inputAssemblyState.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
                aPipelineDescriptor.inputAssemblyState.pNext                  = nullptr;
                aPipelineDescriptor.inputAssemblyState.flags                  = 0;
                aPipelineDescriptor.inputAssemblyState.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                aPipelineDescriptor.inputAssemblyState.primitiveRestartEnable = false;
            };

            auto const configureRasterizer = [] (SMaterialPipelineDescriptor &aPipelineDescriptor) -> void
            {
                aPipelineDescriptor.rasterizerState.sType                     = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
                aPipelineDescriptor.rasterizerState.pNext                     = nullptr;
                aPipelineDescriptor.rasterizerState.flags                     = 0;
                aPipelineDescriptor.rasterizerState.cullMode                  = VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
                aPipelineDescriptor.rasterizerState.frontFace                 = VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;
                aPipelineDescriptor.rasterizerState.polygonMode               = VkPolygonMode::VK_POLYGON_MODE_FILL;
                aPipelineDescriptor.rasterizerState.lineWidth                 = 1.0f;
                aPipelineDescriptor.rasterizerState.rasterizerDiscardEnable   = VK_FALSE; // isCoreMaterial ? VK_TRUE : VK_FALSE;
                aPipelineDescriptor.rasterizerState.depthClampEnable          = VK_FALSE;
                aPipelineDescriptor.rasterizerState.depthBiasEnable           = VK_FALSE;
                aPipelineDescriptor.rasterizerState.depthBiasSlopeFactor      = 0.0f;
                aPipelineDescriptor.rasterizerState.depthBiasConstantFactor   = 0.0f;
                aPipelineDescriptor.rasterizerState.depthBiasClamp            = 0.0f;
            };

            auto const configureMultisampler = [] (SMaterialPipelineDescriptor &aPipelineDescriptor) -> void
            {
                aPipelineDescriptor.multiSampler.sType                        = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
                aPipelineDescriptor.multiSampler.pNext                        = nullptr;
                aPipelineDescriptor.multiSampler.flags                        = 0;
                aPipelineDescriptor.multiSampler.sampleShadingEnable          = VK_FALSE;
                aPipelineDescriptor.multiSampler.rasterizationSamples         = VK_SAMPLE_COUNT_1_BIT;
                aPipelineDescriptor.multiSampler.minSampleShading             = 1.0f;
                aPipelineDescriptor.multiSampler.pSampleMask                  = nullptr;
                aPipelineDescriptor.multiSampler.alphaToCoverageEnable        = VK_FALSE;
                aPipelineDescriptor.multiSampler.alphaToOneEnable             = VK_FALSE;
            };

            auto const configureDepthStencil = [] (SMaterialPipelineDescriptor &aPipelineDescriptor) -> void
            {
                aPipelineDescriptor.depthStencilState.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
                aPipelineDescriptor.depthStencilState.pNext                 = nullptr;
                aPipelineDescriptor.depthStencilState.flags                 = 0;
                aPipelineDescriptor.depthStencilState.depthTestEnable       = VK_TRUE;
                aPipelineDescriptor.depthStencilState.depthWriteEnable      = VK_TRUE;
                aPipelineDescriptor.depthStencilState.depthCompareOp        = VkCompareOp::VK_COMPARE_OP_LESS;
                aPipelineDescriptor.depthStencilState.stencilTestEnable     = VK_FALSE;
                aPipelineDescriptor.depthStencilState.front.passOp          = VkStencilOp::VK_STENCIL_OP_KEEP;
                aPipelineDescriptor.depthStencilState.front.failOp          = VkStencilOp::VK_STENCIL_OP_KEEP;
                aPipelineDescriptor.depthStencilState.front.depthFailOp     = VkStencilOp::VK_STENCIL_OP_KEEP;
                aPipelineDescriptor.depthStencilState.front.compareOp       = VkCompareOp::VK_COMPARE_OP_ALWAYS;
                aPipelineDescriptor.depthStencilState.front.compareMask     = 0;
                aPipelineDescriptor.depthStencilState.front.writeMask       = 0;
                aPipelineDescriptor.depthStencilState.front.reference       = 0;
                aPipelineDescriptor.depthStencilState.back                  = aPipelineDescriptor.depthStencilState.front;
                aPipelineDescriptor.depthStencilState.depthBoundsTestEnable = VK_FALSE;
                aPipelineDescriptor.depthStencilState.minDepthBounds        = 0.0f;
                aPipelineDescriptor.depthStencilState.maxDepthBounds        = 1.0f;
            };

            context.readMaterialAsset = [=] (SFrameGraphRenderContextState       &aState
                                           , SFrameGraphMaterial           const &aMaterial) -> EEngineStatus
            {
                using namespace material;

                static constexpr char const *SHIRABE_MATERIALSYSTEM_CORE_MATERIAL_RESOURCEID = "Core";

                std::string const materialName = aMaterial.readableName;

                auto const &[result, instance] = aMaterialLoader->loadMaterialInstance(materialName, aAssetStorage, aMaterial.materialAssetId, true);
                if(CheckEngineError(result))
                {
                    return result;
                }

                Shared<CMaterialMaster> const &master    = instance->master();
                SMaterialSignature      const &signature = master  ->signature();
                CMaterialConfig         const &config    = instance->config();

                bool const includeSystemBuffers =  (SHIRABE_MATERIALSYSTEM_CORE_MATERIAL_RESOURCEID == master->name());

                SMaterialPipelineDescriptor pipelineDescriptor     {};
                SShaderModuleDescriptor     shaderModuleDescriptor {};
                Vector<SBufferDescription>  bufferDescriptions     {};

                pipelineDescriptor    .name = fmt::format("{}_{}", materialName, "pipeline");
                shaderModuleDescriptor.name = fmt::format("{}_{}", materialName, "shadermodule");

                pipelineDescriptor.includesSystemBuffers = includeSystemBuffers;

                configureInputAssembly(pipelineDescriptor);
                configureRasterizer   (pipelineDescriptor);
                configureMultisampler (pipelineDescriptor);
                configureDepthStencil (pipelineDescriptor);

                for(auto const &[stageKey, stage] : signature.stages)
                {
                    if(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_VERTEX_SHADER_BIT == stageKey)
                    {
                        std::vector<SStageInput> stageInputs(stage.inputs);
                        std::sort(stageInputs.begin(), stageInputs.end(), [] (SStageInput const &aLHS, SStageInput const &aRHS) -> bool { return aLHS.location < aRHS.location; });

                        for(std::size_t k=0; k<stage.inputs.size(); ++k)
                        {
                            SStageInput const &input = stageInputs.at(k);

                            // This number has to be equal to the VkVertexInputBindingDescription::binding index which data should be taken from!
                            VkVertexInputBindingDescription binding;
                            binding.binding   = input.location;
                            binding.stride    = (input.type->byteSize * input.type->vectorSize);
                            binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

                            VkVertexInputAttributeDescription attribute;
                            attribute.binding  = k;
                            attribute.location = input.location;
                            attribute.offset   = 0;
                            attribute.format   = (8 == binding.stride)
                                                 ? VkFormat::VK_FORMAT_R32G32_SFLOAT
                                                 : (12 == binding.stride)
                                                   ? VkFormat::VK_FORMAT_R32G32B32_SFLOAT
                                                   : (16 == binding.stride)
                                                     ? VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT
                                                     : VkFormat::VK_FORMAT_UNDEFINED;

                            pipelineDescriptor.vertexInputBindings  .push_back(binding);
                            pipelineDescriptor.vertexInputAttributes.push_back(attribute);
                        }
                    }

                    if(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT == stageKey)
                    {
                        std::vector<VkPipelineColorBlendAttachmentState> outputs {};
                        outputs.resize(stage.outputs.size());

                        for(auto const &output : stage.outputs)
                        {
                            VkPipelineColorBlendAttachmentState colorBlendAttachmentState{};
                            colorBlendAttachmentState.blendEnable         = VK_TRUE;
                            colorBlendAttachmentState.colorWriteMask      = VkColorComponentFlagBits::VK_COLOR_COMPONENT_R_BIT|
                                                                            VkColorComponentFlagBits::VK_COLOR_COMPONENT_G_BIT|
                                                                            VkColorComponentFlagBits::VK_COLOR_COMPONENT_B_BIT|
                                                                            VkColorComponentFlagBits::VK_COLOR_COMPONENT_A_BIT;
                            colorBlendAttachmentState.srcColorBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE;  // VkBlendFactor::VK_BLEND_FACTOR_SRC_ALPHA;
                            colorBlendAttachmentState.dstColorBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ZERO; // VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
                            colorBlendAttachmentState.colorBlendOp        = VkBlendOp::VK_BLEND_OP_ADD;
                            colorBlendAttachmentState.srcAlphaBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE;
                            colorBlendAttachmentState.dstAlphaBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ZERO;
                            colorBlendAttachmentState.alphaBlendOp        = VkBlendOp::VK_BLEND_OP_ADD;

                            outputs[output.location] = colorBlendAttachmentState;
                        }

                        pipelineDescriptor.colorBlendAttachmentStates = outputs;

                        VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo {};
                        colorBlendCreateInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
                        colorBlendCreateInfo.pNext             = nullptr;
                        colorBlendCreateInfo.flags             = 0;
                        colorBlendCreateInfo.logicOpEnable     = VK_FALSE;
                        colorBlendCreateInfo.logicOp           = VK_LOGIC_OP_COPY;
                        colorBlendCreateInfo.blendConstants[0] = 0.0f;
                        colorBlendCreateInfo.blendConstants[1] = 1.0f;
                        colorBlendCreateInfo.blendConstants[2] = 2.0f;
                        colorBlendCreateInfo.blendConstants[3] = 3.0f;

                        pipelineDescriptor.colorBlendState = colorBlendCreateInfo;
                    }

                    //
                    // Derive data accessors for shader module creation from master material
                    //
                    std::filesystem::path const  stageSpirVFilename = stage.filename;
                    bool                  const  isEmptyFilename    = stageSpirVFilename.empty();
                    if(not isEmptyFilename)
                    {
                        DataSourceAccessor_t dataAccessor = [=] () -> ByteBuffer
                        {
                            asset::AssetID_t const assetUid = asset::assetIdFromUri(stageSpirVFilename);

                            auto const [result, buffer] = aAssetStorage->loadAssetData(assetUid);
                            if(CheckEngineError(result))
                            {
                                CLog::Error("DataSourceAccessor_t::ShaderModule", "Failed to load shader module asset data. Result: {}", result);
                                return {};
                            }

                            return buffer;
                        };

                        shaderModuleDescriptor.shaderStages[stageKey] = dataAccessor;
                    }
                }

                uint32_t const setSubtractionValue = includeSystemBuffers ? 0 : 2;
                uint32_t const setCount            = signature.layoutInfo.setCount - setSubtractionValue;

                std::vector<VkDescriptorSetLayoutCreateInfo> descriptorSets {};
                descriptorSets.resize(setCount);
                pipelineDescriptor.descriptorSetLayoutBindings.resize(setCount);

                for(std::size_t k=0; k<descriptorSets.size(); ++k)
                {
                    VkDescriptorSetLayoutCreateInfo &info = descriptorSets[k];

                    info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                    info.pNext        = nullptr;
                    info.flags        = 0;
                    info.bindingCount = signature.layoutInfo.setBindingCount[k + setSubtractionValue];

                    pipelineDescriptor.descriptorSetLayoutBindings[k].resize(info.bindingCount);
                }

                pipelineDescriptor.descriptorSetLayoutCreateInfos = descriptorSets;

                for(SSubpassInput const &input : signature.subpassInputs)
                {
                    if(not includeSystemBuffers && 2 > input.set)
                    {
                        continue;
                    }

                    VkDescriptorSetLayoutBinding layoutBinding {};
                    layoutBinding.binding            = input.binding;
                    layoutBinding.descriptorType     = VkDescriptorType::VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
                    layoutBinding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT; // Subpass inputs are only accessibly in fragment shaders.
                    layoutBinding.descriptorCount    = 1;
                    layoutBinding.pImmutableSamplers = nullptr;
                    pipelineDescriptor.descriptorSetLayoutBindings[input.set - setSubtractionValue][input.binding] = layoutBinding;
                }

                for(SUniformBuffer const &uniformBuffer : signature.uniformBuffers)
                {
                    if(not includeSystemBuffers && 2 > uniformBuffer.set)
                    {
                        continue;
                    }

                    VkDescriptorSetLayoutBinding layoutBinding {};
                    layoutBinding.binding            = uniformBuffer.binding;
                    layoutBinding.descriptorType     = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    layoutBinding.stageFlags         = VkShaderStageFlagBits::VK_SHADER_STAGE_ALL; // serialization::shaderStageFromPipelineStage(uniformBuffer.stageBinding.value());
                    layoutBinding.descriptorCount    = uniformBuffer.array.layers;
                    layoutBinding.pImmutableSamplers = nullptr;
                    pipelineDescriptor.descriptorSetLayoutBindings[uniformBuffer.set - setSubtractionValue][uniformBuffer.binding] = layoutBinding;

                    CEngineResult<void const *const> bufferDataFetch = config.getBuffer(uniformBuffer.name);
                    if(CheckEngineError(bufferDataFetch.result()))
                    {
                        CLog::Debug("AssetLoader - Materials", "Can't find buffer w/ name {} in config.", uniformBuffer.name);
                        continue;
                    }

                    auto  const *const data = static_cast<uint8_t const *const>(bufferDataFetch.data());
                    std::size_t  const size = uniformBuffer.location.length;

                    auto const dataSource =  [data, size] () -> ByteBuffer
                    {
                        return ByteBuffer(data, size);
                    };

                    SBufferDescription desc {};
                    desc.name                             = fmt::format("{}_uniformbuffer_{}", materialName, uniformBuffer.name);
                    desc.dataSource                       = dataSource;
                    desc.createInfo.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                    desc.createInfo.pNext                 = nullptr;
                    desc.createInfo.flags                 = 0;
                    desc.createInfo.size                  = uniformBuffer.location.length;
                    desc.createInfo.pQueueFamilyIndices   = nullptr;
                    desc.createInfo.queueFamilyIndexCount = 0;
                    desc.createInfo.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
                    desc.createInfo.usage                 = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

                    bufferDescriptions.push_back(desc);
                }

                for(SSampledImage const &sampledImage : signature.sampledImages)
                {
                    if(not includeSystemBuffers && 2 > sampledImage.set)
                    {
                        continue;
                    }

                    VkDescriptorSetLayoutBinding layoutBinding {};
                    layoutBinding.binding            = sampledImage.binding;
                    layoutBinding.descriptorType     = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    layoutBinding.stageFlags         = serialization::shaderStageFromPipelineStage(sampledImage.stageBinding.value());
                    layoutBinding.descriptorCount    = 1;
                    layoutBinding.pImmutableSamplers = nullptr;
                    pipelineDescriptor.descriptorSetLayoutBindings[sampledImage.set - setSubtractionValue][sampledImage.binding] = layoutBinding;
                }

                VkViewport viewPort = {};
                viewPort.x        = 0.0;
                viewPort.y        = 0.0;
                viewPort.width    = 1920.0;
                viewPort.height   = 1080.0;
                viewPort.minDepth = 0.0;
                viewPort.maxDepth = 1.0;
                pipelineDescriptor.viewPort = viewPort;

                Vector<SSampledImage> sampledImages = signature.sampledImages;
                std::sort(sampledImages.begin(), sampledImages.end(), [] (SSampledImage const &aLHS, SSampledImage const &aRHS) -> bool { return (aLHS.binding < aRHS.binding); });

                Vector<asset::AssetId_t>  sampledImageResources {};
                for(auto const &sampledImage : sampledImages)
                {
                    CMaterialConfig::SampledImageMap_t const &assignment = config.getSampledImageAssignment();
                    if(assignment.end() == assignment.find(sampledImage.name))
                    {
                        sampledImageResources.push_back(asset::AssetId_t {}); // Fill gaps...
                        continue;
                    }

                    asset::AssetId_t const &assetId = assignment.at(sampledImage.name);

                    sampledImageResources.push_back( assetId );
                }

                SMaterialDescriptor materialDescriptor {};
                materialDescriptor.name                     = materialName;
                materialDescriptor.pipelineDescriptor       = pipelineDescriptor;
                materialDescriptor.shaderModuleDescriptor   = shaderModuleDescriptor;
                materialDescriptor.uniformBufferDescriptors = bufferDescriptions;
                materialDescriptor.sampledImages            = sampledImageResources;

                std::vector<std::string> pipelineDependencies {};
                pipelineDependencies.push_back(materialDescriptor.shaderModuleDescriptor.name);

                std::vector<Shared<SBuffer>> buffers(materialDescriptor.uniformBufferDescriptors.size());
                for(std::size_t k=0; k<materialDescriptor.uniformBufferDescriptors.size(); ++k)
                {
                    auto const &bufferDescriptor = materialDescriptor.uniformBufferDescriptors.at(k);

                    CEngineResult<Shared<ILogicalResourceObject>> bufferResourceObject = aResourceManager->useDynamicResource<SBuffer>(bufferDescriptor.name, bufferDescriptor);
                    EngineStatusPrintOnError(bufferResourceObject.result(),  "Material::AssetLoader", "Failed to create buffer.");
                    pipelineDependencies.push_back(bufferDescriptor.name);

                    buffers[k] = std::static_pointer_cast<SBuffer>(bufferResourceObject.data());
                }

                if(not includeSystemBuffers)
                {
                    pipelineDependencies.emplace_back("Core_pipeline");
                }

                CEngineResult<OptRef_t<ShaderModuleResourceState_t>> shaderModuleObject = aResourceManager->useDynamicResource<ShaderModuleResourceState_t>(materialDescriptor.shaderModuleDescriptor.name, materialDescriptor.shaderModuleDescriptor);
                EngineStatusPrintOnError(shaderModuleObject.result(),  "Material::AssetLoader", "Failed to create shader module.");

                CEngineResult<OptRef_t<PipelineResourceState_t>> pipelineObject = aResourceManager->useDynamicResource<PipelineResourceState_t>(materialDescriptor.pipelineDescriptor.name, materialDescriptor.pipelineDescriptor);
                EngineStatusPrintOnError(pipelineObject.result(),  "Material::AssetLoader", "Failed to create pipeline.");

                CEngineResult<OptRef_t<MaterialResourceState_t>> materialObject = aResourceManager->useDynamicResource<MaterialResourceState_t>(materialDescriptor.name, materialDescriptor);
                EngineStatusPrintOnError(materialObject.result(),  "Material::AssetLoader", "Failed to create material.");
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.initializeMaterial = [=] (SFrameGraphRenderContextState       &aState
                                              , SFrameGraphMaterial           const &aMaterial
                                              , ResourceId_t                  const &aRenderPassId) -> EEngineStatus
            {
                OptRef_t<MaterialResourceState_t> materialOpt {};
                {
                    auto [success, resource] = fetchResource<MaterialResourceState_t>(aResourceManager, aMaterial.readableName);
                    if(not success)
                    {
                        return EEngineStatus::Ok;
                    }
                    materialOpt = resource;
                }
                MaterialResourceState_t &material = *materialOpt;

                SMaterialPipelineDependencies pipelineDependencies {};
                pipelineDependencies.systemUBOPipelineId   = "Core_pipeline";
                pipelineDependencies.referenceRenderPassId = aRenderPassId;
                pipelineDependencies.subpass               = aState.currentSubpassIndex;
                pipelineDependencies.shaderModuleId        = material.description.shaderModuleDescriptor.name;

                for(auto const &buffer : material.description.uniformBufferDescriptors)
                {
                    SNoDependencies dependencies {};

                    CEngineResult<> const bufferInitialization = aResourceManager->initializeResource<BufferResourceState_t>(buffer.name, dependencies, aVulkanEnvironment);
                    EngineStatusPrintOnError(bufferInitialization.result(), logTag(), "Failed to initialize buffer.");
                    SHIRABE_RETURN_RESULT_ON_ERROR(bufferInitialization.result());
                }

                CEngineResult<> const shaderModuleInitialization = aResourceManager->initializeResource<ShaderModuleResourceState_t>(material.description.shaderModuleDescriptor.name, {}, aVulkanEnvironment);
                EngineStatusPrintOnError(shaderModuleInitialization.result(), logTag(), "Failed to initialize shader module.");
                SHIRABE_RETURN_RESULT_ON_ERROR(shaderModuleInitialization.result());

                CEngineResult<> const pipelineInitialization = aResourceManager->initializeResource<PipelineResourceState_t>(material.description.pipelineDescriptor.name, pipelineDependencies, aVulkanEnvironment);
                EngineStatusPrintOnError(pipelineInitialization.result(), logTag(), "Failed to initialize pipeline.");
                SHIRABE_RETURN_RESULT_ON_ERROR(pipelineInitialization.result());

            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.transferMaterial = [=] (SFrameGraphRenderContextState       &aState
                                          , SFrameGraphMaterial           const &aMaterial) -> EEngineStatus
            {
                VkDevice device = aVulkanEnvironment->getLogicalDevice();

                OptRef_t<MaterialResourceState_t> materialOpt {};
                {
                    auto [success, resource] = fetchResource<MaterialResourceState_t>(aResourceManager, aMaterial.readableName);
                    if(not success)
                    {
                        return EEngineStatus::Ok;
                    }
                    materialOpt = resource;
                }
                MaterialResourceState_t &material = *materialOpt;

                for(auto const &bufferDesc : material.description.uniformBufferDescriptors)
                {
                    OptRef_t<BufferResourceState_t> bufferOpt {};
                    {
                        auto [success, resource] = fetchResource<BufferResourceState_t>(aResourceManager, bufferDesc.name);
                        if(not success)
                        {
                            return EEngineStatus::Ok;
                        }
                        bufferOpt = resource;
                    }
                    BufferResourceState_t &buffer = *bufferOpt;

                    transferBufferData(device, bufferDesc.dataSource(), buffer.gpuApiHandles.attachedMemory);
                }
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
            auto const updateDescriptorSets = [=] (SFrameGraphRenderContextState                     const &aState
                                                 , SFrameGraphMaterial                               const &aMaterialHandle
                                                 , std::vector<OptRef_t<BufferResourceState_t>>      const &aUniformBufferStates
                                                 , std::vector<OptRef_t<TextureViewResourceState_t>> const &aInputAttachmentStates
                                                 , std::vector<SSampledImageBinding>                 const &aTextureViewStates) -> EEngineStatus
            {
                SHIRABE_UNUSED(aState);

                VkDevice      device = aVulkanEnvironment->getLogicalDevice();
                SVulkanState &state  = aVulkanEnvironment->getState();

                VkCommandBuffer commandBuffer  = aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer();

                auto const [success, resource] = fetchResource<PipelineResourceState_t>(aResourceManager, aMaterialHandle.readableName);
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
                                auto const [success, resource] = fetchResource<BufferResourceState_t >(aResourceManager, aGpuBufferHandles[bufferCounter].readableName);
                                if(not success)
                                {
                                    return EEngineStatus::Error;
                                }

                                BufferResourceState_t buffer = *resource;

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
                                auto const [success, resource] = fetchResource<TextureViewResourceState_t >(aResourceManager, aGpuInputAttachmentTextureViewHandles[inputAttachmentCounter].readableName);
                                if(not success)
                                {
                                    return EEngineStatus::Error;
                                }

                                TextureViewResourceState_t &textureView = *resource;

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

                                OptRef_t<TextureViewResourceState_t> textureViewOpt {};
                                OptRef_t<TextureResourceState_t>     textureOpt     {};

                                {
                                    auto [success, resource] = fetchResource<TextureViewResourceState_t>(aResourceManager, aGpuTextureViewHandles[inputImageCounter].image.readableName);
                                    if(not success)
                                    {
                                        return EEngineStatus::Error;
                                    }
                                    textureViewOpt = resource;
                                }

                                {
                                    auto [success, resource] = fetchResource<TextureResourceState_t>(aResourceManager, aGpuTextureViewHandles[inputImageCounter].imageView.readableName);
                                    if(not success)
                                    {
                                        return EEngineStatus::Error;
                                    }
                                    textureOpt = resource;
                                }

                                TextureViewResourceState_t &textureView = *textureViewOpt;
                                TextureResourceState_t     &texture     = *textureOpt;

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
            context.bindMaterial = [=] (SFrameGraphRenderContextState       &aState
                                      , SFrameGraphMaterial           const &aMaterial
                                      , ResourceId_t                  const &aRenderPassId) -> EEngineStatus
            {
                VkDevice device = aVulkanEnvironment->getLogicalDevice();

                OptRef_t<MaterialResourceState_t> materialOpt {};
                {
                    auto [success, resource] = fetchResource<MaterialResourceState_t>(aResourceManager, aMaterial.readableName);
                    if(not success)
                    {
                        return EEngineStatus::Ok;
                    }
                    materialOpt = resource;
                }
                MaterialResourceState_t &material = *materialOpt;

                OptRef_t<RenderPassResourceState_t> renderPassOpt {};
                {
                    auto [success, resource] = fetchResource<RenderPassResourceState_t>(aResourceManager, material.dependencies.pipelineDependencies.referenceRenderPassId);
                    if(not success)
                    {
                        return EEngineStatus::Ok;
                    }
                    renderPassOpt = resource;
                }
                RenderPassResourceState_t &renderPass = *renderPassOpt;

                std::vector<OptRef_t<BufferResourceState_t>>      buffers           {};
                std::vector<OptRef_t<TextureViewResourceState_t>> inputAttachments  {};
                std::vector<SSampledImageBinding>                 textureViews      {};

                for(auto const &bufferDesc : material.description.uniformBufferDescriptors)
                {
                    OptRef_t<BufferResourceState_t> bufferOpt {};
                    {
                        auto [success, resource] = fetchResource<BufferResourceState_t>(aResourceManager, bufferDesc.name);
                        if(not success)
                        {
                            return EEngineStatus::Ok;
                        }
                        bufferOpt = resource;
                    }
                    buffers.push_back(bufferOpt);
                }

                SSubpassDescription const &subPassDesc = renderPass.description.subpassDescriptions.at(aState.currentSubpassIndex);
                for(auto const &inputAttachment : subPassDesc.inputAttachments)
                {
                    uint32_t     const &attachmentIndex           = inputAttachment.attachment;
                    ResourceId_t const &attachementResourceHandle = renderPass.dependencies.attachmentTextureViews.at(attachmentIndex);

                    OptRef_t<TextureViewResourceState_t> textureViewOpt {};
                    {
                        auto [success, resource] = fetchResource<TextureViewResourceState_t>(aResourceManager, attachementResourceHandle);
                        if(not success)
                        {
                            return EEngineStatus::Ok;
                        }
                        textureViewOpt = resource;
                    }
                    inputAttachments.push_back(textureViewOpt);
                }

                for(auto const &sampledImageResourceId : material.description.sampledImages)
                {
                    OptRef_t<TextureResourceState_t> sampledImageOpt {};
                    {
                        auto [success, resource] = fetchResource<TextureResourceState_t>(aResourceManager, sampledImageResourceId);
                        if(not success)
                        {
                            return EEngineStatus::Ok;
                        }
                        sampledImageOpt = resource;
                    }
                    TextureResourceState_t &sampledImage = *sampledImageOpt;

                    

                    std::string const sampledImageResourceId = fmt::format("{}", sampledImageAssetId);

                    Shared<ILogicalResourceObject> logicalTexture      = aResourceManager->useAssetResource(sampledImageResourceId, sampledImageAssetId).data();
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
                        desc.name                 = fmt::format("{}_{}_view", logicalMaterialResource.getDescription().name, sampledImageTexture->getDescription().name);
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

                EEngineStatus const updateResult = updateDescriptorSets(logicalMaterialResource.pipelineResource->getGpuApiResourceHandle()
                                                                      , gpuBufferIds
                                                                      , gpuInputAttachmentTextureViewIds
                                                                      , gpuTextureViewIds);

                auto const result = context.bindPipeline(aState, );
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
