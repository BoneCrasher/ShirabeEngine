#include <cassert>

#include <fmt/format.h>
#include <material/material_loader.h>
#include <material/material_declaration.h>
#include <material/materialserialization.h>
#include <resources/cresourcemanager.h>
#include <resources/resourcetypes.h>

#include "renderer/irenderer.h"
#include "renderer/framegraph/framegraphrendercontext.h"

namespace engine
{
    namespace framegraph
    {
        using namespace engine::rendering;
        using namespace engine::material;

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<Shared<CFrameGraphRenderContext>> CFrameGraphRenderContext::create(
                Shared<IAssetStorage>    const &aAssetStorage,
                Shared<CMaterialLoader>  const &aMaterialLoader,
                Shared<CResourceManager> const &aResourceManager,
                Shared<IRenderContext>   const &aRenderer)
        {
            bool const inputInvalid =
                    nullptr == aAssetStorage    or
                    nullptr == aMaterialLoader  or
                    nullptr == aResourceManager or
                    nullptr == aRenderer;

            if(inputInvalid)
            {
                return { EEngineStatus::Error };
            }

            auto context = makeShared<CFrameGraphRenderContext>(aAssetStorage
                                                                       , aMaterialLoader
                                                                       , aResourceManager
                                                                       , aRenderer);
            if(not context)
            {
                CLog::Error(logTag(), "Failed to create render context from renderer and resourcemanager.");
                return { EEngineStatus::Error };
            }
            else
            {
                return { EEngineStatus::Ok, context };
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CFrameGraphRenderContext::CFrameGraphRenderContext(
                Shared<IAssetStorage>    aAssetStorage,
                Shared<CMaterialLoader>  aMaterialLoader,
                Shared<CResourceManager> aResourceManager,
                Shared<IRenderContext>   aRenderer)
            : mAssetStorage            (std::move(aAssetStorage   ))
            , mMaterialLoader          (std::move(aMaterialLoader ))
            , mResourceManager         (std::move(aResourceManager))
            , mGraphicsAPIRenderContext(std::move(aRenderer       ))
            , mCurrentFrameBufferHandle({})
            , mCurrentRenderPassHandle ({})
            , mCurrentSubpass          (0)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::mapFrameGraphToInternalResource(
                std::string const &aName,
                std::string const &aResourceId)
        {
            mResourceMap[aName].push_back(aResourceId);
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<Vector<std::string>> CFrameGraphRenderContext::getMappedInternalResourceIds(std::string const &aName) const
        {
            if(mResourceMap.end() != mResourceMap.find(aName))
            {
                return { EEngineStatus::Error };
            }

            return { EEngineStatus::Ok, mResourceMap.at(aName) };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::removeMappedInternalResourceIds(std::string const &aName)
        {
            mResourceMap.erase(aName);
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::beginPass()
        {
            EEngineStatus const status = mGraphicsAPIRenderContext->beginSubpass();
            if(CheckEngineError(status))
            {
                // ...
            }

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::endPass()
        {
            EEngineStatus const status = mGraphicsAPIRenderContext->endSubpass();
            if(CheckEngineError(status))
            {
                // ...
            }

            ++mCurrentSubpass;

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::copyImage(SFrameGraphTexture const &aSourceImage,
                                                            SFrameGraphTexture const &aTargetImage)
        {
            Shared<ILogicalResourceObject> source = getUsedResource(aSourceImage.readableName);
            Shared<ILogicalResourceObject> target = getUsedResource(aTargetImage.readableName);

            CEngineResult<> const status = mGraphicsAPIRenderContext->copyImage(source->getGpuApiResourceHandle(), target->getGpuApiResourceHandle());

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::copyImageToBackBuffer(SFrameGraphTexture const &aSourceImageId)
        {
            Shared<ILogicalResourceObject> source = getUsedResource(aSourceImageId.readableName);

            CEngineResult<> const status = mGraphicsAPIRenderContext->copyToBackBuffer(source->getGpuApiResourceHandle());

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::bindSwapChain(std::string const &aSwapChainId)
        {
            //Shared<ILogicalResourceObject> source = getUsedResource(aSwapChainId);

            CEngineResult<> const status = mGraphicsAPIRenderContext->bindSwapChain(GpuApiHandle_t{});

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::present()
        {
            CEngineResult<> const status = mGraphicsAPIRenderContext->present();

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::beginCommandBuffer()
        {
            CEngineResult<> const status = mGraphicsAPIRenderContext->beginGraphicsCommandBuffer();

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::commitCommandBuffer()
        {
            CEngineResult<> const status = mGraphicsAPIRenderContext->commitGraphicsCommandBuffer();

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------

        /**
         * Create a framebuffer and render pass including subpasses for the provided attachment info.
         *
         * @param aRenderPassId        Unique Id of the render pass instance to create.
         * @param aAttachmentInfo      Attachment information describing all subpasses, their attachments, etc...
         * @param aFrameGraphResources List of frame graph resources affiliated with the attachments
         * @return                     EEngineStatus::Ok if successful.
         * @return                     EEngineStatus::Error otherwise.
         */
        CEngineResult<> CFrameGraphRenderContext::createRenderPass(
                std::string                     const &aRenderPassId,
                SFrameGraphAttachmentCollection const &aAttachmentInfo,
                CFrameGraphMutableResources     const &aFrameGraphResources)
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

            // List of resource ids of the attachments.
            FrameGraphResourceIdList const &attachmentResources = aAttachmentInfo.getAttachementResourceIds();

            // Each element in the frame buffer is required to have the same dimensions.
            // These variables will store the first sizes encountered and will validate
            // against them for any subsequent size, to make sure that the attachments
            // to be bound are valid.
            int32_t width  = -1,
                    height = -1,
                    layers = -1;

            // This list will store the readable names of the texture views created upfront, so that the
            // framebuffer can bind to it.
            std::vector<std::string> textureViewIds = {};

            // asdfjialkfdaj;klfj;kl fd
            //
            // The derivation of whether something is an input/color/depth attachment or not is most likely broken.

            //<-----------------------------------------------------------------------------
            // Begin the render pass derivation
            //<-----------------------------------------------------------------------------
            SRenderPassDescription renderPassDesc = {};
            renderPassDesc.name = aRenderPassId;

            // Traverse all referenced attachmentments foreach pass.
            for(auto const &[passUID, attachmentResourceIndexList] : aAttachmentInfo.getAttachmentPassAssignment())
            {
                SSubpassDescription subpassDesc = {};
                for(auto const &index : attachmentResourceIndexList)
                {
                    FrameGraphResourceId_t const &resourceId = attachmentResources.at(index);

                    CEngineResult<Shared<SFrameGraphTextureView> const> const &textureViewFetch = aFrameGraphResources.get<SFrameGraphTextureView>(resourceId);
                    if(not textureViewFetch.successful())
                    {
                        CLog::Error(logTag(), CString::format("Fetching texture view w/ id {} failed.", resourceId));
                        return { EEngineStatus::ResourceError_NotFound };
                    }

                    SFrameGraphTextureView const &textureView = *(textureViewFetch.data());

                    CEngineResult<Shared<SFrameGraphTexture> const> const &textureFetch = aFrameGraphResources.get<SFrameGraphTexture>(textureView.subjacentResource);
                    if(not textureFetch.successful())
                    {
                        CLog::Error(logTag(), CString::format("Fetching texture w/ id {} failed.", textureView.subjacentResource));
                        return { EEngineStatus::ResourceError_NotFound };
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
                    textureViewIds.push_back(textureView.readableName);

                    uint64_t const attachmentIndex = renderPassDesc.attachmentDescriptions.size();

                    // For the choice of image layouts, check: https://www.saschawillems.de/?p=3055
                    SAttachmentDescription attachmentDesc = {};
                    attachmentDesc.stencilLoadOp  = attachmentDesc.loadOp;
                    attachmentDesc.storeOp        = EAttachmentStoreOp::STORE;
                    attachmentDesc.initialLayout  = EImageLayout::UNDEFINED;
                    attachmentDesc.finalLayout    = EImageLayout::TRANSFER_SRC_OPTIMAL; // For now we just assume everything to be presentable...
                    attachmentDesc.loadOp         = (EImageLayout::UNDEFINED == attachmentDesc.initialLayout) ? EAttachmentLoadOp::DONT_CARE : EAttachmentLoadOp::LOAD;
                    attachmentDesc.stencilStoreOp = attachmentDesc.storeOp;
                    attachmentDesc.format         = textureView.format;

                    renderPassDesc.attachmentDescriptions.push_back(attachmentDesc);

                    bool const isColorAttachment = findAttachmentRelationFn(aAttachmentInfo.getAttachementResourceIds(), aAttachmentInfo.getColorAttachments(), textureView.resourceId);
                    bool const isDepthAttachment = findAttachmentRelationFn(aAttachmentInfo.getAttachementResourceIds(), aAttachmentInfo.getDepthAttachments(), textureView.resourceId);
                    bool const isInputAttachment = findAttachmentRelationFn(aAttachmentInfo.getAttachementResourceIds(), aAttachmentInfo.getInputAttachments(), textureView.resourceId);

                    SAttachmentReference attachmentReference {};
                    attachmentReference.attachment = static_cast<uint32_t>(attachmentIndex);

                    if(isColorAttachment)
                    {
                        attachmentReference.layout = EImageLayout::COLOR_ATTACHMENT_OPTIMAL;
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

                        subpassDesc.depthStencilAttachments.push_back(attachmentReference);
                    }
                    else if(isInputAttachment)
                    {
                        attachmentReference.layout = EImageLayout::SHADER_READ_ONLY_OPTIMAL;
                        subpassDesc.inputAttachments.push_back(attachmentReference);
                    }
                }

                renderPassDesc.subpassDescriptions.push_back(subpassDesc);
            }

            renderPassDesc.attachmentExtent.width  = width;
            renderPassDesc.attachmentExtent.height = height;
            renderPassDesc.attachmentExtent.depth  = layers;
            renderPassDesc.attachmentTextureViews  = textureViewIds;

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


            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------

        /**
         *
         * @param aFrameBufferId       Unique Id of the frame buffer instance to create.
         * @param aRenderPassId        Unique Id of the render pass instance to create.
         * @param aFrameGraphResources
         * @return
         */
        CEngineResult<> CFrameGraphRenderContext::createFrameBuffer(std::string const &aFrameBufferId,
                                                                    std::string const &aRenderPassId)
        {
            Shared<SRenderPass>          renderPass     = std::static_pointer_cast<SRenderPass>(getUsedResource(aRenderPassId));
            SRenderPassDescription const renderPassDesc = renderPass->getDescription();

            SFrameBufferDescription frameBufferDesc = {};
            frameBufferDesc.name                  = aFrameBufferId;
            frameBufferDesc.referenceRenderPassId = aRenderPassId;

            std::vector<std::string> const &textureViewIds = renderPassDesc.attachmentTextureViews;

            {
                std::vector<std::string> frameBufferDependencies {};
                frameBufferDependencies.push_back(renderPassDesc.name);
                frameBufferDependencies.insert(frameBufferDependencies.end(), textureViewIds.begin(), textureViewIds.end());

                CEngineResult<Shared<ILogicalResourceObject>> status = mResourceManager->useDynamicResource<SFrameBuffer>(frameBufferDesc.name, frameBufferDesc, std::move(frameBufferDependencies));
                if( EEngineStatus::ResourceManager_ResourceAlreadyCreated == status.result())
                {
                    return EEngineStatus::Ok;
                }
                else
                {
                    EngineStatusPrintOnError(status.result(), logTag(), "Failed to create frame buffer.");
                }

                registerUsedResource(frameBufferDesc.name, status.data());

                mCurrentFrameBufferHandle = frameBufferDesc.name;
            }

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::bindRenderPass(std::string const &aRenderPassId,
                                                                 std::string const &aFrameBufferId)
        {
            Shared<ILogicalResourceObject> renderPass  = getUsedResource(aRenderPassId);
            Shared<ILogicalResourceObject> frameBuffer = getUsedResource(aFrameBufferId);

            EEngineStatus const status = mGraphicsAPIRenderContext->bindRenderPass(renderPass->getGpuApiResourceHandle(), frameBuffer->getGpuApiResourceHandle());
            if( not CheckEngineError(status))
            {
                // TODO: Implication of string -> std::string. Will break, once the underlying type
                //       of the std::string changes.
                mCurrentFrameBufferHandle = aFrameBufferId;
                mCurrentRenderPassHandle  = aRenderPassId;
                mCurrentSubpass           = 0; // Reset!
            }
            return status;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::unbindRenderPass(std::string const &aRenderPassId,
                                                                   std::string const &aFrameBufferId)
        {
            Shared<ILogicalResourceObject> renderPass  = getUsedResource(aRenderPassId);
            Shared<ILogicalResourceObject> frameBuffer = getUsedResource(aFrameBufferId);

            return mGraphicsAPIRenderContext->unbindRenderPass(renderPass->getGpuApiResourceHandle(), frameBuffer->getGpuApiResourceHandle());

            mCurrentFrameBufferHandle = {};
            mCurrentRenderPassHandle  = {};
            mCurrentSubpass           = 0;

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::destroyFrameBuffer(std::string const &aFrameBufferId)
        {
            CEngineResult<> destruction = mResourceManager->discardResource(aFrameBufferId);
            EngineStatusPrintOnError(destruction.result(), logTag(), "Failed to destroy frame buffer.");

            return destruction;

            // return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::destroyRenderPass(std::string const &aRenderPassId)
        {
            CEngineResult<> destruction = mResourceManager->discardResource(aRenderPassId);
            EngineStatusPrintOnError(destruction.result(), logTag(), "Failed to destroy render pass.");

            return destruction;

            // return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::loadTextureAsset(AssetId_t const &aAssetUID)
        {
            SHIRABE_UNUSED(aAssetUID);
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::unloadTextureAsset(AssetId_t const &aAssetUID)
        {
            SHIRABE_UNUSED(aAssetUID);
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::importTexture(SFrameGraphTexture const &aTexture)
        {
            std::string const pid = "";

            mapFrameGraphToInternalResource(aTexture.readableName, pid);

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::createTexture(SFrameGraphTexture const &aTexture)
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
                CEngineResult<Shared<ILogicalResourceObject>> textureObject = mResourceManager->useDynamicResource<STexture>(desc.name, desc);
                if( EEngineStatus::ResourceManager_ResourceAlreadyCreated == textureObject.result())
                {
                    return EEngineStatus::Ok;
                }
                else
                {
                    EngineStatusPrintOnError(textureObject.result(), logTag(), "Failed to create texture.");
                }

                registerUsedResource(desc.name, textureObject.data());
            }

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::destroyTexture(SFrameGraphTexture const &aTexture)
        {
            CLog::Verbose(logTag(), CString::format("Texture:\n{}", convert_to_string(aTexture)));

            CEngineResult<> status = mResourceManager->discardResource(aTexture.readableName);

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::createTextureView(
                SFrameGraphTexture     const &aTexture,
                SFrameGraphTextureView const &aView)
        {
            CLog::Verbose(logTag(), CString::format("TextureView:\n{}", convert_to_string(aView)));

            STextureViewDescription desc = { };
            desc.name                 = aView.readableName;
            desc.textureFormat        = aView.format;
            desc.subjacentTextureId   = aTexture.readableName;
            desc.subjacentTextureInfo = static_cast<graphicsapi::STextureInfo>(aTexture);
            desc.arraySlices          = aView.arraySliceRange;
            desc.mipMapSlices         = aView.mipSliceRange;

            CEngineResult<Shared<ILogicalResourceObject>> textureViewObject = mResourceManager->useDynamicResource<STextureView>(desc.name, desc, {aTexture.readableName });
            EngineStatusPrintOnError(textureViewObject.result(), logTag(), "Failed to create texture.");

            registerUsedResource(desc.name, textureViewObject.data());

            return textureViewObject.result();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::bindTextureView(SFrameGraphTextureView const &aView)
        {
            CLog::Verbose(logTag(), CString::format("TextureView:\n{}", convert_to_string(aView)));

            CEngineResult<Vector<std::string>> const &subjacentResourcesFetch = getMappedInternalResourceIds(aView.readableName);
            if(subjacentResourcesFetch.successful())
            {
                for(std::string const &pid : subjacentResourcesFetch.data())
                {
                    Shared<ILogicalResourceObject> resource = getUsedResource(pid);
                    mGraphicsAPIRenderContext->bindResource(resource->getGpuApiResourceHandle());
                }
            }

            return { subjacentResourcesFetch.result() };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::unbindTextureView(SFrameGraphTextureView const &aView)
        {
            CLog::Verbose(logTag(), CString::format("TextureView:\n{}", convert_to_string(aView)));

            CEngineResult<Vector<std::string>> subjacentResourcesFetch = getMappedInternalResourceIds(aView.readableName);

            if(subjacentResourcesFetch.successful())
            {
                for(std::string const &pid : subjacentResourcesFetch.data())
                {
                    Shared<ILogicalResourceObject> resource = getUsedResource(pid);
                    mGraphicsAPIRenderContext->unbindResource(resource->getGpuApiResourceHandle());
                }
            }

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::destroyTextureView(SFrameGraphTextureView  const &aView)
        {
            CLog::Verbose(logTag(), CString::format("TextureView:\n{}", convert_to_string(aView)));

            CEngineResult<> status = EEngineStatus::Ok;
            status = mResourceManager->discardResource(aView.readableName);

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::loadBufferAsset(AssetId_t const &aAssetUID)
        {
            SHIRABE_UNUSED(aAssetUID);
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::unloadBufferAsset(AssetId_t const &aAssetUID)
        {
            SHIRABE_UNUSED(aAssetUID);
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::createBuffer(SFrameGraphBuffer const &aBuffer)
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

            CEngineResult<Shared<ILogicalResourceObject>> bufferObject = mResourceManager->useDynamicResource<SBuffer>(desc.name, desc);
            EngineStatusPrintOnError(bufferObject.result(), logTag(), "Failed to create buffer.");

            return bufferObject.result();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::destroyBuffer(SFrameGraphBuffer const &aBuffer)
        {
            CEngineResult<> status = EEngineStatus::Ok;
            status = mResourceManager->discardResource(aBuffer.readableName);

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::createBufferView(
                SFrameGraphBuffer      const &aBuffer,
                SFrameGraphBufferView  const &aBufferView)
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

            CEngineResult<Shared<ILogicalResourceObject>> bufferViewObject = mResourceManager->useDynamicResource<SBufferView>(desc.name, desc, {aBuffer.readableName });
            EngineStatusPrintOnError(bufferViewObject.result(), logTag(), "Failed to create buffer view.");

            return bufferViewObject.result();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::bindBufferView(SFrameGraphBufferView  const &aBufferView)
        {
            SHIRABE_UNUSED(aBufferView);
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::unbindBufferView(SFrameGraphBufferView const &aBufferView)
        {
            SHIRABE_UNUSED(aBufferView);
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::destroyBufferView(SFrameGraphBufferView const &aBufferView)
        {
            SHIRABE_UNUSED(aBufferView);
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::loadMeshAsset(SFrameGraphMesh const &aMesh)
        {
            SHIRABE_UNUSED(aMesh);
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::unloadMeshAsset(SFrameGraphMesh const &aMesh)
        {
            SHIRABE_UNUSED(aMesh);
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::bindMesh(SFrameGraphMesh const &aMesh)
        {
            SHIRABE_UNUSED(aMesh);
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::unbindMesh(SFrameGraphMesh const &aMesh)
        {
            SHIRABE_UNUSED(aMesh);
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::loadMaterialAsset(SFrameGraphMaterial const &aMaterial
                                                                    , std::string       const &aRenderPassHandle)
        {
            SHIRABE_UNUSED(aRenderPassHandle);


            registerUsedResource(master->getPipelineDescriptor().name, pipelineObject.data());

            return pipelineObject.result();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::bindMaterial(SFrameGraphMaterial const &aMaterial)
        {
            Shared<ILogicalResourceObject> pipeline = getUsedResource(aMaterial.readableName);
            auto const result = mGraphicsAPIRenderContext->bindPipeline(pipeline->getGpuApiResourceHandle());
            return result;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::unbindMaterial(SFrameGraphMaterial const &aMaterial)
        {
            Shared<ILogicalResourceObject> pipeline = getUsedResource(aMaterial.readableName);
            auto const result = mGraphicsAPIRenderContext->unbindPipeline(pipeline->getGpuApiResourceHandle());
            return result;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::unloadMaterialAsset(SFrameGraphMaterial const &aMaterial)
        {
            // CEngineResult<> status = mResourceManager->destroyResource<CPipeline>(aMaterial.readableName);
            // EngineStatusPrintOnError(status.result(), logTag(), "Failed to destroy pipeline.");
            // return status;
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::render(SFrameGraphMesh     const &aMesh,
                                                         SFrameGraphMaterial const &aMaterial)
        {
            SHIRABE_UNUSED(aMesh);

            loadMaterialAsset  (aMaterial, "");
            bindMaterial       (aMaterial);

            unbindMaterial     (aMaterial);
            unloadMaterialAsset(aMaterial);

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        void CFrameGraphRenderContext::registerUsedResource(  std::string                                               const &aResourceId
                                                            , engine::Shared<engine::resources::ILogicalResourceObject> const &aResource)
        {
            mUsedResources[aResourceId] = aResource;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        void CFrameGraphRenderContext::unregisterUsedResource(std::string const &aResourceId)
        {
            mUsedResources.erase(aResourceId);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        Shared<ILogicalResourceObject> CFrameGraphRenderContext::getUsedResource(std::string const &aResourceId)
        {
            if(mUsedResources.end() == mUsedResources.find(aResourceId))
            {
                return nullptr;
            }
            else
            {
                return mUsedResources[aResourceId];
            }
        }
        //<-----------------------------------------------------------------------------

    }
}
