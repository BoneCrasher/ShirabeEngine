#include <assert.h>

#include <graphicsapi/resources/types/buffer.h>
#include <graphicsapi/resources/types/bufferview.h>
#include <graphicsapi/resources/types/texture.h>
#include <graphicsapi/resources/types/textureview.h>
#include <graphicsapi/resources/types/renderpass.h>
#include <graphicsapi/resources/types/framebuffer.h>
#include <graphicsapi/resources/types/pipeline.h>

#include <material/material_loader.h>
#include <material/material_declaration.h>
#include <material/materialserialization.h>

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
        CEngineResult<CStdSharedPtr_t<CFrameGraphRenderContext>> CFrameGraphRenderContext::create(
                CStdSharedPtr_t<IAssetStorage>        aAssetStorage,
                CStdSharedPtr_t<CMaterialLoader>      aMaterialLoader,
                CStdSharedPtr_t<CResourceManagerBase> aResourceManager,
                CStdSharedPtr_t<IRenderContext>       aRenderer)
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

            auto context = makeCStdSharedPtr<CFrameGraphRenderContext>(aAssetStorage
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
                CStdSharedPtr_t<IAssetStorage>        aAssetStorage,
                CStdSharedPtr_t<CMaterialLoader>      aMaterialLoader,
                CStdSharedPtr_t<CResourceManagerBase> aResourceManager,
                CStdSharedPtr_t<IRenderContext>       aRenderer)
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
                std::string        const &aName,
                PublicResourceId_t const &aResourceId)
        {
            mResourceMap[aName].push_back(aResourceId);
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<Vector<PublicResourceId_t>> CFrameGraphRenderContext::getMappedInternalResourceIds(std::string const &aName) const
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
            CEngineResult<> const status = mGraphicsAPIRenderContext->copyImage(aSourceImage.readableName, aTargetImage.readableName);

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::copyImageToBackBuffer(SFrameGraphTexture const &aSourceImageId)
        {
            CEngineResult<> const status = mGraphicsAPIRenderContext->copyToBackBuffer(aSourceImageId.readableName);

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::bindSwapChain(std::string const &aSwapChainId)
        {
            CEngineResult<> const status = mGraphicsAPIRenderContext->bindSwapChain(aSwapChainId);

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
        CEngineResult<> CFrameGraphRenderContext::createFrameBufferAndRenderPass(
                std::string                     const &aFrameBufferId,
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
            PublicResourceIdList_t textureViewIds = {};

            // asdfjialkfdaj;klfj;kl fd
            //
            // The derivation of whether something is an input/color/depth attachment or not is most likely broken.

            //<-----------------------------------------------------------------------------
            // Begin the render pass derivation
            //<-----------------------------------------------------------------------------
            CRenderPass::SDescriptor renderPassDesc = {};
            renderPassDesc.name = aRenderPassId;

            for(auto const &[passUID, attachmentResourceIndexList] : aAttachmentInfo.getAttachmentPassAssignment())
            {
                SSubpassDescription subpassDesc = {};

                for(auto const &index : attachmentResourceIndexList)
                {
                    FrameGraphResourceId_t const &resourceId = attachmentResources.at(index);

                    CEngineResult<CStdSharedPtr_t<SFrameGraphTextureView> const> const &textureViewFetch = aFrameGraphResources.get<SFrameGraphTextureView>(resourceId);
                    if(not textureViewFetch.successful())
                    {
                        CLog::Error(logTag(), CString::format("Fetching texture view w/ id %1 failed.", resourceId));
                        return { EEngineStatus::ResourceError_NotFound };
                    }

                    SFrameGraphTextureView const &textureView = *(textureViewFetch.data());

                    CEngineResult<CStdSharedPtr_t<SFrameGraphTexture> const> const &textureFetch = aFrameGraphResources.get<SFrameGraphTexture>(textureView.subjacentResource);
                    if(not textureFetch.successful())
                    {
                        CLog::Error(logTag(), CString::format("Fetching texture w/ id %1 failed.", textureView.subjacentResource));
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

            CRenderPass::CCreationRequest const renderPassCreationRequest(renderPassDesc);

            CEngineResult<> status = mResourceManager->createResource<CRenderPass>(renderPassCreationRequest, renderPassDesc.name, false);
            if(EEngineStatus::ResourceManager_ResourceAlreadyCreated == status.result())
            {
                return { EEngineStatus::Ok };
            }
            else if(not (EEngineStatus::Ok == status.result()))
            {
                EngineStatusPrintOnError(status.result(), logTag(), "Failed to create render pass.");
                return { status };
            }

            // Next: Create FrameBuffer Resource Types in VK Backend

            CFrameBuffer::SDescriptor frameBufferDesc = {};
            frameBufferDesc.name   = aFrameBufferId;
            frameBufferDesc.width  = static_cast<uint32_t>(width);
            frameBufferDesc.height = static_cast<uint32_t>(height);
            frameBufferDesc.layers = static_cast<uint32_t>(layers);
            frameBufferDesc.dependencies.push_back(renderPassDesc.name);
            frameBufferDesc.dependencies.insert(frameBufferDesc.dependencies.end(), textureViewIds.begin(), textureViewIds.end());

            CFrameBuffer::CCreationRequest const frameBufferCreationRequest(frameBufferDesc, renderPassDesc.name, textureViewIds);

            status = mResourceManager->createResource<CFrameBuffer>(frameBufferCreationRequest, frameBufferDesc.name, false);
            if(EEngineStatus::ResourceManager_ResourceAlreadyCreated == status.result())
            {
                return EEngineStatus::Ok;
            }
            else
            {
                EngineStatusPrintOnError(status.result(), logTag(), "Failed to create frame buffer.");
            }

            return status;

            // return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::bindFrameBufferAndRenderPass(std::string const &aFrameBufferId,
                                                                               std::string const &aRenderPassId)
        {
            EEngineStatus const status=mGraphicsAPIRenderContext->bindFrameBufferAndRenderPass(aFrameBufferId, aRenderPassId);
            if( not CheckEngineError(status))
            {
                // TODO: Implication of string -> PublicResourceId_t. Will break, once the underlying type
                //       of the PublicResourceId_t changes.
                mCurrentFrameBufferHandle = aFrameBufferId;
                mCurrentRenderPassHandle  = aRenderPassId;
                mCurrentSubpass           = 0; // Reset!
            }
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::unbindFrameBufferAndRenderPass(std::string const &aFrameBufferId,
                                                                                 std::string const &aRenderPassId)
        {
            mCurrentFrameBufferHandle = {};
            mCurrentRenderPassHandle  = {};
            mCurrentSubpass           = 0;

            return mGraphicsAPIRenderContext->unbindFrameBufferAndRenderPass(aFrameBufferId, aRenderPassId);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::destroyFrameBufferAndRenderPass(
                std::string                     const &aFrameBufferId,
                std::string                     const &aRenderPassId)
        {
            CEngineResult<> destruction = mResourceManager->destroyResource<CFrameBuffer>(aFrameBufferId);
            EngineStatusPrintOnError(destruction.result(), logTag(), "Failed to destroy frame buffer.");

            destruction = mResourceManager->destroyResource<CRenderPass>(aRenderPassId);
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
            PublicResourceId_t const pid = "";

            mapFrameGraphToInternalResource(aTexture.readableName, pid);

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::createTexture(SFrameGraphTexture const &aTexture)
        {
            CTexture::SDescriptor desc = {};
            desc.name        = aTexture.readableName;
            desc.textureInfo = aTexture;
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

            CTexture::CCreationRequest const request(desc);

            CLog::Verbose(logTag(), CString::format("Texture:\n%0", to_string(aTexture)));

            CEngineResult<> status = mResourceManager->createResource<CTexture>(request, aTexture.readableName, false);
            if(EEngineStatus::ResourceManager_ResourceAlreadyCreated == status.result())
            {
                return EEngineStatus::Ok;
            }
            else
            {
                EngineStatusPrintOnError(status.result(), logTag(), "Failed to create texture.");
            }

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::destroyTexture(SFrameGraphTexture const &aTexture)
        {
            CLog::Verbose(logTag(), CString::format("Texture:\n%0", to_string(aTexture)));

            CEngineResult<> status = mResourceManager->destroyResource<CTexture>(aTexture.readableName);

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
            CLog::Verbose(logTag(), CString::format("TextureView:\n%0", to_string(aView)));

            CTextureView::SDescriptor desc = { };
            desc.name             = aView.readableName;
            desc.textureFormat    = aView.format;
            desc.subjacentTexture = aTexture;
            desc.arraySlices      = aView.arraySliceRange;
            desc.mipMapSlices     = aView.mipSliceRange;
            desc.dependencies.push_back(aTexture.readableName);

            CTextureView::CCreationRequest const request(desc, aTexture.readableName);

            CEngineResult<> status = mResourceManager->createResource<CTextureView>(request, aView.readableName, false);
            EngineStatusPrintOnError(status.result(), logTag(), "Failed to create texture.");

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::bindTextureView(SFrameGraphTextureView const &aView)
        {
            CLog::Verbose(logTag(), CString::format("TextureView:\n%0", to_string(aView)));

            CEngineResult<Vector<PublicResourceId_t>> const &subjacentResourcesFetch = getMappedInternalResourceIds(aView.readableName);
            if(subjacentResourcesFetch.successful())
            {
                for(PublicResourceId_t const &pid : subjacentResourcesFetch.data())
                {
                    mGraphicsAPIRenderContext->bindResource(pid);
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
            CLog::Verbose(logTag(), CString::format("TextureView:\n%0", to_string(aView)));

            CEngineResult<Vector<PublicResourceId_t>> subjacentResourcesFetch = getMappedInternalResourceIds(aView.readableName);

            if(subjacentResourcesFetch.successful())
            {
                for(PublicResourceId_t const &pid : subjacentResourcesFetch.data())
                {
                    mGraphicsAPIRenderContext->unbindResource(pid);
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
            CLog::Verbose(logTag(), CString::format("TextureView:\n%0", to_string(aView)));

            CEngineResult<> status = EEngineStatus::Ok;
            status = mResourceManager->destroyResource<CTextureView>(aView.readableName);

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
            CBuffer::SDescriptor desc = { };
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

            CBuffer::CCreationRequest const request(desc);

            CEngineResult<> status = mResourceManager->createResource<CBuffer>(request, aBuffer.readableName, false);
            EngineStatusPrintOnError(status.result(), logTag(), "Failed to create buffer.");

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::destroyBuffer(SFrameGraphBuffer const &aBuffer)
        {
            CEngineResult<> status = EEngineStatus::Ok;
            status = mResourceManager->destroyResource<CBuffer>(aBuffer.readableName);

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
            CBufferView::SDescriptor desc = { };
            desc.name = aBuffer.readableName;

            VkBufferViewCreateInfo &createInfo = desc.createInfo;
            createInfo.sType  = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
            createInfo.pNext  = nullptr;
            createInfo.flags  = 0;
            // createInfo.offset = "...";
            // createInfo.buffer = "...";
            // createInfo.format = "...";
            // createInfo.range  = "...";

            CBufferView::CCreationRequest const request(desc, aBuffer.readableName);

            CEngineResult<> status = mResourceManager->createResource<CBufferView>(request, aBufferView.readableName, false);
            EngineStatusPrintOnError(status.result(), logTag(), "Failed to create buffer view.");

            return status;
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
        CEngineResult<> CFrameGraphRenderContext::loadMaterialAsset(SFrameGraphMaterial   const &aMaterial
                                                                    , PublicResourceId_t  const &aRenderPassHandle)
        {
            auto const &[result, instance] = mMaterialLoader->loadMaterialInstance(aMaterial.materialAssetId);
            if(CheckEngineError(result))
            {
                return { result };
            }

            CStdSharedPtr_t<CMaterialMaster> const &master    = instance; // instance->master();
            SMaterialSignature               const &signature = master  ->signature();
            CMaterialConfig                  const &config    = instance->config();

            CPipelineDeclaration::SDescriptor pipelineDescriptor {};

            pipelineDescriptor.inputAssemblyState.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            pipelineDescriptor.inputAssemblyState.pNext                  = nullptr;
            pipelineDescriptor.inputAssemblyState.flags                  = 0;
            pipelineDescriptor.inputAssemblyState.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            pipelineDescriptor.inputAssemblyState.primitiveRestartEnable = false;

            pipelineDescriptor.rasterizerState.sType                     = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            pipelineDescriptor.rasterizerState.pNext                     = nullptr;
            pipelineDescriptor.rasterizerState.flags                     = 0;
            pipelineDescriptor.rasterizerState.cullMode                  = VkCullModeFlagBits::VK_CULL_MODE_NONE;
            pipelineDescriptor.rasterizerState.frontFace                 = VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;
            pipelineDescriptor.rasterizerState.polygonMode               = VkPolygonMode::VK_POLYGON_MODE_FILL;
            pipelineDescriptor.rasterizerState.lineWidth                 = 1.0f;
            pipelineDescriptor.rasterizerState.rasterizerDiscardEnable   = VK_FALSE;
            pipelineDescriptor.rasterizerState.depthClampEnable          = VK_FALSE;
            pipelineDescriptor.rasterizerState.depthBiasEnable           = VK_FALSE;
            pipelineDescriptor.rasterizerState.depthBiasSlopeFactor      = 0.0f;
            pipelineDescriptor.rasterizerState.depthBiasConstantFactor   = 0.0f;
            pipelineDescriptor.rasterizerState.depthBiasClamp            = 0.0f;

            pipelineDescriptor.multiSampler.sType                        = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            pipelineDescriptor.multiSampler.pNext                        = nullptr;
            pipelineDescriptor.multiSampler.flags                        = 0;
            pipelineDescriptor.multiSampler.sampleShadingEnable          = VK_FALSE;
            pipelineDescriptor.multiSampler.rasterizationSamples         = VK_SAMPLE_COUNT_1_BIT;
            pipelineDescriptor.multiSampler.minSampleShading             = 1.0f;
            pipelineDescriptor.multiSampler.pSampleMask                  = nullptr;
            pipelineDescriptor.multiSampler.alphaToCoverageEnable        = VK_FALSE;
            pipelineDescriptor.multiSampler.alphaToOneEnable             = VK_FALSE;

            pipelineDescriptor.depthStencilState.sType                   = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            pipelineDescriptor.depthStencilState.pNext                   = nullptr;
            pipelineDescriptor.depthStencilState.flags                   = 0;
            pipelineDescriptor.depthStencilState.depthTestEnable         = VK_FALSE;
            pipelineDescriptor.depthStencilState.depthWriteEnable        = VK_FALSE;
            pipelineDescriptor.depthStencilState.depthCompareOp          = VkCompareOp::VK_COMPARE_OP_LESS;
            pipelineDescriptor.depthStencilState.stencilTestEnable       = VK_FALSE;
            pipelineDescriptor.depthStencilState.front.passOp            = VkStencilOp::VK_STENCIL_OP_KEEP;
            pipelineDescriptor.depthStencilState.front.failOp            = VkStencilOp::VK_STENCIL_OP_KEEP;
            pipelineDescriptor.depthStencilState.front.depthFailOp       = VkStencilOp::VK_STENCIL_OP_KEEP;
            pipelineDescriptor.depthStencilState.front.compareOp         = VkCompareOp::VK_COMPARE_OP_ALWAYS;
            pipelineDescriptor.depthStencilState.front.compareMask       = 0;
            pipelineDescriptor.depthStencilState.front.writeMask         = 0;
            pipelineDescriptor.depthStencilState.front.reference         = 0;
            pipelineDescriptor.depthStencilState.back                    = pipelineDescriptor.depthStencilState.front;
            pipelineDescriptor.depthStencilState.depthBoundsTestEnable   = VK_FALSE;
            pipelineDescriptor.depthStencilState.minDepthBounds          = 0.0f;
            pipelineDescriptor.depthStencilState.maxDepthBounds          = 1.0f;

            for(auto const &[stageKey, stage] : signature.stages)
            {
                if(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_VERTEX_SHADER_BIT == stageKey)
                {
                    for(uint32_t k=0; k<stage.inputs.size(); ++k)
                    {
                        SStageInput const &input = stage.inputs.at(k);

                        VkVertexInputBindingDescription binding;
                        binding.binding   = k;
                        binding.stride    = input.type.arrayStride;
                        binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

                        VkVertexInputAttributeDescription attribute;
                        attribute.binding  = k;
                        attribute.location = k;
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
                        colorBlendAttachmentState.blendEnable        =VK_TRUE;
                        colorBlendAttachmentState.colorWriteMask     =VkColorComponentFlagBits::VK_COLOR_COMPONENT_R_BIT|
                                                                      VkColorComponentFlagBits::VK_COLOR_COMPONENT_G_BIT|
                                                                      VkColorComponentFlagBits::VK_COLOR_COMPONENT_B_BIT|
                                                                      VkColorComponentFlagBits::VK_COLOR_COMPONENT_A_BIT;
                        colorBlendAttachmentState.srcColorBlendFactor=VkBlendFactor::VK_BLEND_FACTOR_ONE;  // VkBlendFactor::VK_BLEND_FACTOR_SRC_ALPHA;
                        colorBlendAttachmentState.dstColorBlendFactor=VkBlendFactor::VK_BLEND_FACTOR_ZERO; // VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
                        colorBlendAttachmentState.colorBlendOp       =VkBlendOp::VK_BLEND_OP_ADD;
                        colorBlendAttachmentState.srcAlphaBlendFactor=VkBlendFactor::VK_BLEND_FACTOR_ONE;
                        colorBlendAttachmentState.dstAlphaBlendFactor=VkBlendFactor::VK_BLEND_FACTOR_ZERO;
                        colorBlendAttachmentState.alphaBlendOp       =VkBlendOp::VK_BLEND_OP_ADD;

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

                        auto const [result, buffer] = mAssetStorage->loadAssetData(assetUid);
                        if(CheckEngineError(result))
                        {
                            CLog::Error(logTag(), "Failed to load shader module asset data. Result: %0", result);
                            return {};
                        }

                        return buffer;
                    };

                    pipelineDescriptor.shaderStages[stageKey] = dataAccessor;
                }
            }

            std::vector<VkDescriptorSetLayoutCreateInfo> descriptorSets {};
            descriptorSets.resize(signature.layoutInfo.setCount);
            pipelineDescriptor.descriptorSetLayoutBindings.resize(signature.layoutInfo.setCount);

            for(uint32_t k=0; k<descriptorSets.size(); ++k)
            {
                VkDescriptorSetLayoutCreateInfo &info = descriptorSets[k];

                info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                info.pNext        = nullptr;
                info.flags        = 0;
                info.bindingCount = signature.layoutInfo.setBindingCount[k];

                pipelineDescriptor.descriptorSetLayoutBindings[k].resize(info.bindingCount);
            }

            pipelineDescriptor.descriptorSetLayoutCreateInfos = descriptorSets;

            for(SSubpassInput const &input : signature.subpassInputs)
            {
                VkDescriptorSetLayoutBinding layoutBinding {};
                layoutBinding.binding            = input.binding;
                layoutBinding.descriptorType     = VkDescriptorType::VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
                layoutBinding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT; // Subpass inputs are only accessibly in fragment shaders.
                layoutBinding.descriptorCount    = 1;
                layoutBinding.pImmutableSamplers = nullptr;
                pipelineDescriptor.descriptorSetLayoutBindings[input.set][input.binding] = layoutBinding;
            }

             for(SUniformBuffer const &uniformBuffer : signature.uniformBuffers)
            {
                VkDescriptorSetLayoutBinding layoutBinding {};
                layoutBinding.binding            = uniformBuffer.binding;
                layoutBinding.descriptorType     = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                layoutBinding.stageFlags         = serialization::shaderStageFromPipelineStage(uniformBuffer.stageBinding.value());
                layoutBinding.descriptorCount    = 1;
                layoutBinding.pImmutableSamplers = nullptr;
                pipelineDescriptor.descriptorSetLayoutBindings[uniformBuffer.set][uniformBuffer.binding] = layoutBinding;

                // TODO: Arrays?
            }

            for(SSampledImage const &sampledImage : signature.sampledImages)
            {
                VkDescriptorSetLayoutBinding layoutBinding {};
                layoutBinding.binding            = sampledImage.binding;
                layoutBinding.descriptorType     = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                layoutBinding.stageFlags         = serialization::shaderStageFromPipelineStage(sampledImage.stageBinding.value());
                layoutBinding.descriptorCount    = 1;
                layoutBinding.pImmutableSamplers = nullptr;
                pipelineDescriptor.descriptorSetLayoutBindings[sampledImage.set][sampledImage.binding] = layoutBinding;
            }

            VkViewport viewPort = {};
            viewPort.x        = 0.0;
            viewPort.y        = 0.0;
            viewPort.width    = 1920.0;
            viewPort.height   = 1080.0;
            viewPort.minDepth = 0.0;
            viewPort.maxDepth = 1.0;
            pipelineDescriptor.viewPort = viewPort;

            pipelineDescriptor.subpass = mCurrentSubpass;

            PublicResourceId_t     const renderPassHandle   = mCurrentRenderPassHandle;
            PublicResourceIdList_t const textureViewHandles = {};
            PublicResourceIdList_t const bufferViewHandles  = {};

            pipelineDescriptor.dependencies.push_back(renderPassHandle); // Remarks to myself: You are stupid...
            pipelineDescriptor.name = aMaterial.readableName;

            CPipeline::CCreationRequest request(pipelineDescriptor
                                                , renderPassHandle
                                                , textureViewHandles
                                                , bufferViewHandles);

            CEngineResult<> status = mResourceManager->createResource<CPipeline>(request, aMaterial.readableName, false);
            EngineStatusPrintOnError(status.result(), logTag(), "Failed to create pipeline.");

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::bindMaterial(SFrameGraphMaterial const &aMaterial)
        {
            auto const result = mGraphicsAPIRenderContext->bindPipeline(aMaterial.readableName);
            return result;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::unbindMaterial(SFrameGraphMaterial const &aMaterial)
        {
            auto const result = mGraphicsAPIRenderContext->unbindPipeline(aMaterial.readableName);
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

    }
}
