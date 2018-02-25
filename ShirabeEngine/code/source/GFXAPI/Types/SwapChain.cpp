#include "GFXAPI/Types/SwapChain.h"


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
        Descriptor     const& desc,
        ResourceHandle const& swapChainHandle)
      : BaseDeclaration::CreationRequestBase<Descriptor>(desc)
      , m_swapChainHandle(swapChainHandle)
    {}

    ResourceHandle const&
      SwapChainBufferDeclaration::
      CreationRequest::swapChainHandle() const
    {
      return m_swapChainHandle;
    }

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
      UpdateRequest::UpdateRequest(ResourceHandle const&handle)
      : BaseDeclaration::UpdateRequestBase(handle)
    {}

    SwapChainBufferDeclaration::
      DestructionRequest::DestructionRequest(ResourceHandle const&handle)
      : BaseDeclaration::DestructionRequestBase(handle)
    {}

    SwapChainBufferDeclaration::
      Query::Query(ResourceHandle const&handle)
      : BaseDeclaration::QueryBase(handle)
    {}

    SwapChainBufferDeclaration::
      Binding::Binding()
      : BaseDeclaration::BindingBase()
      , renderTargetView()
    {}


    Ptr<SwapChainBuffer>
      SwapChainBuffer::create(
        const SwapChainBuffer::Descriptor &desc,
        const SwapChainBuffer::Binding    &binding)
    {
      return Ptr<SwapChainBuffer>(new SwapChainBuffer(desc, binding));
    }

    SwapChainBuffer::SwapChainBuffer(
      const SwapChainBuffer::Descriptor &descriptor,
      const SwapChainBuffer::Binding    &binding)
      : SwapChainBufferDeclaration()
      , ResourceDescriptorAdapter<SwapChainBufferDeclaration::Descriptor>(descriptor)
      , ResourceBindingAdapter<SwapChainBufferDeclaration::Binding>(binding)
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
      UpdateRequest::UpdateRequest(ResourceHandle const&handle)
      : BaseDeclaration::UpdateRequestBase(handle)
    {}

    SwapChainDeclaration::
      DestructionRequest::DestructionRequest(ResourceHandle const&handle)
      : BaseDeclaration::DestructionRequestBase(handle)
    {}


    SwapChainDeclaration::
      Query::Query(ResourceHandle const&handle)
      : BaseDeclaration::QueryBase(handle)
    {}


    Ptr<SwapChain>
      SwapChain::create(
        const SwapChain::Descriptor &desc,
        const SwapChain::Binding    &binding) {
      return Ptr<SwapChain>(new SwapChain(desc, binding));
    }

    Ptr<RenderTargetView> const&
      SwapChain::getBackBufferRenderTargetView(uint32_t const& index)
    {
      if(index < 0 || index >= m_backBufferRenderTargetViews.size())
        return nullptr;

      return m_backBufferRenderTargetViews[index];
    }

    Ptr<RenderTargetView> const&
      SwapChain::getCurrentBackBufferRenderTargetView()
    {
      return getBackBufferRenderTargetView(m_currentBackBufferIndex);
    }

    SwapChain::SwapChain(
      const SwapChain::Descriptor &descriptor,
      const SwapChain::Binding    &binding)
      : SwapChainDeclaration()
      , ResourceDescriptorAdapter<SwapChainDeclaration::Descriptor>(descriptor)
      , ResourceBindingAdapter<SwapChainDeclaration::Binding>(binding)
      , m_currentBackBufferIndex(0)
    {}
  }
}