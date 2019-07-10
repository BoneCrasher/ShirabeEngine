#ifndef __SHIRABE_GRAPHICSAPI_TYPES_MESH_H__
#define __SHIRABE_GRAPHICSAPI_TYPES_MESH_H__

#include <vector>
#include <stdint.h>

#include <vulkan/vulkan.h>

#include <base/declaration.h>
#include <math/geometric/rect.h>

#include <resources/core/eresourcetype.h>
#include <resources/core/iresource.h>
#include <resources/core/resourcedomaintransfer.h>
#include <resources/core/resourcetraits.h>
#include <resources/core/requestdefaultimplementation.h>

#include "graphicsapi/resources/types/definition.h"

namespace engine
{
    namespace gfxapi
    {
        using namespace math;
        using namespace resources;

        /**
         * The CFrameBufferDeclaration class describes a CFrameBuffer's integration into the resource/backend.
         */
        class SHIRABE_TEST_EXPORT CMeshDeclaration
        {
        public_static_constants:
            static const constexpr EResourceType    sResourceType    = EResourceType   ::MESH;
            static const constexpr EResourceSubType sResourceSubtype = EResourceSubType::MESH_STATIC;

        public_structs:
            /**
             * The SDescriptor struct provides all necessary information about a texture view.
             */
            class SHIRABE_TEST_EXPORT SDescriptor
                : public SDescriptorImplBase<EResourceType::MESH, EResourceSubType::MESH_STATIC>
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
                std::string          name;
                DataSourceAccessor_t dataSourceAccessor;
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
         * FrameBuffer implementation for all texture views in the engine
         */
        class SHIRABE_TEST_EXPORT CMesh
                : public CMeshDeclaration
                , public CResourceDescriptorAdapter<CMeshDeclaration::SDescriptor>
        {
        public_typedefs:
            using MyType_t = CMesh;

        public_constructors:
            /**
             * Construct a texture view from a texture view descriptor.
             * @param aDescriptor
             */
            CMesh(CMesh::SDescriptor const &aDescriptor);
        };

        /**
         *
         */
        SHIRABE_DEFINE_PUBLIC_TRAITS_TYPES(CMesh);

    }
}


#endif
