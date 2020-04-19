#ifndef __SHIRABE_RESOURCETYPES_FRAMEBUFFER_H__
#define __SHIRABE_RESOURCETYPES_FRAMEBUFFER_H__

#include <resources/core/eresourcetype.h>
#include <resources/core/iresource.h>
#include <resources/core/resourcedomaintransfer.h>
#include <resources/core/resourcetraits.h>
#include <resources/core/requestdefaultimplementation.h>

#include "graphicsapi/resources/gfxapi.h"
#include "graphicsapi/resources/types/definition.h"
#include "graphicsapi/resources/types/renderpass.h"

namespace engine
{
    namespace gfxapi
    {
        /**
         * The CFrameBufferDeclaration class describes a CFrameBuffer's integration into the resource/backend.
         */
        class SHIRABE_TEST_EXPORT CFrameBufferDeclaration
        {
        public_static_constants:
            static const constexpr EResourceType    sResourceType    = EResourceType::GAPI_COMPONENT;
            static const constexpr EResourceSubType sResourceSubtype = EResourceSubType::FRAME_BUFFER;

        public_structs:
            /**
             * The SDescriptor struct provides all necessary information about a texture view.
             */
            struct SHIRABE_TEST_EXPORT SDescriptor
                    : public SDescriptorImplBase<EResourceType::GAPI_COMPONENT, EResourceSubType::FRAME_BUFFER>
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
                std::string name;
                uint32_t    width;
                uint32_t    height;
                uint32_t    layers;
            };

            /**
             * The CreationRequest struct describes how a texture view should be created.
             */
            struct SHIRABE_TEST_EXPORT CCreationRequest
                    : public CBaseDeclaration::CCreationRequestBase<SDescriptor>
            {
            public_constructors:
                /**
                 * Construct a creation request from a descriptor, render pass and texture view handles.
                 *
                 * @param aDescriptor         The texture view descriptor to be used for creation.
                 * @param aRenderPassHandle   The render pass describing the usage of the frame buffer.
                 * @param aTextureViewHandles The texture handles of the texture views to be bound for the framebuffer.
                 */
                CCreationRequest(
                        SDescriptor            const &aDescriptor,
                        PublicResourceId_t     const &aRenderPassHandle,
                        PublicResourceIdList_t const &aTextureViewHandles);

            public_methods:
                /**
                 * Return the texture handle of this view.
                 *
                 * @return See brief.
                 */
                PublicResourceId_t const &renderPassHandle() const;

                /**
                 * Return the list of associated texture view handles to be bound.
                 *
                 * @return See brief.
                 */
                PublicResourceIdList_t const &textureViewHandles() const;

                /**
                 * Describe this creation request as string.
                 *
                 * @return See brief.
                 */
                std::string toString() const;

            private_members:
                PublicResourceId_t     mRenderPassHandle;
                PublicResourceIdList_t mTextureViewHandles;
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
         * FrameBuffer implementation for all texture views in the engine
         */
        class SHIRABE_TEST_EXPORT CFrameBuffer
                : public CFrameBufferDeclaration
                , public CResourceDescriptorAdapter<CFrameBufferDeclaration::SDescriptor>
        {
        public_typedefs:
            using MyType_t = CFrameBuffer;

        public_constructors:
            /**
             * Construct a texture view from a texture view descriptor.
             * @param aDescriptor
             */
            CFrameBuffer(CFrameBuffer::SDescriptor const &aDescriptor);
        };

        /**
         *
         */
        SHIRABE_DEFINE_PUBLIC_TRAITS_TYPES(CFrameBuffer);
    }
}

#endif
