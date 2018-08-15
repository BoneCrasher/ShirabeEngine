#ifndef __SHIRABE_SWAPCHAIN_H__
#define __SHIRABE_SWAPCHAIN_H__

#include "resources/core/eresourcetype.h"

#include "resources/core/iresource.h"
#include "resources/core/resourcedomaintransfer.h"
#include "resources/core/resourcetraits.h"
#include "resources/core/requestdefaultimplementation.h"

#include "graphicsapi/resources/gfxapi.h"
#include "graphicsapi/resources/types/definition.h"
#include "graphicsapi/resources/types/texture.h"
#include "graphicsapi/resources/types/textureview.h"

namespace engine
{
    namespace gfxapi
    {
        using namespace resources;

        /**
         * The CSwapChainBufferDeclaration class is the integration of a swap chain buffer into the
         * resource system.
         */
        class CSwapChainBufferDeclaration
        {
        public_static_constants:
            static const constexpr EResourceType    sResourceType    = EResourceType::GAPI_COMPONENT;
            static const constexpr EResourceSubType sResourceSubtype = EResourceSubType::SWAP_CHAIN_BUFFER;

        public_structs:
            /**
             * The SDescriptor struct describes all data required to create a swapchain buffer.
             */
            struct SDescriptor
                    : public CDescriptorImplBase<EResourceType::GAPI_COMPONENT, EResourceSubType::SWAP_CHAIN_BUFFER>
            {
                std::string           name;
                CTexture::SDescriptor texture;
                uint32_t              backBufferIndex;

                SDescriptor();

                std::string toString() const;
            };

            /**
             * The CCreationRequest struct contains all data for swap chain buffer resource creation.
             */
            struct CCreationRequest
                    : public CBaseDeclaration::CCreationRequestBase<SDescriptor>
            {
            public_constructors:
                CCreationRequest(SDescriptor const &aDescriptor);

            public_methods:
                SHIRABE_INLINE PublicResourceId_t swapChainId() const { return mSwapChainId; }

                std::string toString() const;

            private_members:
                PublicResourceId_t mSwapChainId;
            };

            using CUpdateRequest      = CBaseDeclaration::CUpdateRequestBase;
            using CDestructionRequest = CBaseDeclaration::CDestructionRequestBase;
            using CQuery              = CBaseDeclaration::CQueryBase;
        };

        /**
         * The SwapChainBuffer class is the engine entity representing a swap chain buffer instance.
         */
        class CSwapChainBuffer
                : public CSwapChainBufferDeclaration
                , public CResourceDescriptorAdapter<CSwapChainBufferDeclaration::SDescriptor>
        {
        public_typedefs:
            using my_type = CSwapChainBuffer;

            //
            // GAPISwapChain<TGAPIResource> implementation
            //
            // EEngineStatus bind(...);
            //
            // EEngineStatus present(bool verticallySynchronized = true);

        public_constructors:
            SHIRABE_INLINE CSwapChainBuffer(CSwapChainBuffer::SDescriptor const &aDescriptor);

        public_methods:
            static SHIRABE_INLINE CStdSharedPtr_t<CSwapChainBuffer> create(CSwapChainBuffer::SDescriptor const &aDescriptor);
        };

        SHIRABE_DEFINE_PUBLIC_TRAITS_TYPES(CSwapChainBuffer);

        /**
         * The SwapChainDeclaration class is the integration of the swapchain into the resource system.
         */
        class CSwapChainDeclaration
        {
        public_static_constants:
            static const constexpr EResourceType    sResourceType    = EResourceType::GAPI_COMPONENT;
            static const constexpr EResourceSubType sResourceSubtype = EResourceSubType::SWAP_CHAIN;

        public_structs:
            /**
             * The SDescriptor struct describes all data required to create a swapchain.
             */
            struct SDescriptor
                    : public CDescriptorImplBase<EResourceType::GAPI_COMPONENT, EResourceSubType::SWAP_CHAIN>
            {
            public_methods:
                /**
                 * Describe this descriptor as string.
                 *
                 * @return See brief.
                 */
                std::string toString() const;

            public_members:
                std::string           name;
                CTexture::SDescriptor texture;
                bool                  vsyncEnabled;
                bool                  fullscreen;
                unsigned int          windowHandle;
                unsigned int          backBufferCount;
                unsigned int          refreshRateNumerator;
                unsigned int          refreshRateDenominator;
            };

            struct CCreationRequest
                    : public CBaseDeclaration::CCreationRequestBase<SDescriptor>
            {
            public:
                CCreationRequest(
                        SDescriptor const& desc);

                std::string toString() const;
            };

            using CUpdateRequest      = CBaseDeclaration::CUpdateRequestBase;
            using CDestructionRequest = CBaseDeclaration::CDestructionRequestBase;
            using CQuery              = CBaseDeclaration::CQueryBase;
        };

        /**
         * The SwapChain class is the engine entity to represent swap chains.
         */
        class CSwapChain
                : public CSwapChainDeclaration
                , public CResourceDescriptorAdapter<CSwapChainDeclaration::SDescriptor>
        {
        public_typedefs:
            using my_type = CSwapChain;

            //
            // GAPISwapChain<TGAPIResource> implementation
            //
            // EEngineStatus bind(...);
            //
            // EEngineStatus present(bool verticallySynchronized = true);

        public_static_functions:
            /**
             * Create a new swap chain entity for the provided descriptor.
             *
             * @param aDescriptor
             * @return A pointer to a CSwapChain instance, or nullptr;
             */
            static SHIRABE_INLINE CStdSharedPtr_t<CSwapChain> create(CSwapChain::SDescriptor const &aDescriptor);

        public_constructors:
            /**
             * Create a swap chain initialized with it's creation descriptor.
             *
             * @param aDescriptor
             */
             CSwapChain(CSwapChain::SDescriptor const &aDescriptor);

        public_methods:
            /**
             * Access a back buffer render target view for a given back buffer index.
             *
             * @param aIndex
             * @return A valid texture view pointer or nullptr on error.
             */
            CStdSharedPtr_t<CTextureView> const getBackBufferRenderTargetView(uint32_t const &aIndex);

            /**
             * Get the currently selected back buffer pointer in case of double, triple or more buffering.
             *
             * @return A valid texture view pointer or nullptr on error.
             */
            CStdSharedPtr_t<CTextureView> const getCurrentBackBufferRenderTargetView();

        private_members:
            std::size_t                                mCurrentBackBufferIndex;
            std::vector<CStdSharedPtr_t<CTextureView>> mBackBufferRenderTargetViews;
        };

        SHIRABE_DEFINE_PUBLIC_TRAITS_TYPES(CSwapChain);

    }
}

#endif
