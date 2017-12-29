#ifndef __SHIRABE_SWAPCHAIN_H__
#define __SHIRABE_SWAPCHAIN_H__

#include "Resources/System/Core/EResourceType.h"
#include "Resources/System/Core/Handle.h"
#include "Resources/System/Core/IResource.h"
#include "Resources/System/Core/ResourceDomainTransfer.h"
#include "Resources/System/Core/ResourceTraits.h"

#include "GFXAPI/Definitions.h"
#include "GFXAPI/Types/TextureND.h"
#include "GFXAPI/Types/RenderTargetView.h"

#include "Resources/Subsystems/GFXAPI/GFXAPI.h"

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
      struct CreationRequest {
      public:
        CreationRequest(
          Descriptor     const& desc,
          ResourceHandle const& swapChainHandle)
          : _resourceDescriptor(desc)
          , _swapChainHandle(swapChainHandle)
        {}

        Descriptor     const& resourceDescriptor() const; //  const { return ResourceDescriptor<SwapChain>(_resourceDescriptor); }
        ResourceHandle const& swapChainHandle()    const;

        std::string toString() const {
          std::stringstream ss;

          ss
            << "SwapChainBufferCreationRequest: \n"
            << "[\n"
            << _resourceDescriptor.toString() << "\n"
            << "]"
            << std::endl;

          return ss.str();
        }
      private:
        Descriptor     _resourceDescriptor;
        ResourceHandle _swapChainHandle;
      };

      struct UpdateRequest {

      };

      struct DestructionRequest {

      };

      struct Query {

      };

      struct Binding {
        ResourceHandle            handle;
        RenderTargetView::Binding renderTargetView;
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

    const SwapChainBuffer::Descriptor&
      SwapChainBufferDeclaration::CreationRequest
      ::resourceDescriptor() const
    {
      return _resourceDescriptor;
    }

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
      struct CreationRequest {
      public:
        CreationRequest(
          Descriptor const& desc)
          : _resourceDescriptor(desc)
        {}

        Descriptor const& resourceDescriptor() const; //  const { return ResourceDescriptor<SwapChain>(_resourceDescriptor); }

        std::string toString() const {
          std::stringstream ss;

          ss
            << "SwapChainCreationRequest: \n"
            << "[\n"
            << _resourceDescriptor.toString() << "\n"
            << "]"
            << std::endl;

          return ss.str();
        }

      private:
        Descriptor     _resourceDescriptor;
      };

      struct UpdateRequest {

      };

      struct DestructionRequest {

      };

      struct Query {

      };

      struct Binding {
        ResourceHandle                        swapChainHandle;
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

    const SwapChain::Descriptor&
      SwapChainDeclaration::CreationRequest
      ::resourceDescriptor() const
    {
      return _resourceDescriptor;
    }

  }
}

#endif