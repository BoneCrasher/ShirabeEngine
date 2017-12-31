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

        inline Descriptor()
          : name("")
          , texture()
          , backBufferIndex(0)
        {}

        std::string toString() const {
          std::stringstream ss;
          ss
            << "SwapChainBufferDescriptor ('" << name << "'): ";

          return ss.str();
        }
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
          ResourceHandle const& swapChainHandle)
          : BaseDeclaration::CreationRequestBase<Descriptor>(desc)
          , _swapChainHandle(swapChainHandle)
        {}

        ResourceHandle const& swapChainHandle()    const;

        std::string toString() const {
          std::stringstream ss;

          ss
            << "SwapChainBufferCreationRequest: \n"
            << "[\n"
            << resourceDescriptor().toString() << "\n"
            << "]"
            << std::endl;

          return ss.str();
        }
      private:
        ResourceHandle _swapChainHandle;
      };

      class UpdateRequest 
        : public BaseDeclaration::UpdateRequestBase
      {
      public:
        UpdateRequest(ResourceHandle const&handle)
          : BaseDeclaration::UpdateRequestBase(handle)
        {}
      };

      class DestructionRequest
        : public BaseDeclaration::DestructionRequestBase
      {
      public:
        DestructionRequest(ResourceHandle const&handle)
          : BaseDeclaration::DestructionRequestBase(handle)
        {}
      };

      class Query
        : public BaseDeclaration::QueryBase
      {
      public:
        Query(ResourceHandle const&handle)
          : BaseDeclaration::QueryBase(handle)
        {}
      };

      struct Binding
        : public BaseDeclaration::BindingBase
      {
        RenderTargetView::Binding renderTargetView;

        Binding()
          : BaseDeclaration::BindingBase()
          , renderTargetView()
        {}
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
        const SwapChainBuffer::Binding    &binding) {
        return Ptr<SwapChainBuffer>(new SwapChainBuffer(desc, binding));
      }

    private:
      inline SwapChainBuffer(
        const SwapChainBuffer::Descriptor &descriptor,
        const SwapChainBuffer::Binding    &binding)
        : SwapChainBufferDeclaration()
        , ResourceDescriptorAdapter<SwapChainBufferDeclaration::Descriptor>(descriptor)
        , ResourceBindingAdapter<SwapChainBufferDeclaration::Binding>(binding)
      {}
    };

    ResourceHandle const&
      SwapChainBufferDeclaration::CreationRequest
      ::swapChainHandle() const
    {
      return _swapChainHandle;
    }

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

        std::string toString() const {
          std::stringstream ss;

          ss
            << "SwapChainDescriptor ('" << name << "'): ";

          return ss.str();
        }
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
          Descriptor const& desc)
          : BaseDeclaration::CreationRequestBase<Descriptor>(desc)
        {}

        std::string toString() const {
          std::stringstream ss;

          ss
            << "SwapChainCreationRequest: \n"
            << "[\n"
            << resourceDescriptor().toString() << "\n"
            << "]"
            << std::endl;

          return ss.str();
        }
      };

      class UpdateRequest 
        : public BaseDeclaration::UpdateRequestBase
      {
      public:
        UpdateRequest(ResourceHandle const&handle)
          : BaseDeclaration::UpdateRequestBase(handle)
        {}
      };

      class DestructionRequest
        : public BaseDeclaration::DestructionRequestBase
      {
      public:
        DestructionRequest(ResourceHandle const&handle)
          : BaseDeclaration::DestructionRequestBase(handle)
        {}
      };

      class Query
        : public BaseDeclaration::QueryBase
      {
      public:
        Query(ResourceHandle const&handle)
          : BaseDeclaration::QueryBase(handle)
        {}
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
        const SwapChain::Binding    &binding) {
        return Ptr<SwapChain>(new SwapChain(desc, binding));
      }

      Ptr<RenderTargetView> const& getBackBufferRenderTargetView(uint32_t const& index) {
        if(index < 0 || index >= _backBufferRenderTargetViews.size())
          return nullptr;

        return _backBufferRenderTargetViews[index];
      }

      Ptr<RenderTargetView> const& getCurrentBackBufferRenderTargetView() {
        return getBackBufferRenderTargetView(_currentBackBufferIndex);
      }

    private:
      inline SwapChain(
        const SwapChain::Descriptor &descriptor,
        const SwapChain::Binding    &binding)
        : SwapChainDeclaration()
        , ResourceDescriptorAdapter<SwapChainDeclaration::Descriptor>(descriptor)
        , ResourceBindingAdapter<SwapChainDeclaration::Binding>(binding)
        , _currentBackBufferIndex(0)
      {}

      std::size_t                        _currentBackBufferIndex;
      std::vector<Ptr<RenderTargetView>> _backBufferRenderTargetViews;
    };

    DeclareSharedPointerType(SwapChain);
    DefineTraitsPublicTypes(SwapChain);
    
  }
}

#endif