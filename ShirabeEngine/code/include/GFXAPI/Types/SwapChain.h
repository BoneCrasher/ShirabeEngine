#ifndef __SHIRABE_SWAPCHAIN_H__
#define __SHIRABE_SWAPCHAIN_H__

#include "Resources/System/Core/EResourceType.h"
#include "Resources/System/Core/Handle.h"
#include "Resources/System/Core/IResource.h"
#include "Resources/System/Core/ResourceDomainTransfer.h"
#include "Resources/System/Core/ResourceTraits.h"

#include "Resources/Subsystems/GFXAPI/GFXAPI.h"

#include "GFXAPI/Definitions.h"
#include "GFXAPI/Types/TextureND.h"
#include "GFXAPI/Types/RenderTargetView.h"
#include "RequestDefaultImplementation.h"

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
        std::string           name;
        Texture2D::Descriptor texture;
        uint32_t              backBufferIndex;

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
        CreationRequest(
          Descriptor     const& desc,
          ResourceHandle const& swapChainHandle);

        ResourceHandle const& swapChainHandle()    const;

        std::string toString() const;

      private:
        ResourceHandle m_swapChainHandle;
      };

      class UpdateRequest
        : public BaseDeclaration::UpdateRequestBase
      {
      public:
        UpdateRequest(ResourceHandle const&handle);
      };

      class DestructionRequest
        : public BaseDeclaration::DestructionRequestBase
      {
      public:
        DestructionRequest(ResourceHandle const&handle);
      };

      class Query
        : public BaseDeclaration::QueryBase
      {
      public:
        Query(ResourceHandle const&handle);
      };

      struct Binding
        : public BaseDeclaration::BindingBase
      {
        RenderTargetView::Binding renderTargetView;

        Binding();
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
      , public ResourceBindingAdapter<SwapChainBufferDeclaration::Binding>
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
        const SwapChainBuffer::Descriptor &desc,
        const SwapChainBuffer::Binding    &binding);

    private:
      inline SwapChainBuffer(
        const SwapChainBuffer::Descriptor &descriptor,
        const SwapChainBuffer::Binding    &binding);
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
        std::string           name;
        Texture2D::Descriptor texture;
        bool                  vsyncEnabled;
        bool                  fullscreen;
        unsigned int          windowHandle;
        unsigned int          backBufferCount;
        unsigned int          refreshRateNumerator;
        unsigned int          refreshRateDenominator;

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
        UpdateRequest(ResourceHandle const&handle);
      };

      class DestructionRequest
        : public BaseDeclaration::DestructionRequestBase
      {
      public:
        DestructionRequest(ResourceHandle const&handle);
      };

      class Query
        : public BaseDeclaration::QueryBase
      {
      public:
        Query(ResourceHandle const&handle);
      };

      struct Binding
        : public BaseDeclaration::BindingBase
      {
        std::vector<SwapChainBuffer::Binding> backBufferRenderTargetBindings;
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
      , public ResourceBindingAdapter<SwapChainDeclaration::Binding>
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
        const SwapChain::Descriptor &desc,
        const SwapChain::Binding    &binding);

      Ptr<RenderTargetView> const& getBackBufferRenderTargetView(uint32_t const& index);

      Ptr<RenderTargetView> const& getCurrentBackBufferRenderTargetView();

    private:
      inline SwapChain(
        const SwapChain::Descriptor &descriptor,
        const SwapChain::Binding    &binding);

      std::size_t                        m_currentBackBufferIndex;
      std::vector<Ptr<RenderTargetView>> m_backBufferRenderTargetViews;
    };

    DeclareSharedPointerType(SwapChain);
    DefineTraitsPublicTypes(SwapChain);

  }
}

#endif