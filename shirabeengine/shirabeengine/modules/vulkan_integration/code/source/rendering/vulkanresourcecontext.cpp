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
            SHIRABE_DECLARE_LOG_TAG(VulkanFrameGraphResourceContext);
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
            auto createTransientBuffer(Shared<CVulkanEnvironment>        aVulkanEnvironment
                                     , Shared<CResourceManager>          aResourceManager
                                     , Shared<asset::CAssetStorage>      aAssetStorage
                                     , SFrameGraphTransientBuffer const &aBuffer) -> EEngineStatus
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

                CEngineResult<BufferResourceState_t> bufferObject = aResourceManager->useResource<BufferResourceState_t>(desc.name, desc, true);
                EngineStatusPrintOnError(bufferObject.result(), logTag(), "Failed to create buffer.");
                SHIRABE_RETURN_RESULT_ON_ERROR(bufferObject.result());
                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto destroyTransientBuffer(Shared<CVulkanEnvironment>        aVulkanEnvironment
                                      , Shared<CResourceManager>          aResourceManager
                                      , Shared<asset::CAssetStorage>      aAssetStorage
                                      , SFrameGraphTransientBuffer const &aBuffer) -> EEngineStatus
            {
                CEngineResult<> const deinitialization = aResourceManager->discardResource<BufferResourceState_t>(aBuffer.readableName, aVulkanEnvironment);
                EngineStatusPrintOnError(deinitialization.result(), logTag(), "Failed to destroy buffer.");
                SHIRABE_RETURN_RESULT_ON_ERROR(deinitialization.result());
                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto initializePersistentBuffer(Shared<CVulkanEnvironment>         aVulkanEnvironment
                                          , Shared<CResourceManager>           aResourceManager
                                          , Shared<asset::CAssetStorage>       aAssetStorage
                                          , SFrameGraphPersistentBuffer const &aBuffer) -> EEngineStatus
            {
                {
                    auto const &[successCode] = aResourceManager->deinitializeResource<BufferResourceState_t>(aBuffer.readableName, aVulkanEnvironment);
                    EngineStatusPrintOnError(successCode, logTag(), "Failed to initialize persistent buffer.");
                    SHIRABE_RETURN_RESULT_ON_ERROR(successCode);
                }

                return EEngineStatus::Ok;
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto updatePersistentBuffer(Shared<CVulkanEnvironment>         aVulkanEnvironment
                                      , Shared<CResourceManager>           aResourceManager
                                      , Shared<asset::CAssetStorage>       aAssetStorage
                                      , SFrameGraphPersistentBuffer const &aBuffer) -> EEngineStatus
            {
                CEngineResult<> const bufferTransfer = aResourceManager->uploadResource<BufferResourceState_t>(aBuffer.readableName, aVulkanEnvironment);
                EngineStatusPrintOnError(bufferTransfer.result(), logTag(), "Buffer initialization failed.");
                SHIRABE_RETURN_RESULT_ON_ERROR(bufferTransfer.result());

                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto deinitializePersistentBuffer(Shared<CVulkanEnvironment>         aVulkanEnvironment
                                            , Shared<CResourceManager>           aResourceManager
                                            , Shared<asset::CAssetStorage>       aAssetStorage
                                            , SFrameGraphPersistentBuffer const &aBuffer) -> EEngineStatus
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
                SHIRABE_RETURN_RESULT_ON_ERROR(success);
                return EEngineStatus::Ok;
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
                CEngineResult<> const deinitialization = aResourceManager->discardResource<BufferViewResourceState_t>(aView.readableName, aVulkanEnvironment);
                EngineStatusPrintOnError(deinitialization.result(), logTag(), "Failed to deinitialize buffer.");
                SHIRABE_RETURN_RESULT_ON_ERROR(deinitialization.result());
                return EEngineStatus::Ok;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto initializeMesh(Shared<CVulkanEnvironment>  aVulkanEnvironment
                            , Shared<CResourceManager>      aResourceManager
                            , Shared<asset::CAssetStorage>  aAssetStorage
                            , SFrameGraphMesh        const &aMesh) -> EEngineStatus
            {
                EEngineStatus const &attributeBufferOpSuccessCode = detail::initializePersistentBuffer(aVulkanEnvironment, aResourceManager, aAssetStorage, aMesh.attributeBuffer);
                EEngineStatus const &indexBufferOpSuccessCode     = detail::initializePersistentBuffer(aVulkanEnvironment, aResourceManager, aAssetStorage, aMesh.indexBuffer);

                if(CheckEngineError(attributeBufferOpSuccessCode) || CheckEngineError(indexBufferOpSuccessCode))
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
                            , SFrameGraphMesh        const &aMesh) -> EEngineStatus
            {
                EEngineStatus const &attributeBufferOpSuccessCode = detail::updatePersistentBuffer(aVulkanEnvironment, aResourceManager, aAssetStorage, aMesh.attributeBuffer);
                EEngineStatus const &indexBufferOpSuccessCode     = detail::updatePersistentBuffer(aVulkanEnvironment, aResourceManager, aAssetStorage, aMesh.indexBuffer);

                if(CheckEngineError(attributeBufferOpSuccessCode) || CheckEngineError(indexBufferOpSuccessCode))
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
                                , SFrameGraphMesh        const &aMesh) -> EEngineStatus
            {
                EEngineStatus const &attributeBufferOpSuccessCode = detail::deinitializePersistentBuffer(aVulkanEnvironment, aResourceManager, aAssetStorage, aMesh.attributeBuffer);
                EEngineStatus const &indexBufferOpSuccessCode     = detail::deinitializePersistentBuffer(aVulkanEnvironment, aResourceManager, aAssetStorage, aMesh.indexBuffer);

                if(CheckEngineError(attributeBufferOpSuccessCode) || CheckEngineError(indexBufferOpSuccessCode))
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
                                  , SFrameGraphMaterial    const &aMaterial) -> EEngineStatus
            {
                for(auto const &buffer : aMaterial.uniformBuffers)
                {
                    EEngineStatus const &opSuccessCode = detail::initializePersistentBuffer(aVulkanEnvironment, aResourceManager, aAssetStorage, buffer);
                    if(CheckEngineError(opSuccessCode))
                    {
                        return EEngineStatus::InitializationError;
                    }
                }

                for(auto const &texture : aMaterial.textures)
                {
                    EEngineStatus const &opSuccessCode = detail::initializePersistentTexture(aVulkanEnvironment, aResourceManager, aAssetStorage, texture);
                    if(CheckEngineError(opSuccessCode))
                    {
                        return EEngineStatus::InitializationError;
                    }
                }

                auto const [opSuccessCode] = aResourceManager->initializeResource<MaterialResourceState_t>(aMaterial.readableName, aVulkanEnvironment);
                if(CheckEngineError(opSuccessCode))
                {
                    return EEngineStatus::InitializationError;
                }

                return EEngineStatus::Ok;
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto updateMaterial(Shared<CVulkanEnvironment>    aVulkanEnvironment
                              , Shared<CResourceManager>      aResourceManager
                              , Shared<asset::CAssetStorage>  aAssetStorage
                              , SFrameGraphMaterial    const &aMaterial) -> EEngineStatus
            {
                for(auto const &buffer : aMaterial.uniformBuffers)
                {
                    EEngineStatus const &opSuccessCode = detail::updatePersistentBuffer(aVulkanEnvironment, aResourceManager, aAssetStorage, buffer);
                    if(CheckEngineError(opSuccessCode))
                    {
                        return EEngineStatus::UpdateError;
                    }
                }

                for(auto const &texture : aMaterial.textures)
                {
                    EEngineStatus const &opSuccessCode = detail::updatePersistentTexture(aVulkanEnvironment, aResourceManager, aAssetStorage, texture);
                    if(CheckEngineError(opSuccessCode))
                    {
                        return EEngineStatus::UpdateError;
                    }
                }

                auto const [opSuccessCode] = aResourceManager->updateResource<MaterialResourceState_t>(aMaterial.readableName, aVulkanEnvironment);
                if(CheckEngineError(opSuccessCode))
                {
                    return EEngineStatus::UpdateError;
                }

                return EEngineStatus::Ok;
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto deinitializeMaterial(Shared<CVulkanEnvironment>    aVulkanEnvironment
                                    , Shared<CResourceManager>      aResourceManager
                                    , Shared<asset::CAssetStorage>  aAssetStorage
                                    , SFrameGraphMaterial    const &aMaterial) -> EEngineStatus
            {
                for(auto const &buffer : aMaterial.uniformBuffers)
                {
                    EEngineStatus const &opSuccessCode = detail::deinitializePersistentBuffer(aVulkanEnvironment, aResourceManager, aAssetStorage, buffer);
                    if(CheckEngineError(opSuccessCode))
                    {
                        return EEngineStatus::UpdateError;
                    }
                }

                for(auto const &texture : aMaterial.textures)
                {
                    EEngineStatus const &opSuccessCode = detail::deinitializePersistentBuffer(aVulkanEnvironment, aResourceManager, aAssetStorage, texture);
                    if(CheckEngineError(opSuccessCode))
                    {
                        return EEngineStatus::UpdateError;
                    }
                }

                auto const [opSuccessCode] = aResourceManager->deinitializeResource<MaterialResourceState_t>(aMaterial.readableName, aVulkanEnvironment);
                if(CheckEngineError(opSuccessCode))
                {
                    return EEngineStatus::UpdateError;
                }

                return EEngineStatus::Ok;
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto createPipeline(Shared<CVulkanEnvironment>      aVulkanEnvironment
                                , Shared<CResourceManager>      aResourceManager
                                , Shared<asset::CAssetStorage>  aAssetStorage
                                , SFrameGraphPipeline    const &aPipeline) -> EEngineStatus
            {
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto destroyPipeline(Shared<CVulkanEnvironment>      aVulkanEnvironment
                                 , Shared<CResourceManager>      aResourceManager
                                 , Shared<asset::CAssetStorage>  aAssetStorage
                                 , SFrameGraphPipeline    const &aMesh) -> EEngineStatus
            {
            }
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
            context.createTransientBuffer =
                [&](SFrameGraphTransientBuffer const &aBuffer)
                    { return detail::createTransientBuffer(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                         , aBuffer); };
            context.destroyTransientBuffer =
                [&](SFrameGraphTransientBuffer const &aBuffer)
                    { return detail::destroyTransientBuffer(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                          , aBuffer); };
            context.initializePersistentBuffer =
                [&](SFrameGraphPersistentBuffer const &aBuffer)
                    { return detail::initializePersistentBuffer(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                              , aBuffer); };
            context.updatePersistentBuffer =
                [&](SFrameGraphPersistentBuffer const &aBuffer)
                    { return detail::updatePersistentBuffer(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                          , aBuffer); };
            context.deinitializePersistentBuffer =
                [&](SFrameGraphPersistentBuffer const &aBuffer)
                    { return detail::deinitializePersistentBuffer(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                                , aBuffer); };
            context.createBufferView =
                [&] (SFrameGraphBuffer const &aBuffer
                     , SFrameGraphBufferView  const &aView)
                    { return detail::createBufferView(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                    , aBuffer
                                                    , aView); };
            context.destroyBufferView =
                [&] (SFrameGraphBufferView const &aBufferView)
                    { return detail::destroyBufferView(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                     , aBufferView); };


            //---------------------------------------------------------------------------------------------------------------
            // Meshes
            //---------------------------------------------------------------------------------------------------------------
            context.initializeMesh =
                [&] (SFrameGraphMesh const &aMesh)
                    { return detail::initializeMesh(aVulkanEnvironment, aResourceManager, aAssetStorage
                                              , aMesh); };
            context.updateMesh =
                [&] (SFrameGraphMesh const &aMesh)
                    { return detail::updateMesh(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                , aMesh); };
            context.deinitializeMesh =
                [&] (SFrameGraphMesh const &aMesh)
                    { return detail::deinitializeMesh(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                    , aMesh); };

            //---------------------------------------------------------------------------------------------------------------
            // Materials
            //---------------------------------------------------------------------------------------------------------------
            context.initializeMaterial =
                [&] (SFrameGraphMaterial const &aMaterial)
                    { return detail::initializeMaterial(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                      , aMaterial); };
            context.updateMaterial =
                [&] (SFrameGraphMaterial const &aMaterial)
                    { return detail::updateMaterial(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                  , aMaterial); };
            context.deinitializeMaterial =
                [&] (SFrameGraphMaterial const &aMaterial)
                    { return detail::deinitializeMaterial(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                        , aMaterial); };

            //---------------------------------------------------------------------------------------------------------------
            // Pipelines
            //---------------------------------------------------------------------------------------------------------------
            context.createPipeline =
                [&] (SFrameGraphPipeline const &aPipeline)
                    { return detail::createPipeline(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                   , aPipeline); };
            context.destroyPipeline =
                [&] (SFrameGraphPipeline const &aPipeline)
                    { return detail::destroyPipeline(aVulkanEnvironment, aResourceManager, aAssetStorage
                                                     , aPipeline); };
        }
    }
}
