#ifndef __SHIRABE_GFXAPI_BUFFER_H__
#define __SHIRABE_GFXAPI_BUFFER_H__

#include <platform/platform.h>

#include <core/bitfield.h>

#include <resources/core/iresource.h>
#include <resources/core/eresourcetype.h>
#include <resources/core/resourcedomaintransfer.h>
#include <resources/core/resourcetraits.h>
#include <resources/core/requestdefaultimplementation.h>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

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
         * The TextureDeclaration class describes all public declarations of
         * a texture resource object in the engine.
         */
        class SHIRABE_TEST_EXPORT CBufferDeclaration
        {
        public_static_constants:
            static const constexpr EResourceType    sResourceType    = EResourceType   ::BUFFER;
            static const constexpr EResourceSubType sResourceSubtype = EResourceSubType::BUFFER;

        public_structs:
            /**
             * Descriptor implementation for all texture resource types.
             */
            struct SHIRABE_TEST_EXPORT SDescriptor
                    : public SDescriptorImplBase<EResourceType::BUFFER, EResourceSubType::BUFFER>
            {
            public_methods:
                /**
                 * Return a string describing this descriptor.
                 *
                 * @return See brief.
                 */
                std::string toString() const override;

            public_members:
                std::string                  name;
                VkBufferCreateInfo           createInfo;
                Vector<DataSourceAccessor_t> initialData;
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
                explicit CCreationRequest(SDescriptor const &aDescriptor);

            public_destructors:
                ~CCreationRequest() override = default;

            public_methods:
                /**
                 * Return a description of this creation request as string.
                 *
                 * @return See brief.
                 */
                std::string toString() const override;
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
        class SHIRABE_TEST_EXPORT CBuffer
                : public CBufferDeclaration
                , public CResourceDescriptorAdapter<CBufferDeclaration::SDescriptor>
        {
        public:
            /**
             * Construct a texture from it's descriptor.
             *
             * @param aDescriptor Descriptor object containing all information about the texture.
             */
            CBuffer(CBufferDeclaration::SDescriptor const &aDescriptor);
        };

        /**
         *
         */
        SHIRABE_DEFINE_PUBLIC_TRAITS_TYPES(CBuffer);

        /**
         * Enable writing a texture descriptor to ostream interfaces.
         */
        template <uint8_t N>
        std::ostream &operator<<(
                std::ostream                          &aStream,
                CBufferDeclaration::SDescriptor const &aDescriptor)
        {
            return (aStream << aDescriptor.toString());
        }

    }
}

#endif
