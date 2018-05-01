#include "GFXAPI/Types/DepthStencilView.h"

namespace Engine {
  namespace GFXAPI {

    DepthStencilViewDeclaration::Texture
      ::Texture()
      : dimensionNb(0)
      , array()
      , mipMap()
    {}

    DepthStencilViewDeclaration::Descriptor
      ::Descriptor()
      : name("")
      , format(Format::UNKNOWN)
      , texture()
    {}

    std::string
      DepthStencilViewDeclaration::Descriptor
      ::toString() const
    {
      std::stringstream ss;

      ss
        << "RenderTargetDescriptor ('" << name << "'):                             \n"
        << " Format:            " << (uint8_t)format << "\n,"
        << " Dimensions:        " << 2 << "\n,"
        << " Array:             " << (uint8_t)texture.array.size << "\n,"
        << " First array index: " << (uint8_t)texture.array.firstArraySlice << "\n,"
        << " MipMap:            " << (uint8_t)texture.mipMap.mipLevels << "\n,"
        << " Most Detailed MIP: " << (uint8_t)texture.mipMap.firstMipMapLevel << ";";

      return ss.str();
    }

    DepthStencilViewDeclaration::CreationRequest
      ::CreationRequest(
        Descriptor         const&desc,
        PublicResourceId_t const&underlyingTextureHandle)
      : BaseDeclaration::CreationRequestBase<Descriptor>(desc)
      , m_underlyingTextureHandle(underlyingTextureHandle)
    {}

    PublicResourceId_t const&
      DepthStencilViewDeclaration::CreationRequest
      ::underlyingTextureHandle() const
    {
      return m_underlyingTextureHandle;
    }

    std::string
      DepthStencilViewDeclaration::CreationRequest
      ::toString() const {
      std::stringstream ss;

      ss
        << "DepthStencilViewCreationRequest: \n"
        << "[\n"
        << resourceDescriptor().toString() << "\n"
        << "]"
        << std::endl;

      return ss.str();
    }
    
    DepthStencilViewDeclaration::UpdateRequest
      ::UpdateRequest(
        PublicResourceId_t    const& inPublicResourceId,
        SubjacentResourceId_t const& inSubjacentResourceId)
      : BaseDeclaration::UpdateRequestBase(inPublicResourceId, inSubjacentResourceId)
    {}

    DepthStencilViewDeclaration::DestructionRequest
      ::DestructionRequest(
        PublicResourceId_t    const& inPublicResourceId,
        SubjacentResourceId_t const& inSubjacentResourceId)
      : BaseDeclaration::DestructionRequestBase(inPublicResourceId, inSubjacentResourceId)
    {}

    DepthStencilViewDeclaration::Query
      ::Query(
        PublicResourceId_t    const& inPublicResourceId,
        SubjacentResourceId_t const& inSubjacentResourceId)
      : BaseDeclaration::QueryBase(inPublicResourceId, inSubjacentResourceId)
    {}

    DepthStencilView
      ::DepthStencilView(
      const DepthStencilView::Descriptor &descriptor)
      : DepthStencilViewDeclaration()
      , ResourceDescriptorAdapter<DepthStencilViewDeclaration::Descriptor>(descriptor)
    {}

  }
}