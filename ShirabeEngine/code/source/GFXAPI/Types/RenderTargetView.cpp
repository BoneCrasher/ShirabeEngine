#include "GFXAPI/Types/RenderTargetView.h"

namespace Engine {
  namespace GFXAPI {

    RenderTargetViewDeclaration::Descriptor
      ::Descriptor()
      : name("")
      , textureFormat(Format::UNKNOWN)
      , dimensionNb(0)
    {}

    std::string
      RenderTargetViewDeclaration::Descriptor
      ::toString() const {
      std::stringstream ss;

      ss
        << "RenderTargetDescriptor ('" << name << "'): "
        << " Format: " << (uint8_t)textureFormat << ";";

      return ss.str();
    }


    RenderTargetViewDeclaration::CreationRequest
      ::CreationRequest(
        Descriptor     const&desc,
        ResourceHandle const&underlyingTextureHandle)
      : BaseDeclaration::CreationRequestBase<Descriptor>(desc)
      , _underlyingTextureHandle(underlyingTextureHandle)
    {}
    
    ResourceHandle const&
      RenderTargetViewDeclaration::CreationRequest
      ::underlyingTextureHandle() const
    {
      return _underlyingTextureHandle;
    }

    std::string RenderTargetViewDeclaration::CreationRequest
      ::toString() const {
      std::stringstream ss;

      ss
        << "RenderTargetViewCreationRequest: \n"
        << "[\n"
        << resourceDescriptor().toString() << "\n"
        << "]"
        << std::endl;

      return ss.str();
    }

    RenderTargetViewDeclaration::UpdateRequest
      ::UpdateRequest(ResourceHandle const& handle)
      : BaseDeclaration::UpdateRequestBase(handle)
    {}

    RenderTargetViewDeclaration::DestructionRequest
      ::DestructionRequest(ResourceHandle const& handle)
      : BaseDeclaration::DestructionRequestBase(handle)
    {}

    RenderTargetViewDeclaration::Query
      ::Query(ResourceHandle const& handle)
      : BaseDeclaration::QueryBase(handle)
    {}

    RenderTargetViewDeclaration::Binding
      ::Binding()
      : BaseDeclaration::BindingBase()
    {}

    RenderTargetView::
      RenderTargetView(
        const RenderTargetView::Descriptor &descriptor,
        const RenderTargetView::Binding    &binding)
      : RenderTargetViewDeclaration()
      , ResourceDescriptorAdapter<RenderTargetViewDeclaration::Descriptor>(descriptor)
      , ResourceBindingAdapter<RenderTargetViewDeclaration::Binding>(binding)
    {}

  }
}