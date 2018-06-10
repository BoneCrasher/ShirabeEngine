#include "Resources/Subsystems/GFXAPI/Types/TextureView.h"

namespace Engine {
  namespace GFXAPI {

    TextureViewDeclaration::Descriptor
      ::Descriptor()
      : name("")
      , subjacentTexture()
      , textureFormat(Format::Undefined)
    {}

    std::string
      TextureViewDeclaration::Descriptor
      ::toString() const {
      std::stringstream ss;

      ss
        << "RenderTargetDescriptor ('" << name << "'): "
        << " Format: " << (uint8_t)textureFormat << ";";

      return ss.str();
    }


    TextureViewDeclaration::CreationRequest
      ::CreationRequest(
        Descriptor         const&desc,
        PublicResourceId_t const&underlyingTextureHandle)
      : BaseDeclaration::CreationRequestBase<Descriptor>(desc)
      , m_underlyingTextureHandle(underlyingTextureHandle)
    {}
    
    PublicResourceId_t const&
      TextureViewDeclaration::CreationRequest
      ::underlyingTextureHandle() const
    {
      return m_underlyingTextureHandle;
    }

    std::string TextureViewDeclaration::CreationRequest
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

    TextureViewDeclaration::UpdateRequest
      ::UpdateRequest(
        PublicResourceId_t    const& inPublicResourceId,
        SubjacentResourceId_t const& inSubjacentResourceId)
      : BaseDeclaration::UpdateRequestBase(inPublicResourceId, inSubjacentResourceId)
    {}

    TextureViewDeclaration::DestructionRequest
      ::DestructionRequest(
        PublicResourceId_t    const& inPublicResourceId,
        SubjacentResourceId_t const& inSubjacentResourceId)
      : BaseDeclaration::DestructionRequestBase(inPublicResourceId, inSubjacentResourceId)
    {}

    TextureViewDeclaration::Query
      ::Query(
        PublicResourceId_t    const& inPublicResourceId,
        SubjacentResourceId_t const& inSubjacentResourceId)
      : BaseDeclaration::QueryBase(inPublicResourceId, inSubjacentResourceId)
    {}

    TextureView::
      TextureView(
        const TextureView::Descriptor &descriptor)
      : TextureViewDeclaration()
      , ResourceDescriptorAdapter<TextureViewDeclaration::Descriptor>(descriptor)
    {}

  }
}