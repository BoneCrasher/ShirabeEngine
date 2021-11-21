#include <base/string.h>
#include "renderer/rendergraph/renderpass.h"
#include "renderer/rendergraph/subpass.h"
#include "renderer/rendergraph/graphbuilder.h"
#include "renderer/rendergraph/passbuilder.h"


namespace engine
{
    namespace framegraph
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CPassBuilder::CPassBuilder(
            PassUID_t const              &aPassUID,
                Shared<CPassBase>             aPass,
                Shared<CRenderPass>           aEnclosingRenderPass,
                CRHIResourceManager const       &aResourceManager,
                CRenderGraphMutableResources &aOutResourceData)
            : mPassUID             (aPassUID                           )
            , mPass                (std::move(aPass                   ))
            , mEnclosingRenderPass (std::move(aEnclosingRenderPass    ))
            , mResourceData        (aOutResourceData                   )
            , mRHIResourceManager(aResourceManager)
            , mAttachmentCollection(aEnclosingRenderPass->attachments())
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SRenderGraphRenderTarget> CPassBuilder::createRenderTarget(
            std::string                         const &aName,
            SRenderGraphDynamicImageDescription const &aDescriptor)
        {
            SRenderGraphImageDescription description;
            description.isDynamicImage = true;
            description.dynamicImage   = aDescriptor;

            // Basic abstract descriptor of resources being used.
            SRenderGraphRenderTarget &resource = mResourceData.spawnResource<SRenderGraphRenderTarget>();
            resource.description           = description;
            resource.assignedRenderpassUID = mEnclosingRenderPass->getRenderPassUid();
            resource.assignedPassUID       = mPassUID;
            resource.parentResource        = SHIRABE_FRAMEGRAPH_UNDEFINED_RESOURCE;
            resource.subjacentResource     = resource.resourceId; // This is a trick to keep algorithms consistent while supporting r/w from textures and t-views.
            resource.readableName          = aName;
            resource.type                  = ERenderGraphResourceType::Image;

            auto const &[result] = mPass->registerResource<SRenderGraphRenderTarget>(resource.resourceId);
            if(CheckEngineError(result))
            {
                return { result };
            }

            return { EEngineStatus::Ok, resource };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SRenderGraphImage> CPassBuilder::createImage(
            std::string                         const &aName,
            SRenderGraphDynamicImageDescription const &aDescriptor)
        {
            SRenderGraphImageDescription description;
            description.isDynamicImage = true;
            description.dynamicImage   = aDescriptor;

            // Basic abstract descriptor of resources being used.
            SRenderGraphImage &resource = mResourceData.spawnResource<SRenderGraphImage>();
            resource.description           = description;
            resource.assignedRenderpassUID = mEnclosingRenderPass->getRenderPassUid();
            resource.assignedPassUID       = mPassUID;
            resource.parentResource        = SHIRABE_FRAMEGRAPH_UNDEFINED_RESOURCE;
            resource.subjacentResource     = resource.resourceId; // This is a trick to keep algorithms consistent while supporting r/w from textures and t-views.
            resource.readableName          = aName;
            resource.type                  = ERenderGraphResourceType::Image;

            auto const &[result] = mPass->registerResource<SRenderGraphImage>(resource.resourceId);
            if(CheckEngineError(result))
            {
                return { result };
            }

            return { EEngineStatus::Ok, resource };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SRenderGraphImage> CPassBuilder::importImage(
            std::string                            const &aName,
            SRenderGraphPersistentImageDescription const &aDescriptor)
        {
            SRenderGraphImageDescription description;
            description.isDynamicImage  = false;
            description.persistentImage = aDescriptor;

            auto const &[result, resourceDesc] =
                mRHIResourceManager.getResourceDescription<RHIImageResourceState_t>(description.persistentImage.imageId);
            if(CheckEngineError(result))
            {
                return EEngineStatus::ResourceError_NotFound;
            }

            description.dynamicImage = SRenderGraphDynamicImageDescription(resourceDesc.imageInfo);
            description.dynamicImage.requestedUsage.set(ERenderGraphResourceUsage::SampledImage);
            description.dynamicImage.permittedUsage.set(ERenderGraphResourceUsage::SampledImage);

            // Basic abstract descriptor of resources being used.
            SRenderGraphImage &resource = mResourceData.spawnResource<SRenderGraphImage>();
            resource.description           = description;
            resource.assignedRenderpassUID = mEnclosingRenderPass->getRenderPassUid();
            resource.assignedPassUID       = mPassUID;
            resource.parentResource        = SHIRABE_FRAMEGRAPH_UNDEFINED_RESOURCE;
            resource.subjacentResource     = resource.resourceId; // This is a trick to keep algorithms consistent while supporting r/w from textures and t-views.
            resource.readableName          = aName;
            resource.type                  = ERenderGraphResourceType::Image;

            auto const &[registrationResult] = mPass->registerResource<SRenderGraphImage>(resource.resourceId);
            if(CheckEngineError(registrationResult))
            {
                return { registrationResult };
            }

            return { EEngineStatus::Ok, resource };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SRenderGraphImageView> CPassBuilder::useImage(
            SRenderGraphImage                &aImage,
            ERenderGraphViewPurpose    const &aSourceOrTarget,
            EFormat                    const &aRequiredFormat,
            CRange                     const &aArraySliceRange,
            CRange                     const &aMipSliceRange,
            ERenderGraphViewAccessMode const &aMode,
            EEngineStatus              const &aFailCode)
        {
            RenderGraphResourceId_t const subjacentResourceId = aImage.resourceId;

            ERenderGraphViewAccessMode const mode   = aMode;
            ERenderGraphViewPurpose    const source = aSourceOrTarget;

            CRange subjacentArraySliceRange = CRange(0, aImage.description.dynamicImage.arraySize);
            CRange subjacentMipSliceRange   = CRange(0, aImage.description.dynamicImage.mipLevels);
            CRange sourceArraySliceRange    = subjacentArraySliceRange;
            CRange sourceMipSliceRange      = subjacentMipSliceRange;
            CRange adjustedArraySliceRange  = aArraySliceRange;
            CRange adjustedMipSliceRange    = aMipSliceRange;

            auto const [adjustmentResult]
                = adjustArrayAndMipSliceRanges(
                        mResourceData,
                        subjacentArraySliceRange,
                        subjacentMipSliceRange,
                        sourceArraySliceRange,
                        sourceMipSliceRange,
                        aArraySliceRange,
                        aMipSliceRange,
                        adjustedArraySliceRange,
                        adjustedMipSliceRange);
            EngineStatusPrintOnError(adjustmentResult, logTag(), "Couldn't adjust array and mipslice ranges properly.");

            auto const [validationResult]
                = validateArrayAndMipSliceRanges(
                        mResourceData,
                        aImage,
                        adjustedArraySliceRange,
                        adjustedMipSliceRange,
                        true,
                        true);
            EngineStatusPrintOnError(adjustmentResult, logTag(), "Couldn't validate array and mipslice ranges properly.");

            Optional_t<RefWrapper_t<SRenderGraphImageView>> resource{};

            SRenderGraphImageView &view = mResourceData.spawnResource<SRenderGraphImageView>();
            view.description.arraySliceRange = adjustedArraySliceRange;
            view.description.mipSliceRange   = adjustedMipSliceRange;
            view.description.format          = aRequiredFormat;
            view.description.source          = source;
            view.assignedRenderpassUID       = mEnclosingRenderPass->getRenderPassUid();
            view.assignedPassUID             = mPassUID;
            view.parentResource              = aImage.resourceId;
            view.subjacentResource           = subjacentResourceId;
            view.type                        = ERenderGraphResourceType::ImageView;
            view.readableName                = StaticStringHelpers::format("ImageView ID {} - {} #{}"
                                                               , view.resourceId
                                                               , ( mode == ERenderGraphViewAccessMode::Write ? "Write" : "Read" )
                                                               , aImage.resourceId);
            view.description.mode.set(mode);

            switch(view.description.source)
            {
            case ERenderGraphViewPurpose::InputAttachment:
                aImage.description.dynamicImage.requestedUsage.set(ERenderGraphResourceUsage::InputAttachment);
                break;
            case ERenderGraphViewPurpose::ColorAttachment:
                aImage.description.dynamicImage.requestedUsage.set(ERenderGraphResourceUsage::ColorAttachment);
                break;
            case ERenderGraphViewPurpose::DepthAttachment:
                aImage.description.dynamicImage.requestedUsage.set(ERenderGraphResourceUsage::DepthAttachment);
                break;
            case ERenderGraphViewPurpose::ShaderInput:
                aImage.description.dynamicImage.requestedUsage.set(ERenderGraphResourceUsage::SampledImage);
                break;
            default:
                break;
            }

            ++(aImage.referenceCount);

            auto const [registrationResult] = mPass->registerResource<SRenderGraphImageView>(view.resourceId);
            if(CheckEngineError(registrationResult))
            {
                CLog::Error(logTag(), "Failed to register resource w/ id %d in pass.", view.resourceId);
                return { EEngineStatus::Error };
            }

            ++(view.referenceCount);

            return { EEngineStatus::Ok, *resource };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SRenderGraphImageView> CPassBuilder::useImageView(
            SRenderGraphImageView            &aTextureView,
            ERenderGraphViewPurpose    const &aSourceOrTarget,
            EFormat                    const &aRequiredFormat,
            CRange                     const &aArraySliceRange,
            CRange                     const &aMipSliceRange,
            ERenderGraphViewAccessMode const &aMode,
            EEngineStatus              const &aFailCode)
        {
            ERenderGraphViewAccessMode const mode   = aMode;
            ERenderGraphViewPurpose    const source = aSourceOrTarget;

            CRange sourceArraySliceRange    = aTextureView.description.arraySliceRange;
            CRange sourceMipSliceRange      = aTextureView.description.mipSliceRange;
            CRange adjustedArraySliceRange  = aArraySliceRange;
            CRange adjustedMipSliceRange    = aMipSliceRange;

            RenderGraphResourceId_t const subjacentResourceId = aTextureView.subjacentResource;
            auto const &[result, sharedImage] = mResourceData.getResourceMutable<SRenderGraphImage>(subjacentResourceId);
            if(CheckEngineError(result))
            {
                return { result };
            }

            CRange subjacentArraySliceRange = CRange(0, sharedImage->description.dynamicImage.arraySize);
            CRange subjacentMipSliceRange   = CRange(0, sharedImage->description.dynamicImage.mipLevels);

            auto const [adjustmentResult]
                = adjustArrayAndMipSliceRanges(
                    mResourceData,
                    subjacentArraySliceRange,
                    subjacentMipSliceRange,
                    sourceArraySliceRange,
                    sourceMipSliceRange,
                    aArraySliceRange,
                    aMipSliceRange,
                    adjustedArraySliceRange,
                    adjustedMipSliceRange);
            EngineStatusPrintOnError(adjustmentResult, logTag(), "Couldn't adjust array and mipslice ranges properly.");

            auto const [validationResult]
                = validateArrayAndMipSliceRanges(
                    mResourceData,
                    aTextureView,
                    adjustedArraySliceRange,
                    adjustedMipSliceRange,
                    true,
                    true);
            EngineStatusPrintOnError(adjustmentResult, logTag(), "Couldn't validate array and mipslice ranges properly.");

            Optional_t<RefWrapper_t<SRenderGraphImageView>> resource{};

            // Can we cull?
            bool duplicateFound = false;
            CEngineResult<RenderGraphResourceId_t> const duplicateViewIdQuery
                = findDuplicateImageView(subjacentResourceId
                                         , aRequiredFormat
                                         , source
                                         , adjustedArraySliceRange
                                         , adjustedMipSliceRange
                                         , mode);

            if(not duplicateViewIdQuery.successful() && not (EEngineStatus::RenderGraph_PassBuilder_DuplicateTextureViewId == duplicateViewIdQuery.result()))
            {
                return { duplicateViewIdQuery.result() };
            }

            duplicateFound = duplicateViewIdQuery.resultEquals(EEngineStatus::RenderGraph_PassBuilder_DuplicateTextureViewId);
            if(duplicateFound)
            {
                RenderGraphResourceId_t const &duplicateViewId = duplicateViewIdQuery.data();

                CEngineResult<Shared<SRenderGraphImageView>> const &viewFetch = mResourceData.getResourceMutable<SRenderGraphImageView>(duplicateViewId);
                if(not viewFetch.successful())
                {
                    return { viewFetch.result() };
                }
                resource = *(viewFetch.data());
            }

            if(not duplicateFound)
            {
                auto const &[result, texture] = mResourceData.getResourceMutable<SRenderGraphImage>(subjacentResourceId);
                if(CheckEngineError(result))
                {
                    return { result };
                }

                SRenderGraphImageView &view = mResourceData.spawnResource<SRenderGraphImageView>();
                view.description.arraySliceRange = adjustedArraySliceRange;
                view.description.mipSliceRange   = adjustedMipSliceRange;
                view.description.format          = aRequiredFormat;
                view.description.source          = source;
                view.assignedRenderpassUID       = mEnclosingRenderPass->getRenderPassUid();
                view.assignedPassUID             = mPassUID;
                view.parentResource              = aTextureView.resourceId;
                view.subjacentResource           = subjacentResourceId;
                view.type                        = ERenderGraphResourceType::ImageView;
                view.readableName                = StaticStringHelpers::format("ImageView ID {} - {} #{}"
                                                                   , view.resourceId
                                                                   , ( mode == ERenderGraphViewAccessMode::Write ? "Write" : "Read" )
                                                                   , aTextureView.resourceId);
                view.description.mode.set(mode);

                switch(view.description.source)
                {
                    case ERenderGraphViewPurpose::InputAttachment:
                        texture->description.dynamicImage.requestedUsage.set(ERenderGraphResourceUsage::InputAttachment);
                        break;
                    case ERenderGraphViewPurpose::ColorAttachment:
                        texture->description.dynamicImage.requestedUsage.set(ERenderGraphResourceUsage::ColorAttachment);
                        break;
                    case ERenderGraphViewPurpose::DepthAttachment:
                        texture->description.dynamicImage.requestedUsage.set(ERenderGraphResourceUsage::DepthAttachment);
                        break;
                    case ERenderGraphViewPurpose::ShaderInput:
                        texture->description.dynamicImage.requestedUsage.set(ERenderGraphResourceUsage::SampledImage);
                        break;
                    default:
                        break;
                }

                ++(texture->referenceCount);

                resource = view;
            }

            if(resource.has_value())
            {
                SRenderGraphResource &ref = resource->get();

                auto const [registrationResult] = mPass->registerResource<SRenderGraphImageView>(ref.resourceId);
                if(CheckEngineError(registrationResult))
                {
                    CLog::Error(logTag(), "Failed to register resource reference w/ id %d in pass.", ref.resourceId);
                    return { EEngineStatus::Error };
                }
                ++(ref.referenceCount);

                return { EEngineStatus::Ok, *resource };
            }

            return { aFailCode, *resource };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SRenderGraphImageView> CPassBuilder::writeAttachment(
            SRenderGraphImage                   &aImage,
            SRenderGraphWriteTextureFlags const &aFlags)
        {
            bool const isColorAttachment = (ERenderGraphWriteTarget::Color == aFlags.writeTarget);
            bool const isDepthAttachment = (ERenderGraphWriteTarget::Depth == aFlags.writeTarget);

            ERenderGraphViewPurpose source = isColorAttachment
                                            ? ERenderGraphViewPurpose::ColorAttachment
                                            : isDepthAttachment
                                              ? ERenderGraphViewPurpose::DepthAttachment
                                              : ERenderGraphViewPurpose::Undefined;

            auto const &[result, data] = useImage(aImage
                                                  , source
                                                  , aFlags.requiredFormat
                                                  , aFlags.arraySliceRange
                                                  , aFlags.mipSliceRange
                                                  , ERenderGraphViewAccessMode::Write
                                                  , EEngineStatus::RenderGraph_PassBuilder_WriteResourceFailed);

            if(not CheckEngineError(result))
            {
                RenderGraphResourceId_t const imageId     = data.subjacentResource;
                RenderGraphResourceId_t const imageViewId = data.resourceId;

                if(isColorAttachment)
                {
                    mAttachmentCollection.addColorAttachment(mPassUID, imageId, imageViewId);
                }
                else if(isDepthAttachment)
                {
                    mAttachmentCollection.addDepthAttachment(mPassUID, imageId, imageViewId);
                }
            }

            return { result, data };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SRenderGraphImageView> CPassBuilder::writeAttachment(
            SRenderGraphImageView               &aImageView,
            SRenderGraphWriteTextureFlags const &aFlags)
        {
            bool const isColorAttachment = (ERenderGraphWriteTarget::Color == aFlags.writeTarget);
            bool const isDepthAttachment = (ERenderGraphWriteTarget::Depth == aFlags.writeTarget);

            ERenderGraphViewPurpose source = isColorAttachment
                                                ? ERenderGraphViewPurpose::ColorAttachment
                                                : isDepthAttachment
                                                      ? ERenderGraphViewPurpose::DepthAttachment
                                                      : ERenderGraphViewPurpose::Undefined;

            auto const &[result, data] = useImageView(aImageView
                                                      , source
                                                      , aFlags.requiredFormat
                                                      , aFlags.arraySliceRange
                                                      , aFlags.mipSliceRange
                                                      , ERenderGraphViewAccessMode::Write
                                                      , EEngineStatus::RenderGraph_PassBuilder_WriteResourceFailed);

            if(not CheckEngineError(result))
            {
                RenderGraphResourceId_t const imageId     = data.subjacentResource;
                RenderGraphResourceId_t const imageViewId = data.resourceId;

                if(isColorAttachment)
                {
                    mAttachmentCollection.addColorAttachment(mPassUID, imageId, imageViewId);
                }
                else if(isDepthAttachment)
                {
                    mAttachmentCollection.addDepthAttachment(mPassUID, imageId, imageViewId);
                }
            }

            return { result, data };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SRenderGraphImageView> CPassBuilder::readAttachment(
            SRenderGraphImage                  &aImage,
            SRenderGraphReadTextureFlags const &aFlags)
        {                        
            ERenderGraphViewPurpose source = ERenderGraphViewPurpose::InputAttachment;

            auto const &[result, data]  = useImage(aImage
                                                   , source
                                                   , aFlags.requiredFormat
                                                   , aFlags.arraySliceRange
                                                   , aFlags.mipSliceRange
                                                   , ERenderGraphViewAccessMode::Read
                                                   , EEngineStatus::RenderGraph_PassBuilder_ReadResourceFailed);

            if(not CheckEngineError(result))
            {
                RenderGraphResourceId_t const imageId     = data.subjacentResource;
                RenderGraphResourceId_t const imageViewId = data.resourceId;

                mAttachmentCollection.addInputAttachment(mPassUID, imageId, imageViewId);
            }

            return { result, data };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SRenderGraphImageView> CPassBuilder::readAttachment(
            SRenderGraphImageView              &aImageView,
            SRenderGraphReadTextureFlags const &aFlags)
        {
            ERenderGraphViewPurpose source = ERenderGraphViewPurpose::InputAttachment;

            auto const &[result, data] = useImageView(aImageView
                                                      , source
                                                      , aFlags.requiredFormat
                                                      , aFlags.arraySliceRange
                                                      , aFlags.mipSliceRange
                                                      , ERenderGraphViewAccessMode::Read
                                                      , EEngineStatus::RenderGraph_PassBuilder_ReadResourceFailed);

            if(not CheckEngineError(result))
            {
                RenderGraphResourceId_t const imageId     = data.subjacentResource;
                RenderGraphResourceId_t const imageViewId = data.resourceId;

                mAttachmentCollection.addInputAttachment(mPassUID, imageId, imageViewId);
            }

            return { result, data };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SRenderGraphImageView> CPassBuilder::readImage(
            SRenderGraphImage                      &subjacentTargetResource,
            SRenderGraphTextureResourceFlags const &aFlags)
        {
            ERenderGraphViewPurpose source = ERenderGraphViewPurpose::ShaderInput;

            return useImage(subjacentTargetResource
                            , source
                            , aFlags.requiredFormat
                            , aFlags.arraySliceRange
                            , aFlags.mipSliceRange
                            , ERenderGraphViewAccessMode::Read
                            , EEngineStatus::RenderGraph_PassBuilder_ReadResourceFailed);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<SRenderGraphBuffer> CPassBuilder::createBuffer(std::string const &aName, SRenderGraphDynamicBufferDescription const &aBufferDescription)
        {
            SRenderGraphBuffer buffer = mResourceData.spawnResource<SRenderGraphBuffer>();
            buffer.readableName                 = aName;
            buffer.description.isDynamicBuffer  = true;
            buffer.description.dynamicBuffer    = aBufferDescription;

            auto const [registrationResult] = mPass->registerResource<SRenderGraphBuffer>(buffer.resourceId);
            if(CheckEngineError(registrationResult))
            {
                CLog::Error(logTag(), "Failed to register resource w/ id %d in pass.", buffer.resourceId);
                return { EEngineStatus::Error };
            }

            return { EEngineStatus::Ok, buffer };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<SRenderGraphBuffer> CPassBuilder::importBuffer(std::string const &aName, SRenderGraphPersistentBufferDescription const &aBufferDescription)
        {
            SRenderGraphBuffer buffer = mResourceData.spawnResource<SRenderGraphBuffer>();
            buffer.readableName                 = aName;
            buffer.description.isDynamicBuffer  = false;
            buffer.description.persistentBuffer = aBufferDescription;

            auto const &[result, resourceDesc] =
                mRHIResourceManager.getResourceDescription<RHIBufferResourceState_t>(aBufferDescription.bufferResourceId);

            if(CheckEngineError(result))
            {
                return EEngineStatus::ResourceError_NotFound;
            }

            buffer.description.dynamicBuffer.bufferUsage = resourceDesc.createInfo.usage;
            buffer.description.dynamicBuffer.sizeInBytes = resourceDesc.createInfo.size;

            auto const [registrationResult] = mPass->registerResource<SRenderGraphBuffer>(buffer.resourceId);
            if(CheckEngineError(registrationResult))
            {
                CLog::Error(logTag(), "Failed to register resource w/ id %d in pass.", buffer.resourceId);
                return { EEngineStatus::Error };
            }

            return { EEngineStatus::Ok, buffer };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<SRenderGraphBufferView> CPassBuilder::readBuffer(SRenderGraphBuffer &subjacentTargetResource, CRange const &aSubrange)
        {
            SRenderGraphBufferView bufferView = mResourceData.spawnResource<SRenderGraphBufferView>();
            bufferView.readableName = StaticStringHelpers::format("Buffer_%d_View_%d", subjacentTargetResource.resourceId, bufferView.resourceId);
            bufferView.description.mode     = ERenderGraphViewAccessMode::Read;
            bufferView.description.subrange = aSubrange;

            auto const [registrationResult] = mPass->registerResource<SRenderGraphBufferView>(bufferView.resourceId);
            if(CheckEngineError(registrationResult))
            {
                CLog::Error(logTag(), "Failed to register resource w/ id %d in pass.", bufferView.resourceId);
                return { EEngineStatus::Error };
            }

            ++(bufferView.referenceCount);

            return { EEngineStatus::Ok, bufferView };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SRenderGraphMesh> CPassBuilder::useMesh(SRenderGraphMeshDescription const &aMeshDescription)
        {
            SRenderGraphMesh &meshResource = mResourceData.spawnResource<SRenderGraphMesh>();
            meshResource.readableName          = aMeshDescription.meshResourceId;
            meshResource.type                  = ERenderGraphResourceType::Mesh;
            meshResource.assignedRenderpassUID = mEnclosingRenderPass->getRenderPassUid();
            meshResource.assignedPassUID       = mPassUID;
            meshResource.isExternalResource    = true;
            meshResource.parentResource        = 0;
            meshResource.referenceCount        = 0;
            meshResource.subjacentResource     = 0;
            meshResource.description           = aMeshDescription;

            auto const [registrationResult] = mPass->registerResource<SRenderGraphMesh>(meshResource.resourceId);
            if(CheckEngineError(registrationResult))
            {
                CLog::Error(logTag(), "Failed to register resource w/ id %d in pass.", meshResource.resourceId);
                return { EEngineStatus::Error };
            }

            return { EEngineStatus::Ok, meshResource };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<SRenderGraphMaterial> CPassBuilder::useMaterial(std::string const &aMaterialId, SRenderGraphPipelineConfig const &aPipelineConfigOverride)
        {
            // TODO: Fetch material from material system and create specialization
            SRenderGraphMaterialDescription renderGraphMaterialDescription;
            renderGraphMaterialDescription.materialResourceId       = aResourceId;
            renderGraphMaterialDescription.sharedMaterialResourceId = materialResourceDescription.systemMaterialId;

            for(auto const &bufferDesc : materialResourceDescription.uniformBufferDescriptors)
            {
                SRenderGraphPersistentBufferDescription renderGraphBufferDesc;
                    renderGraphBufferDesc.bufferResourceId = bufferDesc.name;

                          renderGraphMaterialDescription.buffers.push_back(renderGraphBufferDesc);
                      }
                    for(auto const &imageDesc : materialResourceDescription.sampledImages){
                          SRenderGraphPersistentImageDescription renderGraphImageDesc;
                          renderGraphImageDesc.imageId = imageDesc.name;

                          renderGraphMaterialDescription.images.push_back(renderGraphImageDesc);
                      }

            SRenderGraphMaterial &materialResource = mResourceData.spawnResource<SRenderGraphMaterial>();
            materialResource.readableName          = aMaterialDescription.materialResourceId;
            materialResource.type                  = ERenderGraphResourceType::Material;
            materialResource.assignedRenderpassUID = mEnclosingRenderPass->getRenderPassUid();
            materialResource.assignedPassUID       = mPassUID;
            materialResource.isExternalResource    = true;
            materialResource.parentResource        = 0;
            materialResource.referenceCount        = 0;
            materialResource.subjacentResource     = 0;
            materialResource.description           = aMaterialDescription;

            for(auto const &bufferDesc : aMaterialDescription.buffers)
            {
                auto [bufferCreationResult, buffer] = importBuffer(bufferDesc.bufferResourceId, bufferDesc);

                auto const [result, bufferResource] = readBuffer(buffer, { 0, static_cast<int32_t>(buffer.description.dynamicBuffer.sizeInBytes) });
                materialResource.buffers.push_back(bufferResource);
            }

            for(auto const &imageDesc : aMaterialDescription.images)
            {
                auto [imageCreationResult, image] = importImage(imageDesc.imageId, imageDesc);

                SRenderGraphTextureResourceFlags flags;
                flags.arraySliceRange = CRange(0, 1);
                flags.mipSliceRange   = CRange(0, 1);
                flags.requiredFormat  = EFormat::Automatic;

                auto const [result, textureResource] = readImage(image, flags);
                materialResource.images.push_back(textureResource);
            }

            auto const [registrationResult] = mPass->registerResource<SRenderGraphMaterial>(materialResource.resourceId);
            if(CheckEngineError(registrationResult))
            {
                CLog::Error(logTag(), "Failed to register resource w/ id %d in pass.", materialResource.resourceId);
                return { EEngineStatus::Error };
            }

            return { EEngineStatus::Ok, materialResource };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SRenderGraphPipeline> CPassBuilder::usePipeline(rhi::ResourceId_t      const &aPipelineResourceId
                                                                      , SRenderGraphPipelineConfig const &aPipelineConfig)
        {
            SRenderGraphPipeline &resource = mResourceData.spawnResource<SRenderGraphPipeline>();
            resource.readableName             = aPipelineResourceId;
            resource.type                     = ERenderGraphResourceType::Pipeline;
            resource.assignedRenderpassUID    = mEnclosingRenderPass->getRenderPassUid();
            resource.assignedPassUID          = mPassUID;
            resource.isExternalResource       = false;
            resource.parentResource           = 0;
            resource.referenceCount           = 0;
            resource.subjacentResource        = 0;
            resource.pipelineResourceId       = aPipelineResourceId;
            resource.pipelineConfiguration    = aPipelineConfig;

            auto const [registrationResult] = mPass->registerResource<SRenderGraphPipeline>(resource.resourceId);
            if(CheckEngineError(registrationResult))
            {
                CLog::Error(logTag(), "Failed to register resource w/ id %d in pass.", resource.resourceId);
                return { EEngineStatus::Error };
            }

            return { EEngineStatus::Ok, resource };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<RenderGraphResourceId_t> CPassBuilder::findDuplicateImageView(
            RenderGraphResourceId_t               const &aSubjacentResourceId,
            RenderGraphFormat_t                   const &aFormat,
            ERenderGraphViewPurpose               const &aViewSource,
            CRange                                const &aArrayRange,
            CRange                                const &aMipRange,
            CBitField<ERenderGraphViewAccessMode> const &aMode)
        {
            for(RenderGraphResourceId_t const &viewId : mResourceData.imageViews())
            {
                CEngineResult<Shared<SRenderGraphImageView>> viewFetch = mResourceData.getResource<SRenderGraphImageView>(viewId);
                if(not viewFetch.successful())
                {
                    continue;
                }

                SRenderGraphImageView const &compareView = *(viewFetch.data());
                bool const equal =
                                (compareView.description.arraySliceRange == aArrayRange)
                                and (compareView.description.mipSliceRange == aMipRange)
                                and ((RenderGraphFormat_t::Automatic == aFormat) or (compareView.description.format == aFormat))
                                and (compareView.description.source == aViewSource)
                                and (compareView.description.mode.check(aMode))
                                and (compareView.subjacentResource == aSubjacentResourceId);

                if(equal)
                {
                    return { EEngineStatus::RenderGraph_PassBuilder_DuplicateTextureViewId, compareView.resourceId };
                }
            }

            return { EEngineStatus::Ok, 0 };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CPassBuilder::adjustArrayAndMipSliceRanges(
            CRenderGraphResources const &aResourceData,
            CRange                const &aSubjacentArraySliceRange,
            CRange                const &aSubjacentMipSliceRange,
            CRange                const &aSourceArraySliceRange,
            CRange                const &aSourceMipSliceRange,
            CRange                const &aTargetArraySliceRange,
            CRange                const &aTargetMipSliceRange,
            CRange                      &aAdjustedArraySliceRange,
            CRange                      &aAdjustedMipSliceRange)
        {
            aAdjustedArraySliceRange = aTargetArraySliceRange;
            aAdjustedMipSliceRange   = aTargetMipSliceRange;

            aAdjustedArraySliceRange.offset = (aSourceArraySliceRange.offset + aTargetArraySliceRange.offset);
            aAdjustedMipSliceRange.offset   = (aSourceMipSliceRange.offset   + aTargetMipSliceRange.offset);

            bool const arraySliceRangeOutOfBounds =
                           (aAdjustedArraySliceRange.length > aTargetArraySliceRange.length)
                           or ((static_cast<int32_t>(aAdjustedArraySliceRange.offset) + static_cast<int32_t>(aAdjustedArraySliceRange.length)) > aSubjacentArraySliceRange.length);

            bool const mipSliceRangeOutOfBounds =
                           (aAdjustedMipSliceRange.length   > aTargetMipSliceRange.length)
                           or ((static_cast<int32_t>(aAdjustedMipSliceRange.offset) + static_cast<int32_t>(aAdjustedMipSliceRange.length)) > aSubjacentMipSliceRange.length);

            if(arraySliceRangeOutOfBounds or mipSliceRangeOutOfBounds)
            {
                CLog::Error(logTag(), StaticStringHelpers::format("Derived subresource range out of bounds (Array:{}, Mip:{}).", arraySliceRangeOutOfBounds, mipSliceRangeOutOfBounds));
                return { EEngineStatus::Error };
            }

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CPassBuilder::validateArrayAndMipSliceRanges(
            CRenderGraphResources const &aResourceData,
            SRenderGraphResource  const &aSourceResource,
            CRange                const &aArraySliceRange,
            CRange                const &aMipSliceRange,
            bool                         aValidateReads,
            bool                         aValidateWrites)
        {
#if defined SHIRABE_DEBUG || defined SHIRABE_TEST
            if(aValidateReads)
            {
                CEngineResult<bool> const isBeingReadQuery =
                                              isImageBeingReadInSubresourceRange(
                                                  aResourceData.imageViews()
                                                  , aResourceData
                                                  , aSourceResource
                                                  , aArraySliceRange
                                                  , aMipSliceRange);

                if(not isBeingReadQuery.successful())
                {
                    return { isBeingReadQuery.result() };
                }

                bool const isBeingRead = isBeingReadQuery.data();
                if(isBeingRead)
                {
                    CLog::Error(logTag(),
                        StaticStringHelpers::format(
                            "Resource is already being read at the specified ranges (Array: {}[{}]; Mip: {}[{}])",
                            aArraySliceRange.offset,
                            aArraySliceRange.length,
                            aMipSliceRange.offset,
                            aMipSliceRange.length));
                    return { EEngineStatus::RenderGraph_PassBuilder_TextureIsBeingRead };
                }
            }

            if(aValidateWrites)
            {
                CEngineResult<bool> const isBeingWrittenQuery =
                                              isImageBeingWrittenInSubresourceRange(
                                                  aResourceData.imageViews()
                                                  , aResourceData
                                                  , aSourceResource
                                                  , aArraySliceRange
                                                  , aMipSliceRange);
                if(not isBeingWrittenQuery.successful())
                {
                    return { isBeingWrittenQuery.result() };
                }

                bool const isBeingWritten = isBeingWrittenQuery.data();
                if(isBeingWritten)
                {
                    CLog::Error(logTag(),
                        StaticStringHelpers::format(
                            "Resource is already being written at the specified ranges (Array: {}[{}]; Mip: {}[{}])",
                            aArraySliceRange.offset,
                            aArraySliceRange.length,
                            aMipSliceRange.offset,
                            aMipSliceRange.length));
                    return { EEngineStatus::RenderGraph_PassBuilder_TextureIsBeingWritten };
                }
            }
#endif

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<bool> CPassBuilder::isImageBeingReadInSubresourceRange(
                RefIndex_t            const &aResourceViews,
                CRenderGraphResources const &aResources,
                SRenderGraphResource  const &aSourceResource,
                CRange                const &aArraySliceRange,
                CRange                const &aMipSliceRange)
        {
            // This test method can check for overlapping regions for array slices and mip slices
            //  given a specific set of options to test against.
            // If "Write" is checked, this is usually done due to read operations to take place.
            // If a "Read" and "Write" of two subresources does not overlap, both operations
            //  as such are valid.
            // The whole operation setup is based on first come first serve.
            for(RefIndex_t::value_type const &viewRef : aResourceViews)
            {
                CEngineResult<Shared<SRenderGraphImageView>> const &viewFetch = aResources.getResource<SRenderGraphImageView>(viewRef);
                if(not viewFetch.successful())
                {
                    return { viewFetch.result(), false };
                }

                SRenderGraphImageView const &view = *(viewFetch.data());

                bool correctMode = view.description.mode.check(ERenderGraphViewAccessMode::Read);
                if(not correctMode)
                {
                    continue;
                }

                bool const commonPass      = aSourceResource.assignedPassUID   == view.assignedPassUID;
                bool const commonId        = aSourceResource.resourceId        == view.resourceId;
                bool const commonSubjacent = aSourceResource.subjacentResource == view.subjacentResource;
                if(not (commonPass && commonId && commonSubjacent))
                {
                    continue;
                }

                bool const arraySliceOverlap = view.description.arraySliceRange.overlapsWith(aArraySliceRange);
                if(not arraySliceOverlap)
                {
                    continue;
                }

                bool const mipSliceOverlap = view.description.mipSliceRange.overlapsWith(aMipSliceRange);
                if(mipSliceOverlap)
                {
                    continue;
                }

                return { EEngineStatus::Ok, true };
            }

            return { EEngineStatus::Ok, false };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<bool> CPassBuilder::isImageBeingWrittenInSubresourceRange(
                RefIndex_t           const &aResourceViews,
                CRenderGraphResources const &aResources,
                SRenderGraphResource  const &aSourceResource,
                CRange               const &aArraySliceRange,
                CRange               const &aMipSliceRange)
        {
            // This test method can check for overlapping regions for array slices and mip slices
            //  given a specific set of options to test against.
            // If "Write" is checked, this is usually done due to read operations to take place.
            // If a "Read" and "Write" of two subresources does not overlap, both operations
            //  as such are valid.
            // The whole operation setup is based on first come first serve.
            for(RefIndex_t::value_type const &viewRef : aResourceViews)
            {
                CEngineResult<Shared<SRenderGraphImageView>> const &viewFetch = aResources.getResource<SRenderGraphImageView>(viewRef);
                if(not viewFetch.successful())
                {

                }

                SRenderGraphImageView const &view = *(viewFetch.data());

                bool correctMode = view.description.mode.check(ERenderGraphViewAccessMode::Write);
                if(not correctMode)
                {
                    continue;
                }

                bool const commonPass      = aSourceResource.assignedPassUID   == view.assignedPassUID;
                bool const commonId        = aSourceResource.resourceId        == view.resourceId;
                bool const commonSubjacent = aSourceResource.subjacentResource == view.subjacentResource;
                if(not (commonPass && !commonId && commonSubjacent))
                {
                    continue;
                }

                bool const arraySliceOverlap = view.description.arraySliceRange.overlapsWith(aArraySliceRange);
                if(not arraySliceOverlap)
                {
                    continue;
                }
                else
                {
                    bool const mipSliceOverlap = view.description.mipSliceRange.overlapsWith(aMipSliceRange);
                    if(not mipSliceOverlap)
                    {
                        continue;
                    }
                }

                return { EEngineStatus::Ok, true };
            }

            return { EEngineStatus::Ok, false };
        }
        //<-----------------------------------------------------------------------------
    }
}
