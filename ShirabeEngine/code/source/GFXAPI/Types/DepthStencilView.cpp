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
        Descriptor     const&desc,
        ResourceHandle const&underlyingTextureHandle)
      : BaseDeclaration::CreationRequestBase<Descriptor>(desc)
      , _underlyingTextureHandle(underlyingTextureHandle)
    {}

    ResourceHandle const&
      DepthStencilViewDeclaration::CreationRequest
      ::underlyingTextureHandle() const
    {
      return _underlyingTextureHandle;
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
      ::UpdateRequest(ResourceHandle const& handle)
      : BaseDeclaration::UpdateRequestBase(handle)
    {}

    DepthStencilViewDeclaration::DestructionRequest
      ::DestructionRequest(ResourceHandle const& handle)
      : BaseDeclaration::DestructionRequestBase(handle)
    {}

    DepthStencilViewDeclaration::Query
      ::Query(ResourceHandle const& handle)
      : BaseDeclaration::QueryBase(handle)
    {}

    DepthStencilViewDeclaration::Binding
      ::Binding()
      : BaseDeclaration::BindingBase()
    {}
    
    DepthStencilView
      ::DepthStencilView(
      const DepthStencilView::Descriptor &descriptor,
      const DepthStencilView::Binding    &binding)
      : DepthStencilViewDeclaration()
      , ResourceDescriptorAdapter<DepthStencilViewDeclaration::Descriptor>(descriptor)
      , ResourceBindingAdapter<DepthStencilViewDeclaration::Binding>(binding)
    {}

  }
}