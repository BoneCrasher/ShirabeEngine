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
      : _resourceDescriptor(desc)
      , _underlyingTextureHandle(underlyingTextureHandle)
    {}

    DepthStencilViewDeclaration::Descriptor const&
      DepthStencilViewDeclaration::CreationRequest
      ::resourceDescriptor() const
    {
      return _resourceDescriptor;
    }

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
        << _resourceDescriptor.toString() << "\n"
        << "]"
        << std::endl;

      return ss.str();
    }

    DepthStencilViewDeclaration::Binding
      ::Binding()
      : handle(ResourceHandle::Invalid())
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