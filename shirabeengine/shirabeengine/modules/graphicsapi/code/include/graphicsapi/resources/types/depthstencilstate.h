#ifndef __SHIRABE_RESOURCETYPES_DEPTHSTENCIL_STATE_H__
#define __SHIRABE_RESOURCETYPES_DEPTHSTENCIL_STATE_H__

#include <resources/core/eresourcetype.h>
#include <resources/core/iresource.h>
#include <resources/core/resourcedomaintransfer.h>
#include <resources/core/resourcetraits.h>
#include <resources/core/requestdefaultimplementation.h>

#include "graphicsapi/definitions.h"
#include "graphicsapi/resources/gfxapi.h"

namespace engine
{
    namespace gfxapi
    {
        using namespace engine::resources;

        /**
         * The CDepthStencilStateDeclaration class describes the depth stencil state integration
         * into the resource system.
         */
        class CDepthStencilStateDeclaration
        {
        public_static_constants:
            static const constexpr EResourceType    resource_type    = EResourceType::GAPI_STATE;
            static const constexpr EResourceSubType resource_subtype = EResourceSubType::DEPTH_STENCIL_STATE;

        public_structs:
            /**
             * The SDescriptor struct defines all necessary criteria for the creation
             * of a depth stencil state.
             */
            struct SDescriptor
                    : public SDescriptorImplBase<EResourceType::GAPI_STATE, EResourceSubType::DEPTH_STENCIL_STATE>
            {
            public_enums:
                /**
                 * The EDepthWriteMask enum describes the depth write behaviour of the depth test/write stage.
                 */
                enum class EDepthWriteMask
                        : uint8_t
                {
                    Zero = 0,
                    All
                };

                /**
                 * The EStencilOp enum describes how stencil values... (read up...) TBDone
                 */
                enum class EStencilOp
                        : uint8_t
                {
                    Keep = 1,          // Do nothing
                    Zero,              // Write 0
                    Replace,           // Write RefValue
                    IncrementSaturate, // Increment and clamp to MaxValue, if exceeded
                    DecrementSaturate, // Decrement and clamp to 0, if exceeded
                    Invert,            // Invert the bits
                    Increment,         // Increment and wrap to 0, if exceeded
                    Decrement          // Decrement and wrap to MaxValue, if exceeded
                };

            public_structs:
                /**
                 * The StencilCriteria struct binds together information
                 * about depth stencil calculation behaviour.
                 */
                struct SStencilCriteria
                {
                    EStencilOp   failOp;
                    EStencilOp   depthFailOp;
                    EStencilOp   passOp;
                    EComparison  stencilFunc;
                };

            public_constructors:
                /**
                 * Construct an empty descriptor.
                 */
                SDescriptor();

            public_methods:
                /**
                 * Describe this descriptor as string.
                 * ]
                 * @return See brief.
                 */
                std::string toString() const;

            public_members:
                std::string      name;
                bool             enableDepth;
                EDepthWriteMask  depthMask;
                EComparison      depthFunc;
                bool             enableStencil;
                uint8_t          stencilReadMask;
                uint8_t          stencilWriteMask;
                SStencilCriteria stencilFrontfaceCriteria;
                SStencilCriteria stencilBackfaceCriteria;
            };

            /**
             * The CreationRequest struct contains all data to create a depth stencil state.
             */
            struct CCreationRequest
                    : public CBaseDeclaration::CCreationRequestBase<SDescriptor>
            {
            public:
                CCreationRequest(SDescriptor const&desc);

                std::string toString() const;
            };

            /**
             * The CUpdateRequest struct contains all data to update a depth stencil state.
             */
            using CUpdateRequest = CBaseDeclaration::CUpdateRequestBase;

            /**
             * The DestructionRquest struct contains all data to destroy a depth stencil state.
             */
            using CDestructionRequest = CBaseDeclaration::CDestructionRequestBase;

            /**
             * The QueryRequest struct continas all data to query any information from a depth stencil state.
             */
            using CQuery = CBaseDeclaration::CQueryBase;
        };

        /**
         * The CDepthStencilState class implements a graphics API entity to deal with depth stencil states,
         * integrated into the resource system.
         */
        class CDepthStencilState
                : public CDepthStencilStateDeclaration
                , public CResourceDescriptorAdapter<CDepthStencilStateDeclaration::SDescriptor>
        {
        public_constructors:
            CDepthStencilState(CDepthStencilState::SDescriptor const &aDescriptor);
        };

        /**
         *
         */
        SHIRABE_DEFINE_PUBLIC_TRAITS_TYPES(CDepthStencilState);

    }
}

#endif
