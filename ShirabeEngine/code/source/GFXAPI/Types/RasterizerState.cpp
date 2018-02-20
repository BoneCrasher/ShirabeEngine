#include "GFXAPI/Types/RasterizerState.h"

namespace Engine {
  namespace GFXAPI {

    RasterizerStateDeclaration::Descriptor
      ::Descriptor()
      : name("")
      , fillMode(FillMode::Solid)
      , cullMode(CullMode::Back)
      , antialiasRasterLines(false)
      , multisamplingEnabled(false)
      , scissorEnabled(false)
      , depthClipEnabled(true)
    {}

    std::string
      RasterizerStateDeclaration::Descriptor
      ::toString() const {
      std::stringstream ss;

      ss
        << "RasterizerStateDescriptor('" << name << "'):\n";

      return ss.str();
    }

    RasterizerStateDeclaration::CreationRequest
      ::CreationRequest(Descriptor const&desc)
      : BaseDeclaration::CreationRequestBase<Descriptor>(desc)
    {}

    std::string
      RasterizerStateDeclaration::CreationRequest
      ::toString() const
    {
      std::stringstream ss;

      ss
        << "RasterizerStateCreationRequest: \n"
        << "[\n"
        << resourceDescriptor().toString() << "\n"
        << "]"
        << std::endl;

      return ss.str();
    }

    RasterizerStateDeclaration::UpdateRequest
      ::UpdateRequest(ResourceHandle const& handle)
      : BaseDeclaration::UpdateRequestBase(handle)
    {}

    RasterizerStateDeclaration::DestructionRequest
      ::DestructionRequest(ResourceHandle const& handle)
      : BaseDeclaration::DestructionRequestBase(handle)
    {}

    RasterizerStateDeclaration::Query
      ::Query(ResourceHandle const& handle)
      : BaseDeclaration::QueryBase(handle)
    {}

    RasterizerStateDeclaration::Binding
      ::Binding()
      : BaseDeclaration::BindingBase()
    {}

    RasterizerState
      ::RasterizerState(
        RasterizerState::Descriptor const &descriptor,
        RasterizerState::Binding    const &binding)
      : RasterizerStateDeclaration()
      , ResourceDescriptorAdapter<RasterizerState::Descriptor>(descriptor)
      , ResourceBindingAdapter<RasterizerState::Binding >(binding)
    {}
  }
}