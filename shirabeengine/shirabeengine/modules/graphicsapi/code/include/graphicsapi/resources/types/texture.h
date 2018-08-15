#ifndef __SHIRABE_GFXAPI_TEXTURE_H__
#define __SHIRABE_GFXAPI_TEXTURE_H__

#include <platform/platform.h>

#include <core/bitfield.h>

#include <resources/core/iresource.h>
#include <resources/core/eresourcetype.h>
#include <resources/core/resourcedomaintransfer.h>
#include <resources/core/resourcetraits.h>
#include <resources/core/requestdefaultimplementation.h>

#include "graphicsapi/definitions.h"
#include "graphicsapi/resources/gfxapi.h"
#include "graphicsapi/resources/types/definition.h"

namespace engine
{
    namespace gfxapi
    {
        using namespace engine::core;
        using namespace engine::resources;

        /**
         * The SMultisapmling struct describes multisampling properties for various gfxapi
         * related components in the system.
         */
        struct SMultisapmling
        {
        public_members:
            uint8_t
                size,
                quality;
        };

        /**
         * The STextureInfo struct describes general texture attributes.
         */
        struct SHIRABE_TEST_EXPORT STextureInfo
        {
        public_constructors:
            STextureInfo();

        public_methods:
            void assignTextureInfoParameters(STextureInfo const&other);

        public_members:
            uint32_t
                width,  // 0 - Undefined
                height, // At least 1
                depth;  // At least 1
            EFormat
                format;
            uint16_t
                arraySize; // At least 1 (basically everything is a vector...)
            uint16_t
                mipLevels;
            SMultisapmling
                multisampling;
        };

        /**
         * Explicit interface definition for array slice ranges.
         */
        using ArraySlices_t = CRange;
        /**
         * Explicit interface definition for mip slice ranges.
         */
        using MipSlices_t   = CRange;

        /**
         * The TextureDeclaration class describes all public declarations of
         * a texture resource object in the engine.
         */
        class SHIRABE_TEST_EXPORT CTextureDeclaration
        {
        public_static_constants:
            static const constexpr EResourceType    sResourceType    = EResourceType::TEXTURE;
            static const constexpr EResourceSubType sResourceSubtype = EResourceSubType::UNKNOWN;

        public_structs:
            /**
             * Descriptor implementation for all texture resource types.
             */
            struct SHIRABE_TEST_EXPORT SDescriptor
                    : public SDescriptorImplBase<EResourceType::TEXTURE, EResourceSubType::UNKNOWN>
            {
            public_methods:
                /**
                 * Return a string describing this descriptor.
                 *
                 * @return See brief.
                 */
                std::string toString() const;

            public_members:
                std::string                 name;
                STextureInfo                textureInfo;
                EResourceUsage              cpuGpuUsage;
                CBitField<EBufferBinding>   gpuBinding;
                Vector<CResourceDataSource> initialData;
            };

        public_classes:
            /**
             * Describes a creation request for texture resource types.
             */
            class SHIRABE_TEST_EXPORT CCreationRequest
                    : public CBaseDeclaration::CCreationRequestBase<SDescriptor>
            {
            public_constructors:
                /**
                 * Create a creation request from a texture descriptor.
                 * @param desc
                 */
                CCreationRequest(SDescriptor const &aDescriptor);

            public_destructors:
                virtual ~CCreationRequest() = default;

            public_methods:
                /**
                 * Return a description of this creation request as string.
                 *
                 * @return See brief.
                 */
                std::string toString() const;
            };

            /**
             *
             */
            using CUpdateRequest = CBaseDeclaration::CUpdateRequestBase;
            /**
             *
             */
            using CDestructionRequest = CBaseDeclaration::CDestructionRequestBase;
            /**
             *
             */
            using CQuery = CBaseDeclaration::CQueryBase;
        };

        /**
         * Common base class for any texture used in the engine.
         */
        class SHIRABE_TEST_EXPORT CTexture
                : public CTextureDeclaration
                , public CResourceDescriptorAdapter<CTextureDeclaration::SDescriptor>
        {
        public:
            /**
             * Construct a texture from it's descriptor.
             *
             * @param aDescriptor Descriptor object containing all information about the texture.
             */
            CTexture(CTextureDeclaration::SDescriptor const &aDescriptor);
        };

        /**
         *
         */
        SHIRABE_DEFINE_PUBLIC_TRAITS_TYPES(CTexture);

        /**
         * Enable writing a texture descriptor to ostream interfaces.
         */
        template <uint8_t N>
        std::ostream &operator<<(
                std::ostream                           &aStream,
                CTextureDeclaration::SDescriptor const &aDescriptor)
        {
            return (aStream << aDescriptor.toString());
        }

    }
}

#endif
