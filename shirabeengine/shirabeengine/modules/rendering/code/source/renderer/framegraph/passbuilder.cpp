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
        SFrameGraphResource CPassBuilder::createTexture(
                std::string        const &aName,
                SFrameGraphTexture const &aDescriptor)
        {
            // if(!desc.validate())
            //   throw std::exception("Invalid texture descriptor.");

            // Basic abstract descriptor of resources being used.
            SFrameGraphTexture &resource = mResourceData.spawnResource<SFrameGraphTexture>();
            resource.assignedPassUID     = mPassUID;
            resource.parentResource      = 0;
            resource.subjacentResource   = resource.resourceId; // This is a trick to keep algorithms consistent while supporting r/w from textures and t-views.
            resource.readableName        = aName;
            resource.type                = EFrameGraphResourceType::Texture;
            resource.assignTextureParameters(aDescriptor);

            CStdUniquePtr_t<CPassBase::CMutableAccessor> accessor = mPass->getMutableAccessor(CPassKey<CPassBuilder>());
            accessor->registerResource(resource.resourceId);

            return resource;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        FrameGraphResourceId_t CPassBuilder::findDuplicateTextureView(
                FrameGraphResourceId_t               const &aSubjacentResourceId,
                FrameGraphFormat_t                   const &aFormat,
                EFrameGraphViewSource                const &aViewSource,
                CRange                               const &aArrayRange,
                CRange                               const &aMipRange,
                CBitField<EFrameGraphViewAccessMode> const &aMode)
        {
            for(FrameGraphResourceId_t const &viewId : mResourceData.textureViews())
            {
                SFrameGraphTextureView const &compareView = *mResourceData.get<SFrameGraphTextureView>(viewId);

                bool const equal =
                        (compareView.arraySliceRange == aArrayRange)
                        && (compareView.mipSliceRange == aMipRange)
                        && (aFormat == FrameGraphFormat_t::Automatic
                           || (compareView.format == aFormat))
                        && (compareView.source == aViewSource)
                        && (compareView.mode.check(aMode))
                        && (compareView.subjacentResource == aSubjacentResourceId);

                if(equal)
                    return compareView.resourceId;
            }

            return 0;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void CPassBuilder::adjustArrayAndMipSliceRanges(
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
                CStdSharedPtr_t<SFrameGraphTexture> const subjacentRef = aResourceData.get<SFrameGraphTexture>(aSourceResource.subjacentResource);

#if defined SHIRABE_DEBUG || defined SHIRABE_TEST
                if(!subjacentRef)
                    throw std::runtime_error(CString::format("Subjacent resource handle w/ id %0 is empty.", aSourceResource.subjacentResource));
#endif

                CStdSharedPtr_t<SFrameGraphTextureView> const parentRef = aResourceData.get<SFrameGraphTextureView>(aSourceResource.resourceId);

#if defined SHIRABE_DEBUG || defined SHIRABE_TEST
                if(!parentRef)
                    throw std::runtime_error(CString::format("Parent resource handle w/ id %0 is empty.", aSourceResource.resourceId));
#endif

                SFrameGraphTexture     const &subjacent = *subjacentRef;
                SFrameGraphTextureView const &parent    = *parentRef;

                aAdjustedArraySliceRange.offset = (parent.arraySliceRange.offset + aArraySliceRange.offset);
                aAdjustedMipSliceRange.offset   = (parent.mipSliceRange.offset   + aMipSliceRange.offset);

#if defined SHIRABE_DEBUG || defined SHIRABE_TEST
                bool const arraySliceRangeOutOfBounds =
                        (aAdjustedArraySliceRange.length > parent.arraySliceRange.length)
                        || ((aAdjustedArraySliceRange.offset + aAdjustedArraySliceRange.length) > subjacent.arraySize);

                bool const mipSliceRangeOutOfBounds =
                        (aAdjustedMipSliceRange.length   > parent.mipSliceRange.length)
                        || ((aAdjustedMipSliceRange.offset   + aAdjustedMipSliceRange.length) > subjacent.mipLevels);

                if(arraySliceRangeOutOfBounds || mipSliceRangeOutOfBounds)
                    throw std::runtime_error(CString::format("Derived subresource range out of bounds (Array:%0, Mip:%1).", arraySliceRangeOutOfBounds, mipSliceRangeOutOfBounds));
#endif

            }
            else // Texture
            {

#if defined SHIRABE_DEBUG || defined SHIRABE_TEST
                CStdSharedPtr_t<SFrameGraphTexture> const subjacentRef = aResourceData.get<SFrameGraphTexture>(aSourceResource.resourceId);
                if(!subjacentRef)
                    throw std::runtime_error(CString::format("Subjacent resource handle w/ id %0 is empty.", aSourceResource.subjacentResource));

                SFrameGraphTexture const &subjacent = *subjacentRef;

                bool const arraySliceRangeOutOfBounds = ((aAdjustedArraySliceRange.offset + aAdjustedArraySliceRange.length) > subjacent.arraySize);
                bool const mipSliceRangeOutOfBounds   = ((aAdjustedMipSliceRange.offset   + aAdjustedMipSliceRange.length)   > subjacent.mipLevels);

                if(arraySliceRangeOutOfBounds || mipSliceRangeOutOfBounds)
                    throw std::runtime_error(CString::format("Subresource range out of bounds (Array:%0, Mip:%1).", arraySliceRangeOutOfBounds, mipSliceRangeOutOfBounds));
#endif

            }
        }//<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void CPassBuilder::validateArrayAndMipSliceRanges(
                CFrameGraphResources const &aResourceData,
                SFrameGraphResource  const &aSourceResource,
                CRange               const &aArraySliceRange,
                CRange               const &aMipSliceRange,
                bool                        aValidateReads,
                bool                        aValidateWrites)
        {
#if defined SHIRABE_DEBUG || defined SHIRABE_TEST
            bool const isBeingRead =
                    isTextureBeingReadInSubresourceRange(
                        aResourceData.textureViews(),
                        aResourceData,
                        aSourceResource,
                        aArraySliceRange,
                        aMipSliceRange);

            if(isBeingRead)
                throw std::runtime_error(
                        CString::format(
                            "Resource is already being read or written at the specified ranges (Array: %0[%1]; Mip: %2[%3])",
                            aArraySliceRange.offset,
                            aArraySliceRange.length,
                            aMipSliceRange.offset,
                            aMipSliceRange.length).c_str());

            bool const isBeingWritten =
                    isTextureBeingWrittenInSubresourceRange(
                        aResourceData.textureViews(),
                        aResourceData, aSourceResource,
                        aArraySliceRange,
                        aMipSliceRange);

            if(isBeingWritten)
                throw std::runtime_error(
                        CString::format(
                            "Resource is already being read or written at the specified ranges (Array: %0[%1]; Mip: %2[%3])",
                            aArraySliceRange.offset,
                            aArraySliceRange.length,
                            aMipSliceRange.offset,
                            aMipSliceRange.length).c_str());
#endif
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
        bool CPassBuilder::isTextureBeingReadInSubresourceRange(
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
                bool correctMode = view.mode.check(EFrameGraphViewAccessMode::Read);
                if(!correctMode)
                    continue;

                bool const commonPass      = aSourceResource.assignedPassUID   == view.assignedPassUID;
                bool const commonId        = aSourceResource.resourceId        == view.resourceId;
                bool const commonSubjacent = aSourceResource.subjacentResource == view.subjacentResource;
                if(!(commonPass && commonId && commonSubjacent))
                    continue;

                bool const arraySliceOverlap = view.arraySliceRange.overlapsWith(aArraySliceRange);
                if(!arraySliceOverlap)
                    continue;

                bool const mipSliceOverlap = view.mipSliceRange.overlapsWith(aMipSliceRange);
                if(!mipSliceOverlap)
                    continue;

                return true;
            }

            return false;
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
                    continue;

                bool const mipSliceOverlap = view.mipSliceRange.overlapsWith(aMipSliceRange);
                if(!mipSliceOverlap)
                    continue;

                return true;
            }

            return false;
        }
        //<-----------------------------------------------------------------------------
    }
}
