#include "vulkan_integration/rendering/vulkanresourcecontext.h"

#include <core/enginetypehelper.h>
#include <mesh/loader.h>
#include <mesh/declaration.h>
#include <material/loader.h>
#include <material/declaration.h>
#include <material/serialization.h>
#include <renderer/framegraph/framegraphcontexts.h>

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
                                  , SFrameGraphAttachmentCollection const &aAttachmentInfo
                                  , CFrameGraphMutableResources     const &aFrameGraphResources) -> EEngineStatus
            {
                //<-----------------------------------------------------------------------------
                // Helper function to find attachment indices in index lists.
                //<-----------------------------------------------------------------------------
                auto const findAttachmentRelationFn =
                               [] (Vector<FrameGraphResourceId_t> const &aResourceIdIndex,
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

                        CEngineResult<Shared<SFrameGraphTextureView>> const textureViewFetch = aFrameGraphResources.getResource<SFrameGraphTextureView>(resourceId);
                        if(not textureViewFetch.successful())
                        {
                            CLog::Error(logTag(), CString::format("Fetching texture view w/ id {} failed.", resourceId));
                            return EEngineStatus::ResourceError_NotFound;
                        }

                        SFrameGraphTextureView const &textureView = *(textureViewFetch.data());

                        CEngineResult<Shared<SFrameGraphTextureView>> const parentTextureViewFetch = aFrameGraphResources.getResource<SFrameGraphTextureView>(textureView.parentResource);
                        if(not parentTextureViewFetch.successful())
                        {
                            CLog::Error(logTag(), CString::format("Fetching parent texture view  w/ id {} failed.", textureView.parentResource));
                            return EEngineStatus::ResourceError_NotFound;
                        }

                        // If the parent texture view is null, the parent is a texture object.
                        Shared<SFrameGraphTextureView> const parentTextureView = (parentTextureViewFetch.data());

                        CEngineResult<Shared<SFrameGraphTexture>> const textureFetch = aFrameGraphResources.getResource<SFrameGraphTexture>(textureView.subjacentResource);
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
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------F------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto destroyRenderPass(Shared<CVulkanEnvironment>      aVulkanEnvironment
                                   , Shared<CResourceManager>      aResourceManager
                                   , Shared<asset::CAssetStorage>  aAssetStorage
                                   , ResourceId_t           const &aRenderPassId) -> EEngineStatus
            {
                CEngineResult<> const deinitialized = aResourceManager->discardResource<RenderPassResourceState_t>(aRenderPassId, aVulkanEnvironment);
                EngineStatusPrintOnError(deinitialized.result(), logTag(), "Failed to destroy render pass.");
                SHIRABE_RETURN_RESULT_ON_ERROR(deinitialized.result());
            };
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
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto destroyFrameBuffer(Shared<CVulkanEnvironment>      aVulkanEnvironment
                                    , Shared<CResourceManager>      aResourceManager
                                    , Shared<asset::CAssetStorage>  aAssetStorage
                                    , ResourceId_t           const &aFrameBufferId) -> EEngineStatus
            {
                CEngineResult<> const deinitialized = aResourceManager->discardResource<FrameBufferResourceState_t>(aFrameBufferId, aVulkanEnvironment);
                EngineStatusPrintOnError(deinitialized.result(), logTag(), "Failed to destroy framebuffer.");
                SHIRABE_RETURN_RESULT_ON_ERROR(deinitialized.result());

                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto createTransientTexture(Shared<CVulkanEnvironment>           aVulkanEnvironment
                                        , Shared<CResourceManager>           aResourceManager
                                        , Shared<asset::CAssetStorage>       aAssetStorage
                                        , SFrameGraphTransientTexture const &aTexture) -> EEngineStatus
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
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto destroyTransientTexture(Shared<CVulkanEnvironment>           aVulkanEnvironment
                                         , Shared<CResourceManager>           aResourceManager
                                         , Shared<asset::CAssetStorage>       aAssetStorage
                                         , SFrameGraphTransientTexture const &aTexture) -> EEngineStatus
            {
                CEngineResult<> const deinitialized = aResourceManager->discardResource<TextureResourceState_t>(aTexture.readableName, aVulkanEnvironment);
                EngineStatusPrintOnError(deinitialized.result(), logTag(), "Failed to destroy transient texture.");
                SHIRABE_RETURN_RESULT_ON_ERROR(deinitialized.result());

                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto initializePersistentTexture(Shared<CVulkanEnvironment>            aVulkanEnvironment
                                             , Shared<CResourceManager>            aResourceManager
                                             , Shared<asset::CAssetStorage>        aAssetStorage
                                             , SFrameGraphPersistentTexture const &aTexture) -> EEngineStatus
            {
                {
                    auto const &[successCode] = aResourceManager->initializeResource<TextureResourceState_t>(aTexture.readableName, aVulkanEnvironment);
                    EngineStatusPrintOnError(successCode, logTag(), "Failed to initialize persistent texture.");
                    SHIRABE_RETURN_RESULT_ON_ERROR(successCode);
                }

                return EEngineStatus::Ok;
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto updatePersistentTexture(Shared<CVulkanEnvironment>          aVulkanEnvironment
                                       , Shared<CResourceManager>            aResourceManager
                                       , Shared<asset::CAssetStorage>        aAssetStorage
                                       , SFrameGraphPersistentTexture const &aTexture) -> EEngineStatus
            {
                {
                    auto const &[successCode] = aResourceManager->updateResource<TextureResourceState_t>(aTexture.readableName, aVulkanEnvironment);
                    EngineStatusPrintOnError(successCode, logTag(), "Failed to update persistent texture.");
                    SHIRABE_RETURN_RESULT_ON_ERROR(successCode);
                }

                return EEngineStatus::Ok;
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto deinitializePersistentTexture(Shared<CVulkanEnvironment>          aVulkanEnvironment
                                             , Shared<CResourceManager>            aResourceManager
                                             , Shared<asset::CAssetStorage>        aAssetStorage
                                             , SFrameGraphPersistentTexture const &aTexture) -> EEngineStatus
            {
                {
                    auto const &[successCode] = aResourceManager->deinitializeResource<TextureResourceState_t>(aTexture.readableName, aVulkanEnvironment);
                    EngineStatusPrintOnError(successCode, logTag(), "Failed to deinitialize persistent texture.");
                    SHIRABE_RETURN_RESULT_ON_ERROR(successCode);
                }

                return EEngineStatus::Ok;
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto createTextureView(Shared<CVulkanEnvironment>         aVulkanEnvironment
                                   , Shared<CResourceManager>         aResourceManager
                                   , Shared<asset::CAssetStorage>     aAssetStorage
                                   , SFrameGraphTexture        const &aTexture
                                   , SFrameGraphTextureView    const &aView) -> EEngineStatus
            {
                STextureViewDescription desc = { };
                desc.name               = aView.readableName;
                desc.textureFormat      = aView.format;
                desc.subjacentTextureId = aTexture.readableName;
                desc.arraySlices        = aView.arraySliceRange;
                desc.mipMapSlices       = aView.mipSliceRange;

                CEngineResult<TextureViewResourceState_t> textureViewObject = aResourceManager->useResource<TextureViewResourceState_t>(desc.name, desc, true);
                EngineStatusPrintOnError(textureViewObject.result(), logTag(), "Failed to create texture view.");
                SHIRABE_RETURN_RESULT_ON_ERROR(textureViewObject.result());

                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto destroyTextureView(Shared<CVulkanEnvironment>      aVulkanEnvironment
                                    , Shared<CResourceManager>      aResourceManager
                                    , Shared<asset::CAssetStorage>  aAssetStorage
                                    , SFrameGraphTextureView const &aTextureView) -> EEngineStatus
            {
                CEngineResult<> const deinitialized = aResourceManager->discardResource<TextureViewResourceState_t>(aTextureView.readableName, aVulkanEnvironment);
                EngineStatusPrintOnError(deinitialized.result(), logTag(), "Failed to destroy textureview.");
                SHIRABE_RETURN_RESULT_ON_ERROR(deinitialized.result());

                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto createBuffer(Shared<CVulkanEnvironment>      aVulkanEnvironment
                              , Shared<CResourceManager>      aResourceManager
                              , Shared<asset::CAssetStorage>  aAssetStorage
                              , SFrameGraphBuffer      const &aBuffer) -> EEngineStatus
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

                CEngineResult<BufferResourceState_t> bufferObject = aResourceManager->useResource<BufferResourceState_t>(desc.name, desc);
                EngineStatusPrintOnError(bufferObject.result(), logTag(), "Failed to create buffer.");

                return bufferObject.result();
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto transferBuffer(Shared<CVulkanEnvironment>      aVulkanEnvironment
                                , Shared<CResourceManager>      aResourceManager
                                , Shared<asset::CAssetStorage>  aAssetStorage
                                , SFrameGraphBuffer      const &aBuffer) -> EEngineStatus
            {
                OptRef_t<BufferResourceState_t> bufferOpt {};
                {
                    auto [success, resource] = fetchResource<BufferResourceState_t>(aBuffer.readableName, aResourceManager, aVulkanEnvironment);
                    if(not success)
                    {
                        return EEngineStatus::Ok;
                    }
                    bufferOpt = resource;
                }
                BufferResourceState_t &buffer = *bufferOpt;

                CEngineResult<> const bufferTransfer = aResourceManager->uploadResource<BufferResourceState_t>(aBuffer.readableName, aVulkanEnvironment);
                EngineStatusPrintOnError(bufferTransfer.result(), logTag(), "Buffer initialization failed.");
                SHIRABE_RETURN_RESULT_ON_ERROR(bufferTransfer.result());

                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto destroyBuffer(Shared<CVulkanEnvironment>      aVulkanEnvironment
                               , Shared<CResourceManager>      aResourceManager
                               , Shared<asset::CAssetStorage>  aAssetStorage
                               , SFrameGraphBuffer      const &aBuffer) -> EEngineStatus
            {
                OptRef_t<BufferResourceState_t> resourceOpt {};
                {
                    auto [success, resource] = fetchResource<BufferResourceState_t>(aBuffer.readableName, aResourceManager, aVulkanEnvironment);
                    if(not success)
                    {
                        return EEngineStatus::Ok;
                    }
                    resourceOpt = resource;
                }
                BufferResourceState_t &buffer = *resourceOpt;

                CEngineResult<> const deinitialization = aResourceManager->discardResource<BufferResourceState_t>(aBuffer.readableName, aVulkanEnvironment);
                EngineStatusPrintOnError(deinitialization.result(), logTag(), "Failed to deinitialize buffer.");
                SHIRABE_RETURN_RESULT_ON_ERROR(deinitialization.result());
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto createBufferView(Shared<CVulkanEnvironment>      aVulkanEnvironment
                                  , Shared<CResourceManager>      aResourceManager
                                  , Shared<asset::CAssetStorage>  aAssetStorage
                                  , SFrameGraphBuffer      const &aBuffer
                                  , SFrameGraphBufferView  const &aView) -> EEngineStatus
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

                auto const &[success, resource] = aResourceManager->useResource<BufferViewResourceState_t>(desc.name, desc);
                EngineStatusPrintOnError(success, logTag(), "Failed to create buffer view.");
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto destroyBufferView(Shared<CVulkanEnvironment>      aVulkanEnvironment
                                   , Shared<CResourceManager>      aResourceManager
                                   , Shared<asset::CAssetStorage>  aAssetStorage
                                   , SFrameGraphBufferView  const &aView) -> EEngineStatus
            {
                OptRef_t<BufferViewResourceState_t> resourceOpt {};
                {
                    auto [success, resource] = fetchResource<BufferViewResourceState_t>(aView.readableName, aResourceManager, aVulkanEnvironment);
                    if(not success)
                    {
                        return EEngineStatus::Ok;
                    }
                    resourceOpt = resource;
                }
                BufferViewResourceState_t &bufferView = *resourceOpt;

                CEngineResult<> const deinitialization = aResourceManager->discardResource<BufferViewResourceState_t>(aView.readableName, aVulkanEnvironment);
                EngineStatusPrintOnError(deinitialization.result(), logTag(), "Failed to deinitialize buffer.");
                SHIRABE_RETURN_RESULT_ON_ERROR(deinitialization.result());
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
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

            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto initializeMaterial(Shared<CVulkanEnvironment>     aVulkanEnvironment
                                    , Shared<CResourceManager>     aResourceManager
                                    , Shared<asset::CAssetStorage> aAssetStorage
                                    , SFrameGraphMaterial   const &aMaterial
                                    , ResourceId_t          const &aRenderPassId) -> EEngineStatus
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

                    CEngineResult<> const bufferInitialization = aResourceManager->initializeResource<BufferResourceState_t>(buffer.name, aVulkanEnvironment);
                    EngineStatusPrintOnError(bufferInitialization.result(), logTag(), "Failed to initialize buffer.");
                    SHIRABE_RETURN_RESULT_ON_ERROR(bufferInitialization.result());
                }

                for(auto const &sampledImageAssetId : material.description.sampledImages)
                {
                    std::string const sampledImageResourceId = fmt::format("{}", sampledImageAssetId);

                    OptRef_t <TextureResourceState_t> sampledImageOpt{};
                    {
                        auto[success, resource] = fetchResource<TextureResourceState_t>(aResourceManager, sampledImageResourceId);
                        if( not success )
                        {
                            return EEngineStatus::Ok;
                        }
                        sampledImageOpt = resource;
                    }
                    TextureResourceState_t &sampledImage = *sampledImageOpt;

                    std::string const sampledImageViewResourceId = fmt::format("{}_{}_view", material.description.name, sampledImage.description.name);
                    OptRef_t<TextureViewResourceState_t> sampledImageViewOpt {};
                    {
                        auto [success, resource] = fetchResource<TextureViewResourceState_t>(aResourceManager, sampledImageViewResourceId);
                        if(not success)
                        {
                            return EEngineStatus::Ok;
                        }
                        sampledImageViewOpt = resource;
                    }
                    TextureViewResourceState_t &sampledImageView = *sampledImageViewOpt;

                    CEngineResult<> const textureInitialization = aResourceManager->initializeResource<TextureResourceState_t>(sampledImageResourceId, aVulkanEnvironment);
                    EngineStatusPrintOnError(textureInitialization.result(), logTag(), "Failed to initialize texture.");
                    SHIRABE_RETURN_RESULT_ON_ERROR(textureInitialization.result());

                    STextureViewDependencies textureViewInitDependencies {};
                    textureViewInitDependencies.subjacentTextureId = sampledImageResourceId;

                    CEngineResult<> const textureViewInitialization = aResourceManager->initializeResource<TextureViewResourceState_t>(sampledImageViewResourceId, aVulkanEnvironment);
                    EngineStatusPrintOnError(textureViewInitialization.result(), logTag(), "Failed to initialize texture view.");
                    SHIRABE_RETURN_RESULT_ON_ERROR(textureViewInitialization.result());
                }

                CEngineResult<> const shaderModuleInitialization = aResourceManager->initializeResource<ShaderModuleResourceState_t>(material.description.shaderModuleDescriptor.name, aVulkanEnvironment);
                EngineStatusPrintOnError(shaderModuleInitialization.result(), logTag(), "Failed to initialize shader module.");
                SHIRABE_RETURN_RESULT_ON_ERROR(shaderModuleInitialization.result());

                CEngineResult<> const pipelineInitialization = aResourceManager->initializeResource<PipelineResourceState_t>(material.description.pipelineDescriptor.name, aVulkanEnvironment);
                EngineStatusPrintOnError(pipelineInitialization.result(), logTag(), "Failed to initialize pipeline.");
                SHIRABE_RETURN_RESULT_ON_ERROR(pipelineInitialization.result());
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto transferMaterial(Shared<CVulkanEnvironment>     aVulkanEnvironment
                                  , Shared<CResourceManager>     aResourceManager
                                  , Shared<asset::CAssetStorage> aAssetStorage
                                  , SFrameGraphMaterial   const &aMaterial) -> EEngineStatus
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

                for(auto const &sampledImageAssetId : material.description.sampledImages)
                {
                    std::string const sampledImageResourceId = fmt::format("{}", sampledImageAssetId);

                    OptRef_t <TextureResourceState_t> sampledImageOpt{};
                    {
                        auto[success, resource] = fetchResource<TextureResourceState_t>(aResourceManager, sampledImageResourceId);
                        if( not success )
                        {
                            return EEngineStatus::Ok;
                        }
                        sampledImageOpt = resource;
                    }
                    TextureResourceState_t &sampledImage = *sampledImageOpt;

                    // Make sure, that the texture is in the correct pre-transfer layout!
                    if(sampledImage.description.gpuBinding.check(EBufferBinding::TextureInput))
                    {
                        EEngineStatus const layoutTransfer =
                                                performImageLayoutTransferImpl(aState
                                                                               , sampledImage
                                                                               , CRange(0, 1)
                                                                               , CRange(0, 1)
                                                                               , VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
                                                                               , VK_IMAGE_LAYOUT_UNDEFINED
                                                                               , VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

                        EngineStatusPrintOnError(layoutTransfer, logTag(), "Failed to transfer texture.");
                        SHIRABE_RETURN_RESULT_ON_ERROR(layoutTransfer);
                    }

                    CEngineResult<> const textureTransfer = aResourceManager->transferResource<TextureResourceState_t>(sampledImageResourceId, aVulkanEnvironment);
                    EngineStatusPrintOnError(textureTransfer.result(), logTag(), "Failed to transfer texture.");
                    SHIRABE_RETURN_RESULT_ON_ERROR(textureTransfer.result());

                    // Make sure, that the texture is in the correct post-transfer layout!
                    if(sampledImage.description.gpuBinding.check(EBufferBinding::TextureInput))
                    {
                        EEngineStatus const layoutTransfer =
                                                performImageLayoutTransferImpl(aState
                                                                               , sampledImage
                                                                               , CRange(0, 1)
                                                                               , CRange(0, 1)
                                                                               , VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
                                                                               , VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
                                                                               , VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

                        EngineStatusPrintOnError(layoutTransfer, logTag(), "Failed to transfer texture.");
                        SHIRABE_RETURN_RESULT_ON_ERROR(layoutTransfer);
                    }
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
            auto updateDescriptorSets(Shared<CVulkanEnvironment>                                 aVulkanEnvironment
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
                                descriptorWrite.dstSet           = aMaterialHandle.gpuApiHandles.descriptorSets[startSetIndex + k];
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
                                descriptorWrite.dstSet           = aMaterialHandle.gpuApiHandles.descriptorSets[startSetIndex + k];
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
                                descriptorWrite.dstSet           = aMaterialHandle.gpuApiHandles.descriptorSets[startSetIndex + k];
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
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto const bindPipeline = [=] (PipelineResourceState_t &aPipeline) -> EEngineStatus
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
            context.bindMaterial = [=] (SFrameGraphMaterial const &aMaterial) -> EEngineStatus
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

                for(auto const &sampledImageAssetId : material.description.sampledImages)
                {
                    OptRef_t<TextureResourceState_t> sampledImageOpt {};
                    {
                        std::string const sampledImageResourceId = fmt::format("{}", sampledImageAssetId);
                        auto [success, resource] = fetchResource<TextureResourceState_t>(aResourceManager, sampledImageResourceId);
                        if(not success)
                        {
                            return EEngineStatus::Ok;
                        }
                        sampledImageOpt = resource;
                    }

                    if(not sampledImageOpt.has_value())
                    {
                        textureViews.push_back({}); // Fill gaps
                    }
                    else
                    {
                        TextureResourceState_t &sampledImage = *sampledImageOpt;

                        std::string const sampledImageViewResourceId = fmt::format("{}_{}_view", material.description.name, sampledImage.description.name);

                        OptRef_t<TextureViewResourceState_t> sampledImageViewOpt {};
                        {
                            auto [success, resource] = fetchResource<TextureViewResourceState_t>(aResourceManager, sampledImageViewResourceId);
                            if(not success)
                            {
                                return EEngineStatus::Ok;
                            }
                            sampledImageViewOpt = resource;
                        }

                        SSampledImageBinding binding {};
                        binding.image     = sampledImageOpt;
                        binding.imageView = sampledImageViewOpt;

                        textureViews.push_back(binding);
                    }
                }

                EEngineStatus const updateResult = updateDescriptorSets(aState
                                                                        , material
                                                                        , buffers
                                                                        , inputAttachments
                                                                        , textureViews);

                OptRef_t<PipelineResourceState_t> pipelineResourceOpt {};
                {
                    auto [success, resource] = fetchResource<PipelineResourceState_t>(aResourceManager, material.description.pipelineDescriptor.name);
                    if(not success)
                    {
                        return EEngineStatus::Ok;
                    }
                    pipelineResourceOpt = resource;
                }
                PipelineResourceState_t &pipelineResource = *pipelineResourceOpt;

                auto const result = bindPipeline(aState, pipelineResource);
                return result;
                };
            //<-----------------------------------------------------------------------------
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        framegraph::SFrameGraphResourceContext CreateResourceContextForVulkan(Shared<CVulkanEnvironment>   aVulkanEnvironment
                                                                            , Shared<CResourceManager>     aResourceManager
                                                                            , Shared<asset::CAssetStorage> aAssetStorage)
        {
            using namespace local;
            using namespace resources;

            framegraph::SFrameGraphResourceContext context {};

            // --------------------------------------------------------------------------------------------
            // RenderPass
            // --------------------------------------------------------------------------------------------
            context.createRenderPass =
                [&] (ResourceId_t                      const &aRenderPassId
                     , std::vector<PassUID_t>          const &aPassExecutionOrder
                     , SFrameGraphAttachmentCollection const &aAttachmentInfo
                     , CFrameGraphMutableResources     const &aFrameGraphResources)
                    { return detail::createRenderPass(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                    , aRenderPassId
                                                    , aPassExecutionOrder
                                                    , aAttachmentInfo
                                                    , aFrameGraphResources); };
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
                [&](SFrameGraphTransientTexture const &aTexture)
                    { return detail::createTransientTexture(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                            , aTexture); };
            context.destroyTransientTexture =
                [&](SFrameGraphTransientTexture const &aTexture)
                    { return detail::destroyTransientTexture(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                             , aTexture); };
            context.initializePersistentTexture =
                [&](SFrameGraphPersistentTexture const &aTexture)
                    { return detail::initializePersistentTexture(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                             , aTexture); };
            context.updatePersistentTexture =
                [&](SFrameGraphPersistentTexture const &aTexture)
                    { return detail::updatePersistentTexture(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                             , aTexture); };
            context.deinitializePersistentTexture =
                [&](SFrameGraphPersistentTexture const &aTexture)
                    { return detail::deinitializePersistentTexture(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                             , aTexture); };
            context.createTextureView =
                [&] (SFrameGraphTexture const &aTexture
                     , SFrameGraphTextureView  const &aView)
                    { return detail::createTextureView(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                      , aTexture
                                                      , aView); };
            context.destroyTextureView =
                [&] (SFrameGraphTextureView const &aTextureView)
                    { return detail::destroyTextureView(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                      , aTextureView); };

            // --------------------------------------------------------------------------------------------
            // Buffers
            // --------------------------------------------------------------------------------------------
            context.createBuffer =
                [&] (SFrameGraphBuffer const &aBuffer)
                    { return detail::createBuffer(aVulkanEnvironment, aResourceManager, aAssetStorage, aBuffer); };
            context.transferBuffer =
                [&] (SFrameGraphBuffer const &aBuffer)
                    { return detail::transferBuffer(aVulkanEnvironment, aResourceManager, aAssetStorage, aBuffer); };
            context.destroyBuffer =
                [&] (SFrameGraphBuffer const &aBuffer)
                    { return detail::destroyBuffer(aVulkanEnvironment, aResourceManager, aAssetStorage, aBuffer); };
            context.createBufferView =
                [&] (SFrameGraphBuffer     const &aBuffer
                   , SFrameGraphBufferView const &aView)
                    { return detail::createBufferView(aVulkanEnvironment, aResourceManager, aAssetStorage, aBuffer, aView); };
            context.destroyBufferView =
                [&] (SFrameGraphBufferView const &aView)
                    { return detail::destroyBufferView(aVulkanEnvironment, aResourceManager, aAssetStorage, aView); };


        }
    }
}
