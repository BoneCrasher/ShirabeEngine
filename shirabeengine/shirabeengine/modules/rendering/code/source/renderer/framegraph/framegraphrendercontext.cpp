#include <cassert>

#include <fmt/format.h>
#include <material/loader.h>
#include <material/declaration.h>
#include <material/serialization.h>
#include <resources/cresourcemanager.h>
#include <resources/resourcetypes.h>

#include "renderer/irenderer.h"
#include "renderer/framegraph/framegraphrendercontext.h"

namespace engine::framegraph
{
    using namespace engine::rendering;
    using namespace engine::material;

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<Shared<CFrameGraphRenderContext>> CFrameGraphRenderContext::create(
            Shared<IAssetStorage>    const &aAssetStorage,
            Shared<CResourceManager> const &aResourceManager,
            Shared<IRenderContext>   const &aRenderer)
    {
        bool const inputInvalid =
                nullptr == aAssetStorage    or
                nullptr == aResourceManager or
                nullptr == aRenderer;

        if(inputInvalid)
        {
            return { EEngineStatus::Error };
        }

        auto context = makeShared<CFrameGraphRenderContext>(aAssetStorage
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
            Shared<CResourceManager> aResourceManager,
            Shared<IRenderContext>   aRenderer)
        : mAssetStorage            (std::move(aAssetStorage   ))
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
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CFrameGraphRenderContext::clearAttachments(std::string const &aRenderPassId)
    {
        Shared<SRenderPass> renderPass = getUsedResourceTyped<SRenderPass>(aRenderPassId);

        EEngineStatus const status = mGraphicsAPIRenderContext->clearAttachments(renderPass->getGpuApiResourceHandle(), mCurrentSubpass);
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
    //
    //<-----------------------------------------------------------------------------
    EEngineStatus CFrameGraphRenderContext::performImageLayoutTransfer(  SFrameGraphTexture const &aImageHandle
                                                                       , CRange             const &aArrayRange
                                                                       , CRange             const &aMipRange
                                                                       , VkImageAspectFlags const &aAspectFlags
                                                                       , VkImageLayout      const &aSourceLayout
                                                                       , VkImageLayout      const &aTargetLayout)
    {
        Shared<ILogicalResourceObject> source = getUsedResource(aImageHandle.readableName);
        CEngineResult<> const status = mGraphicsAPIRenderContext->performImageLayoutTransfer( source->getGpuApiResourceHandle()
                                                                                             , aArrayRange
                                                                                             , aMipRange
                                                                                             , aAspectFlags
                                                                                             , aSourceLayout
                                                                                             , aTargetLayout);

        return status.result();
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
    CEngineResult<> CFrameGraphRenderContext::beginGraphicsFrame()
    {
        CEngineResult<> const status = mGraphicsAPIRenderContext->beginGraphicsFrame();

        return status;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CEngineResult<> CFrameGraphRenderContext::endGraphicsFrame()
    {
        CEngineResult<> const status = mGraphicsAPIRenderContext->endGraphicsFrame();

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
    CEngineResult<> CFrameGraphRenderContext::beginFrameCommandBuffers()
    {
        CEngineResult<> const status = mGraphicsAPIRenderContext->beginFrameCommandBuffers();

        return status;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CEngineResult<> CFrameGraphRenderContext::commitFrameCommandBuffers()
    {
        CEngineResult<> const status = mGraphicsAPIRenderContext->commitFrameCommandBuffers();

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
            std::vector<PassUID_t>          const &aPassExecutionOrder,
            SFrameGraphAttachmentCollection const &aAttachmentInfo,
            CFrameGraphMutableResources     const &aFrameGraphResources)
    {

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
        Shared<SRenderPass>          renderPass     = getUsedResourceTyped<SRenderPass>(aRenderPassId);
        SRenderPassDescription const renderPassDesc = renderPass->getDescription();

        SFrameBufferDescription frameBufferDesc = {};
        frameBufferDesc.name = aFrameBufferId;
        // frameBufferDesc.referenceRenderPassId = aRenderPassId;


        {
            std::vector<std::string> frameBufferDependencies {};
            frameBufferDependencies.push_back(renderPassDesc.name);
            // frameBufferDependencies.insert(frameBufferDependencies.end(), textureViewIds.begin(), textureViewIds.end());

            CEngineResult<Shared<ILogicalResourceObject>> status = mResourceManager->useDynamicResource<SFrameBuffer>(frameBufferDesc.name, frameBufferDesc);
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
    CEngineResult<> CFrameGraphRenderContext::bindRenderPass(std::string                     const &aRenderPassId,
                                                             std::string                     const &aFrameBufferId,
                                                             std::vector<PassUID_t>          const &aPassExecutionOrder,
                                                             SFrameGraphAttachmentCollection const &aAttachmentInfo,
                                                             CFrameGraphMutableResources     const &aFrameGraphResources
                                                             )
    {
        Shared<SRenderPass>  renderPass  = getUsedResourceTyped<SRenderPass>(aRenderPassId);
        Shared<SFrameBuffer> frameBuffer = getUsedResourceTyped<SFrameBuffer>(aFrameBufferId);

        SRenderPassDescription const &renderPassDesc = renderPass->getDescription();

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
                    return { EEngineStatus::ResourceError_NotFound };
                }

                SFrameGraphTextureView const &textureView = *(textureViewFetch.data());

                // The texture view's dimensions are valid. Register it for the frame buffer texture view id list.
                if(renderPassDependencies.attachmentTextureViews.end() == std::find_if( renderPassDependencies.attachmentTextureViews.begin()
                                                                                      , renderPassDependencies.attachmentTextureViews.end()
                                                                                      , [textureView] (std::string const &aViewId) -> bool { return (aViewId == textureView.readableName); }))
                {
                    renderPassDependencies.attachmentTextureViews.push_back(textureView.readableName);
                }
            }
        }

        EEngineStatus const renderPassLoaded = renderPass->initialize(renderPassDependencies).result(); // Make sure the resource is loaded before it is used in a command...
        EngineStatusPrintOnError(renderPassLoaded, logTag(), "Failed to load renderpass in backend.");
        SHIRABE_RETURN_RESULT_ON_ERROR(renderPassLoaded);

        SFrameBufferDependencies frameBufferDependencies {};
        frameBufferDependencies.referenceRenderPassId  = aRenderPassId;
        frameBufferDependencies.attachmentExtent       = renderPassDesc.attachmentExtent;
        frameBufferDependencies.attachmentTextureViews = renderPassDependencies.attachmentTextureViews;

        EEngineStatus const frameBufferLoaded = frameBuffer->initialize(frameBufferDependencies).result();
        EngineStatusPrintOnError(frameBufferLoaded, logTag(), "Failed to load framebuffer in backend.");
        SHIRABE_RETURN_RESULT_ON_ERROR(renderPassLoaded);

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
        Shared<SFrameBuffer> frameBuffer = getUsedResourceTyped<SFrameBuffer>(aFrameBufferId);
        frameBuffer->unload();
        return frameBuffer->deinitialize(*(frameBuffer->getCurrentDependencies()));
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CEngineResult<> CFrameGraphRenderContext::destroyRenderPass(std::string const &aRenderPassId)
    {
        Shared<SRenderPass> renderPass = getUsedResourceTyped<SRenderPass>(aRenderPassId);
        renderPass->unload();
        return renderPass->deinitialize(*(renderPass->getCurrentDependencies()));

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

        Shared<STexture> resource = getUsedResourceTyped<STexture>(aTexture.readableName);
        resource->unload();
        return resource->deinitialize(*(resource->getCurrentDependencies()));
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
        desc.subjacentTextureInfo = static_cast<graphicsapi::STextureInfo>(aTexture);
        desc.arraySlices          = aView.arraySliceRange;
        desc.mipMapSlices         = aView.mipSliceRange;

        CEngineResult<Shared<ILogicalResourceObject>> textureViewObject = mResourceManager->useDynamicResource<STextureView>(desc.name, desc);
        EngineStatusPrintOnError(textureViewObject.result(), logTag(), "Failed to create texture.");

        registerUsedResource(desc.name, textureViewObject.data());

        Shared<STexture> texture = getUsedResourceTyped<STexture>(aTexture.readableName);
        texture->initialize({}).result();

        STextureViewDependencies dependencies {};
        dependencies.subjacentTextureId = aTexture.readableName;
        Shared<STextureView> textureView = getUsedResourceTyped<STextureView>(aView.readableName);
        textureView->initialize(dependencies).result();

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

        Shared<STextureView> resource = getUsedResourceTyped<STextureView>(aView.readableName);
        resource->unload();
        return resource->deinitialize(*(resource->getCurrentDependencies()));
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
        Shared<SBuffer> resource = getUsedResourceTyped<SBuffer>(aBuffer.readableName);
        resource->unload();
        return resource->deinitialize(*(resource->getCurrentDependencies()));
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

        CEngineResult<Shared<ILogicalResourceObject>> bufferViewObject = mResourceManager->useDynamicResource<SBufferView>(desc.name, desc);
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
        Shared<SBufferView> resource = getUsedResourceTyped<SBufferView>(aBufferView.readableName);
        resource->unload();
        return resource->deinitialize(*(resource->getCurrentDependencies()));
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CEngineResult<> CFrameGraphRenderContext::readMeshAsset(SFrameGraphMesh const &aMesh)
    {
        CEngineResult<Shared<ILogicalResourceObject>> meshObject = mResourceManager->useAssetResource(aMesh.readableName, aMesh.meshAssetId);
        if(CheckEngineError(meshObject.result()))
        {
            CLog::Error(logTag(), "Cannot use material asset {} with id {}", aMesh.readableName, aMesh.meshAssetId);
            return meshObject.result();
        }

        Shared<SMesh> mesh = std::static_pointer_cast<SMesh>(meshObject.data());
        registerUsedResource(aMesh.readableName, mesh);

        return meshObject.result();
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
        Shared<SMesh> mesh = std::static_pointer_cast<SMesh>(getUsedResource(aMesh.readableName));

        SMeshDependencies dependencies {};
        EEngineStatus const status = mesh->initialize(dependencies).result();

        Shared<SBuffer> attributeBuffer = mesh->vertexDataBufferResource;
        Shared<SBuffer> indexBuffer     = mesh->indexBufferResource;
        attributeBuffer->initialize({});
        indexBuffer    ->initialize({});

        mGraphicsAPIRenderContext->transferBufferData(attributeBuffer->getDescription().dataSource(), attributeBuffer->getGpuApiResourceHandle());
        mGraphicsAPIRenderContext->transferBufferData(indexBuffer    ->getDescription().dataSource(), indexBuffer    ->getGpuApiResourceHandle());

        return mGraphicsAPIRenderContext->bindAttributeAndIndexBuffers(attributeBuffer->getGpuApiResourceHandle(), indexBuffer->getGpuApiResourceHandle(), mesh->getDescription().offsets);
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
    CEngineResult<> CFrameGraphRenderContext::loadMaterialAsset(SFrameGraphMaterial const &aMaterial)
    {
        CEngineResult<Shared<ILogicalResourceObject>> materialObject = mResourceManager->useAssetResource(aMaterial.readableName, aMaterial.materialAssetId);
        if(CheckEngineError(materialObject.result()))
        {
            CLog::Error(logTag(), "Cannot use material asset {} with id {}", aMaterial.readableName, aMaterial.materialAssetId);
            return materialObject.result();
        }

        Shared<SMaterial> material = std::static_pointer_cast<SMaterial>(materialObject.data());
        registerUsedResource(aMaterial.readableName, material);

        return materialObject.result();
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CEngineResult<> CFrameGraphRenderContext::bindMaterial(SFrameGraphMaterial const &aMaterial
                                                           , std::string       const &aRenderPassHandle)
    {
        Shared<SMaterial> material = std::static_pointer_cast<SMaterial>(getUsedResource(aMaterial.readableName));

        SMaterialDependencies dependencies {};
        dependencies.pipelineDependencies.systemUBOPipelineId   = "Core_pipeline";
        dependencies.pipelineDependencies.referenceRenderPassId = aRenderPassHandle;
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
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CEngineResult<> CFrameGraphRenderContext::unbindMaterial(SFrameGraphMaterial const &aMaterial)
    {
        Shared<SMaterial> material = std::static_pointer_cast<SMaterial>(getUsedResource(aMaterial.readableName));

        for(auto const &sampledImageAssetId : material->getDescription().sampledImages)
        {
            std::string const sampledImageResourceId = fmt::format("{}", sampledImageAssetId);

            Shared<STexture> sampledImageTexture = std::static_pointer_cast<STexture>(getUsedResource(sampledImageResourceId));
            if(nullptr != sampledImageTexture)
            {
                std::string    const viewId = fmt::format("{}_{}_view", material->getDescription().name, sampledImageTexture->getDescription().name);
                Shared<STextureView> view   = std::static_pointer_cast<STextureView>(getUsedResource(viewId));

                view->unload();
                view->deinitialize(*(view->getCurrentDependencies()));
            }
        }

        auto const result = mGraphicsAPIRenderContext->unbindPipeline(material->pipelineResource->getGpuApiResourceHandle());
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
        loadMaterialAsset(aMaterial);
        bindMaterial(aMaterial, mCurrentRenderPassHandle);

        if(EFrameGraphResourceType::Undefined != aMesh.type)
        {
            readMeshAsset(aMesh);
            bindMesh     (aMesh);
            Shared<SMesh> mesh = std::static_pointer_cast<SMesh>(getUsedResource(aMesh.readableName));
            mGraphicsAPIRenderContext->drawIndex(mesh->getDescription().indexSampleCount);
            unbindMesh     (aMesh);
            unloadMeshAsset(aMesh);
        }

        unbindMaterial     (aMaterial);
        unloadMaterialAsset(aMaterial);

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------

    CEngineResult<> CFrameGraphRenderContext::drawFullscreenQuadWithMaterial(SFrameGraphMaterial const &aMaterial)
    {
        loadMaterialAsset(aMaterial);
        bindMaterial(aMaterial, mCurrentRenderPassHandle);

        mGraphicsAPIRenderContext->drawQuad();

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
