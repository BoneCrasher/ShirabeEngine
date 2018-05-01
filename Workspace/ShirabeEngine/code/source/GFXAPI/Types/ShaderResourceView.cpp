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
      
    ShaderResourceViewDeclaration::Descriptor
      ::Descriptor()
      : DescriptorImplBase<EResourceType::GAPI_VIEW, EResourceSubType::SHADER_RESOURCE_VIEW>()
      , shaderResourceDimension(Texture())
    {}

    ShaderResourceViewDeclaration::CreationRequest
      ::CreationRequest(
        Descriptor         const&desc,
        PublicResourceId_t const&underlyingBufferHandle)
      : BaseDeclaration::CreationRequestBase<Descriptor>(desc)
      , m_underlyingBufferHandle(underlyingBufferHandle)
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
        Texture texture = std::get<Texture>(shaderResourceDimension);
        ss
          << " Dimensions:        " << (uint8_t)texture.dimensionNb << "\n,"
          << " Array:             " << (uint8_t)texture.array.size << "\n,"
          << " First array index: " << (uint8_t)texture.array.firstArraySlice << "\n,"
          << " MipMap:            " << (uint8_t)texture.mipMap.mipLevels << "\n,"
          << " Most Detailed MIP: " << (uint8_t)texture.mipMap.firstMipMapLevel << ";";
      }
      else { // StructuredBuffer
        StructuredBuffer structuredBuffer = std::get<StructuredBuffer>(shaderResourceDimension);
        ss
          << " First elem. off.: " << (uint8_t)structuredBuffer.firstElementOffset << ","
          << " Elem. byte-size:  " << (uint8_t)structuredBuffer.elementWidthInBytes << ";";
      }

      return ss.str();
    }

    PublicResourceId_t const&
      ShaderResourceViewDeclaration::CreationRequest
      ::underlyingBufferHandle() const
    {
      return m_underlyingBufferHandle;
    }

    std::string
      ShaderResourceViewDeclaration::CreationRequest
      ::toString() const {
      std::stringstream ss;

      ss
        << "ShaderResourceViewCreationRequest: \n"
        << "[\n"
        << resourceDescriptor().toString() << "\n"
        << "]"
        << std::endl;

      return ss.str();
    }

    ShaderResourceViewDeclaration::UpdateRequest
      ::UpdateRequest(
        PublicResourceId_t    const& inPublicResourceId,
        SubjacentResourceId_t const& inSubjacentResourceId)
      : BaseDeclaration::UpdateRequestBase(inPublicResourceId, inSubjacentResourceId)
    {}

    ShaderResourceViewDeclaration::DestructionRequest
      ::DestructionRequest(
        PublicResourceId_t    const& inPublicResourceId,
        SubjacentResourceId_t const& inSubjacentResourceId)
      : BaseDeclaration::DestructionRequestBase(inPublicResourceId, inSubjacentResourceId)
    {}
    ShaderResourceViewDeclaration::Query
      ::Query(
        PublicResourceId_t    const& inPublicResourceId,
        SubjacentResourceId_t const& inSubjacentResourceId)
      : BaseDeclaration::QueryBase(inPublicResourceId, inSubjacentResourceId)
    {}
    
    ShaderResourceView
      ::ShaderResourceView(
        const ShaderResourceView::Descriptor &descriptor)
      : ShaderResourceViewDeclaration()
      , ResourceDescriptorAdapter<ShaderResourceViewDeclaration::Descriptor>(descriptor)
    {}
  }
}