#include <base/string.h>
#include "renderer/framegraph/pass.h"
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
                PassUID_t                   const &aPassUID,
                Shared<CPassBase>         aPass,
                CFrameGraphMutableResources       &aOutResourceData)
            : mPassUID             (aPassUID                         )
            , mPass                (aPass                            )
            , mResourceData        (aOutResourceData                 )
            , mAttachmentCollection(aOutResourceData.getAttachments())
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SFrameGraphResource> CPassBuilder::createTexture(
                std::string        const &aName,
                SFrameGraphTexture const &aDescriptor)
        {
            // Basic abstract descriptor of resources being used.
            SFrameGraphTexture &resource = mResourceData.spawnResource<SFrameGraphTexture>();
            resource.assignTextureParameters(aDescriptor);
            resource.assignedPassUID     = mPassUID;
            resource.parentResource      = SHIRABE_FRAMEGRAPH_UNDEFINED_RESOURCE;
            resource.subjacentResource   = resource.resourceId; // This is a trick to keep algorithms consistent while supporting r/w from textures and t-views.
            resource.readableName        = aName;
            resource.type                = EFrameGraphResourceType::Texture;

            Unique<CPassBase::CMutableAccessor> accessor = mPass->getMutableAccessor(CPassKey<CPassBuilder>());
            accessor->registerResource(resource.resourceId);

            return { EEngineStatus::Ok, resource };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SFrameGraphResource> CPassBuilder::importTexture(
                std::string        const &aName,
                SFrameGraphTexture const &aDescriptor)
        {
            SFrameGraphTexture &resource = mResourceData.spawnResource<SFrameGraphTexture>();
            resource.assignTextureParameters(aDescriptor);
            resource.assignedPassUID     = mPassUID;
            resource.parentResource      = SHIRABE_FRAMEGRAPH_UNDEFINED_RESOURCE;
            resource.subjacentResource   = resource.resourceId; // This is a trick to keep algorithms consistent while supporting r/w from textures and t-views.
            resource.readableName        = aName;
            resource.type                = EFrameGraphResourceType::Texture;
            resource.isExternalResource  = true;

            Unique<CPassBase::CMutableAccessor> accessor = mPass->getMutableAccessor(CPassKey<CPassBuilder>());
            accessor->registerResource(resource.resourceId);

            return { EEngineStatus::Ok, resource };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<FrameGraphResourceId_t> CPassBuilder::findDuplicateTextureView(
                FrameGraphResourceId_t               const &aSubjacentResourceId,
                FrameGraphFormat_t                   const &aFormat,
                EFrameGraphViewPurpose                const &aViewSource,
                CRange                               const &aArrayRange,
                CRange                               const &aMipRange,
                CBitField<EFrameGraphViewAccessMode> const &aMode)
        {
            for(FrameGraphResourceId_t const &viewId : mResourceData.textureViews())
            {
                CEngineResult<Shared<SFrameGraphTextureView> const> viewFetch = mResourceData.get<SFrameGraphTextureView>(viewId);
                if(not viewFetch.successful())
                {
                    continue;
                }

                SFrameGraphTextureView const &compareView = *(viewFetch.data());
                bool const equal =
                        (compareView.arraySliceRange == aArrayRange)
                        and (compareView.mipSliceRange == aMipRange)
                        and ((FrameGraphFormat_t::Automatic == aFormat) or (compareView.format == aFormat))
                        and (compareView.source == aViewSource)
                        and (compareView.mode.check(aMode))
                        and (compareView.subjacentResource == aSubjacentResourceId);

                if(equal)
                {
                    return { EEngineStatus::FrameGraph_PassBuilder_DuplicateTextureViewId, compareView.resourceId };
                }
            }

            return { EEngineStatus::Ok, 0 };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CPassBuilder::adjustArrayAndMipSliceRanges(
                CFrameGraphResources const &aResourceData,
                SFrameGraphResource  const &aSourceResource,
                CRange               const &aArraySliceRange,
                CRange               const &aMipSliceRange,
                CRange                     &aAdjustedArraySliceRange,
                CRange                     &aAdjustedMipSliceRange)
        {
            aAdjustedArraySliceRange = aArraySliceRange;
            aAdjustedMipSliceRange   = aMipSliceRange;

            if(EFrameGraphResourceType::TextureView == aSourceResource.type)
            {
                CEngineResult<Shared<SFrameGraphTexture> const> subjacentFetch = aResourceData.get<SFrameGraphTexture>(aSourceResource.subjacentResource);
#if defined SHIRABE_DEBUG || defined SHIRABE_TEST
                if(not subjacentFetch.successful())
                {
                    CLog::Error(logTag(), CString::format("Subjacent resource handle w/ id %0 is empty."));
                    return { EEngineStatus::Error };
                }
#endif

                CEngineResult<Shared<SFrameGraphTextureView> const> parentFetch = aResourceData.get<SFrameGraphTextureView>(aSourceResource.resourceId);
#if defined SHIRABE_DEBUG || defined SHIRABE_TEST
                if(not parentFetch.successful())
                {
                    CLog::Error(logTag(), CString::format("Parent resource handle w/ id %0 is empty.", aSourceResource.resourceId));
                    return { EEngineStatus::Error };
                }
#endif

                SFrameGraphTexture     const &subjacent = *(subjacentFetch.data());
                SFrameGraphTextureView const &parent    = *(parentFetch.data());

                aAdjustedArraySliceRange.offset = (parent.arraySliceRange.offset + aArraySliceRange.offset);
                aAdjustedMipSliceRange.offset   = (parent.mipSliceRange.offset   + aMipSliceRange.offset);

#if defined SHIRABE_DEBUG || defined SHIRABE_TEST
                bool const arraySliceRangeOutOfBounds =
                        (aAdjustedArraySliceRange.length > parent.arraySliceRange.length)
                        or ((aAdjustedArraySliceRange.offset + static_cast<uint32_t>(aAdjustedArraySliceRange.length)) > subjacent.arraySize);

                bool const mipSliceRangeOutOfBounds =
                        (aAdjustedMipSliceRange.length   > parent.mipSliceRange.length)
                        or ((aAdjustedMipSliceRange.offset   + static_cast<uint32_t>(aAdjustedMipSliceRange.length)) > subjacent.mipLevels);

                if(arraySliceRangeOutOfBounds or mipSliceRangeOutOfBounds)
                {
                    CLog::Error(logTag(), CString::format("Derived subresource range out of bounds (Array:%0, Mip:%1).", arraySliceRangeOutOfBounds, mipSliceRangeOutOfBounds));
                    return { EEngineStatus::Error };
                }
#endif

            }
#if defined SHIRABE_DEBUG || defined SHIRABE_TEST
            else // Texture
            {
                CEngineResult<Shared<SFrameGraphTexture> const> subjacentFetch = aResourceData.get<SFrameGraphTexture>(aSourceResource.resourceId);
                if(not subjacentFetch.successful())
                {
                    CLog::Error(logTag(), CString::format("Subjacent resource handle w/ id %0 is empty.", aSourceResource.subjacentResource));
                    return { EEngineStatus::Error };
                }

                SFrameGraphTexture const &subjacent = *(subjacentFetch.data());

                bool const arraySliceRangeOutOfBounds = ((aAdjustedArraySliceRange.offset + static_cast<uint32_t>(aAdjustedArraySliceRange.length)) > subjacent.arraySize);
                bool const mipSliceRangeOutOfBounds   = ((aAdjustedMipSliceRange.offset   + static_cast<uint32_t>(aAdjustedMipSliceRange.length))   > subjacent.mipLevels);

                if(arraySliceRangeOutOfBounds or mipSliceRangeOutOfBounds)
                {
                    CLog::Error(logTag(), CString::format("Subresource range out of bounds (Array:%0, Mip:%1).", arraySliceRangeOutOfBounds, mipSliceRangeOutOfBounds));
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
                CFrameGraphResources const &aResourceData,
                SFrameGraphResource  const &aSourceResource,
                CRange               const &aArraySliceRange,
                CRange               const &aMipSliceRange,
                bool                        aValidateReads,
                bool                        aValidateWrites)
        {
#if defined SHIRABE_DEBUG || defined SHIRABE_TEST
            if(aValidateReads)
            {
                CEngineResult<bool> const isBeingReadQuery =
                        isTextureBeingReadInSubresourceRange(
                            aResourceData.textureViews(),
                            aResourceData,
                            aSourceResource,
                            aArraySliceRange,
                            aMipSliceRange);

                if(not isBeingReadQuery.successful())
                {
                    return { isBeingReadQuery.result() };
                }

                bool const isBeingRead = isBeingReadQuery.data();
                if(isBeingRead)
                {
                    CLog::Error(logTag(),
                                CString::format(
                                    "Resource is already being read at the specified ranges (Array: %0[%1]; Mip: %2[%3])",
                                    aArraySliceRange.offset,
                                    aArraySliceRange.length,
                                    aMipSliceRange.offset,
                                    aMipSliceRange.length));
                    return { EEngineStatus::FrameGraph_PassBuilder_TextureIsBeingRead };
                }
            }

            if(aValidateWrites)
            {
                CEngineResult<bool> const isBeingWrittenQuery =
                        isTextureBeingWrittenInSubresourceRange(
                            aResourceData.textureViews(),
                            aResourceData, aSourceResource,
                            aArraySliceRange,
                            aMipSliceRange);
                if(not isBeingWrittenQuery.successful())
                {
                    return { isBeingWrittenQuery.result() };
                }

                bool const isBeingWritten = isBeingWrittenQuery.data();
                if(isBeingWritten)
                {
                    CLog::Error(logTag(),
                            CString::format(
                                "Resource is already being written at the specified ranges (Array: %0[%1]; Mip: %2[%3])",
                                aArraySliceRange.offset,
                                aArraySliceRange.length,
                                aMipSliceRange.offset,
                                aMipSliceRange.length));
                    return { EEngineStatus::FrameGraph_PassBuilder_TextureIsBeingWritten };
                }
            }
#endif

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SFrameGraphResource> CPassBuilder::forwardTexture(
                SFrameGraphResource             const &aSubjacentTargetResource,
                SFrameGraphTextureResourceFlags const &aFlags)
        {
            Unique<CPassBase::CMutableAccessor> accessor = mPass->getMutableAccessor(CPassKey<CPassBuilder>());

            OptionalRef_t<SFrameGraphResource> resource = {};

            FrameGraphResourceId_t const subjacentResourceId =
                    (aSubjacentTargetResource.type == EFrameGraphResourceType::Texture)
                     ? aSubjacentTargetResource.resourceId
                     : aSubjacentTargetResource.subjacentResource;

            EFrameGraphViewAccessMode const mode   = EFrameGraphViewAccessMode::Forward;
            EFrameGraphViewPurpose     const source = EFrameGraphViewPurpose::Undefined;

            CRange adjustedArraySliceRange = aFlags.arraySliceRange;
            CRange adjustedMipSliceRange   = aFlags.mipSliceRange;

            adjustArrayAndMipSliceRanges(
                        mResourceData,
                        aSubjacentTargetResource,
                        aFlags.arraySliceRange,
                        aFlags.mipSliceRange,
                        adjustedArraySliceRange,
                        adjustedMipSliceRange);

            // Can we cull?
            bool duplicateFound = false;
            if(aSubjacentTargetResource.type == EFrameGraphResourceType::TextureView)
            {
                CEngineResult<FrameGraphResourceId_t> const duplicateViewIdQuery =
                        findDuplicateTextureView(
                            subjacentResourceId,
                            aFlags.requiredFormat,
                            source,
                            adjustedArraySliceRange,
                            adjustedMipSliceRange,
                            mode);
                if(not duplicateViewIdQuery.successful())
                {
                    return { duplicateViewIdQuery.result() };
                }

                duplicateFound = duplicateViewIdQuery.resultEquals(EEngineStatus::FrameGraph_PassBuilder_DuplicateTextureViewId);
                if(duplicateFound)
                {
                    FrameGraphResourceId_t const &duplicateViewId = duplicateViewIdQuery.data();

                    CEngineResult<Shared<SFrameGraphTextureView>> const &viewFetch = mResourceData.getMutable<SFrameGraphTextureView>(duplicateViewId);
                    if(not viewFetch.successful())
                    {
                        return { viewFetch.result() };
                    }

                    resource = *(viewFetch.data());
                }
            }

            if(not duplicateFound)
            {
                SFrameGraphTextureView &view = mResourceData.spawnResource<SFrameGraphTextureView>();
                view.arraySliceRange    = adjustedArraySliceRange;
                view.mipSliceRange      = adjustedMipSliceRange;
                view.format             = aFlags.requiredFormat;
                view.source             = source;
                view.assignedPassUID    = mPassUID;
                view.parentResource     = aSubjacentTargetResource.resourceId;
                view.subjacentResource  = subjacentResourceId;
                view.readableName       = CString::format("TextureView ID %0 - Forward #%1", view.resourceId, aSubjacentTargetResource.resourceId);
                view.type               = EFrameGraphResourceType::TextureView;
                view.mode.set(mode);

                CEngineResult<Shared<SFrameGraphResource>> const &subjacentFetch = mResourceData.getMutable<SFrameGraphResource>(subjacentResourceId);
                if(not subjacentFetch.successful())
                {
                    return { subjacentFetch.result() };
                }

                SFrameGraphResource &subjacent = *(subjacentFetch.data());
                ++(subjacent.referenceCount);

                resource = view;
            }

            if(resource.has_value())
            {
                SFrameGraphResource &ref = resource->get();

                accessor->registerResource(ref.resourceId);
                ++(ref.referenceCount);

                return { EEngineStatus::Ok, *resource };
            }

            return { EEngineStatus::FrameGraph_PassBuilder_ForwardResourceFailed, *resource };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SFrameGraphResource> CPassBuilder::acceptTexture(SFrameGraphResource const &aSubjacentTargetResource)
        {
            Unique<CPassBase::CMutableAccessor> accessor = mPass->getMutableAccessor(CPassKey<CPassBuilder>());

            FrameGraphResourceId_t const subjacentResourceId =
                    (aSubjacentTargetResource.type == EFrameGraphResourceType::Texture)
                     ? aSubjacentTargetResource.resourceId
                     : aSubjacentTargetResource.subjacentResource;

            EFrameGraphViewAccessMode const mode   = EFrameGraphViewAccessMode::Accept;
            EFrameGraphViewPurpose     const source = EFrameGraphViewPurpose::Undefined;

            // TODO: Naming correct? SHouldn't it be parent resource?
            CEngineResult<Shared<SFrameGraphTextureView> const> const &subjacentTargetFetch = mResourceData.get<SFrameGraphTextureView>(aSubjacentTargetResource.resourceId);
            if(not subjacentTargetFetch.successful())
            {
                return { subjacentTargetFetch.result() };
            }

            SFrameGraphTextureView const &subjacentTextureView = *(subjacentTargetFetch.data());

            CRange adjustedArraySliceRange = subjacentTextureView.arraySliceRange;
            CRange adjustedMipSliceRange   = subjacentTextureView.mipSliceRange;

            adjustArrayAndMipSliceRanges(
                        mResourceData,
                        aSubjacentTargetResource,
                        subjacentTextureView.arraySliceRange,
                        subjacentTextureView.mipSliceRange,
                        adjustedArraySliceRange,
                        adjustedMipSliceRange);

            Optional_t<RefWrapper_t<SFrameGraphTextureView>> resource{};

            // Can we cull?
            bool duplicateFound = false;
            if(aSubjacentTargetResource.type == EFrameGraphResourceType::TextureView)
            {
                CEngineResult<FrameGraphResourceId_t> const duplicateViewIdQuery =
                        findDuplicateTextureView(
                            subjacentResourceId,
                            subjacentTextureView.format,
                            source,
                            adjustedArraySliceRange,
                            adjustedMipSliceRange,
                            mode);
                if(not duplicateViewIdQuery.successful())
                {
                    return { duplicateViewIdQuery.result() };
                }

                duplicateFound = duplicateViewIdQuery.resultEquals(EEngineStatus::FrameGraph_PassBuilder_DuplicateTextureViewId);
                if(duplicateFound)
                {
                    FrameGraphResourceId_t const &duplicateViewId = duplicateViewIdQuery.data();

                    CEngineResult<Shared<SFrameGraphTextureView>> const &viewFetch = mResourceData.getMutable<SFrameGraphTextureView>(duplicateViewId);
                    if(not viewFetch.successful())
                    {
                        return { viewFetch.result() };
                    }

                    resource = *(viewFetch.data());
                }
            }

            if(not duplicateFound)
            {
                SFrameGraphTextureView &view = mResourceData.spawnResource<SFrameGraphTextureView>();
                view.arraySliceRange    = adjustedArraySliceRange;
                view.mipSliceRange      = adjustedMipSliceRange;
                view.format             = subjacentTextureView.format;
                view.source             = source;
                view.assignedPassUID    = mPassUID;
                view.parentResource     = aSubjacentTargetResource.resourceId;
                view.subjacentResource  = subjacentResourceId;
                view.readableName       = CString::format("TextureView ID %0 - Forward #%1", view.resourceId, aSubjacentTargetResource.resourceId);
                view.type               = EFrameGraphResourceType::TextureView;
                view.mode.set(mode);

                CEngineResult<Shared<SFrameGraphResource>> const &subjacentFetch = mResourceData.getMutable<SFrameGraphResource>(subjacentResourceId);
                if(not subjacentFetch.successful())
                {
                    return { subjacentFetch.result() };
                }

                SFrameGraphResource &subjacent = *(subjacentFetch.data());
                ++(subjacent.referenceCount);

                resource = view;
            }

            if(resource.has_value())
            {
                SFrameGraphResource &ref = resource->get();

                accessor->registerResource(ref.resourceId);
                ++(ref.referenceCount);

                return { EEngineStatus::Ok, *resource };
            }

            return { EEngineStatus::FrameGraph_PassBuilder_AcceptResourceFailed, *resource };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SFrameGraphResource> CPassBuilder::useTexture(
                SFrameGraphResource          const &aSubjacentTargetResource,
                EFrameGraphViewPurpose       const &aSourceOrTarget,
                EFormat                      const &aRequiredFormat,
                CRange                       const &aArraySliceRange,
                CRange                       const &aMipSliceRange,
                EFrameGraphViewAccessMode    const &aMode,
                EEngineStatus                const &aFailCode)
        {
            Unique<CPassBase::CMutableAccessor> accessor = mPass->getMutableAccessor(CPassKey<CPassBuilder>());

            FrameGraphResourceId_t const subjacentResourceId =
                    (EFrameGraphResourceType::Texture == aSubjacentTargetResource.type)
                     ? aSubjacentTargetResource.resourceId
                     : aSubjacentTargetResource.subjacentResource;

            EFrameGraphViewAccessMode const mode   = aMode;
            EFrameGraphViewPurpose    const source = aSourceOrTarget;

            CRange adjustedArraySliceRange = aArraySliceRange;
            CRange adjustedMipSliceRange   = aMipSliceRange;

            adjustArrayAndMipSliceRanges(
                        mResourceData,
                        aSubjacentTargetResource,
                        aArraySliceRange,
                        aMipSliceRange,
                        adjustedArraySliceRange,
                        adjustedMipSliceRange);

            validateArrayAndMipSliceRanges(
                        mResourceData,
                        aSubjacentTargetResource,
                        adjustedArraySliceRange,
                        adjustedMipSliceRange,
                        true,
                        true);

            Optional_t<RefWrapper_t<SFrameGraphTextureView>> resource{};

            // Can we cull?
            bool duplicateFound = false;
            if(EFrameGraphResourceType::TextureView == aSubjacentTargetResource.type)
            {
                CEngineResult<FrameGraphResourceId_t> const duplicateViewIdQuery = findDuplicateTextureView(subjacentResourceId,
                                                                                                            aRequiredFormat,
                                                                                                            source,
                                                                                                            adjustedArraySliceRange,
                                                                                                            adjustedMipSliceRange,
                                                                                                            mode);

                if(not duplicateViewIdQuery.successful())
                {
                    return { duplicateViewIdQuery.result() };
                }

                duplicateFound = duplicateViewIdQuery.resultEquals(EEngineStatus::FrameGraph_PassBuilder_DuplicateTextureViewId);
                if(duplicateFound)
                {
                    FrameGraphResourceId_t const &duplicateViewId = duplicateViewIdQuery.data();

                    CEngineResult<Shared<SFrameGraphTextureView>> const &viewFetch = mResourceData.getMutable<SFrameGraphTextureView>(duplicateViewId);
                    if(not viewFetch.successful())
                    {
                        return { viewFetch.result() };
                    }

                    resource = *(viewFetch.data());
                }
            }

            if(not duplicateFound)
            {
                auto const &[result, texture] = mResourceData.getMutable<SFrameGraphTexture>(subjacentResourceId);
                if(CheckEngineError(result))
                {
                    return { result };
                }

                SFrameGraphTextureView &view = mResourceData.spawnResource<SFrameGraphTextureView>();
                view.arraySliceRange    = adjustedArraySliceRange;
                view.mipSliceRange      = adjustedMipSliceRange;
                view.format             = aRequiredFormat;
                view.source             = source;
                view.assignedPassUID    = mPassUID;
                view.parentResource     = aSubjacentTargetResource.resourceId;
                view.subjacentResource  = subjacentResourceId;
                view.readableName       = CString::format("TextureView ID %0 - %1 #%2", view.resourceId,
                                                                                        ( mode == EFrameGraphViewAccessMode::Write ? "Write" : "Read" ),
                                                                                        aSubjacentTargetResource.resourceId);
                view.type               = EFrameGraphResourceType::TextureView;
                view.mode.set(mode);

                switch(view.source)
                {
                case EFrameGraphViewPurpose::InputAttachment:
                    texture->requestedUsage.set(EFrameGraphResourceUsage::InputAttachment);
                    break;
                case EFrameGraphViewPurpose::ColorAttachment:
                    texture->requestedUsage.set(EFrameGraphResourceUsage::ColorAttachment);
                    break;
                case EFrameGraphViewPurpose::DepthAttachment:
                    texture->requestedUsage.set(EFrameGraphResourceUsage::DepthAttachment);
                    break;
                case EFrameGraphViewPurpose::ShaderInput:
                    texture->requestedUsage.set(EFrameGraphResourceUsage::SampledImage);
                    break;
                default:
                    break;
                }

                ++(texture->referenceCount);

                resource = view;
            }

            if(resource.has_value())
            {
                SFrameGraphResource &ref = resource->get();

                accessor->registerResource(ref.resourceId);
                ++(ref.referenceCount);

                return { EEngineStatus::Ok, *resource };
            }

            return { aFailCode, *resource };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SFrameGraphResource> CPassBuilder::writeAttachment(
                SFrameGraphResource          const &aSubjacentTargetResource,
                SFrameGraphWriteTextureFlags const &aFlags)
        {
            bool const isColorAttachment = (EFrameGraphWriteTarget::Color == aFlags.writeTarget);
            bool const isDepthAttachment = (EFrameGraphWriteTarget::Depth == aFlags.writeTarget);

            EFrameGraphViewPurpose source = isColorAttachment
                                                ? EFrameGraphViewPurpose::ColorAttachment
                                                : isDepthAttachment
                                                      ? EFrameGraphViewPurpose::DepthAttachment
                                                      : EFrameGraphViewPurpose::Undefined;

            auto const &[result, data] = useTexture(aSubjacentTargetResource,
                                                    source,
                                                    aFlags.requiredFormat,
                                                    aFlags.arraySliceRange,
                                                    aFlags.mipSliceRange,
                                                    EFrameGraphViewAccessMode::Write,
                                                    EEngineStatus::FrameGraph_PassBuilder_WriteResourceFailed);

            if(not CheckEngineError(result))
            {
                if(isColorAttachment)
                {
                    mAttachmentCollection.addColorAttachment(mPassUID, data.resourceId);
                }
                else if(isDepthAttachment)
                {
                    mAttachmentCollection.addDepthAttachment(mPassUID, data.resourceId);
                }
            }

            return { result, data };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SFrameGraphResource> CPassBuilder::readAttachment(
                SFrameGraphResource         const &aSubjacentTargetResource,
                SFrameGraphReadTextureFlags const &aFlags)
        {                        
            EFrameGraphViewPurpose source = EFrameGraphViewPurpose::InputAttachment;

            auto const &[result, data]  = useTexture(aSubjacentTargetResource,
                                                     source,
                                                     aFlags.requiredFormat,
                                                     aFlags.arraySliceRange,
                                                     aFlags.mipSliceRange,
                                                     EFrameGraphViewAccessMode::Read,
                                                     EEngineStatus::FrameGraph_PassBuilder_ReadResourceFailed);

            if(not CheckEngineError(result))
            {
                mAttachmentCollection.addInputAttachment(mPassUID, data.resourceId);
            }

            return { result, data };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SFrameGraphResource> CPassBuilder::readTexture(
                SFrameGraphResource             const &aSubjacentTargetResource,
                SFrameGraphTextureResourceFlags const &aFlags)
        {
            EFrameGraphViewPurpose source = EFrameGraphViewPurpose::ShaderInput;

            return useTexture(aSubjacentTargetResource,
                              source,
                              aFlags.requiredFormat,
                              aFlags.arraySliceRange,
                              aFlags.mipSliceRange,
                              EFrameGraphViewAccessMode::Read,
                              EEngineStatus::FrameGraph_PassBuilder_ReadResourceFailed);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SFrameGraphMaterial> CPassBuilder::useMaterial(asset::AssetId_t const &aMaterialId)
        {
            SFrameGraphMaterial &materialResource = mResourceData.spawnResource<SFrameGraphMaterial>();
            materialResource.readableName       = CString::format("Material %0", aMaterialId);
            materialResource.type               = EFrameGraphResourceType::Material;
            materialResource.assignedPassUID    = mPassUID;
            materialResource.materialAssetId    = aMaterialId;
            materialResource.isExternalResource = false;
            materialResource.parentResource     = 0;
            materialResource.referenceCount     = 0;
            materialResource.subjacentResource  = 0;

            return { EEngineStatus::Ok, materialResource };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SFrameGraphMesh> CPassBuilder::useMesh(asset::AssetId_t const &aMeshId)
        {
            SFrameGraphMesh &meshResource = mResourceData.spawnResource<SFrameGraphMesh>();
            meshResource.readableName       = CString::format("Mesh %0", aMeshId);
            meshResource.type               = EFrameGraphResourceType::Mesh;
            meshResource.assignedPassUID    = mPassUID;
            meshResource.meshAssetId        = aMeshId;
            meshResource.isExternalResource = false;
            meshResource.parentResource     = 0;
            meshResource.referenceCount     = 0;
            meshResource.subjacentResource  = 0;

            return { EEngineStatus::Ok, meshResource };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<bool> CPassBuilder::isTextureBeingReadInSubresourceRange(
                RefIndex_t           const &aResourceViews,
                CFrameGraphResources const &aResources,
                SFrameGraphResource  const &aSourceResource,
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
                CEngineResult<Shared<SFrameGraphTextureView> const> const &viewFetch = aResources.get<SFrameGraphTextureView>(viewRef);
                if(not viewFetch.successful())
                {
                    return { viewFetch.result(), false };
                }

                SFrameGraphTextureView const &view = *(viewFetch.data());

                bool correctMode = view.mode.check(EFrameGraphViewAccessMode::Read);
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

                bool const arraySliceOverlap = view.arraySliceRange.overlapsWith(aArraySliceRange);
                if(not arraySliceOverlap)
                {
                    continue;
                }

                bool const mipSliceOverlap = view.mipSliceRange.overlapsWith(aMipSliceRange);
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
        CEngineResult<bool> CPassBuilder::isTextureBeingWrittenInSubresourceRange(
                RefIndex_t           const &aResourceViews,
                CFrameGraphResources const &aResources,
                SFrameGraphResource  const &aSourceResource,
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
                CEngineResult<Shared<SFrameGraphTextureView> const> const &viewFetch = aResources.get<SFrameGraphTextureView>(viewRef);
                if(not viewFetch.successful())
                {

                }

                SFrameGraphTextureView const &view = *(viewFetch.data());

                bool correctMode = view.mode.check(EFrameGraphViewAccessMode::Write);
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

                bool const arraySliceOverlap = view.arraySliceRange.overlapsWith(aArraySliceRange);
                if(not arraySliceOverlap)
                {
                    continue;
                }
                else
                {
                    bool const mipSliceOverlap = view.mipSliceRange.overlapsWith(aMipSliceRange);
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
