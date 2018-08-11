#include "GraphicsAPI/Resources//Types/RasterizerState.h"

namespace engine {
  namespace gfxapi {

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

    RasterizerStateDeclaration::CUpdateRequest
      ::CUpdateRequest(
        PublicResourceId_t const& inPublicResourceId)
      : BaseDeclaration::CUpdateRequestBase(inPublicResourceId)
    {}

    RasterizerStateDeclaration::CDestructionRequest
      ::CDestructionRequest(
        PublicResourceId_t const& inPublicResourceId)
      : BaseDeclaration::CDestructionRequestBase(inPublicResourceId)
    {}

    RasterizerStateDeclaration::Query
      ::Query(
        PublicResourceId_t const& inPublicResourceId)
      : BaseDeclaration::QueryBase(inPublicResourceId)
    {}

    RasterizerState
      ::RasterizerState(
        RasterizerState::Descriptor const &descriptor)
      : RasterizerStateDeclaration()
      , ResourceDescriptorAdapter<RasterizerState::Descriptor>(descriptor)
    {}
  }
}