#ifndef __SHIRABE_RESOURCETYPES_RENDERPASS_H__
#define __SHIRABE_RESOURCETYPES_RENDERPASS_H__

#include <resources/core/eresourcetype.h>
#include <resources/core/iresource.h>
#include <resources/core/resourcedomaintransfer.h>
#include <resources/core/resourcetraits.h>
#include <resources/core/requestdefaultimplementation.h>

#include "graphicsapi/resources/gfxapi.h"
#include "graphicsapi/resources/types/definition.h"

namespace engine
{
    namespace gfxapi
    {
        enum class EAttachmentLoadOp
        {
            LOAD      = 0,
            CLEAR     = 1,
            DONT_CARE = 2,
        };

        enum class EAttachmentStoreOp
        {
            STORE     = 0,
            DONT_CARE = 1,
        };

        enum EImageLayout
        {
            UNDEFINED                                      = 0,
            GENERAL                                        = 1,
            COLOR_ATTACHMENT_OPTIMAL                       = 2,
            DEPTH_STENCIL_ATTACHMENT_OPTIMAL               = 3,
            DEPTH_STENCIL_READ_ONLY_OPTIMAL                = 4,
            SHADER_READ_ONLY_OPTIMAL                       = 5,
            TRANSFER_SRC_OPTIMAL                           = 6,
            TRANSFER_DST_OPTIMAL                           = 7,
            PREINITIALIZED                                 = 8,
            DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL     = 1000117000,
            DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL     = 1000117001,
            PRESENT_SRC_KHR                                = 1000001002,
            SHARED_PRESENT_KHR                             = 1000111000,
            SHADING_RATE_OPTIMAL_NV                        = 1000164003,
            FRAGMENT_DENSITY_MAP_OPTIMAL_EXT               = 1000218000,
            DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL_KHR = DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL,
            DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL_KHR = DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL,
        };

        struct SAttachmentDescription
        {
            EFormat            format;
            EAttachmentLoadOp  loadOp;
            EAttachmentStoreOp storeOp;
            EAttachmentLoadOp  stencilLoadOp;
            EAttachmentStoreOp stencilStoreOp;
            EImageLayout       initialLayout;
            EImageLayout       finalLayout;
        };

        struct SAttachmentReference
        {
            uint32_t     attachment;
            EImageLayout layout;
        };

        struct SSubpassDescription
        {
            std::vector<SAttachmentReference> inputAttachments;
            std::vector<SAttachmentReference> colorAttachments;
            std::vector<SAttachmentReference> resolveAttachments;
            std::vector<SAttachmentReference> depthStencilAttachments;
            std::vector<uint32_t>             preserveStencilAttachments;
        };

        /**
         * The CRenderPassDeclaration class describes a CRenderPass's integration into the resource/backend.
         */
        class SHIRABE_TEST_EXPORT CRenderPassDeclaration
        {
        public_static_constants:
            static const constexpr EResourceType    sResourceType    = EResourceType::GAPI_COMPONENT;
            static const constexpr EResourceSubType sResourceSubtype = EResourceSubType::RENDER_PASS;

        public_structs:
            /**
             * The SDescriptor struct provides all necessary information about a texture view.
             */
            struct SHIRABE_TEST_EXPORT SDescriptor
                    : public SDescriptorImplBase<EResourceType::GAPI_COMPONENT, EResourceSubType::RENDER_PASS>
            {
            public_constructors:
                /**
                 * Default-Construct an empty SDescriptor.
                 */
                SDescriptor();

            public_methods:
                /**
                 * Describe this descriptor as string.
                 *
                 * @return See brief.
                 */
                std::string toString() const;

            public_members:
                std::string                         name;
                std::vector<SAttachmentDescription> attachmentDescriptions;
                std::vector<SSubpassDescription>    subpassDescriptions;
            };

            /**
             * The CreationRequest struct describes how a render pass should be created.
             */
            struct SHIRABE_TEST_EXPORT CCreationRequest
                    : public CBaseDeclaration::CCreationRequestBase<SDescriptor>
            {
            public_constructors:
                /**
                 * Construct a creation request from a descriptor.
                 *
                 * @param aDescriptor              The texture view descriptor to be used for creation.
                 */
                CCreationRequest(SDescriptor const &aDescriptor);

            public_methods:

                /**
                 * Describe this creation request as string.
                 *
                 * @return See brief.
                 */
                std::string toString() const;

            private_members:
            };

            /**
             * TBDone
             */
            using CUpdateRequest = CBaseDeclaration::CUpdateRequestBase;

            /**
             * TBDone
             */
            using CDestructionRequest = CBaseDeclaration::CDestructionRequestBase;

            /**
             * TBDone
             */
            using CQuery = CBaseDeclaration::CQueryBase;
        };

        /**
         * RenderPass implementation for all render passes in the engine
         */
        class SHIRABE_TEST_EXPORT CRenderPass
                : public CRenderPassDeclaration
                , public CResourceDescriptorAdapter<CRenderPassDeclaration::SDescriptor>
        {
        public_typedefs:
            using MyType_t = CRenderPass;

        public_constructors:
            /**
             * Construct a render pass from a render pass descriptor.
             * @param aDescriptor
             */
            CRenderPass(CRenderPass::SDescriptor const &aDescriptor);
        };

        /**
         *
         */
        SHIRABE_DEFINE_PUBLIC_TRAITS_TYPES(CRenderPass);
    }
}

#endif
