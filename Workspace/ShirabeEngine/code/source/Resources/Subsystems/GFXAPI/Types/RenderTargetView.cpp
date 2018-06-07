#include "Resources/Subsystems/GFXAPI/Types/RenderTargetView.h"

namespace Engine {
  namespace GFXAPI {

    RenderTargetViewDeclaration::Descriptor
      ::Descriptor()
      : name("")
      , subjacentTexture()
      , textureFormat(Format::Undefined)
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
        Descriptor         const&desc,
        PublicResourceId_t const&underlyingTextureHandle)
      : BaseDeclaration::CreationRequestBase<Descriptor>(desc)
      , m_underlyingTextureHandle(underlyingTextureHandle)
    {}
    
    PublicResourceId_t const&
      RenderTargetViewDeclaration::CreationRequest
      ::underlyingTextureHandle() const
    {
      return m_underlyingTextureHandle;
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
      ::UpdateRequest(
        PublicResourceId_t    const& inPublicResourceId,
        SubjacentResourceId_t const& inSubjacentResourceId)
      : BaseDeclaration::UpdateRequestBase(inPublicResourceId, inSubjacentResourceId)
    {}

    RenderTargetViewDeclaration::DestructionRequest
      ::DestructionRequest(
        PublicResourceId_t    const& inPublicResourceId,
        SubjacentResourceId_t const& inSubjacentResourceId)
      : BaseDeclaration::DestructionRequestBase(inPublicResourceId, inSubjacentResourceId)
    {}

    RenderTargetViewDeclaration::Query
      ::Query(
        PublicResourceId_t    const& inPublicResourceId,
        SubjacentResourceId_t const& inSubjacentResourceId)
      : BaseDeclaration::QueryBase(inPublicResourceId, inSubjacentResourceId)
    {}

    RenderTargetView::
      RenderTargetView(
        const RenderTargetView::Descriptor &descriptor)
      : RenderTargetViewDeclaration()
      , ResourceDescriptorAdapter<RenderTargetViewDeclaration::Descriptor>(descriptor)
    {}

  }
}