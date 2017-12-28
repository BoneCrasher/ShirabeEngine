#include "GFXAPI/Types/ShaderResourceView.h"


namespace Engine {
  namespace Resources {

    ShaderResourceViewDeclaration::Texture
      ::Texture()
      : dimensionNb(0)
      , dimensions({0, 0, 0})
      , isCube(false)
      , array()
      , mipMap()
    {}

    ShaderResourceViewDeclaration::StructuredBuffer
      ::StructuredBuffer()
      : firstElementOffset(0)
      , elementWidthInBytes(0)
    {}

    ShaderResourceViewDeclaration::ShaderResourceDimension
      ::ShaderResourceDimension()
      : texture()
      , structuredBuffer()
    {}

    ShaderResourceViewDeclaration::Binding
      ::Binding()
      : handle(ResourceHandle::Invalid())
    {}

    ShaderResourceViewDeclaration::Descriptor
      ::Descriptor()
      : name("")
      , shaderResourceDimension()
    {}

    ShaderResourceViewDeclaration::CreationRequest
      ::CreationRequest(
        Descriptor     const&desc,
        ResourceHandle const&underlyingBufferHandle)
      : _resourceDescriptor(desc)
      , _underlyingBufferHandle(underlyingBufferHandle)
    {}

    std::string
      ShaderResourceViewDeclaration::Descriptor
      ::toString() const
    {
      std::stringstream ss;

      ss
        << "RenderTargetDescriptor ('" << name << "'): \n"
        << " Format:     " << (uint8_t)format << "\n,";

      if(srvType == EShaderResourceDimension::Texture) {
        ss
          << " Dimensions:        " << (uint8_t)shaderResourceDimension.texture.dimensionNb << "\n,"
          << " Array:             " << (uint8_t)shaderResourceDimension.texture.array.size << "\n,"
          << " First array index: " << (uint8_t)shaderResourceDimension.texture.array.firstArraySlice << "\n,"
          << " MipMap:            " << (uint8_t)shaderResourceDimension.texture.mipMap.mipLevels << "\n,"
          << " Most Detailed MIP: " << (uint8_t)shaderResourceDimension.texture.mipMap.firstMipMapLevel << ";";
      }
      else { // StructuredBuffer
        ss
          << " First elem. off.: " << (uint8_t)shaderResourceDimension.structuredBuffer.firstElementOffset << ","
          << " Elem. byte-size:  " << (uint8_t)shaderResourceDimension.structuredBuffer.elementWidthInBytes << ";";
      }

      return ss.str();
    }

    ShaderResourceViewDeclaration::Descriptor const&
      ShaderResourceViewDeclaration::CreationRequest
      ::CreationRequest
      ::resourceDescriptor() const
    {
      return _resourceDescriptor;
    }

    ResourceHandle const&
      ShaderResourceViewDeclaration::CreationRequest
      ::underlyingBufferHandle() const
    {
      return _underlyingBufferHandle;
    }

    std::string
      ShaderResourceViewDeclaration::CreationRequest
      ::toString() const {
      std::stringstream ss;

      ss
        << "ShaderResourceViewCreationRequest: \n"
        << "[\n"
        << _resourceDescriptor.toString() << "\n"
        << "]"
        << std::endl;

      return ss.str();
    }

    ShaderResourceView
      ::ShaderResourceView(
        const ShaderResourceView::Descriptor &descriptor,
        const ShaderResourceView::Binding    &binding)
      : ShaderResourceViewDeclaration()
      , ResourceDescriptorAdapter<ShaderResourceViewDeclaration::Descriptor>(descriptor)
      , ResourceBindingAdapter<ShaderResourceViewDeclaration::Binding>(binding)
    {}
  }
}