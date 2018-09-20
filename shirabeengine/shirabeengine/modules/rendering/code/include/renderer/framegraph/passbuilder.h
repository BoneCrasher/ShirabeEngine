#ifndef __SHIRABE_RENDERER_PASSBUILDER_H__
#define __SHIRABE_RENDERER_PASSBUILDER_H__

#include <string>
#include <functional>

#include <log/log.h>
#include <core/basictypes.h>
#include <core/enginetypehelper.h>
#include <core/random.h>
#include <core/uid.h>
#include <resources/core/resourcedomaintransfer.h>
#include "renderer/framegraph/framegraphdata.h"

namespace engine
{
    namespace framegraph
    {
        using engine::core::IUIDGenerator;
        using namespace engine::resources;

        class CPassBase;

        /**
         * The EPassResourceConstraintFlags enum describes various parameters which can be constrained.
         */
        enum EPassResourceConstraintFlags
        {
            None          = 0,
            TextureWidth  = 1,
            TextureHeight = 2,
            TextureSize   = 3,
        };

        /**
         * The SPassResourceConstraint struct describes a constraint on aTarget based on aSource
         * defined by a flags.
         */
        struct SPassResourceConstraint
        {
        public_constructors:
            SHIRABE_INLINE SPassResourceConstraint(
                    SFrameGraphResource          const &aTarget,
                    SFrameGraphResource          const &aSource,
                    EPassResourceConstraintFlags const &aFlags)
                : target(aTarget)
                , source(aSource)
                , flags(aFlags)
            {}

        public_members:
            SFrameGraphResource          target;
            SFrameGraphResource          source;
            EPassResourceConstraintFlags flags;
        };

        /**
         * The PassBuilder class is the setup interface for all passes and collects all public resource and state requests
         * of a pass, performing some validation to make sure that passes do not request invalid behaviour.
         */
        class CPassBuilder
        {
            SHIRABE_DECLARE_LOG_TAG(CPassBuilder);

        public_constructors:
            /**
             * Construct a pass builder.
             *
             * @param aPassUID         The pass' UID.
             * @param aPass            The pass instance to build up.
             * @param aOutResourceData Container for the pass' resources requested/used.
             */
            CPassBuilder(
                    PassUID_t                   const &aPassUID,
                    CStdSharedPtr_t<CPassBase>         aPass,
                    CFrameGraphMutableResources       &aOutResourceData);

        public_methods:
            /**
             * Return the assigned pass UID.
             *
             * @return See brief.
             */
            SHIRABE_INLINE PassUID_t const &assignedPassUID() const
            {
                return mPassUID;
            }

            /**
             * Request the creation of a texture resource in the framegraph.
             *
             * @param aName       Name of the texture to create.
             * @param aDescriptor Descriptor of the texture to create.
             * @return            Return a framegraph resource handle for the texture
             *                    creation.
             */
            SFrameGraphResource createTexture(
                    std::string        const &aName,
                    SFrameGraphTexture const &aDescriptor);

            // createBuffer
            // create~?

            /**
             * Request the import of a texture resource in the framegraph.
             *
             * @param aName       Name of the texture to import.
             * @param aDescriptor Descriptor of the texture to import.
             * @return            Return a framegraph resource handle for the texture
             *                    import.
             */
            SFrameGraphResource importTexture(
                    std::string        const &aName,
                    SFrameGraphTexture const &aDescriptor);

            /**
             * Request a forward-op of a texture.
             *
             * @param aSubjacentTargetResource The resource id of the texture to forward.
             * @param aFlags                   Flags detailing the texture forward operation.
             * @param aArraySliceRange         The array range within the texture to forward.
             * @param aMipSliceRange           The mip-range of within the texture to forward.
             * @return                         Return a framegraph resource handle for the texture
             *                                 forwarding.
             */
            SFrameGraphResource forwardTexture(
                    SFrameGraphResource      const &aSubjacentTargetResource,
                    SFrameGraphResourceFlags const &aFlags,
                    CRange                   const &aArraySliceRange = CRange(0, 1),
                    CRange                   const &aMipSliceRange   = CRange(0, 1));

            /**
             * Request a write operation on a subjacent texture instance.
             *
             * @param aSubjacentTargetResource The resource id of the texture to write.
             * @param aFlags                   Flags detailing the texture write operation.
             * @param aArraySliceRange         The array range within the texture to write to.
             * @param aMipSliceRange           The mip-range of within the texture to write to.
             * @return                         Return a framegraph resource handle for the texture
             *                                 creation.
             */
            SFrameGraphResource writeTexture(
                    SFrameGraphResource          const &aSubjacentTargetResource,
                    SFrameGraphWriteTextureFlags const &aFlags,
                    CRange                       const &aArraySliceRange = CRange(0, 1),
                    CRange                       const &aMipSliceRange   = CRange(0, 1));

            /**
             * Request a read operation on a subjacent texture instance.
             *
             * @param aSubjacentTargetResource The resource id of the texture to read.
             * @param aFlags                   Flags detailing the texture read operation.
             * @param aArraySliceRange         The array range within the texture to read to.
             * @param aMipSliceRange           The mip-range of within the texture to read to.
             * @return                         Return a framegraph resource handle for the texture
             *                                 creation.
             */
            SFrameGraphResource readTexture(
                    SFrameGraphResource         const &aSubjacentTargetResource,
                    SFrameGraphReadTextureFlags const &aFlags,
                    CRange                      const &aArraySliceRange = CRange(0, 1),
                    CRange                      const &aMipSliceRange   = CRange(0, 1));

            /**
             * Import a list of renderables for use in this pass, creating a
             * renderable list resource view for the specified range.
             *
             * @param aCollectionName         Name of the renderable collection to import.
             * @param aRenderableListResource Source list of renderables to base on.
             * @return                        Return a frame graph resource handle for the
             *                                list view created.
             */
            SFrameGraphResource importRenderables(
                    std::string         const &aCollectionName,
                    SFrameGraphResource const &aRenderableListResource);

        private_methods:
            /**
             * Search through all texture views of a subjacent texture and check
             * whether a duplicate texture view already exists.
             *
             * @param aSubjacentResourceId The subjacent texture's resource id.
             * @param aFormat              The format of the resource view.
             * @param aViewSource          The view source of the resource view.
             * @param aArrayRange          The array range of the resource view.
             * @param aMipRange            The mip range of the resource view.
             * @param aMode                The access mode flags of the resource view.
             * @return                     Returns the resource UID of a possibly duplicate
             *                             view or 0, if no duplication was found.
             */
            FrameGraphResourceId_t findDuplicateTextureView(
                    FrameGraphResourceId_t               const &aSubjacentResourceId,
                    FrameGraphFormat_t                   const &aFormat,
                    EFrameGraphViewSource                const &aViewSource,
                    CRange                               const &aArrayRange,
                    CRange                               const &aMipRange,
                    CBitField<EFrameGraphViewAccessMode> const &aMode);

            /**
             * Adjust the array and mipslice ranges on cascading resource read/writes, which all base on the zero-based
             * index of the previous resource's r/w ranges.
             *
             * E.g.:
             *
             * 1. Texture (Array-Size: 1, Mip-Count: 2)
             * 2. Texture Read -> View (Array: Off 0 Len 1, Mip: Off 1 Len 1)
             * 3. Texture Write based on 2. -> View (Array: Off 0 Len 1, Mip: Off 0 Len 1)
             *
             * Adjustments:
             *
             * 2. Is valid.
             * 3. Is adjusted to: Array: Off 0 Len 1, Mip: Off 1 Len 1)
             *
             * @param aResourceData            The framegraph resources array in which to search for parent
             *                                 and subjacent resources.
             * @param aSourceResource          The resource to adjust.
             * @param aArraySliceRange         The resource array range to adjust.
             * @param aMipSliceRange           The resource mip range to adjust.
             * @param aAdjustedArraySliceRange Output handle for the corrected array range.
             * @param aAdjustedMipSliceRange   Output handle for the corrected mip range.
             */
            void adjustArrayAndMipSliceRanges(
                    CFrameGraphResources const &aResourceData,
                    SFrameGraphResource  const &aSourceResource,
                    CRange               const &aArraySliceRange,
                    CRange               const &aMipSliceRange,
                    CRange                     &aAdjustedArraySliceRange,
                    CRange                     &aAdjustedMipSliceRange);

            /**
             * Validate array and mip slice ranges for read and/or write to be in valid bounds with
             * respect to the subjacent texture resource.
             *
             * @param aResourceData    The framegraph resources array in which to search for parent
             *                         and subjacent resources.
             * @param aSourceResource  The resource to validate.
             * @param aArraySliceRange The resource array range to validate.
             * @param aMipSliceRange   The resource mip range to validate.
             * @param aValidateReads   Flag indicating, whether read operations should be validated.
             * @param aValidateWrites  Flag indicating, wheter write operations should be validated.
             */
            void validateArrayAndMipSliceRanges(
                    CFrameGraphResources const &aResourceData,
                    SFrameGraphResource  const &aSourceResource,
                    CRange               const &aArraySliceRange,
                    CRange               const &aMipSliceRange,
                    bool                        aValidateReads  = true,
                    bool                        aValidateWrites = true);

            /**
             * Check, whether a specific resource view is already being read by other
             * resource views of the pass to be set-up.
             *
             * @param aResourceViews   The current list of resource view ids attached to the pass.
             * @param aResourceData    The framegraph resources array in which to search for parent
             *                         and subjacent resources.
             * @param aSourceResource  The resource to validate.
             * @param aArraySliceRange The resource array range to validate.
             * @param aMipSliceRange   The resource mip range to validate.
             * @return                 True, if no simultaneous overlapping reads are performed.
             */
            bool isTextureBeingReadInSubresourceRange(
                    RefIndex_t           const &aResourceViews,
                    CFrameGraphResources const &aResources,
                    SFrameGraphResource  const &aSourceResource,
                    CRange               const &aArraySliceRange,
                    CRange               const &aMipSliceRange);

            /**
             * Check, whether a specific resource view is already being written by other
             * resource views of the pass to be set-up.
             *
             * @param aResourceViews   The current list of resource view ids attached to the pass.
             * @param aResourceData    The framegraph resources array in which to search for parent
             *                         and subjacent resources.
             * @param aSourceResource  The resource to validate.
             * @param aArraySliceRange The resource array range to validate.
             * @param aMipSliceRange   The resource mip range to validate.
             * @return                 True, if no simultaneous overlapping writes are performed.
             */
            bool isTextureBeingWrittenInSubresourceRange(
                    RefIndex_t           const &aResourceViews,
                    CFrameGraphResources const &aResources,
                    SFrameGraphResource  const &aSourceResource,
                    CRange               const &aArraySliceRange,
                    CRange               const &aMipSliceRange);

        private_members:
            PassUID_t                    mPassUID;
            CStdSharedPtr_t<CPassBase>   mPass;
            CFrameGraphMutableResources &mResourceData;
        };

    }
}

#endif
