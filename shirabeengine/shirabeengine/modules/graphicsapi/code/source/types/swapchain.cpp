#include "GraphicsAPI/Resources/Types/SwapChain.h"


namespace engine {
  namespace gfxapi {

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
      CUpdateRequest::CUpdateRequest(
        PublicResourceId_t const& inPublicResourceId)
      : BaseDeclaration::CUpdateRequestBase(inPublicResourceId)
    {}

    SwapChainBufferDeclaration::
      CDestructionRequest::CDestructionRequest(
        PublicResourceId_t const& inPublicResourceId)
      : BaseDeclaration::CDestructionRequestBase(inPublicResourceId)
    {}

    SwapChainBufferDeclaration::
      Query::Query(
        PublicResourceId_t const& inPublicResourceId)
      : BaseDeclaration::QueryBase(inPublicResourceId)
    {}
    
    CStdSharedPtr_t<SwapChainBuffer>
      SwapChainBuffer::create(
        const SwapChainBuffer::Descriptor &desc)
    {
      return CStdSharedPtr_t<SwapChainBuffer>(new SwapChainBuffer(desc));
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
      CUpdateRequest::CUpdateRequest(
        PublicResourceId_t const& inPublicResourceId)
      : BaseDeclaration::CUpdateRequestBase(inPublicResourceId)
    {}

    SwapChainDeclaration::
      CDestructionRequest::CDestructionRequest(
        PublicResourceId_t const& inPublicResourceId)
      : BaseDeclaration::CDestructionRequestBase(inPublicResourceId)
    {}


    SwapChainDeclaration::
      Query::Query(
        PublicResourceId_t const& inPublicResourceId)
      : BaseDeclaration::QueryBase(inPublicResourceId)
    {}


    CStdSharedPtr_t<SwapChain>
      SwapChain::create(
        const SwapChain::Descriptor &desc) {
      return CStdSharedPtr_t<SwapChain>(new SwapChain(desc));
    }

    CStdSharedPtr_t<TextureView> const&
      SwapChain::getBackBufferRenderTargetView(uint32_t const& index)
    {
      if(index < 0 || index >= m_backBufferRenderTargetViews.size())
        return nullptr;

      return m_backBufferRenderTargetViews[index];
    }

    CStdSharedPtr_t<TextureView> const&
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