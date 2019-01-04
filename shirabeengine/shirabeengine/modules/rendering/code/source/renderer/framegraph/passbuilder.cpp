#include <core/string.h>
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
                CStdSharedPtr_t<CPassBase>         aPass,
                CFrameGraphMutableResources       &aOutResourceData)
            : mPassUID(aPassUID)
            , mPass(aPass)
            , mResourceData(aOutResourceData)
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
            resource.parentResource      = 0;
            resource.subjacentResource   = resource.resourceId; // This is a trick to keep algorithms consistent while supporting r/w from textures and t-views.
            resource.readableName        = aName;
            resource.type                = EFrameGraphResourceType::Texture;

            CStdUniquePtr_t<CPassBase::CMutableAccessor> accessor = mPass->getMutableAccessor(CPassKey<CPassBuilder>());
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
            resource.parentResource      = 0;
            resource.subjacentResource   = resource.resourceId; // This is a trick to keep algorithms consistent while supporting r/w from textures and t-views.
            resource.readableName        = aName;
            resource.type                = EFrameGraphResourceType::Texture;
            resource.isExternalResource  = true;

            CStdUniquePtr_t<CPassBase::CMutableAccessor> accessor = mPass->getMutableAccessor(CPassKey<CPassBuilder>());
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
                EFrameGraphViewSource                const &aViewSource,
                CRange                               const &aArrayRange,
                CRange                               const &aMipRange,
                CBitField<EFrameGraphViewAccessMode> const &aMode)
        {
            for(FrameGraphResourceId_t const &viewId : mResourceData.textureViews())
            {
                CEngineResult<CStdSharedPtr_t<SFrameGraphTextureView> const> viewFetch = mResourceData.get<SFrameGraphTextureView>(viewId);
                if(not viewFetch.successful())
                {
                    continue;
                }

                SFrameGraphTextureView const &compareView = *(viewFetch.data());
                bool const equal =
                        (compareView.arraySliceRange == aArrayRange)
                        and (compareView.mipSliceRange == aMipRange)
                        and ((aFormat == FrameGraphFormat_t::Automatic) or (compareView.format == aFormat))
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
            if(aSourceResource.type == EFrameGraphResourceType::TextureView)
            {
                CEngineResult<CStdSharedPtr_t<SFrameGraphTexture> const> subjacentFetch = aResourceData.get<SFrameGraphTexture>(aSourceResource.subjacentResource);
#if defined SHIRABE_DEBUG || defined SHIRABE_TEST
                if(not subjacentFetch.successful())
                {
                    CLog::Error(logTag(), CString::format("Subjacent resource handle w/ id %0 is empty."));
                    return { EEngineStatus::Error };
                }
#endif

                CEngineResult<CStdSharedPtr_t<SFrameGraphTextureView> const> parentFetch = aResourceData.get<SFrameGraphTextureView>(aSourceResource.resourceId);
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
                CEngineResult<CStdSharedPtr_t<SFrameGraphTexture> const> subjacentFetch = aResourceData.get<SFrameGraphTexture>(aSourceResource.resourceId);
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
                SFrameGraphResource      const &aSubjacentTargetResource,
                SFrameGraphResourceFlags const &aFlags,
                CRange                   const &aArraySliceRange,
                CRange                   const &aMipSliceRange)
        {
            CStdUniquePtr_t<CPassBase::CMutableAccessor> accessor = mPass->getMutableAccessor(CPassKey<CPassBuilder>());

            OptionalRef_t<SFrameGraphResource> resource = {};

            FrameGraphResourceId_t const subjacentResourceId =
                    (aSubjacentTargetResource.type == EFrameGraphResourceType::Texture)
                     ? aSubjacentTargetResource.resourceId
                     : aSubjacentTargetResource.subjacentResource;

            EFrameGraphViewAccessMode const mode   = EFrameGraphViewAccessMode::Forward;
            EFrameGraphViewSource     const source = EFrameGraphViewSource::Undefined;

            CRange adjustedArraySliceRange = aArraySliceRange;
            CRange adjustedMipSliceRange   = aMipSliceRange;

            adjustArrayAndMipSliceRanges(
                        mResourceData,
                        aSubjacentTargetResource,
                        aArraySliceRange,
                        aMipSliceRange,
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

                    CEngineResult<CStdSharedPtr_t<SFrameGraphTextureView>> const &viewFetch = mResourceData.getMutable<SFrameGraphTextureView>(duplicateViewId);
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

                CEngineResult<CStdSharedPtr_t<SFrameGraphResource>> const &subjacentFetch = mResourceData.getMutable<SFrameGraphResource>(subjacentResourceId);
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
            CStdUniquePtr_t<CPassBase::CMutableAccessor> accessor = mPass->getMutableAccessor(CPassKey<CPassBuilder>());

            FrameGraphResourceId_t const subjacentResourceId =
                    (aSubjacentTargetResource.type == EFrameGraphResourceType::Texture)
                     ? aSubjacentTargetResource.resourceId
                     : aSubjacentTargetResource.subjacentResource;

            EFrameGraphViewAccessMode const mode   = EFrameGraphViewAccessMode::Accept;
            EFrameGraphViewSource     const source = EFrameGraphViewSource::Undefined;

            // TODO: Naming correct? SHouldn't it be parent resource?
            CEngineResult<CStdSharedPtr_t<SFrameGraphTextureView> const> const &subjacentTargetFetch = mResourceData.get<SFrameGraphTextureView>(aSubjacentTargetResource.resourceId);
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

                    CEngineResult<CStdSharedPtr_t<SFrameGraphTextureView>> const &viewFetch = mResourceData.getMutable<SFrameGraphTextureView>(duplicateViewId);
                    if(not viewFetch.successful())
                    {
                        return { viewFetch.result() };
                    }

                    resource = *(viewFetch.data());
                }
            }

            if(!duplicateFound)
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

                CEngineResult<CStdSharedPtr_t<SFrameGraphResource>> const &subjacentFetch = mResourceData.getMutable<SFrameGraphResource>(subjacentResourceId);
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
        SFrameGraphResource CPassBuilder::writeTexture(
                SFrameGraphResource          const &aSubjacentTargetResource,
                SFrameGraphWriteTextureFlags const &aFlags,
                CRange                       const &aArraySliceRange,
                CRange                       const &aMipSliceRange)
        {
            FrameGraphResourceId_t const subjacentResourceId =
                    (aSubjacentTargetResource.type == EFrameGraphResourceType::Texture)
                     ? aSubjacentTargetResource.resourceId
                     : aSubjacentTargetResource.subjacentResource;

            EFrameGraphViewAccessMode const mode = EFrameGraphViewAccessMode::Write;

            EFrameGraphViewSource source = EFrameGraphViewSource::Undefined;
            if(aFlags.writeTarget == EFrameGraphWriteTarget::Color)
            {
                source = EFrameGraphViewSource::Color;
            }
            else if(aFlags.writeTarget == EFrameGraphWriteTarget::Depth)
            {
                source = EFrameGraphViewSource::Depth;
            }

            CStdUniquePtr_t<CPassBase::CMutableAccessor> accessor = mPass->getMutableAccessor(CPassKey<CPassBuilder>());

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

            Optional_t<RefWrapper_t<SFrameGraphTextureView>> ref{};

            // Can we cull?
            bool duplicateFound = false;
            if(aSubjacentTargetResource.type == EFrameGraphResourceType::TextureView)
            {
                FrameGraphResourceId_t const duplicateViewId =
                        findDuplicateTextureView(
                            subjacentResourceId,
                            aFlags.requiredFormat,
                            source,
                            adjustedArraySliceRange,
                            adjustedMipSliceRange,
                            mode);

                duplicateFound = (duplicateViewId > 0);
                if(duplicateFound)
                {
                    SFrameGraphTextureView &view = *mResourceData.getMutable<SFrameGraphTextureView>(duplicateViewId);

                    ref = view;
                }
            }

            if(!duplicateFound)
            {
                SFrameGraphTextureView &view = mResourceData.spawnResource<SFrameGraphTextureView>();
                view.arraySliceRange    = adjustedArraySliceRange;
                view.mipSliceRange      = adjustedMipSliceRange;
                view.format             = aFlags.requiredFormat;
                view.source             = source;
                view.assignedPassUID    = mPassUID;
                view.parentResource     = aSubjacentTargetResource.resourceId;
                view.subjacentResource  = subjacentResourceId;
                view.readableName       = CString::format("TextureView ID %0 - Write #%1", view.resourceId, aSubjacentTargetResource.resourceId);
                view.type               = EFrameGraphResourceType::TextureView;
                view.mode.set(mode);

                CStdSharedPtr_t<SFrameGraphResource> subjacent = mResourceData.getMutable<SFrameGraphResource>(subjacentResourceId);
                ++subjacent->referenceCount;

                ref = view;
            }

            accessor->registerResource(ref->get().resourceId);
            ++(ref->get().referenceCount);

            return *ref;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        SFrameGraphResource CPassBuilder::readTexture(
                SFrameGraphResource         const &aSubjacentTargetResource,
                SFrameGraphReadTextureFlags const &aFlags,
                CRange                      const &aArraySliceRange,
                CRange                      const &aMipSliceRange)
        {
            FrameGraphResourceId_t const subjacentResourceId =
                    (aSubjacentTargetResource.type == EFrameGraphResourceType::Texture)
                     ? aSubjacentTargetResource.resourceId
                     : aSubjacentTargetResource.subjacentResource;

            EFrameGraphViewAccessMode const mode = EFrameGraphViewAccessMode::Read;

            EFrameGraphViewSource source = EFrameGraphViewSource::Undefined;
            if(aFlags.source == EFrameGraphReadSource::Color)
            {
                source = EFrameGraphViewSource::Color;
            }
            else if(aFlags.source == EFrameGraphReadSource::Depth)
            {
                source = EFrameGraphViewSource::Depth;
            }

            CStdUniquePtr_t<CPassBase::CMutableAccessor> accessor = mPass->getMutableAccessor(CPassKey<CPassBuilder>());

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
                        false,
                        true);

            Optional_t<RefWrapper_t<SFrameGraphTextureView>> ref{};

            // Can we cull?
            bool duplicateFound = false;
            if(aSubjacentTargetResource.type == EFrameGraphResourceType::TextureView)
            {
                FrameGraphResourceId_t const duplicateViewId =
                        findDuplicateTextureView(
                            subjacentResourceId,
                            aFlags.requiredFormat,
                            source,
                            adjustedArraySliceRange,
                            adjustedMipSliceRange,
                            mode);

                duplicateFound = (duplicateViewId > 0);
                if(duplicateFound)
                {
                    SFrameGraphTextureView &view = *mResourceData.getMutable<SFrameGraphTextureView>(duplicateViewId);

                    ref = view;
                }
            }

            if(!duplicateFound)
            {
                SFrameGraphTextureView &view = mResourceData.spawnResource<SFrameGraphTextureView>();
                view.arraySliceRange    = adjustedArraySliceRange;
                view.mipSliceRange      = adjustedMipSliceRange;
                view.format             = aFlags.requiredFormat;
                view.source             = source;
                view.assignedPassUID    = mPassUID;
                view.parentResource     = aSubjacentTargetResource.resourceId;
                view.subjacentResource  = subjacentResourceId;
                view.readableName       = CString::format("TextureView ID %0 - Read #%1", view.resourceId, aSubjacentTargetResource.resourceId);
                view.type               = EFrameGraphResourceType::TextureView;
                view.mode.set(mode);

                CStdSharedPtr_t<SFrameGraphResource> subjacent = mResourceData.getMutable<SFrameGraphResource>(subjacentResourceId);
                ++subjacent->referenceCount;

                ref = view;
            }

            accessor->registerResource(ref->get().resourceId);
            ++(ref->get().referenceCount);

            return *ref;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        SFrameGraphResource CPassBuilder::importRenderables(
                std::string         const &aCollectionName,
                SFrameGraphResource const &aRenderableListResource)
        {
            SFrameGraphRenderableListView &resource = mResourceData.spawnResource<SFrameGraphRenderableListView>();
            resource.assignedPassUID                = mPassUID;
            resource.readableName                   = aCollectionName;
            resource.parentResource                 = aRenderableListResource.resourceId;
            resource.subjacentResource              = aRenderableListResource.resourceId;
            resource.type                           = EFrameGraphResourceType::RenderableListView;
            resource.isExternalResource             = false;

            CStdSharedPtr_t<SFrameGraphRenderableList> const listRef =
                    mResourceData.get<SFrameGraphRenderableList>(aRenderableListResource.resourceId);

#if defined SHIRABE_DEBUG || defined SHIRABE_TEST
            if(!listRef)
                throw std::runtime_error(CString::format("Renderable list resource handle w/ id %0 is empty.", aRenderableListResource.resourceId));
#endif

            SFrameGraphRenderableList const &list = *listRef;

            for(uint64_t k=0; k<list.renderableList.size(); ++k)
            {
                resource.renderableRefIndices.push_back(k);
            }

            CStdUniquePtr_t<CPassBase::CMutableAccessor> accessor = mPass->getMutableAccessor(CPassKey<CPassBuilder>());
            accessor->registerResource(resource.resourceId);

            return resource;
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
                CEngineResult<CStdSharedPtr_t<SFrameGraphTextureView> const> const &viewFetch = aResources.get<SFrameGraphTextureView>(viewRef);
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
        bool CPassBuilder::isTextureBeingWrittenInSubresourceRange(
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
            for(RefIndex_t::value_type const&viewRef : aResourceViews)
            {
                SFrameGraphTextureView const&view = *aResources.get<SFrameGraphTextureView>(viewRef);

                bool correctMode = view.mode.check(EFrameGraphViewAccessMode::Write);
                if(!correctMode)
                    continue;

                bool const commonPass      = aSourceResource.assignedPassUID   == view.assignedPassUID;
                bool const commonId        = aSourceResource.resourceId        == view.resourceId;
                bool const commonSubjacent = aSourceResource.subjacentResource == view.subjacentResource;
                if(!(commonPass && !commonId && commonSubjacent))
                    continue;

                bool const arraySliceOverlap = view.arraySliceRange.overlapsWith(aArraySliceRange);
                if(!arraySliceOverlap)
                {
                    continue;
                }
                else
                {
                    bool const mipSliceOverlap = view.mipSliceRange.overlapsWith(aMipSliceRange);
                    if(!mipSliceOverlap)
                    {
                        continue;
                    }
                }

                return true;
            }

            return false;
        }
        //<-----------------------------------------------------------------------------
    }
}
