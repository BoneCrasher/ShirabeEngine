#include <base/string.h>
#include "renderer/framegraph/renderpass.h"
#include "renderer/framegraph/subpass.h"
#include "renderer/framegraph/graphbuilder.h"
#include "renderer/framegraph/passbuilder.h"


namespace engine
{
    namespace framegraph
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CPassBuilder::CPassBuilder(
                PassUID_t const             &aPassUID,
                Shared<CPassBase>            aPass,
                Shared<CRenderPass>          aEnclosingRenderPass,
                CRenderGraphMutableResources &aOutResourceData)
            : mPassUID             (aPassUID                           )
            , mPass                (std::move(aPass                   ))
            , mEnclosingRenderPass (std::move(aEnclosingRenderPass    ))
            , mResourceData        (aOutResourceData                   )
            , mAttachmentCollection(aEnclosingRenderPass->attachments())
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SRenderGraphRenderTarget> CPassBuilder::createRenderTarget(
            std::string                 const &aName,
            SRenderGraphImageDescription const &aDescriptor)
        {
            // Basic abstract descriptor of resources being used.
            SRenderGraphRenderTarget &resource = mResourceData.spawnResource<SRenderGraphRenderTarget>();
            resource.description           = aDescriptor;
            resource.assignedRenderpassUID = mEnclosingRenderPass->getRenderPassUid();
            resource.assignedPassUID       = mPassUID;
            resource.parentResource        = SHIRABE_FRAMEGRAPH_UNDEFINED_RESOURCE;
            resource.subjacentResource     = resource.resourceId; // This is a trick to keep algorithms consistent while supporting r/w from textures and t-views.
            resource.readableName          = aName;
            resource.type                  = ERenderGraphResourceType::Texture;

            mPass->registerResource(resource.resourceId);

            return { EEngineStatus::Ok, resource };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SRenderGraphTransientImage> CPassBuilder::createImage(
                std::string                  const &aName,
                SRenderGraphImageDescription const &aDescriptor)
        {
            // Basic abstract descriptor of resources being used.
            SRenderGraphTransientImage &resource = mResourceData.spawnResource<SRenderGraphTransientImage>();
            resource.description           = aDescriptor;
            resource.assignedRenderpassUID = mEnclosingRenderPass->getRenderPassUid();
            resource.assignedPassUID       = mPassUID;
            resource.parentResource        = SHIRABE_FRAMEGRAPH_UNDEFINED_RESOURCE;
            resource.subjacentResource     = resource.resourceId; // This is a trick to keep algorithms consistent while supporting r/w from textures and t-views.
            resource.readableName          = aName;
            resource.type                  = ERenderGraphResourceType::Texture;

            mPass->registerResource(resource.resourceId);

            return { EEngineStatus::Ok, resource };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SRenderGraphImageView> CPassBuilder::useImage(
            SRenderGraphImage                &aTexture,
            ERenderGraphViewPurpose    const &aSourceOrTarget,
            EFormat                    const &aRequiredFormat,
            CRange                     const &aArraySliceRange,
            CRange                     const &aMipSliceRange,
            ERenderGraphViewAccessMode const &aMode,
            EEngineStatus              const &aFailCode)
        {
            RenderGraphResourceId_t const subjacentResourceId = aTexture.resourceId;

            ERenderGraphViewAccessMode const mode   = aMode;
            ERenderGraphViewPurpose    const source = aSourceOrTarget;

            CRange adjustedArraySliceRange = aArraySliceRange;
            CRange adjustedMipSliceRange   = aMipSliceRange;

            adjustArrayAndMipSliceRanges(
                        mResourceData,
                        aTexture,
                        aArraySliceRange,
                        aMipSliceRange,
                        adjustedArraySliceRange,
                        adjustedMipSliceRange);

            validateArrayAndMipSliceRanges(
                        mResourceData,
                        aTexture,
                        adjustedArraySliceRange,
                        adjustedMipSliceRange,
                        true,
                        true);

            Optional_t<RefWrapper_t<SRenderGraphImageView>> resource{};

            SRenderGraphImageView &view = mResourceData.spawnResource<SRenderGraphImageView>();
            view.description.arraySliceRange = adjustedArraySliceRange;
            view.description.mipSliceRange   = adjustedMipSliceRange;
            view.description.format          = aRequiredFormat;
            view.description.source          = source;
            view.assignedRenderpassUID       = mEnclosingRenderPass->getRenderPassUid();
            view.assignedPassUID             = mPassUID;
            view.parentResource              = aTexture.resourceId;
            view.subjacentResource           = subjacentResourceId;
            view.type                        = ERenderGraphResourceType::TextureView;
            view.readableName                = CString::format("TextureView ID {} - {} #{}"
                                                               , view.resourceId
                                                               , ( mode == ERenderGraphViewAccessMode::Write ? "Write" : "Read" )
                                                               , aTexture.resourceId);
            view.description.mode.set(mode);

            switch(view.description.source)
            {
            case ERenderGraphViewPurpose::InputAttachment:
                aTexture.description.requestedUsage.set(ERenderGraphResourceUsage::InputAttachment);
                break;
            case ERenderGraphViewPurpose::ColorAttachment:
                aTexture.description.requestedUsage.set(ERenderGraphResourceUsage::ColorAttachment);
                break;
            case ERenderGraphViewPurpose::DepthAttachment:
                aTexture.description.requestedUsage.set(ERenderGraphResourceUsage::DepthAttachment);
                break;
            case ERenderGraphViewPurpose::ShaderInput:
                aTexture.description.requestedUsage.set(ERenderGraphResourceUsage::SampledImage);
                break;
            default:
                break;
            }

            ++(aTexture.referenceCount);

            mPass->registerResource(view.resourceId);
            ++(view.referenceCount);

            return { EEngineStatus::Ok, *resource };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SRenderGraphImageView> CPassBuilder::useImageView(
            SRenderGraphImageView             &aTextureView,
            ERenderGraphViewPurpose       const &aSourceOrTarget,
            EFormat                      const &aRequiredFormat,
            CRange                       const &aArraySliceRange,
            CRange                       const &aMipSliceRange,
            ERenderGraphViewAccessMode    const &aMode,
            EEngineStatus                const &aFailCode)
        {
            RenderGraphResourceId_t const subjacentResourceId = aTextureView.subjacentResource;

            ERenderGraphViewAccessMode const mode   = aMode;
            ERenderGraphViewPurpose    const source = aSourceOrTarget;

            CRange adjustedArraySliceRange = aArraySliceRange;
            CRange adjustedMipSliceRange   = aMipSliceRange;

            adjustArrayAndMipSliceRanges(
                mResourceData,
                aTextureView,
                aArraySliceRange,
                aMipSliceRange,
                adjustedArraySliceRange,
                adjustedMipSliceRange);

            validateArrayAndMipSliceRanges(
                mResourceData,
                aTextureView,
                adjustedArraySliceRange,
                adjustedMipSliceRange,
                true,
                true);

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
                view.type                        = ERenderGraphResourceType::TextureView;
                view.readableName                = CString::format("TextureView ID {} - {} #{}"
                                                                   , view.resourceId
                                                                   , ( mode == ERenderGraphViewAccessMode::Write ? "Write" : "Read" )
                                                                   , aTextureView.resourceId);
                view.description.mode.set(mode);

                switch(view.description.source)
                {
                    case ERenderGraphViewPurpose::InputAttachment:
                        texture->description.requestedUsage.set(ERenderGraphResourceUsage::InputAttachment);
                        break;
                    case ERenderGraphViewPurpose::ColorAttachment:
                        texture->description.requestedUsage.set(ERenderGraphResourceUsage::ColorAttachment);
                        break;
                    case ERenderGraphViewPurpose::DepthAttachment:
                        texture->description.requestedUsage.set(ERenderGraphResourceUsage::DepthAttachment);
                        break;
                    case ERenderGraphViewPurpose::ShaderInput:
                        texture->description.requestedUsage.set(ERenderGraphResourceUsage::SampledImage);
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

                mPass->registerResource(ref.resourceId);
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
            SRenderGraphImageView             &aImageView,
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
            SRenderGraphImageView            &aImageView,
            SRenderGraphReadTextureFlags const &aFlags)
        {
            ERenderGraphViewPurpose source = ERenderGraphViewPurpose::InputAttachment;

            auto const &[result, data]  = useImageView(aImageView
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
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SRenderGraphMaterial> CPassBuilder::useMaterial(SRenderGraphMaterialDescription const &aMaterialDescription)
        {
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

            return { EEngineStatus::Ok, materialResource };
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

            return { EEngineStatus::Ok, meshResource };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SRenderGraphPipeline> CPassBuilder::usePipeline(resources::ResourceId_t      const &aSharedMaterialResourceid
                                                                      , SRenderGraphPipelineConfig const &aPipelineConfig)
        {
            SRenderGraphPipeline &resource = mResourceData.spawnResource<SRenderGraphPipeline>();
            resource.readableName             = aSharedMaterialResourceid;
            resource.type                     = ERenderGraphResourceType::Pipeline;
            resource.assignedRenderpassUID    = mEnclosingRenderPass->getRenderPassUid();
            resource.assignedPassUID          = mPassUID;
            resource.isExternalResource       = false;
            resource.parentResource           = 0;
            resource.referenceCount           = 0;
            resource.subjacentResource        = 0;
            resource.sharedMaterialResourceId = aSharedMaterialResourceid;
            resource.pipelineConfiguration    = aPipelineConfig;

            return { EEngineStatus::Ok, resource };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<RenderGraphResourceId_t> CPassBuilder::findDuplicateImageView(
            RenderGraphResourceId_t               const &aSubjacentResourceId,
            RenderGraphFormat_t                   const &aFormat,
            ERenderGraphViewPurpose                const &aViewSource,
            CRange                               const &aArrayRange,
            CRange                               const &aMipRange,
            CBitField<ERenderGraphViewAccessMode> const &aMode)
        {
            for(RenderGraphResourceId_t const &viewId : mResourceData.textureViews())
            {
                CEngineResult<Shared<SRenderGraphImageView>> viewFetch = mResourceData.getResource<SRenderGraphImageView>(viewId);
                if(not viewFetch.successful())
                {
                    continue;
                }

                SRenderGraphImageView const &compareView = *(viewFetch.data());
                bool const                  equal        =
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
            SRenderGraphResource  const &aSourceResource,
            CRange               const &aArraySliceRange,
            CRange               const &aMipSliceRange,
            CRange                     &aAdjustedArraySliceRange,
            CRange                     &aAdjustedMipSliceRange)
        {
            aAdjustedArraySliceRange = aArraySliceRange;
            aAdjustedMipSliceRange   = aMipSliceRange;

            if(ERenderGraphResourceType::TextureView == aSourceResource.type)
            {
                CEngineResult<Shared<SRenderGraphImage>> subjacentFetch = aResourceData.getResource<SRenderGraphImage>(aSourceResource.subjacentResource);
#if defined SHIRABE_DEBUG || defined SHIRABE_TEST
                if(not subjacentFetch.successful())
                {
                    CLog::Error(logTag(), CString::format("Subjacent resource handle w/ id {} is empty."));
                    return { EEngineStatus::Error };
                }
#endif

                CEngineResult<Shared<SRenderGraphImageView>> parentFetch = aResourceData.getResource<SRenderGraphImageView>(aSourceResource.resourceId);
#if defined SHIRABE_DEBUG || defined SHIRABE_TEST
                if(not parentFetch.successful())
                {
                    CLog::Error(logTag(), CString::format("Parent resource handle w/ id {} is empty.", aSourceResource.resourceId));
                    return { EEngineStatus::Error };
                }
#endif

                SRenderGraphImage     const &subjacent = *(subjacentFetch.data());
                SRenderGraphImageView const &parent    = *(parentFetch.data());

                aAdjustedArraySliceRange.offset = (parent.description.arraySliceRange.offset + aArraySliceRange.offset);
                aAdjustedMipSliceRange.offset   = (parent.description.mipSliceRange.offset   + aMipSliceRange.offset);

#if defined SHIRABE_DEBUG || defined SHIRABE_TEST
                bool const arraySliceRangeOutOfBounds =
                               (aAdjustedArraySliceRange.length > parent.description.arraySliceRange.length)
                               or ((aAdjustedArraySliceRange.offset + static_cast<uint32_t>(aAdjustedArraySliceRange.length)) > subjacent.description.arraySize);

                bool const mipSliceRangeOutOfBounds =
                               (aAdjustedMipSliceRange.length   > parent.description.mipSliceRange.length)
                               or ((aAdjustedMipSliceRange.offset   + static_cast<uint32_t>(aAdjustedMipSliceRange.length)) > subjacent.description.mipLevels);

                if(arraySliceRangeOutOfBounds or mipSliceRangeOutOfBounds)
                {
                    CLog::Error(logTag(), CString::format("Derived subresource range out of bounds (Array:{}, Mip:{}).", arraySliceRangeOutOfBounds, mipSliceRangeOutOfBounds));
                    return { EEngineStatus::Error };
                }
#endif

            }
#if defined SHIRABE_DEBUG || defined SHIRABE_TEST
            else // Texture
            {
                CEngineResult<Shared<SRenderGraphImage>> subjacentFetch = aResourceData.getResource<SRenderGraphImage>(aSourceResource.resourceId);
                if(not subjacentFetch.successful())
                {
                    CLog::Error(logTag(), CString::format("Subjacent resource handle w/ id {} is empty.", aSourceResource.subjacentResource));
                    return { EEngineStatus::Error };
                }

                SRenderGraphImage const &subjacent = *(subjacentFetch.data());

                bool const arraySliceRangeOutOfBounds = ((aAdjustedArraySliceRange.offset + static_cast<uint32_t>(aAdjustedArraySliceRange.length)) > subjacent.description.arraySize);
                bool const mipSliceRangeOutOfBounds   = ((aAdjustedMipSliceRange.offset   + static_cast<uint32_t>(aAdjustedMipSliceRange.length))   > subjacent.description.mipLevels);

                if(arraySliceRangeOutOfBounds or mipSliceRangeOutOfBounds)
                {
                    CLog::Error(logTag(), CString::format("Subresource range out of bounds (Array:{}, Mip:{}).", arraySliceRangeOutOfBounds, mipSliceRangeOutOfBounds));
                    return { EEngineStatus::Error };
                }
            }
#endif

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
                                                  aResourceData.textureViews()
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
                        CString::format(
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
                                                  aResourceData.textureViews()
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
                        CString::format(
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
