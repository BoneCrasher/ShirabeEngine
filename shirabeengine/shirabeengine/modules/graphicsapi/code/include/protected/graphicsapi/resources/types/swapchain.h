#ifndef __SHIRABE_SWAPCHAIN_H__
#define __SHIRABE_SWAPCHAIN_H__

#include "Resources/Core/EResourceType.h"

#include "Resources/Core/IResource.h"
#include "Resources/Core/ResourceDomainTransfer.h"
#include "Resources/Core/ResourceTraits.h"
#include "Resources/Core/RequestDefaultImplementation.h"

#include "GraphicsAPI/Resources/GFXAPI.h"
#include "GraphicsAPI/Resources/Types/Definition.h"
#include "GraphicsAPI/Resources/Types/Texture.h"
#include "GraphicsAPI/Resources/Types/TextureView.h"

namespace Engine {
  namespace GFXAPI {
    using namespace Resources;

    class SwapChainBufferDeclaration {
    public:
      static const constexpr EResourceType    resource_type    = EResourceType::GAPI_COMPONENT;
      static const constexpr EResourceSubType resource_subtype = EResourceSubType::SWAP_CHAIN_BUFFER;

      /**********************************************************************************************//**
       * \struct	SwapChainDescriptorImpl
       *
       * \brief	Implementation of the SwapChainDescriptor wrapper.
       **************************************************************************************************/
      struct Descriptor
        : public DescriptorImplBase<EResourceType::GAPI_COMPONENT, EResourceSubType::SWAP_CHAIN_BUFFER>
      {
        std::string         name;
        Texture::Descriptor texture;
        uint32_t            backBufferIndex;

        Descriptor();

        std::string toString() const;
      };

      /**********************************************************************************************//**
       * \struct	SwapChainCreationRequestImpl
       *
       * \brief	A swap chain creation request implementation.
       **************************************************************************************************/
      struct CreationRequest
        : public BaseDeclaration::CreationRequestBase<Descriptor>
      {
      public:
        CreationRequest(Descriptor const& desc);

        std::string toString() const;
        
        inline PublicResourceId_t swapChainId() const { return m_swapChainId; }

      private:
        PublicResourceId_t m_swapChainId;
      };

      class UpdateRequest
        : public BaseDeclaration::UpdateRequestBase
      {
      public:
        UpdateRequest(
          PublicResourceId_t const& inPublicResourceId);
      };

      class DestructionRequest
        : public BaseDeclaration::DestructionRequestBase
      {
      public:
        DestructionRequest(
          PublicResourceId_t const& inPublicResourceId);
      };

      class Query
        : public BaseDeclaration::QueryBase
      {
      public:
        Query(
          PublicResourceId_t const& inPublicResourceId);
      };

    };

    /**********************************************************************************************//**
     * \class	GAPISwapChain
     *
     * \brief	A gapi swap chain.
     **************************************************************************************************/
    class SwapChainBuffer
      : public SwapChainBufferDeclaration
      , public ResourceDescriptorAdapter<SwapChainBufferDeclaration::Descriptor>
    {
    public:
      using my_type = SwapChainBuffer;

      //
      // GAPISwapChain<TGAPIResource> implementation
      // 
      // EEngineStatus bind(...);
      // 
      // EEngineStatus present(bool verticallySynchronized = true);

      inline static Ptr<SwapChainBuffer> create(
        const SwapChainBuffer::Descriptor &desc);

    private:
      inline SwapChainBuffer(
        const SwapChainBuffer::Descriptor &descriptor);
    };


    DeclareSharedPointerType(SwapChainBuffer);
    DefineTraitsPublicTypes(SwapChainBuffer);

    class SwapChainDeclaration {
    public:
      static const constexpr EResourceType    resource_type    = EResourceType::GAPI_COMPONENT;
      static const constexpr EResourceSubType resource_subtype = EResourceSubType::SWAP_CHAIN;

      /**********************************************************************************************//**
       * \struct	SwapChainDescriptorImpl
       *
       * \brief	Implementation of the SwapChainDescriptor wrapper.
       **************************************************************************************************/
      struct Descriptor
        : public DescriptorImplBase<EResourceType::GAPI_COMPONENT, EResourceSubType::SWAP_CHAIN>
      {
        std::string         name;
        Texture::Descriptor texture;
        bool                vsyncEnabled;
        bool                fullscreen;
        unsigned int        windowHandle;
        unsigned int        backBufferCount;
        unsigned int        refreshRateNumerator;
        unsigned int        refreshRateDenominator;

        std::string toString() const;
      };

      /**********************************************************************************************//**
       * \struct	SwapChainCreationRequestImpl
       *
       * \brief	A swap chain creation request implementation.
       **************************************************************************************************/
      struct CreationRequest
        : public BaseDeclaration::CreationRequestBase<Descriptor> {
      public:
        CreationRequest(
          Descriptor const& desc);

        std::string toString() const;
      };

      class UpdateRequest
        : public BaseDeclaration::UpdateRequestBase
      {
      public:
        UpdateRequest(
          PublicResourceId_t const& inPublicResourceId);
      };

      class DestructionRequest
        : public BaseDeclaration::DestructionRequestBase
      {
      public:
        DestructionRequest(
          PublicResourceId_t const& inPublicResourceId);
      };

      class Query
        : public BaseDeclaration::QueryBase
      {
      public:
        Query(
          PublicResourceId_t const& inPublicResourceId);
      };
    };

    /**********************************************************************************************//**
     * \class	GAPISwapChain
     *
     * \brief	A gapi swap chain.
     **************************************************************************************************/
    class SwapChain
      : public SwapChainDeclaration
      , public ResourceDescriptorAdapter<SwapChainDeclaration::Descriptor>
    {
    public:
      using my_type = SwapChain;

      //
      // GAPISwapChain<TGAPIResource> implementation
      // 
      // EEngineStatus bind(...);
      // 
      // EEngineStatus present(bool verticallySynchronized = true);

      inline static Ptr<SwapChain> create(
        const SwapChain::Descriptor &desc);

      Ptr<TextureView> const& getBackBufferRenderTargetView(uint32_t const& index);

      Ptr<TextureView> const& getCurrentBackBufferRenderTargetView();

    private:
      inline SwapChain(
        const SwapChain::Descriptor &descriptor);

      std::size_t                        m_currentBackBufferIndex;
      std::vector<Ptr<TextureView>> m_backBufferRenderTargetViews;
    };

    DeclareSharedPointerType(SwapChain);
    DefineTraitsPublicTypes(SwapChain);

  }
}

#endif