#include <core/enginetypehelper.h>
#include <asset/mesh/loader.h>
#include <asset/mesh/declaration.h>
#include <asset/material/loader.h>
#include <asset/material/declaration.h>
#include <asset/material/serialization.h>
#include "renderer/rendergraph/framegraphcontexts.h"
#include "renderer/vulkan_rendering/rendering/vulkanresourcecontext.h"

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

        namespace local
        {
            SHIRABE_DECLARE_LOG_TAG(VulkanRenderGraphResourceContext)
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        namespace detail
        {
            using namespace local;
            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto createRenderPass(Shared<CVulkanEnvironment>               aVulkanEnvironment
                                  , Shared<CResourceManager>               aResourceManager
                                  , Shared<asset::CAssetStorage>           aAssetStorage
                                  , ResourceId_t                    const &aRenderPassId
                                  , std::vector<PassUID_t>          const &aPassExecutionOrder
                                  , SRenderGraphAttachmentCollection const &aAttachmentInfo
                                  , CRenderGraphMutableResources     const &aRenderGraphResources) -> EEngineStatus
            {
                //<-----------------------------------------------------------------------------
                // Helper function to find attachment indices in index lists.
                //<-----------------------------------------------------------------------------
                auto const findAttachmentRelationFn =
                               [] (Vector<RenderGraphResourceId_t> const &aResourceIdIndex,
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
                auto const addIfNotAddedYet =
                               [] (Vector<SSubpassDependency> &aDependencies, SSubpassDependency const &aToBeInserted) -> void
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

                auto const &imageResourceIdList   = aAttachmentInfo.getAttachmentImageResourceIds();
                auto const &viewResourceIdList    = aAttachmentInfo.getAttachmentImageViewResourceIds();
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
                        RenderGraphResourceId_t const &resourceId = viewResourceIdList.at(index);

                        CEngineResult<Shared<SRenderGraphImageView>> const textureViewFetch = aRenderGraphResources.getResource<SRenderGraphImageView>(resourceId);
                        if(not textureViewFetch.successful())
                        {
                            CLog::Error(logTag(), CString::format("Fetching texture view w/ id {} failed.", resourceId));
                            return EEngineStatus::ResourceError_NotFound;
                        }

                        SRenderGraphImageView const &textureView = *(textureViewFetch.data());

                        CEngineResult<Shared<SRenderGraphImageView>> const parentTextureViewFetch = aRenderGraphResources.getResource<SRenderGraphImageView>(textureView.parentResource);
                        if(not parentTextureViewFetch.successful())
                        {
                            CLog::Error(logTag(), CString::format("Fetching parent texture view  w/ id {} failed.", textureView.parentResource));
                            return EEngineStatus::ResourceError_NotFound;
                        }

                        // If the parent texture view is null, the parent is a texture object.
                        Shared<SRenderGraphImageView> const parentTextureView = (parentTextureViewFetch.data());

                        CEngineResult<Shared<SRenderGraphImage>> const textureFetch = aRenderGraphResources.getResource<SRenderGraphImage>(textureView.subjacentResource);
                        if(not textureFetch.successful())
                        {
                            CLog::Error(logTag(), CString::format("Fetching texture w/ id {} failed.", textureView.subjacentResource));
                            return EEngineStatus::ResourceError_NotFound;
                        }

                        SRenderGraphImage const &texture = *(textureFetch.data());

                        // Validation first!
                        bool dimensionsValid = true;
                        if(0 > width)
                        {
                            width  = static_cast<int32_t>(texture.description.dynamicImage.width);
                            height = static_cast<int32_t>(texture.description.dynamicImage.height);
                            layers = textureView.description.arraySliceRange.length;

                            dimensionsValid = (0 < width and 0 < height and 0 < layers);
                        }
                        else
                        {
                            bool const validWidth  = (width  == static_cast<int32_t>(texture.description.dynamicImage.width));
                            bool const validHeight = (height == static_cast<int32_t>(texture.description.dynamicImage.height));
                            bool const validLayers = (layers == static_cast<int32_t>(textureView.description.arraySliceRange.length));

                            dimensionsValid = (validWidth and validHeight and validLayers);
                        }

                        if(not dimensionsValid)
                        {
                            EngineStatusPrintOnError(EEngineStatus::RenderGraph_RenderContext_AttachmentDimensionsInvalid, logTag(), "Invalid image view dimensions for frame buffer creation.");
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

                        bool const isColorAttachment = findAttachmentRelationFn(aAttachmentInfo.getAttachmentImageViewResourceIds(), aAttachmentInfo.getColorAttachments(), textureView.resourceId);
                        bool const isDepthAttachment = findAttachmentRelationFn(aAttachmentInfo.getAttachmentImageViewResourceIds(), aAttachmentInfo.getDepthAttachments(), textureView.resourceId);
                        bool const isInputAttachment = findAttachmentRelationFn(aAttachmentInfo.getAttachmentImageViewResourceIds(), aAttachmentInfo.getInputAttachments(), textureView.resourceId);

                        if(nullptr != parentTextureView && ERenderGraphResourceType::ImageView == parentTextureView->type)
                        {
                            bool const isParentColorAttachment = findAttachmentRelationFn(aAttachmentInfo.getAttachmentImageViewResourceIds(), aAttachmentInfo.getColorAttachments(), parentTextureView->resourceId);
                            bool const isParentDepthAttachment = findAttachmentRelationFn(aAttachmentInfo.getAttachmentImageViewResourceIds(), aAttachmentInfo.getDepthAttachments(), parentTextureView->resourceId);
                            bool const isParentInputAttachment = findAttachmentRelationFn(aAttachmentInfo.getAttachmentImageViewResourceIds(), aAttachmentInfo.getInputAttachments(), parentTextureView->resourceId);

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
                        else if(nullptr != parentTextureView && ERenderGraphResourceType::Image == parentTextureView->type)
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
                            if(textureView.description.mode.check(ERenderGraphViewAccessMode::Read))
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
                            attachmentDesc.format         = texture.description.dynamicImage.format;

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

                        auto const predicate = [textureView] (std::string const &aViewId) -> bool { return (aViewId == textureView.readableName); };

                        if(renderPassDesc.attachmentTextureViews.end() == std::find_if( renderPassDesc.attachmentTextureViews.begin()
                                                                                        , renderPassDesc.attachmentTextureViews.end()
                                                                                        , predicate))
                        {
                            renderPassDesc.attachmentTextureViews.push_back(textureView.readableName);
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
                    CEngineResult<OptRef_t<RenderPassResourceState_t>> renderPassObject = aResourceManager->useResource<RenderPassResourceState_t>(renderPassDesc.name, renderPassDesc, true);
                    EngineStatusPrintOnError(renderPassObject.result(), logTag(), "Failed to create render pass.");
                    SHIRABE_RETURN_RESULT_ON_ERROR(renderPassObject.result());
                }

                return EEngineStatus::Ok;
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------F------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto destroyRenderPass(Shared<CVulkanEnvironment>      aVulkanEnvironment
                                   , Shared<CResourceManager>      aResourceManager
                                   , Shared<asset::CAssetStorage>  aAssetStorage
                                   , ResourceId_t           const &aRenderPassId) -> EEngineStatus
            {
                CEngineResult<> const deinitialized = aResourceManager->discardResource<RenderPassResourceState_t>(aRenderPassId, aVulkanEnvironment.get());
                EngineStatusPrintOnError(deinitialized.result(), logTag(), "Failed to destroy render pass.");
                SHIRABE_RETURN_RESULT_ON_ERROR(deinitialized.result());
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto createFrameBuffer(Shared<CVulkanEnvironment>      aVulkanEnvironment
                                   , Shared<CResourceManager>      aResourceManager
                                   , Shared<asset::CAssetStorage>  aAssetStorage
                                   , ResourceId_t           const &aFrameBufferId
                                   , ResourceId_t           const &aRenderPassId) -> EEngineStatus
            {
                SFrameBufferDescription desc {};
                desc.name                 = aFrameBufferId;
                desc.renderPassResourceId = aRenderPassId;
                {
                    CEngineResult<OptionalRef_t<FrameBufferResourceState_t>> frameBufferObject = aResourceManager->useResource<FrameBufferResourceState_t>(desc.name, desc, true);
                    EngineStatusPrintOnError(frameBufferObject.result(), logTag(), "Failed to create framebuffer.");
                    SHIRABE_RETURN_RESULT_ON_ERROR(frameBufferObject.result());
                }

                return EEngineStatus::Ok;
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto destroyFrameBuffer(Shared<CVulkanEnvironment>      aVulkanEnvironment
                                    , Shared<CResourceManager>      aResourceManager
                                    , Shared<asset::CAssetStorage>  aAssetStorage
                                    , ResourceId_t           const &aFrameBufferId) -> EEngineStatus
            {
                CEngineResult<> const deinitialized = aResourceManager->discardResource<FrameBufferResourceState_t>(aFrameBufferId, aVulkanEnvironment.get());
                EngineStatusPrintOnError(deinitialized.result(), logTag(), "Failed to destroy framebuffer.");
                SHIRABE_RETURN_RESULT_ON_ERROR(deinitialized.result());

                return EEngineStatus::Ok;
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto createTransientTexture(Shared<CVulkanEnvironment>      aVulkanEnvironment
                                        , Shared<CResourceManager>      aResourceManager
                                        , Shared<asset::CAssetStorage>  aAssetStorage
                                        , SRenderGraphImage const      &aTexture) -> EEngineStatus
            {
                STextureDescription desc = {};
                desc.name        = aTexture.readableName;
                desc.textureInfo = static_cast<graphicsapi::STextureInfo>(aTexture.description.dynamicImage);
                // Always set those...
                desc.gpuBinding.set(EBufferBinding::CopySource);
                desc.gpuBinding.set(EBufferBinding::CopyTarget);

                if(aTexture.description.dynamicImage.requestedUsage.check(ERenderGraphResourceUsage::ColorAttachment))
                {
                    desc.gpuBinding.set(EBufferBinding::ColorAttachment);
                }

                if(aTexture.description.dynamicImage.requestedUsage.check(ERenderGraphResourceUsage::DepthAttachment))
                {
                    desc.gpuBinding.set(EBufferBinding::DepthAttachment);
                }

                if(aTexture.description.dynamicImage.requestedUsage.check(ERenderGraphResourceUsage::InputAttachment))
                {
                    desc.gpuBinding.set(EBufferBinding::InputAttachment);
                }

                desc.cpuGpuUsage = EResourceUsage::CPU_None_GPU_ReadWrite;

                {
                    CEngineResult<TextureResourceState_t> textureObject = aResourceManager->useResource<TextureResourceState_t>(desc.name, desc, true);
                    if( EEngineStatus::ResourceManager_ResourceAlreadyCreated == textureObject.result())
                    {
                        return EEngineStatus::Ok;
                    }
                    else
                    {
                        EngineStatusPrintOnError(textureObject.result(), logTag(), "Failed to create texture.");
                    }
                }
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto destroyTransientTexture(Shared<CVulkanEnvironment>     aVulkanEnvironment
                                         , Shared<CResourceManager>     aResourceManager
                                         , Shared<asset::CAssetStorage> aAssetStorage
                                         , SRenderGraphImage const     &aTexture) -> EEngineStatus
            {
                CEngineResult<> const deinitialized = aResourceManager->discardResource<TextureResourceState_t>(aTexture.readableName, aVulkanEnvironment.get());
                EngineStatusPrintOnError(deinitialized.result(), logTag(), "Failed to destroy transient texture.");
                SHIRABE_RETURN_RESULT_ON_ERROR(deinitialized.result());

                return EEngineStatus::Ok;
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto initializePersistentTexture(Shared<CVulkanEnvironment>      aVulkanEnvironment
                                             , Shared<CResourceManager>      aResourceManager
                                             , Shared<asset::CAssetStorage>  aAssetStorage
                                             , SRenderGraphImage      const &aTexture) -> EEngineStatus
            {
                {
                    CEngineResult<> const status = aResourceManager->initializeResource<TextureResourceState_t>(aTexture.readableName, aVulkanEnvironment.get());
                    EngineStatusPrintOnError(status.result(), logTag(), "Failed to initialize persistent texture.");
                    SHIRABE_RETURN_RESULT_ON_ERROR(status.result());
                }

                return EEngineStatus::Ok;
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto updatePersistentTexture(Shared<CVulkanEnvironment>    aVulkanEnvironment
                                       , Shared<CResourceManager>      aResourceManager
                                       , Shared<asset::CAssetStorage>  aAssetStorage
                                       , SRenderGraphImage      const &aTexture) -> EEngineStatus
            {
                {
                    CEngineResult<> const status = aResourceManager->updateResource<TextureResourceState_t>(aTexture.readableName, aVulkanEnvironment.get());
                    EngineStatusPrintOnError(status.result(), logTag(), "Failed to update persistent texture.");
                    SHIRABE_RETURN_RESULT_ON_ERROR(status.result());
                }

                return EEngineStatus::Ok;
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto deinitializePersistentTexture(Shared<CVulkanEnvironment>    aVulkanEnvironment
                                             , Shared<CResourceManager>      aResourceManager
                                             , Shared<asset::CAssetStorage>  aAssetStorage
                                             , SRenderGraphImage      const &aTexture) -> EEngineStatus
            {
                {
                    CEngineResult<> const status = aResourceManager->deinitializeResource<TextureResourceState_t>(aTexture.readableName, aVulkanEnvironment.get());
                    EngineStatusPrintOnError(status.result(), logTag(), "Failed to deinitialize persistent texture.");
                    SHIRABE_RETURN_RESULT_ON_ERROR(status.result());
                }

                return EEngineStatus::Ok;
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto createTextureView(Shared<CVulkanEnvironment>      aVulkanEnvironment
                                   , Shared<CResourceManager>      aResourceManager
                                   , Shared<asset::CAssetStorage>  aAssetStorage
                                   , std::string            const &aTextureName
                                   , SRenderGraphImageView  const &aView) -> EEngineStatus
            {
                STextureViewDescription desc = { };
                desc.name               = aView.readableName;
                desc.textureFormat      = aView.description.format;
                desc.subjacentTextureId = aTextureName;
                desc.arraySlices        = aView.description.arraySliceRange;
                desc.mipMapSlices       = aView.description.mipSliceRange;

                CEngineResult<TextureViewResourceState_t> textureViewObject = aResourceManager->useResource<TextureViewResourceState_t>(desc.name, desc, true);
                EngineStatusPrintOnError(textureViewObject.result(), logTag(), "Failed to create texture view.");
                SHIRABE_RETURN_RESULT_ON_ERROR(textureViewObject.result());

                return EEngineStatus::Ok;
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto destroyTextureView(Shared<CVulkanEnvironment>      aVulkanEnvironment
                                    , Shared<CResourceManager>      aResourceManager
                                    , Shared<asset::CAssetStorage>  aAssetStorage
                                    , SRenderGraphImageView const &aTextureView) -> EEngineStatus
            {
                CEngineResult<> const deinitialized = aResourceManager->discardResource<TextureViewResourceState_t>(aTextureView.readableName, aVulkanEnvironment.get());
                EngineStatusPrintOnError(deinitialized.result(), logTag(), "Failed to destroy textureview.");
                SHIRABE_RETURN_RESULT_ON_ERROR(deinitialized.result());

                return EEngineStatus::Ok;
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto createTransientBuffer(Shared<CVulkanEnvironment>   aVulkanEnvironment
                                     , Shared<CResourceManager>     aResourceManager
                                     , Shared<asset::CAssetStorage> aAssetStorage
                                     , SRenderGraphBuffer const    &aBuffer) -> EEngineStatus
            {
                SBufferDescription desc = { };
                desc.name = aBuffer.readableName;

                VkBufferCreateInfo &createInfo = desc.createInfo;
                createInfo.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                createInfo.pNext                 = nullptr;
                createInfo.flags                 = 0;
                createInfo.usage                 = aBuffer.description.dynamicBuffer.bufferUsage;
                createInfo.size                  = aBuffer.description.dynamicBuffer.sizeInBytes;
                // Determined in backend
                // createInfo.sharingMode           = ...;
                // createInfo.queueFamilyIndexCount = ...;
                // createInfo.pQueueFamilyIndices   = ...;

                CEngineResult<BufferResourceState_t> bufferObject = aResourceManager->useResource<BufferResourceState_t>(desc.name, desc, true);
                EngineStatusPrintOnError(bufferObject.result(), logTag(), "Failed to create buffer.");
                SHIRABE_RETURN_RESULT_ON_ERROR(bufferObject.result());
                return EEngineStatus::Ok;
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto destroyTransientBuffer(Shared<CVulkanEnvironment>    aVulkanEnvironment
                                      , Shared<CResourceManager>      aResourceManager
                                      , Shared<asset::CAssetStorage>  aAssetStorage
                                      , SRenderGraphBuffer const     &aBuffer) -> EEngineStatus
            {
                CEngineResult<> const deinitialization = aResourceManager->discardResource<BufferResourceState_t>(aBuffer.readableName, aVulkanEnvironment);
                EngineStatusPrintOnError(deinitialization.result(), logTag(), "Failed to destroy buffer.");
                SHIRABE_RETURN_RESULT_ON_ERROR(deinitialization.result());
                return EEngineStatus::Ok;
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto initializePersistentBuffer(Shared<CVulkanEnvironment>   aVulkanEnvironment
                                          , Shared<CResourceManager>     aResourceManager
                                          , Shared<asset::CAssetStorage> aAssetStorage
                                          , SRenderGraphBuffer const    &aBuffer) -> EEngineStatus
            {
                {
                    auto const &[successCode] = aResourceManager->initializeResource<BufferResourceState_t>(aBuffer.readableName, aVulkanEnvironment);
                    EngineStatusPrintOnError(successCode, logTag(), "Failed to initialize persistent buffer.");
                    SHIRABE_RETURN_RESULT_ON_ERROR(successCode);
                }

                return EEngineStatus::Ok;
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto updatePersistentBuffer(Shared<CVulkanEnvironment>   aVulkanEnvironment
                                      , Shared<CResourceManager>     aResourceManager
                                      , Shared<asset::CAssetStorage> aAssetStorage
                                      , SRenderGraphBuffer const    &aBuffer) -> EEngineStatus
            {
                CEngineResult<> const bufferTransfer = aResourceManager->uploadResource<BufferResourceState_t>(aBuffer.readableName, aVulkanEnvironment);
                EngineStatusPrintOnError(bufferTransfer.result(), logTag(), "Buffer initialization failed.");
                SHIRABE_RETURN_RESULT_ON_ERROR(bufferTransfer.result());

                return EEngineStatus::Ok;
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto deinitializePersistentBuffer(Shared<CVulkanEnvironment>   aVulkanEnvironment
                                            , Shared<CResourceManager>     aResourceManager
                                            , Shared<asset::CAssetStorage> aAssetStorage
                                            , SRenderGraphBuffer const    &aBuffer) -> EEngineStatus
            {
                {
                    auto const &[successCode] = aResourceManager->deinitializeResource<TextureResourceState_t>(aBuffer.readableName, aVulkanEnvironment);
                    EngineStatusPrintOnError(successCode, logTag(), "Failed to deinitialize persistent buffer.");
                    SHIRABE_RETURN_RESULT_ON_ERROR(successCode);
                }

                return EEngineStatus::Ok;
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto createBufferView(Shared<CVulkanEnvironment>      aVulkanEnvironment
                                  , Shared<CResourceManager>      aResourceManager
                                  , Shared<asset::CAssetStorage>  aAssetStorage
                                  , std::string             const &aBufferName
                                  , SRenderGraphBufferView  const &aView) -> EEngineStatus
            {
                SBufferViewDescription desc = { };
                desc.name = aBufferName;

                VkBufferViewCreateInfo &createInfo = desc.createInfo;
                createInfo.sType  = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
                createInfo.pNext  = nullptr;
                createInfo.flags  = 0;
                // createInfo.offset = "...";
                // createInfo.buffer = "...";
                // createInfo.format = "...";
                // createInfo.range  = "...";

                auto const &[success, resource] = aResourceManager->useResource<BufferViewResourceState_t>(desc.name, desc);
                EngineStatusPrintOnError(success, logTag(), "Failed to create buffer view.");
                SHIRABE_RETURN_RESULT_ON_ERROR(success);
                return EEngineStatus::Ok;
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto destroyBufferView(Shared<CVulkanEnvironment>      aVulkanEnvironment
                                   , Shared<CResourceManager>      aResourceManager
                                   , Shared<asset::CAssetStorage>  aAssetStorage
                                   , SRenderGraphBufferView  const &aView) -> EEngineStatus
            {
                CEngineResult<> const deinitialization = aResourceManager->discardResource<BufferViewResourceState_t>(aView.readableName, aVulkanEnvironment);
                EngineStatusPrintOnError(deinitialization.result(), logTag(), "Failed to deinitialize buffer.");
                SHIRABE_RETURN_RESULT_ON_ERROR(deinitialization.result());
                return EEngineStatus::Ok;
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto initializeMesh(Shared<CVulkanEnvironment>      aVulkanEnvironment
                                , Shared<CResourceManager>      aResourceManager
                                , Shared<asset::CAssetStorage>  aAssetStorage
                                , SRenderGraphMesh        const &aMesh) -> EEngineStatus
            {
                OptRef_t<MeshResourceState_t> meshOpt {};
                {
                    auto [success, resource] = aResourceManager->getResource<MeshResourceState_t>(aMesh.description.meshResourceId, aVulkanEnvironment);
                    if(CheckEngineError(success))
                    {
                        return EEngineStatus::Ok;
                    }
                    meshOpt = resource;
                }
                MeshResourceState_t &mesh = *meshOpt;

                auto const &[attributeBufferOpSuccessCode] = aResourceManager->initializeResource<BufferResourceState_t>(mesh.description.attributeBufferDesc.name, aVulkanEnvironment);
                if(CheckEngineError(attributeBufferOpSuccessCode))
                {
                    return EEngineStatus::InitializationError;
                }
                else
                {
                    return EEngineStatus::Ok;
                }
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto updateMesh(Shared<CVulkanEnvironment>      aVulkanEnvironment
                            , Shared<CResourceManager>      aResourceManager
                            , Shared<asset::CAssetStorage>  aAssetStorage
                            , SRenderGraphMesh        const &aMesh) -> EEngineStatus
            {
                OptRef_t<MeshResourceState_t> meshOpt {};
                {
                    auto [success, resource] = aResourceManager->getResource<MeshResourceState_t>(aMesh.description.meshResourceId, aVulkanEnvironment);
                    if(CheckEngineError(success))
                    {
                        return EEngineStatus::Ok;
                    }
                    meshOpt = resource;
                }
                MeshResourceState_t &mesh = *meshOpt;

                auto const &[attributeBufferOpSuccessCode] = aResourceManager->updateResource<BufferResourceState_t>(mesh.description.attributeBufferDesc.name, aVulkanEnvironment);
                if(CheckEngineError(attributeBufferOpSuccessCode))
                {
                    return EEngineStatus::InitializationError;
                }
                else
                {
                    return EEngineStatus::Ok;
                }
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto deinitializeMesh(Shared<CVulkanEnvironment>    aVulkanEnvironment
                                , Shared<CResourceManager>      aResourceManager
                                , Shared<asset::CAssetStorage>  aAssetStorage
                                , SRenderGraphMesh        const &aMesh) -> EEngineStatus
            {
                OptRef_t<MeshResourceState_t> meshOpt {};
                {
                    auto [success, resource] = aResourceManager->getResource<MeshResourceState_t>(aMesh.description.meshResourceId, aVulkanEnvironment);
                    if(CheckEngineError(success))
                    {
                        return EEngineStatus::Ok;
                    }
                    meshOpt = resource;
                }
                MeshResourceState_t &mesh = *meshOpt;

                auto const &[attributeBufferOpSuccessCode] = aResourceManager->deinitializeResource<BufferResourceState_t>(mesh.description.attributeBufferDesc.name, aVulkanEnvironment);
                if(CheckEngineError(attributeBufferOpSuccessCode))
                {
                    return EEngineStatus::InitializationError;
                }
                else
                {
                    return EEngineStatus::Ok;
                }
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto initializeMaterial(Shared<CVulkanEnvironment>    aVulkanEnvironment
                                  , Shared<CResourceManager>      aResourceManager
                                  , Shared<asset::CAssetStorage>  aAssetStorage
                                  , SRenderGraphMaterial    const &aMaterial) -> EEngineStatus
            {
                auto const result0 = aResourceManager->initializeResource<PipelineResourceState_t>(aMaterial.description.sharedMaterialResourceId, aVulkanEnvironment);
                auto const result1 = aResourceManager->initializeResource<MaterialResourceState_t>(aMaterial.description.materialResourceId, aVulkanEnvironment);

                for(auto const &buffer : aMaterial.description.buffers)
                    auto const result2 = aResourceManager->initializeResource<BufferResourceState_t>(buffer.bufferResourceId, aVulkanEnvironment);
                for(auto const &image : aMaterial.description.images)
                    auto const result3 = aResourceManager->initializeResource<TextureResourceState_t>(image.imageId);

                return EEngineStatus::Ok;
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto updateMaterial(Shared<CVulkanEnvironment>    aVulkanEnvironment
                              , Shared<CResourceManager>      aResourceManager
                              , Shared<asset::CAssetStorage>  aAssetStorage
                              , SRenderGraphMaterial    const &aMaterial) -> EEngineStatus
            {
                auto const result0 = aResourceManager->updateResource<MaterialResourceState_t>(aMaterial.description.sharedMaterialResourceId, aVulkanEnvironment);
                auto const result1 = aResourceManager->updateResource<MaterialResourceState_t>(aMaterial.description.materialResourceId, aVulkanEnvironment);

                for(auto const &buffer : aMaterial.description.buffers)
                    auto const result2 = aResourceManager->updateResource<BufferResourceState_t>(buffer.bufferResourceId, aVulkanEnvironment);
                for(auto const &image : aMaterial.description.images)
                    auto const result3 = aResourceManager->updateResource<TextureResourceState_t>(image.imageId);

                return EEngineStatus::Ok;
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto deinitializeMaterial(Shared<CVulkanEnvironment>    aVulkanEnvironment
                                    , Shared<CResourceManager>      aResourceManager
                                    , Shared<asset::CAssetStorage>  aAssetStorage
                                    , SRenderGraphMaterial    const &aMaterial) -> EEngineStatus
            {
                auto const result0 = aResourceManager->deinitializeResource<MaterialResourceState_t>(aMaterial.description.sharedMaterialResourceId, aVulkanEnvironment);
                auto const result1 = aResourceManager->deinitializeResource<MaterialResourceState_t>(aMaterial.description.materialResourceId, aVulkanEnvironment);

                for(auto const &buffer : aMaterial.description.buffers)
                    auto const result2 = aResourceManager->deinitializeResource<BufferResourceState_t>(buffer.bufferResourceId, aVulkanEnvironment);
                for(auto const &image : aMaterial.description.images)
                    auto const result3 = aResourceManager->deinitializeResource<TextureResourceState_t>(image.imageId);

                return EEngineStatus::Ok;
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto createPipeline(Shared<CVulkanEnvironment>       aVulkanEnvironment
                                , Shared<CResourceManager>       aResourceManager
                                , Shared<asset::CAssetStorage>   aAssetStorage
                                , SRenderGraphPipeline    const &aPipeline
                                , resources::ResourceId_t const &aRenderPassResourceId
                                , uint32_t const                &aSubpassIndex) -> EEngineStatus
            {
                auto const configureInputAssembly =
                               [] (SMaterialPipelineDescriptor &aPipelineDescriptor) -> void
                                   {
                                       aPipelineDescriptor.inputAssemblyState.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
                                       aPipelineDescriptor.inputAssemblyState.pNext                  = nullptr;
                                       aPipelineDescriptor.inputAssemblyState.flags                  = 0;
                                       aPipelineDescriptor.inputAssemblyState.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                                       aPipelineDescriptor.inputAssemblyState.primitiveRestartEnable = false;
                                   };

                auto const configureRasterizer =
                               [] (SMaterialPipelineDescriptor &aPipelineDescriptor) -> void
                                   {
                                       aPipelineDescriptor.rasterizerState.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
                                       aPipelineDescriptor.rasterizerState.pNext                   = nullptr;
                                       aPipelineDescriptor.rasterizerState.flags                   = 0;
                                       aPipelineDescriptor.rasterizerState.cullMode                = VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
                                       aPipelineDescriptor.rasterizerState.frontFace               = VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;
                                       aPipelineDescriptor.rasterizerState.polygonMode             = VkPolygonMode::VK_POLYGON_MODE_FILL;
                                       aPipelineDescriptor.rasterizerState.lineWidth               = 1.0f;
                                       aPipelineDescriptor.rasterizerState.rasterizerDiscardEnable = VK_FALSE; // isCoreMaterial ? VK_TRUE : VK_FALSE;
                                       aPipelineDescriptor.rasterizerState.depthClampEnable        = VK_FALSE;
                                       aPipelineDescriptor.rasterizerState.depthBiasEnable         = VK_FALSE;
                                       aPipelineDescriptor.rasterizerState.depthBiasSlopeFactor    = 0.0f;
                                       aPipelineDescriptor.rasterizerState.depthBiasConstantFactor = 0.0f;
                                       aPipelineDescriptor.rasterizerState.depthBiasClamp          = 0.0f;
                                   };

                auto const configureMultisampler =
                               [] (SMaterialPipelineDescriptor &aPipelineDescriptor) -> void
                                   {
                                       aPipelineDescriptor.multiSampler.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
                                       aPipelineDescriptor.multiSampler.pNext                 = nullptr;
                                       aPipelineDescriptor.multiSampler.flags                 = 0;
                                       aPipelineDescriptor.multiSampler.sampleShadingEnable   = VK_FALSE;
                                       aPipelineDescriptor.multiSampler.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
                                       aPipelineDescriptor.multiSampler.minSampleShading      = 1.0f;
                                       aPipelineDescriptor.multiSampler.pSampleMask           = nullptr;
                                       aPipelineDescriptor.multiSampler.alphaToCoverageEnable = VK_FALSE;
                                       aPipelineDescriptor.multiSampler.alphaToOneEnable      = VK_FALSE;
                                   };

                auto const configureDepthStencil =
                               [] (SMaterialPipelineDescriptor &aPipelineDescriptor) -> void
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

                SMaterialPipelineDescriptor descriptor;
                descriptor.name                  = aPipeline.readableName;
                descriptor.includesSystemBuffers = true; // This function will only create pipelines of non-core materials.

                configureInputAssembly(descriptor);
                configureRasterizer(descriptor);
                configureMultisampler(descriptor);
                configureDepthStencil(descriptor);

                auto const &[fetchSharedMaterialStatus, sharedMaterialOptRef] = aResourceManager->template getResource<BasePipelineResourceState_t>(aPipeline.sharedMaterialResourceId, aVulkanEnvironment);
                if(CheckEngineError(fetchSharedMaterialStatus))
                {
                    return fetchSharedMaterialStatus;
                }
                BasePipelineResourceState_t const &sharedMaterial = *sharedMaterialOptRef;

                // for(auto const &[stageKey, stage] : aMaster.stages())
                // {
                //     if(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_VERTEX_SHADER_BIT == stageKey)
                //     {
                //         std::vector<SStageInput> stageInputs(stage.inputs);
                //         std::sort(stageInputs.begin(), stageInputs.end(), [] (SStageInput const &aLHS, SStageInput const &aRHS) -> bool { return aLHS.location < aRHS.location; });
//
                //         for(std::size_t k=0; k<stage.inputs.size(); ++k)
                //         {
                //             SStageInput const &input = stageInputs.at(k);
//
                //             // This number has to be equal to the VkVertexInputBindingDescription::binding index which data should be taken from!
                //             VkVertexInputBindingDescription binding;
                //             binding.binding   = input.location;
                //             binding.stride    = (input.type->byteSize * input.type->vectorSize);
                //             binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
//
                //             VkVertexInputAttributeDescription attribute;
                //             attribute.binding  = k;
                //             attribute.location = input.location;
                //             attribute.offset   = 0;
                //             attribute.format   = (8 == binding.stride)
                //                                  ? VkFormat::VK_FORMAT_R32G32_SFLOAT
                //                                  : (12 == binding.stride)
                //                                    ? VkFormat::VK_FORMAT_R32G32B32_SFLOAT
                //                                    : (16 == binding.stride)
                //                                      ? VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT
                //                                      : VkFormat::VK_FORMAT_UNDEFINED;
//
                //             pipelineDescriptor.vertexInputBindings  .push_back(binding);
                //             pipelineDescriptor.vertexInputAttributes.push_back(attribute);
                //         }
                //     }
//
                //     if(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT == stageKey)
                //     {
                //         std::vector<VkPipelineColorBlendAttachmentState> outputs {};
                //         outputs.resize(stage.outputs.size());
//
                //         for(auto const &output : stage.outputs)
                //         {
                //             VkPipelineColorBlendAttachmentState colorBlendAttachmentState{};
                //             colorBlendAttachmentState.blendEnable         = VK_TRUE;
                //             colorBlendAttachmentState.colorWriteMask      = VkColorComponentFlagBits::VK_COLOR_COMPONENT_R_BIT|
                //                                                             VkColorComponentFlagBits::VK_COLOR_COMPONENT_G_BIT|
                //                                                             VkColorComponentFlagBits::VK_COLOR_COMPONENT_B_BIT|
                //                                                             VkColorComponentFlagBits::VK_COLOR_COMPONENT_A_BIT;
                //             colorBlendAttachmentState.srcColorBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE;  // VkBlendFactor::VK_BLEND_FACTOR_SRC_ALPHA;
                //             colorBlendAttachmentState.dstColorBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ZERO; // VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
                //             colorBlendAttachmentState.colorBlendOp        = VkBlendOp::VK_BLEND_OP_ADD;
                //             colorBlendAttachmentState.srcAlphaBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE;
                //             colorBlendAttachmentState.dstAlphaBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ZERO;
                //             colorBlendAttachmentState.alphaBlendOp        = VkBlendOp::VK_BLEND_OP_ADD;
//
                //             outputs[output.location] = colorBlendAttachmentState;
                //         }
//
                //         pipelineDescriptor.colorBlendAttachmentStates = outputs;
//
                //         VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo {};
                //         colorBlendCreateInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
                //         colorBlendCreateInfo.pNext             = nullptr;
                //         colorBlendCreateInfo.flags             = 0;
                //         colorBlendCreateInfo.logicOpEnable     = VK_FALSE;
                //         colorBlendCreateInfo.logicOp           = VK_LOGIC_OP_COPY;
                //         colorBlendCreateInfo.blendConstants[0] = 0.0f;
                //         colorBlendCreateInfo.blendConstants[1] = 1.0f;
                //         colorBlendCreateInfo.blendConstants[2] = 2.0f;
                //         colorBlendCreateInfo.blendConstants[3] = 3.0f;
//
                //         pipelineDescriptor.colorBlendState = colorBlendCreateInfo;
                //     }
//
                //     //
                //     // Derive data accessors for shader module creation from sharedMaterial material
                //     //
                //     std::filesystem::path const  stageSpirVFilename = stage.filename;
                //     bool                  const  isEmptyFilename    = stageSpirVFilename.empty();
                //     if(not isEmptyFilename)
                //     {
                //         DataSourceAccessor_t dataAccessor = [=] () -> ByteBuffer
                //             {
                //                 asset::AssetID_t const assetUid = asset::assetIdFromUri(stageSpirVFilename);
//
                //                 auto const [result, buffer] = aAssetStorage->loadAssetData(assetUid);
                //                 if(CheckEngineError(result))
                //                 {
                //                     CLog::Error("DataSourceAccessor_t::ShaderModule", "Failed to load shader module asset data. Result: {}", result);
                //                     return {};
                //                 }
//
                //                 return buffer;
                //             };
//
                //         shaderModuleDescriptor.shaderStages[stageKey] = dataAccessor;
                //     }
                // }
//
                // uint32_t const setSubtractionValue = aIncludeSystemBuffers ? 0 : 2;
                // uint32_t const setCount            = aMaster.layoutInfo().setCount - setSubtractionValue;
//
                // std::vector<VkDescriptorSetLayoutCreateInfo> descriptorSets {};
                // descriptorSets.resize(setCount);
                // pipelineDescriptor.descriptorSetLayoutBindings.resize(setCount);
//
                // for(std::size_t k=0; k<descriptorSets.size(); ++k)
                // {
                //     VkDescriptorSetLayoutCreateInfo &info = descriptorSets[k];
//
                //     info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                //     info.pNext        = nullptr;
                //     info.flags        = 0;
                //     info.bindingCount = aMaster.layoutInfo().setBindingCount[k + setSubtractionValue];
//
                //     pipelineDescriptor.descriptorSetLayoutBindings[k].resize(info.bindingCount);
                // }
//
                // pipelineDescriptor.descriptorSetLayoutCreateInfos = descriptorSets;
//
                // for(SSubpassInput const &input : aMaster.subpassInputs())
                // {
                //     if(not aIncludeSystemBuffers && 2 > input.set)
                //     {
                //         continue;
                //     }
//
                //     VkDescriptorSetLayoutBinding layoutBinding {};
                //     layoutBinding.binding            = input.binding;
                //     layoutBinding.descriptorType     = VkDescriptorType::VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
                //     layoutBinding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT; // Subpass inputs are only accessibly in fragment shaders.
                //     layoutBinding.descriptorCount    = 1;
                //     layoutBinding.pImmutableSamplers = nullptr;
                //     pipelineDescriptor.descriptorSetLayoutBindings[input.set - setSubtractionValue][input.binding] = layoutBinding;
                // }
//
                // for(SUniformBuffer const &uniformBuffer : aMaster.uniformBuffers())
                // {
                //     if(not aIncludeSystemBuffers && 2 > uniformBuffer.set)
                //     {
                //         continue;
                //     }
//
                //     VkDescriptorSetLayoutBinding layoutBinding {};
                //     layoutBinding.binding            = uniformBuffer.binding;
                //     layoutBinding.descriptorType     = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                //     layoutBinding.stageFlags         = VkShaderStageFlagBits::VK_SHADER_STAGE_ALL; // serialization::shaderStageFromPipelineStage(uniformBuffer.stageBinding.value());
                //     layoutBinding.descriptorCount    = uniformBuffer.array.layers;
                //     layoutBinding.pImmutableSamplers = nullptr;
                //     pipelineDescriptor.descriptorSetLayoutBindings[uniformBuffer.set - setSubtractionValue][uniformBuffer.binding] = layoutBinding;
//
                //     CEngineResult<void const *const> bufferDataFetch = aInstance.config().getBuffer(uniformBuffer.name);
                //     if(CheckEngineError(bufferDataFetch.result()))
                //     {
                //         CLog::Debug("AssetLoader - Materials", "Can't find buffer w/ name {} in config.", uniformBuffer.name);
                //         continue;
                //     }
//
                //     auto  const *const data = static_cast<uint8_t const *const>(bufferDataFetch.data());
                //     std::size_t  const size = uniformBuffer.location.length;
//
                //     auto const dataSource =  [data, size] () -> ByteBuffer
                //         {
                //             return ByteBuffer(data, size);
                //         };
//
                //     SBufferDescription desc {};
                //     desc.name                             = fmt::format("{}_uniformbuffer_{}", aMaterialName, uniformBuffer.name);
                //     desc.dataSource                       = dataSource;
                //     desc.createInfo.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                //     desc.createInfo.pNext                 = nullptr;
                //     desc.createInfo.flags                 = 0;
                //     desc.createInfo.size                  = uniformBuffer.location.length;
                //     desc.createInfo.pQueueFamilyIndices   = nullptr;
                //     desc.createInfo.queueFamilyIndexCount = 0;
                //     desc.createInfo.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
                //     desc.createInfo.usage                 = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
//
                //     bufferDescriptions.push_back(desc);
                // }
//
                // for(SSampledImage const &sampledImage : aMaster.sampledImages())
                // {
                //     if(not aIncludeSystemBuffers && 2 > sampledImage.set)
                //     {
                //         continue;
                //     }
//
                //     VkDescriptorSetLayoutBinding layoutBinding {};
                //     layoutBinding.binding            = sampledImage.binding;
                //     layoutBinding.descriptorType     = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                //     layoutBinding.stageFlags         = serialization::shaderStageFromPipelineStage(sampledImage.stageBinding.value());
                //     layoutBinding.descriptorCount    = 1;
                //     layoutBinding.pImmutableSamplers = nullptr;
                //     pipelineDescriptor.descriptorSetLayoutBindings[sampledImage.set - setSubtractionValue][sampledImage.binding] = layoutBinding;
                // }
//
                // VkViewport viewPort = {};
                // viewPort.x        = 0.0;
                // viewPort.y        = 0.0;
                // viewPort.width    = 1920.0;
                // viewPort.height   = 1080.0;
                // viewPort.minDepth = 0.0;
                // viewPort.maxDepth = 1.0;
                // pipelineDescriptor.viewPort = viewPort;
//
                // return {true, pipelineDescriptor, shaderModuleDescriptor, bufferDescriptions };


                return EEngineStatus::Ok;
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto destroyPipeline(Shared<CVulkanEnvironment>      aVulkanEnvironment
                                 , Shared<CResourceManager>      aResourceManager
                                 , Shared<asset::CAssetStorage>  aAssetStorage
                                 , SRenderGraphPipeline    const &aMesh) -> EEngineStatus
            {
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        framegraph::SRenderGraphResourceContext CreateResourceContextForVulkan(Shared<CVulkanEnvironment>   aVulkanEnvironment
                                                                            , Shared<CResourceManager>     aResourceManager
                                                                            , Shared<asset::CAssetStorage> aAssetStorage)
        {
            using namespace local;
            using namespace resources;

            framegraph::SRenderGraphResourceContext context {};

            // --------------------------------------------------------------------------------------------
            // RenderPass
            // --------------------------------------------------------------------------------------------
            context.createRenderPass =
                [&] (ResourceId_t                      const &aRenderPassId
                     , std::vector<PassUID_t>          const &aPassExecutionOrder
                     , SRenderGraphAttachmentCollection const &aAttachmentInfo
                     , CRenderGraphMutableResources     const &aRenderGraphResources)
                    { return detail::createRenderPass(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                    , aRenderPassId
                                                    , aPassExecutionOrder
                                                    , aAttachmentInfo
                                                    , aRenderGraphResources); };
            context.destroyRenderPass =
                [&] (ResourceId_t const &aRenderPassId)
                    { return detail::destroyRenderPass(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                     , aRenderPassId); };

            // --------------------------------------------------------------------------------------------
            // FrameBuffer
            // --------------------------------------------------------------------------------------------
            context.createFrameBuffer =
                [&] (ResourceId_t   const &aFrameBufferId
                     , ResourceId_t const &aRenderPassId)
                    { return detail::createFrameBuffer(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                     , aFrameBufferId
                                                     , aRenderPassId); };
            context.destroyFrameBuffer =
                [&] (ResourceId_t const &aFrameBufferId)
                    { return detail::destroyFrameBuffer(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                      , aFrameBufferId); };

            // --------------------------------------------------------------------------------------------
            // Textures
            // --------------------------------------------------------------------------------------------
            context.createTransientTexture =
                [&](SRenderGraphImage const &aTexture)
                    { return detail::createTransientTexture(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                            , aTexture); };
            context.destroyTransientTexture =
                [&](SRenderGraphImage const &aTexture)
                    { return detail::destroyTransientTexture(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                             , aTexture); };
            context.initializePersistentImage =
                [&](SRenderGraphImage const &aTexture)
                    { return detail::initializePersistentTexture(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                             , aTexture); };
            context.updatePersistentTexture =
                [&](SRenderGraphImage const &aTexture)
                    { return detail::updatePersistentTexture(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                             , aTexture); };
            context.deinitializePersistentTexture =
                [&](SRenderGraphImage const &aTexture)
                    { return detail::deinitializePersistentTexture(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                             , aTexture); };
            context.createTextureView =
                [&](resources::ResourceId_t const   &aTexture
                     , SRenderGraphImageView  const &aView)
                    { return detail::createTextureView(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                      , aTexture
                                                      , aView); };
            context.destroyTextureView =
                [&](SRenderGraphImageView const &aTextureView)
                    { return detail::destroyTextureView(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                      , aTextureView); };

            // --------------------------------------------------------------------------------------------
            // Buffers
            // --------------------------------------------------------------------------------------------
            context.createTransientBuffer =
                [&](SRenderGraphBuffer const &aBuffer)
                    { return detail::createTransientBuffer(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                         , aBuffer); };
            context.destroyTransientBuffer =
                [&](SRenderGraphBuffer const &aBuffer)
                    { return detail::destroyTransientBuffer(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                          , aBuffer); };
            context.initializePersistentBuffer =
                [&](SRenderGraphBuffer const &aBuffer)
                    { return detail::initializePersistentBuffer(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                              , aBuffer); };
            context.updatePersistentBuffer =
                [&](SRenderGraphBuffer const &aBuffer)
                    { return detail::updatePersistentBuffer(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                          , aBuffer); };
            context.deinitializePersistentBuffer =
                [&](SRenderGraphBuffer const &aBuffer)
                    { return detail::deinitializePersistentBuffer(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                                , aBuffer); };
            context.createBufferView =
                [&] (resources::ResourceId_t const  &aBuffer
                     , SRenderGraphBufferView const &aView)
                    { return detail::createBufferView(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                    , aBuffer
                                                    , aView); };
            context.destroyBufferView =
                [&] (SRenderGraphBufferView const &aBufferView)
                    { return detail::destroyBufferView(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                     , aBufferView); };


            //---------------------------------------------------------------------------------------------------------------
            // Meshes
            //---------------------------------------------------------------------------------------------------------------
            context.initializeMesh =
                [&] (SRenderGraphMesh const &aMesh)
                    { return detail::initializeMesh(aVulkanEnvironment, aResourceManager, aAssetStorage
                                              , aMesh); };
            context.updateMesh =
                [&] (SRenderGraphMesh const &aMesh)
                    { return detail::updateMesh(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                , aMesh); };
            context.deinitializeMesh =
                [&] (SRenderGraphMesh const &aMesh)
                    { return detail::deinitializeMesh(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                    , aMesh); };

            //---------------------------------------------------------------------------------------------------------------
            // Materials
            //---------------------------------------------------------------------------------------------------------------
            context.initializeMaterial =
                [&] (SRenderGraphMaterial const &aMaterial)
                    { return detail::initializeMaterial(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                      , aMaterial); };
            context.updateMaterial =
                [&] (SRenderGraphMaterial const &aMaterial)
                    { return detail::updateMaterial(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                  , aMaterial); };
            context.deinitializeMaterial =
                [&] (SRenderGraphMaterial const &aMaterial)
                    { return detail::deinitializeMaterial(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                        , aMaterial); };

            //---------------------------------------------------------------------------------------------------------------
            // Pipelines
            //---------------------------------------------------------------------------------------------------------------
            context.createPipeline =
                [&] (SRenderGraphPipeline const      &aPipeline
                     , resources::ResourceId_t const &aRenderPassResourceId
                     , uint32_t const                &aSubpassIndex)
                    { return detail::createPipeline(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                   , aPipeline, aRenderPassResourceId, aSubpassIndex); };
            context.destroyPipeline =
                [&] (SRenderGraphPipeline const &aPipeline)
                    { return detail::destroyPipeline(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                     , aPipeline); };
        }
    }
}
