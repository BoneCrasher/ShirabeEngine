#ifndef __SHIRABE_RESOURCETYPES_TEXTUREVIEW_H__
#define __SHIRABE_RESOURCETYPES_TEXTUREVIEW_H__

#include <resources/core/eresourcetype.h>
#include <resources/core/iresource.h>
#include <resources/core/resourcedomaintransfer.h>
#include <resources/core/resourcetraits.h>
#include <resources/core/requestdefaultimplementation.h>

#include "graphicsapi/resources/gfxapi.h"
#include "graphicsapi/resources/types/definition.h"
#include "graphicsapi/resources/types/texture.h"

namespace engine
{
    namespace gfxapi
    {
        /**
         * The CTextureViewDeclaration class describes a CTextureView's integration into the resource/backend.
         */
        class SHIRABE_TEST_EXPORT CTextureViewDeclaration
        {
        public_static_constants:
            static const constexpr EResourceType    sResourceType    = EResourceType::GAPI_VIEW;
            static const constexpr EResourceSubType sResourceSubtype = EResourceSubType::TEXTURE_VIEW;

        public_structs:
            /**
             * The SDescriptor struct provides all necessary information about a texture view.
             */
            struct SHIRABE_TEST_EXPORT SDescriptor
                    : public SDescriptorImplBase<EResourceType::GAPI_VIEW, EResourceSubType::TEXTURE_VIEW>
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
                std::string   name;
                STextureInfo  subjacentTexture;
                EFormat        textureFormat;
                ArraySlices_t arraySlices;
                MipSlices_t   mipMapSlices;
                // TODO: Distinguish binding and read/write mode
            };

            /**
             * The CreationRequest struct describes how a texture view should be created.
             */
            struct SHIRABE_TEST_EXPORT CCreationRequest
                    : public CBaseDeclaration::CCreationRequestBase<SDescriptor>
            {
            public_constructors:
                /**
                 * Construct a creation request from a descriptor and underlying texture handle
                 * upon which the texture view should be based.
                 *
                 * @param aDescriptor              The texture view descriptor to be used for creation.
                 * @param aUnderlyingTextureHandle The texture handle to base the descriptor on.
                 */
                CCreationRequest(
                        SDescriptor        const& aDescriptor,
                        PublicResourceId_t const& aUnderlyingTextureHandle);

            public_methods:
                /**
                 * Return the texture handle of this view.
                 *
                 * @return See brief.
                 */
                PublicResourceId_t const &underlyingTextureHandle() const;

                /**
                 * Describe this creation request as string.
                 *
                 * @return See brief.
                 */
                std::string toString() const;

            private_members:
                PublicResourceId_t mUnderlyingTextureHandle;
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
         * TextureView implementation for all texture views in the engine
         */
        class SHIRABE_TEST_EXPORT CTextureView
                : public CTextureViewDeclaration
                , public CResourceDescriptorAdapter<CTextureViewDeclaration::SDescriptor>
        {
        public_typedefs:
            using MyType_t = CTextureView;

        public_constructors:
            /**
             * Construct a texture view from a texture view descriptor.
             * @param aDescriptor
             */
            CTextureView(CTextureView::SDescriptor const &aDescriptor);
        };

        /**
         *
         */
        SHIRABE_DEFINE_PUBLIC_TRAITS_TYPES(CTextureView);
    }
}

#endif
