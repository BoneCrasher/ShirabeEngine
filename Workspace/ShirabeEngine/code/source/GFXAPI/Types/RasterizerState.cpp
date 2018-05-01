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
      ::UpdateRequest(
        PublicResourceId_t    const& inPublicResourceId,
        SubjacentResourceId_t const& inSubjacentResourceId)
      : BaseDeclaration::UpdateRequestBase(inPublicResourceId, inSubjacentResourceId)
    {}

    RasterizerStateDeclaration::DestructionRequest
      ::DestructionRequest(
        PublicResourceId_t    const& inPublicResourceId,
        SubjacentResourceId_t const& inSubjacentResourceId)
      : BaseDeclaration::DestructionRequestBase(inPublicResourceId, inSubjacentResourceId)
    {}

    RasterizerStateDeclaration::Query
      ::Query(
        PublicResourceId_t    const& inPublicResourceId,
        SubjacentResourceId_t const& inSubjacentResourceId)
      : BaseDeclaration::QueryBase(inPublicResourceId, inSubjacentResourceId)
    {}

    RasterizerState
      ::RasterizerState(
        RasterizerState::Descriptor const &descriptor)
      : RasterizerStateDeclaration()
      , ResourceDescriptorAdapter<RasterizerState::Descriptor>(descriptor)
    {}
  }
}