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
      ::CreationRequest(const Descriptor &desc)
      : _resourceDescriptor(desc)
    {}

    const RenderTargetViewDeclaration::Descriptor&
      RenderTargetViewDeclaration::CreationRequest
      ::resourceDescriptor() const
    {
      return  _resourceDescriptor;
    }

    std::string RenderTargetViewDeclaration::CreationRequest
      ::toString() const {
      std::stringstream ss;

      ss
        << "RenderTargetViewCreationRequest: \n"
        << "[\n"
        << _resourceDescriptor.toString() << "\n"
        << "]"
        << std::endl;

      return ss.str();
    }

    RenderTargetViewDeclaration::Binding
      ::Binding()
      : handle(ResourceHandle::Invalid())
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