#include "GraphicsAPI/Resources/Types/SwapChain.h"


namespace Engine {
  namespace GFXAPI {

    SwapChainBufferDeclaration::
      Descriptor::Descriptor()
      : name("")
      , texture()
      , backBufferIndex(0)
    {}

    std::string
      SwapChainBufferDeclaration::
      Descriptor::toString() const
    {
      std::stringstream ss;
      ss
        << "SwapChainBufferDescriptor ('" << name << "'): ";

      return ss.str();
    }

    SwapChainBufferDeclaration::
      CreationRequest::CreationRequest(
        Descriptor     const& desc)
      : BaseDeclaration::CreationRequestBase<Descriptor>(desc)
    {}

    std::string
      SwapChainBufferDeclaration::
      CreationRequest::toString() const {
      std::stringstream ss;

      ss
        << "SwapChainBufferCreationRequest: \n"
        << "[\n"
        << resourceDescriptor().toString() << "\n"
        << "]"
        << std::endl;

      return ss.str();
    }

    SwapChainBufferDeclaration::
      UpdateRequest::UpdateRequest(
        PublicResourceId_t const& inPublicResourceId)
      : BaseDeclaration::UpdateRequestBase(inPublicResourceId)
    {}

    SwapChainBufferDeclaration::
      DestructionRequest::DestructionRequest(
        PublicResourceId_t const& inPublicResourceId)
      : BaseDeclaration::DestructionRequestBase(inPublicResourceId)
    {}

    SwapChainBufferDeclaration::
      Query::Query(
        PublicResourceId_t const& inPublicResourceId)
      : BaseDeclaration::QueryBase(inPublicResourceId)
    {}
    
    Ptr<SwapChainBuffer>
      SwapChainBuffer::create(
        const SwapChainBuffer::Descriptor &desc)
    {
      return Ptr<SwapChainBuffer>(new SwapChainBuffer(desc));
    }

    SwapChainBuffer::SwapChainBuffer(
      const SwapChainBuffer::Descriptor &descriptor)
      : SwapChainBufferDeclaration()
      , ResourceDescriptorAdapter<SwapChainBufferDeclaration::Descriptor>(descriptor)
    {}


    /**********************************************************************************************//**
     * \struct	SwapChainDescriptorImpl
     *
     * \brief	Implementation of the SwapChainDescriptor wrapper.
     **************************************************************************************************/
    std::string
      SwapChainDeclaration::
      Descriptor::toString() const
    {
      std::stringstream ss;

      ss
        << "SwapChainDescriptor ('" << name << "'): ";

      return ss.str();
    }


    /**********************************************************************************************//**
     * \struct	SwapChainCreationRequestImpl
     *
     * \brief	A swap chain creation request implementation.
     **************************************************************************************************/

    SwapChainDeclaration::
      CreationRequest::CreationRequest(
        Descriptor const& desc)
      : BaseDeclaration::CreationRequestBase<Descriptor>(desc)
    {}

    std::string
      SwapChainDeclaration::
      CreationRequest::toString() const
    {
      std::stringstream ss;

      ss
        << "SwapChainCreationRequest: \n"
        << "[\n"
        << resourceDescriptor().toString() << "\n"
        << "]"
        << std::endl;

      return ss.str();
    }


    SwapChainDeclaration::
      UpdateRequest::UpdateRequest(
        PublicResourceId_t const& inPublicResourceId)
      : BaseDeclaration::UpdateRequestBase(inPublicResourceId)
    {}

    SwapChainDeclaration::
      DestructionRequest::DestructionRequest(
        PublicResourceId_t const& inPublicResourceId)
      : BaseDeclaration::DestructionRequestBase(inPublicResourceId)
    {}


    SwapChainDeclaration::
      Query::Query(
        PublicResourceId_t const& inPublicResourceId)
      : BaseDeclaration::QueryBase(inPublicResourceId)
    {}


    Ptr<SwapChain>
      SwapChain::create(
        const SwapChain::Descriptor &desc) {
      return Ptr<SwapChain>(new SwapChain(desc));
    }

    Ptr<TextureView> const&
      SwapChain::getBackBufferRenderTargetView(uint32_t const& index)
    {
      if(index < 0 || index >= m_backBufferRenderTargetViews.size())
        return nullptr;

      return m_backBufferRenderTargetViews[index];
    }

    Ptr<TextureView> const&
      SwapChain::getCurrentBackBufferRenderTargetView()
    {
      return getBackBufferRenderTargetView(m_currentBackBufferIndex);
    }

    SwapChain::SwapChain(
      const SwapChain::Descriptor &descriptor)
      : SwapChainDeclaration()
      , ResourceDescriptorAdapter<SwapChainDeclaration::Descriptor>(descriptor)
      , m_currentBackBufferIndex(0)
    {}
  }
}