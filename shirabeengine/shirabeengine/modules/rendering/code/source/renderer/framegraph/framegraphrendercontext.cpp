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
