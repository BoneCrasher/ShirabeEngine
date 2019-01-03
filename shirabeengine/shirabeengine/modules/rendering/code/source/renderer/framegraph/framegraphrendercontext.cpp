#include <assert.h>

#include <graphicsapi/resources/types/renderpass.h>
#include <graphicsapi/resources/types/framebuffer.h>

#include "renderer/irenderer.h"
#include "renderer/framegraph/framegraphrendercontext.h"

namespace engine
{
    namespace framegraph
    {
        using namespace engine::rendering;

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<CStdSharedPtr_t<CFrameGraphRenderContext>> CFrameGraphRenderContext::create(
                CStdSharedPtr_t<IAssetStorage>        aAssetStorage,
                CStdSharedPtr_t<CResourceManagerBase> aResourceManager,
                CStdSharedPtr_t<IRenderContext>       aRenderer)
        {
            bool const inputInvalid =
                    nullptr == aAssetStorage    or
                    nullptr == aResourceManager or
                    nullptr == aRenderer;

            if(inputInvalid)
            {
                return { EEngineStatus::Error };
            }

            CStdSharedPtr_t<CFrameGraphRenderContext> context = CStdSharedPtr_t<CFrameGraphRenderContext>(new CFrameGraphRenderContext(aAssetStorage, aResourceManager, aRenderer));
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
                CStdSharedPtr_t<CResourceManagerBase> aResourceManager,
                CStdSharedPtr_t<IRenderContext>       aRenderer)
            : mAssetStorage(aAssetStorage)
            , mResourceManager(aResourceManager)
            , mGraphicsAPIRenderContext(aRenderer)
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
        CEngineResult<> CFrameGraphRenderContext::bindCommandBuffer()
        {
            CEngineResult<> const status = mGraphicsAPIRenderContext->bindGraphicsCommandBuffer();

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
        CEngineResult<> CFrameGraphRenderContext::bindSwapChain(SFrameGraphResource const &aSwapChainResource)
        {
            CEngineResult<> const status = mGraphicsAPIRenderContext->bindSwapChain(aSwapChainResource.readableName);

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
        CEngineResult<> CFrameGraphRenderContext::createFrameBufferAndRenderPass(
                std::string                     const &aFrameBufferId,
                std::string                     const &aRenderPassId,
                SFrameGraphAttachmentCollection const &aAttachmentInfo,
                CFrameGraphMutableResources     const &aFrameGraphResources)
        {
            //<-----------------------------------------------------------------------------
            // Helper function to find attachment indices in index lists.
            //<-----------------------------------------------------------------------------
            auto const findAttachmentRelationFn = [] (Vector<uint64_t> const &aRelationIndices, uint64_t const &aIndex) -> bool
            {
                auto const predicate = [&] (uint64_t const &aTestIndex) -> bool
                {
                    return (aIndex == aTestIndex);
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
                    FrameGraphResourceId_t const &resourceId = attachmentResources[index];

                    CStdSharedPtr_t<SFrameGraphTextureView> const &textureView = aFrameGraphResources.get<SFrameGraphTextureView>(resourceId);
                    assert(nullptr != textureView);
                    CStdSharedPtr_t<SFrameGraphTexture>     const &texture     = aFrameGraphResources.get<SFrameGraphTexture>(textureView->subjacentResource);
                    assert(nullptr != texture);

                    // Validation first!
                    bool dimensionsValid = true;
                    if(0 > width)
                    {
                        width  = static_cast<int32_t>(texture->width);
                        height = static_cast<int32_t>(texture->height);
                        layers = textureView->arraySliceRange.length;

                        dimensionsValid = (0 < width and 0 < height and 0 < layers);
                    }
                    else
                    {
                        bool const validWidth  = (width  == static_cast<int32_t>(texture->width));
                        bool const validHeight = (height == static_cast<int32_t>(texture->height));
                        bool const validLayers = (layers == static_cast<int32_t>(textureView->arraySliceRange.length));

                        dimensionsValid = (validWidth and validHeight and validLayers);
                    }

                    if(not dimensionsValid)
                    {
                        EngineStatusPrintOnError(EEngineStatus::Error, logTag(), "Invalid image view dimensions for frame buffer creation.");
                        return { EEngineStatus::Error };
                    }

                    // The texture view's dimensions are valid. Register it for the frame buffer texture view id list.
                    textureViewIds.push_back(textureView->readableName);

                    uint64_t const attachmentIndex = renderPassDesc.attachmentDescriptions.size();

                    SAttachmentDescription attachmentDesc = {};
                    attachmentDesc.loadOp         = EAttachmentLoadOp::LOAD;
                    attachmentDesc.stencilLoadOp  = attachmentDesc.loadOp;
                    attachmentDesc.storeOp        = EAttachmentStoreOp::STORE;
                    attachmentDesc.stencilStoreOp = attachmentDesc.storeOp;
                    attachmentDesc.format         = textureView->format;
                    attachmentDesc.initialLayout  = EImageLayout::UNDEFINED;       // For now we don't care about the initial layout...
                    attachmentDesc.finalLayout    = EImageLayout::PRESENT_SRC_KHR; // For now we just assume everything to be presentable...

                    renderPassDesc.attachmentDescriptions.push_back(attachmentDesc);

                    bool const isColorAttachment = findAttachmentRelationFn(aAttachmentInfo.getColorAttachments(), attachmentIndex);
                    bool const isDepthAttachment = findAttachmentRelationFn(aAttachmentInfo.getDepthAttachments(), attachmentIndex);
                    bool const isInputAttachment = findAttachmentRelationFn(aAttachmentInfo.getInputAttachments(), attachmentIndex);

                    SAttachmentReference attachmentReference {};
                    attachmentReference.attachment = static_cast<uint32_t>(attachmentIndex);

                    if(isColorAttachment)
                    {
                        attachmentReference.layout = EImageLayout::COLOR_ATTACHMENT_OPTIMAL;
                        subpassDesc.colorAttachments.push_back(attachmentReference);
                    }
                    else if(isDepthAttachment)
                    {
                        attachmentReference.layout = EImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
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
            else
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

            if(aTexture.requestedUsage.check(EFrameGraphResourceUsage::ColorAttachment))
                desc.gpuBinding.set(EBufferBinding::ColorAttachment);
            if(aTexture.requestedUsage.check(EFrameGraphResourceUsage::DepthAttachment))
                desc.gpuBinding.set(EBufferBinding::DepthAttachment);
            if(aTexture.requestedUsage.check(EFrameGraphResourceUsage::InputAttachment))
                desc.gpuBinding.set(EBufferBinding::InputAttachment);

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
        CEngineResult<> CFrameGraphRenderContext::createBuffer(
                FrameGraphResourceId_t const &aResourceId,
                SFrameGraphResource    const &aResource,
                SFrameGraphBuffer      const &aBuffer)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::createBufferView(
                FrameGraphResourceId_t const &aResourceId,
                SFrameGraphResource    const &aResource,
                SFrameGraphBufferView  const &aBufferView)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::loadTextureAsset(AssetId_t const &aAssetUID)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::loadBufferAsset(AssetId_t const &aAssetUID)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::loadMeshAsset(AssetId_t const &aAssetUID)
        {
            return EEngineStatus::Ok;
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
        CEngineResult<> CFrameGraphRenderContext::bindBufferView(FrameGraphResourceId_t  const &aResourceId)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::bindMesh(AssetId_t const&aAssetUID)
        {
            return EEngineStatus::Ok;
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
        CEngineResult<> CFrameGraphRenderContext::unbindBufferView(FrameGraphResourceId_t const &aResourceId)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::unbindMesh(AssetId_t const &aAssetUID)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::unloadTextureAsset(AssetId_t const &aAssetUID)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::unloadBufferAsset(AssetId_t const &aAssetUID)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::unloadMeshAsset(AssetId_t const &aAssetUID)
        {
            return EEngineStatus::Ok;
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
        CEngineResult<> CFrameGraphRenderContext::destroyBuffer(FrameGraphResourceId_t const &aResourceId)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::destroyBufferView(FrameGraphResourceId_t const &aResourceId)
        {
            return EEngineStatus::Ok;
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
        CEngineResult<> CFrameGraphRenderContext::render(SRenderable const &aRenderable)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

    }
}
