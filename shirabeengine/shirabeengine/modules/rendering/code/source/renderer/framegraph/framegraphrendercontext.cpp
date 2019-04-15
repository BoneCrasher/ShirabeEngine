#include <assert.h>

#include <graphicsapi/resources/types/renderpass.h>
#include <graphicsapi/resources/types/framebuffer.h>
#include <graphicsapi/resources/types/pipeline.h>

#include <material/material_loader.h>
#include <material/material_declaration.h>

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

            CStdSharedPtr_t<CFrameGraphRenderContext> context = CStdSharedPtr_t<CFrameGraphRenderContext>(new CFrameGraphRenderContext(aAssetStorage, aMaterialLoader, aResourceManager, aRenderer));
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
            : mAssetStorage(aAssetStorage)
            , mMaterialLoader(aMaterialLoader)
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
        CEngineResult<> CFrameGraphRenderContext::beginPass()
        {
            CEngineResult<> const status = mGraphicsAPIRenderContext->beginSubpass();

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::endPass()
        {
            CEngineResult<> const status = mGraphicsAPIRenderContext->endSubpass();

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
                    attachmentDesc.loadOp         = EAttachmentLoadOp::DONT_CARE;
                    attachmentDesc.stencilLoadOp  = attachmentDesc.loadOp;
                    attachmentDesc.storeOp        = EAttachmentStoreOp::DONT_CARE;
                    attachmentDesc.stencilStoreOp = attachmentDesc.storeOp;
                    attachmentDesc.format         = textureView.format;
                    attachmentDesc.initialLayout  = EImageLayout::UNDEFINED;
                    attachmentDesc.finalLayout    = EImageLayout::TRANSFER_SRC_OPTIMAL; // For now we just assume everything to be presentable...

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
            return mGraphicsAPIRenderContext->bindFrameBufferAndRenderPass(aFrameBufferId, aRenderPassId);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::unbindFrameBufferAndRenderPass(std::string const &aFrameBufferId,
                                                                                 std::string const &aRenderPassId)
        {
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
        CEngineResult<> CFrameGraphRenderContext::createBuffer(
                FrameGraphResourceId_t const &aResourceId,
                SFrameGraphResource    const &aResource,
                SFrameGraphBuffer      const &aBuffer)
        {
            SHIRABE_UNUSED(aResourceId);
            SHIRABE_UNUSED(aResource);
            SHIRABE_UNUSED(aBuffer);

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::destroyBuffer(FrameGraphResourceId_t const &aResourceId)
        {
            SHIRABE_UNUSED(aResourceId);
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
            SHIRABE_UNUSED(aResourceId);
            SHIRABE_UNUSED(aResource);
            SHIRABE_UNUSED(aBufferView);

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::bindBufferView(FrameGraphResourceId_t  const &aResourceId)
        {
            SHIRABE_UNUSED(aResourceId);
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::unbindBufferView(FrameGraphResourceId_t const &aResourceId)
        {
            SHIRABE_UNUSED(aResourceId);
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::destroyBufferView(FrameGraphResourceId_t const &aResourceId)
        {
            SHIRABE_UNUSED(aResourceId);
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
        CEngineResult<> CFrameGraphRenderContext::loadMaterialAsset(SFrameGraphMaterial const &aMaterial)
        {
            SMaterial material {};
            material.inputAssemblyState.topology = EMaterialPrimitiveTopology::TriangleList;

            auto const &[result, instance] = mMaterialLoader->loadMaterialInstance(aMaterial.materialAssetId);
            if(CheckEngineError(result))
            {
                return { result };
            }

            CStdSharedPtr_t<CMaterialMaster> const &master    = instance->master();
            SMaterialSignature               const &signature = master  ->signature();
            CMaterialConfig                  const &config    = instance->config();

            for(auto const [stageKey, stage] : signature.stages)
            {
                if(EShaderStage::Vertex == stageKey)
                {
                    for(uint32_t k=0; k<stage.inputs.size(); ++k)
                    {
                        SStageInput const &input = stage.inputs.at(k);

                        SMaterialVertexInputBinding binding {};
                        binding.binding   = k;
                        binding.stride    = input.type.byteSize;
                        binding.inputRate = EMaterialVertexInputRate::Vertex;

                        SMaterialVertexAttributeDescription attribute {};
                        attribute.binding    = k;
                        attribute.location   = input.location;
                        attribute.byteOffset = 0;
                        attribute.format     = (2 == binding.stride)
                                                    ? EFormat::R32G32_SFLOAT
                                                    : (3 == binding.stride)
                                                           ? EFormat::R32G32B32_SFLOAT
                                                           : (4 == binding.stride)
                                                                  ? EFormat::R32G32B32A32_FLOAT
                                                                  : EFormat::Undefined;

                        material.vertexInputState.bufferBindings   .push_back(binding);
                        material.vertexInputState.attributeBindings.push_back(attribute);
                    }
                }

                if(EShaderStage::Fragment == stageKey)
                {
                    for(SStageOutput const &output : stage.outputs)
                    {
                        material...
                    }
                }
            }

            Vector<SMaterialSet> sets;

            for(SUniformBuffer const &uniformBuffer : signature.uniformBuffers)
            {
                // Fill up...
                while(sets.size() < uniformBuffer.set)
                {
                    sets.push_back({});
                }

                SMaterialSet             &set      = sets[uniformBuffer.set];
                Vector<SMaterialBinding> &bindings = set.bindings;

                while(bindings.size() < uniformBuffer.binding)
                {
                    bindings.push_back({});
                }

                SMaterialBinding &binding = bindings[uniformBuffer.binding];

                // bindings[uniformBuffer.binding].descriptorType  = ;
                // bindings[uniformBuffer.binding].descriptorCount = ;
                // bindings[uniformBuffer.binding].stageFlags      = ;
            }

            for(SSampledImage const &sampledImage : signature.sampledImages)
            {

            }

            CPipelineDeclaration::SDescriptor pipelineDescriptor {};
            pipelineDescriptor.name     = "";
            pipelineDescriptor.material = material;

            CPipeline::CCreationRequest request(pipelineDescriptor, {}, {});

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::unloadMaterialAsset(SFrameGraphMaterial const &aMaterial)
        {
            SHIRABE_UNUSED(aMaterial);
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::bindMaterial(SFrameGraphMaterial const &aMaterial)
        {
            SHIRABE_UNUSED(aMaterial);
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::unbindMaterial(SFrameGraphMaterial const &aMaterial)
        {
            SHIRABE_UNUSED(aMaterial);
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CFrameGraphRenderContext::render(SFrameGraphMesh     const &aMesh,
                                                         SFrameGraphMaterial const &aMaterial)
        {

            //
            // Traverse signature and derive pipeline configuration UNLESS a pipeline was
            // created already for the given material instance.
            //


            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

    }
}
