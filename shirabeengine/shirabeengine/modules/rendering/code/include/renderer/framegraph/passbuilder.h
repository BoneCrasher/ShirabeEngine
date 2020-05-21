#ifndef __SHIRABE_RENDERER_PASSBUILDER_H__
#define __SHIRABE_RENDERER_PASSBUILDER_H__

#include <string>
#include <functional>

#include <log/log.h>
#include <core/basictypes.h>
#include <core/enginetypehelper.h>
#include <core/random.h>
#include <core/uid.h>
#include <graphicsapi/definitions.h>

#include "renderer/vulkan_resources/resources/vulkanresourceoperations.h"
#include "renderer/framegraph/framegraphdata.h"

namespace engine
{
    namespace framegraph
    {
        using engine::core::IUIDGenerator;
        using namespace engine::graphicsapi;

        class CPassBase;
        class CRenderPass;

        #ifdef None
        #undef None
        #endif
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
                    SRenderGraphResource          const &aTarget,
                    SRenderGraphResource          const &aSource,
                    EPassResourceConstraintFlags const &aFlags)
                : target(aTarget)
                , source(aSource)
                , flags(aFlags)
            {}

        public_members:
            SRenderGraphResource          target;
            SRenderGraphResource          source;
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
                    PassUID_t              const &aPassUID,
                    Shared<CPassBase>             aPass,
                    Shared<CRenderPass>           aEnclosingRenderPass,
                    CResourceManager const       &aResourceManager,
                    CRenderGraphMutableResources &aOutResourceData);

        public_methods:
            /**
             * Return the assigned pass UID.
             *
             * @return See brief.
             */
             [[nodiscard]]
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
            CEngineResult<SRenderGraphImage> createImage(
                std::string                         const &aName,
                SRenderGraphDynamicImageDescription const &aDescriptor);

            CEngineResult<SRenderGraphImage> importImage(
                std::string                            const &aName,
                SRenderGraphPersistentImageDescription const &aDescriptor);

            /**
             * Request the creation of a texture resource in the framegraph.
             *
             * @param aName       Name of the texture to create.
             * @param aDescriptor Descriptor of the texture to create.
             * @return            Return a framegraph resource handle for the texture
             *                    creation.
             */
            CEngineResult<SRenderGraphRenderTarget> createRenderTarget(
                std::string                           const &aName,
                SRenderGraphDynamicImageDescription const &aDescriptor);

            /**
             * Request a write operation on a subjacent texture instance.
             *
             * @param aSubjacentTargetResource The resource id of the texture to write.
             * @param aFlags                   Flags detailing the texture write operation.
             * @return                         Return a framegraph resource handle for the texture
             *                                 creation.
             */
            CEngineResult<SRenderGraphImageView> writeAttachment(
                SRenderGraphImage                   &aImage,
                SRenderGraphWriteTextureFlags const &aFlags);

            /**
             * Request a write operation on a subjacent texture instance.
             *
             * @param aSubjacentTargetResource The resource id of the texture to write.
             * @param aFlags                   Flags detailing the texture write operation.
             * @return                         Return a framegraph resource handle for the texture
             *                                 creation.
             */
            CEngineResult<SRenderGraphImageView> writeAttachment(
                SRenderGraphImageView               &aImageView,
                SRenderGraphWriteTextureFlags const &aFlags);

            /**
             * Request a read operation on a subjacent texture instance.
             *
             * @param aSubjacentTargetResource The resource id of the texture to read.
             * @param aFlags                   Flags detailing the texture read operation.
             * @return                         Return a framegraph resource handle for the texture
             *                                 creation.
             */
            CEngineResult<SRenderGraphImageView> readAttachment(
                SRenderGraphImage                  &aImage,
                SRenderGraphReadTextureFlags const &aFlags);

            /**
             * Request a read operation on a subjacent texture instance.
             *
             * @param aSubjacentTargetResource The resource id of the texture to read.
             * @param aFlags                   Flags detailing the texture read operation.
             * @return                         Return a framegraph resource handle for the texture
             *                                 creation.
             */
            CEngineResult<SRenderGraphImageView> readAttachment(
                SRenderGraphImageView              &aImageView,
                SRenderGraphReadTextureFlags const &aFlags);

            /**
             * Request a read operation on a subjacent texture instance.
             *
             * @param aSubjacentTargetResource The resource id of the texture to read.
             * @param aFlags                   Flags detailing the texture read operation.
             * @return                         Return a framegraph resource handle for the texture
             *                                 creation.
             */
            CEngineResult<SRenderGraphImageView> readImage(
                SRenderGraphImage                      &subjacentTargetResource,
                SRenderGraphTextureResourceFlags const &aFlags);

            CEngineResult<SRenderGraphBuffer> createBuffer(
                std::string                          const &aName,
                SRenderGraphDynamicBufferDescription const &aBufferDescription);

            CEngineResult<SRenderGraphBuffer> importBuffer(
                std::string                             const &aName,
                SRenderGraphPersistentBufferDescription const &aBufferDescription);

            CEngineResult<SRenderGraphBufferView> readBuffer(
                SRenderGraphBuffer &subjacentTargetResource,
                CRange const       &aSubrange);

            CEngineResult<SRenderGraphMesh> useMesh(SRenderGraphMeshDescription const &aMeshDescription);

            CEngineResult<SRenderGraphMaterial> useMaterial(resources::ResourceId_t const &aResourceId);
            CEngineResult<SRenderGraphMaterial> useMaterial(SRenderGraphMaterialDescription const &aMaterialDescription);

            CEngineResult<SRenderGraphPipeline> usePipeline(resources::ResourceId_t      const &aSharedMaterialResourceid
                                                            , SRenderGraphPipelineConfig const &aPipelineDescription);

            // Buffers?

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
            CEngineResult<RenderGraphResourceId_t> findDuplicateImageView(
                    RenderGraphResourceId_t               const &aSubjacentResourceId,
                    RenderGraphFormat_t                   const &aFormat,
                    ERenderGraphViewPurpose               const &aViewSource,
                    CRange                                const &aArrayRange,
                    CRange                                const &aMipRange,
                    CBitField<ERenderGraphViewAccessMode> const &aMode);

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
            CEngineResult<> adjustArrayAndMipSliceRanges(
                    CRenderGraphResources const &aResourceData,
                    CRange                const &aSubjacentArraySliceRange,
                    CRange                const &aSubjacentMipSliceRange,
                    CRange                const &aSourceArraySliceRange,
                    CRange                const &aSourceMipSliceRange,
                    CRange                const &aTargetArraySliceRange,
                    CRange                const &aTargetMipSliceRange,
                    CRange                      &aAdjustedArraySliceRange,
                    CRange                      &aAdjustedMipSliceRange);

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
            CEngineResult<> validateArrayAndMipSliceRanges(
                    CRenderGraphResources const &aResourceData,
                    SRenderGraphResource  const &aSourceResource,
                    CRange                const &aArraySliceRange,
                    CRange                const &aMipSliceRange,
                    bool                         aValidateReads  = true,
                    bool                         aValidateWrites = true);

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
            CEngineResult<bool> isImageBeingReadInSubresourceRange(
                    RefIndex_t           const &aResourceViews,
                    CRenderGraphResources const &aResources,
                    SRenderGraphResource  const &aSourceResource,
                    CRange                const &aArraySliceRange,
                    CRange                const &aMipSliceRange);

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
            CEngineResult<bool> isImageBeingWrittenInSubresourceRange(
                    RefIndex_t            const &aResourceViews,
                    CRenderGraphResources const &aResources,
                    SRenderGraphResource  const &aSourceResource,
                    CRange                const &aArraySliceRange,
                    CRange                const &aMipSliceRange);

        private_methods:
            CEngineResult<SRenderGraphImageView> useImage(
                SRenderGraphImage                &aTexture,
                ERenderGraphViewPurpose    const &aSourceOrTarget,
                EFormat                    const &aRequiredFormat,
                CRange                     const &aArraySliceRange,
                CRange                     const &aMipSliceRange,
                ERenderGraphViewAccessMode const &aMode,
                EEngineStatus              const &aFailCode);

            CEngineResult<SRenderGraphImageView> useImageView(
                SRenderGraphImageView            &aTextureView,
                ERenderGraphViewPurpose    const &aSourceOrTarget,
                EFormat                    const &aRequiredFormat,
                CRange                     const &aArraySliceRange,
                CRange                     const &aMipSliceRange,
                ERenderGraphViewAccessMode const &aMode,
                EEngineStatus              const &aFailCode);

        private_members:
            PassUID_t                         mPassUID;
            Shared<CPassBase>                 mPass;
            Shared<CRenderPass>               mEnclosingRenderPass;
            CRenderGraphMutableResources     &mResourceData;
            SRenderGraphAttachmentCollection &mAttachmentCollection;
            CResourceManager const           &mResourceManager;
        };
    }
}

#endif
