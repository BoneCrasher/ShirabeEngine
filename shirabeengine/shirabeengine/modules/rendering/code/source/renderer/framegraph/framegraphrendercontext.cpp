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
        CStdSharedPtr_t<CFrameGraphRenderContext> CFrameGraphRenderContext::create(
                CStdSharedPtr_t<IAssetStorage>        aAssetStorage,
                CStdSharedPtr_t<CResourceManagerBase> aResourceManager,
                CStdSharedPtr_t<IRenderContext>       aRenderer)
        {
            assert(aAssetStorage    != nullptr);
            assert(aResourceManager != nullptr);
            assert(aRenderer        != nullptr);

            CStdSharedPtr_t<CFrameGraphRenderContext> context = nullptr;
            context = CStdSharedPtr_t<CFrameGraphRenderContext>(new CFrameGraphRenderContext(aAssetStorage, aResourceManager, aRenderer));
            if(!context)
                CLog::Error(logTag(), "Failed to create render context from renderer and resourcemanager.");

            return context;
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
        void CFrameGraphRenderContext::mapFrameGraphToInternalResource(
                std::string        const &aName,
                PublicResourceId_t const &aResourceId)
        {
            mResourceMap[aName].push_back(aResourceId);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        Vector<PublicResourceId_t> CFrameGraphRenderContext::getMappedInternalResourceIds(std::string const &aName) const
        {
            if(mResourceMap.find(aName) == mResourceMap.end())
                return {};

            return mResourceMap.at(aName);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void CFrameGraphRenderContext::removeMappedInternalResourceIds(std::string const &aName)
        {
            mResourceMap.erase(aName);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::bindCommandBuffer()
        {
            EEngineStatus const status = mGraphicsAPIRenderContext->bindGraphicsCommandBuffer();

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::commitCommandBuffer()
        {
            EEngineStatus const status = mGraphicsAPIRenderContext->commitGraphicsCommandBuffer();

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::bindSwapChain(SFrameGraphResource const &aSwapChainResource)
        {
            EEngineStatus const status = mGraphicsAPIRenderContext->bindSwapChain(aSwapChainResource.readableName);

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::present()
        {
            EEngineStatus const status = mGraphicsAPIRenderContext->present();

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::createFrameBufferAndRenderPass(
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

            for(auto const &[passUID, attachmentResourceIdList] : aAttachmentInfo.getAttachmentPassAssignment())
            {
                SSubpassDescription subpassDesc = {};

                for(auto const &id : attachmentResourceIdList)
                {
                    CStdSharedPtr_t<SFrameGraphTextureView> const &textureView = aFrameGraphResources.get<SFrameGraphTextureView>(id);
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
                        HandleEngineStatusError(EEngineStatus::Error, "Invalid image view dimensions for frame buffer creation.");
                    }

                    // Create the underlying resources upfront, so that the renderpass and framebuffer creation can take place properly.
                    // Performing the creation and registration here will also cause the index of texture view public resource ids to
                    // match up with the index of attachment descriptions, which is also a requirement of the FrameBuffer <-> RenderPass
                    // system.
                    createTexture(*texture);
                    createTextureView(*texture, *textureView);
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

            EEngineStatus status = mResourceManager->createResource<CRenderPass>(renderPassCreationRequest, renderPassDesc.name, false);
            if(EEngineStatus::ResourceManager_ResourceAlreadyCreated == status)
                return EEngineStatus::Ok;
            else
                HandleEngineStatusError(status, "Failed to create render pass.");

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
            if(EEngineStatus::ResourceManager_ResourceAlreadyCreated == status)
                return EEngineStatus::Ok;
            else
                HandleEngineStatusError(status, "Failed to create frame buffer.");

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::importTexture(SFrameGraphTexture const &aTexture)
        {
            PublicResourceId_t const pid = "";

            mapFrameGraphToInternalResource(aTexture.readableName, pid);

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::createTexture(SFrameGraphTexture const &aTexture)
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

            EEngineStatus status = mResourceManager->createResource<CTexture>(request, aTexture.readableName, false);
            if(EEngineStatus::ResourceManager_ResourceAlreadyCreated == status)
                return EEngineStatus::Ok;
            else
                HandleEngineStatusError(status, "Failed to create texture.");

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::createTextureView(
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

            EEngineStatus status = mResourceManager->createResource<CTextureView>(request, aView.readableName, false);
            HandleEngineStatusError(status, "Failed to create texture.");

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::createBuffer(
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
        EEngineStatus CFrameGraphRenderContext::createBufferView(
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
        EEngineStatus CFrameGraphRenderContext::loadTextureAsset(AssetId_t const &aAssetUID)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::loadBufferAsset(AssetId_t const &aAssetUID)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::loadMeshAsset(AssetId_t const &aAssetUID)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::bindTextureView(SFrameGraphTextureView const &aView)
        {
            CLog::Verbose(logTag(), CString::format("TextureView:\n%0", to_string(aView)));

            Vector<PublicResourceId_t> const &subjacentResources = getMappedInternalResourceIds(aView.readableName);

            for(PublicResourceId_t const&pid : subjacentResources)
            {
                mGraphicsAPIRenderContext->bindResource(pid);
            }

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::bindBufferView(FrameGraphResourceId_t  const &aResourceId)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::bindMesh(AssetId_t const&aAssetUID)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::unbindTextureView(SFrameGraphTextureView const &aView)
        {
            CLog::Verbose(logTag(), CString::format("TextureView:\n%0", to_string(aView)));

            Vector<PublicResourceId_t> const&subjacentResources = getMappedInternalResourceIds(aView.readableName);

            for(PublicResourceId_t const&pid : subjacentResources)
            {
                mGraphicsAPIRenderContext->unbindResource(pid);
            }

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::unbindBufferView(FrameGraphResourceId_t const &aResourceId)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::unbindMesh(AssetId_t const &aAssetUID)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::unloadTextureAsset(AssetId_t const &aAssetUID)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::unloadBufferAsset(AssetId_t const &aAssetUID)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::unloadMeshAsset(AssetId_t const &aAssetUID)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::destroyTexture(SFrameGraphTexture const &aTexture)
        {
            CLog::Verbose(logTag(), CString::format("Texture:\n%0", to_string(aTexture)));

            EEngineStatus status = mResourceManager->destroyResource<CTexture>(aTexture.readableName);

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::destroyTextureView(SFrameGraphTextureView  const &aView)
        {
            CLog::Verbose(logTag(), CString::format("TextureView:\n%0", to_string(aView)));

            EEngineStatus status = EEngineStatus::Ok;
            status = mResourceManager->destroyResource<CTextureView>(aView.readableName);

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::destroyBuffer(FrameGraphResourceId_t const &aResourceId)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::destroyBufferView(FrameGraphResourceId_t const &aResourceId)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::destroyFrameBufferAndRenderPass(
                std::string                     const &aFrameBufferId,
                std::string                     const &aRenderPassId,
                SFrameGraphAttachmentCollection const &aAttachmentInfo,
                CFrameGraphMutableResources     const &aFrameGraphResources)
        {
            EEngineStatus status = mResourceManager->destroyResource<CFrameBuffer>(aFrameBufferId);
            HandleEngineStatusError(status, "Failed to destroy frame buffer.");

            FrameGraphResourceIdList texturesToFree;

            for(FrameGraphResourceId_t const &attachmentResourceId : aAttachmentInfo.getAttachementResourceIds())
            {
                CStdSharedPtr_t<SFrameGraphTextureView> textureView = aFrameGraphResources.get<SFrameGraphTextureView>(attachmentResourceId);
                assert(nullptr != textureView);

                status = mResourceManager->destroyResource<CTextureView>(textureView->readableName);
                HandleEngineStatusError(status, "Failed to free texture view.");

                // Make sure to uniquely register the subjacent resource for later release.
                auto const &iterator = std::find(texturesToFree.begin(), texturesToFree.end(), textureView->subjacentResource);
                if(texturesToFree.end() != iterator)
                {
                    texturesToFree.push_back(textureView->subjacentResource);
                }
            }

            // Free the subjacent textures UNLESS the resource is external/persistent...
            for(FrameGraphResourceId_t const &textureId : texturesToFree)
            {
                CStdSharedPtr_t<SFrameGraphTexture> texture = aFrameGraphResources.get<SFrameGraphTexture>(textureId);
                assert(nullptr != texture);

                if(not texture->isExternalResource)
                {
                    status = mResourceManager->destroyResource<CTexture>(texture->readableName);
                    HandleEngineStatusError(status, "Failed to free texture.");
                }
            }

            status = mResourceManager->destroyResource<CRenderPass>(aRenderPassId);
            HandleEngineStatusError(status, "Failed to destroy render pass.");

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::render(SRenderable const &aRenderable)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

    }
}
