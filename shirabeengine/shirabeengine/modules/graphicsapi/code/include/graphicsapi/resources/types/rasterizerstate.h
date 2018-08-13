#ifndef __SHIRABE_RESOURCETYPES_RASTERIZER_STATE_H__
#define __SHIRABE_RESOURCETYPES_RASTERIZER_STATE_H__

#include "resources/core/eresourcetype.h"
#include "resources/core/iresource.h"

#include "resources/core/resourcedomaintransfer.h"
#include "resources/core/resourcetraits.h"
#include "resources/core/requestdefaultimplementation.h"

#include "graphicsapi/definitions.h"
#include "graphicsapi/resources/gfxapi.h"

namespace engine
{
    namespace gfxapi
    {
        using namespace engine::resources;

        class CRasterizerStateDeclaration
        {
        public_static_constants:
            static const constexpr EResourceType    sResourceType    = EResourceType::GAPI_STATE;
            static const constexpr EResourceSubType sResourceSubtype = EResourceSubType::RASTERIZER_STATE;

        public_enums:
            /**
             * The EFillMode enum describes, how the rasterizer should derive fragments from transformed output patches.
             */
            enum class EFillMode
            {
                WireFrame, //< Select only the pixels directly affected by the lines connecting the vertices.
                Solid      //< Select all pixels affected by the edges AND the area within the edges.
            };

            /**
             * The ECullMode enum describes, if and how backface culling should be performed.
             */
            enum class ECullMode
            {
                None,  //< Deactivate backface culling
                Front, //< Cull all faces, which are considered "front facing"
                Back   //< Cull all faces, which are considered "back facing".
            }; // In any case, we expect front to be counter clockwise!

        public_structs:
            /**
             * This descriptor provides all information required to create a engine rasterizer state.
             */
            struct SDescriptor
                    : public CDescriptorImplBase<EResourceType::GAPI_STATE, EResourceSubType::RASTERIZER_STATE>
            {
            public_constructors:
                /**
                 * Construct an empty descriptor.
                 */
                SDescriptor();

                /**
                 * Describe this descriptor as string.
                 *
                 * @return See brief.
                 */
                std::string toString() const;

            public_members:
                std::string name;
                EFillMode   fillMode;
                ECullMode   cullMode;
                bool        antialiasRasterLines;
                bool        multisamplingEnabled; // Requires respective format
                bool        scissorEnabled;
                bool        depthClipEnabled;
            };

            /**
             * The CCreationRequest class is required to create new rasterizer states and wraps
             * all information required to do so...
             */
            struct CCreationRequest
                    : public CBaseDeclaration::CCreationRequestBase<SDescriptor>
            {
            public:
                CCreationRequest(SDescriptor const&desc);

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
         * The CRasterizerState class is the engine rasterizer state implementation,
         * which will be mapped down to the respective graphics API.
         */
        class CRasterizerState
                : public CRasterizerStateDeclaration
                , public CResourceDescriptorAdapter<CRasterizerStateDeclaration::SDescriptor>
        {
        public:
            /**
             * Create a default rasterizer state based on its descriptor.
             *
             * @param aDescriptor The rasterizer state descriptor used for state re/creation.
             */
            CRasterizerState(CRasterizerState::SDescriptor const &aDescriptor);
        };

        SHIRABE_DEFINE_PUBLIC_TRAITS_TYPES(CRasterizerState);
    }
}

#endif
