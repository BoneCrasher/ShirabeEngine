#include "GFXAPI/Types/Texture.h"

namespace Engine {
  namespace Resources {

    std::string
      TextureDeclaration::Descriptor
      ::toString() const
    {
      std::stringstream ss;

      ss
        << "TextureDescriptor"
        << " ('" << name << "'): "
        << " Dimensions: 3, "
        << " Format: " << (uint8_t)textureInfo.format << ", "
        << " MipMaps: " << (textureInfo.mipLevels > 1 ? "true" : "false") << "; Levels: " << textureInfo.mipLevels << ", "
        << " Array: " << (textureInfo.arraySize > 1 ? "true" : "false") << "; Layers: " << textureInfo.arraySize << ", "
        << " Multisampling: " << (textureInfo.multisampling.size > 1 ? "true" : "false")
        << "; Count/Quality:  " << textureInfo.multisampling.size << "/" << textureInfo.multisampling.quality << ", "
        << " CPU-GPU-Usage: " << (uint8_t)cpuGpuUsage << ", "
        << " GPU-Binding:  "  << (uint8_t)gpuBinding.value() << ";";

      return ss.str();
    }
    
    TextureDeclaration::CreationRequest
      ::CreationRequest(
        const Descriptor &desc)
      : BaseDeclaration::CreationRequestBase<Descriptor>(desc)
    {}

    std::string
      TextureDeclaration::CreationRequest
      ::toString() const
    {
      std::stringstream ss;

      ss
        << "TextureCreationRequest: \n"
        << "[\n"
        << resourceDescriptor().toString() << "\n"
        << "]"
        << std::endl;

      return ss.str();
    }

    TextureDeclaration::UpdateRequest
      ::UpdateRequest(
        PublicResourceId_t    const& inPublicResourceId,
        SubjacentResourceId_t const& inSubjacentResourceId)
      : BaseDeclaration::UpdateRequestBase(inPublicResourceId, inSubjacentResourceId)
    {}

    TextureDeclaration::DestructionRequest
      ::DestructionRequest(
        PublicResourceId_t    const& inPublicResourceId,
        SubjacentResourceId_t const& inSubjacentResourceId)
      : BaseDeclaration::DestructionRequestBase(inPublicResourceId, inSubjacentResourceId)
    {}

    TextureDeclaration::Query
      ::Query(
        PublicResourceId_t    const& inPublicResourceId,
        SubjacentResourceId_t const& inSubjacentResourceId)
      : BaseDeclaration::QueryBase(inPublicResourceId, inSubjacentResourceId)
    {}

    Texture
      ::Texture(
        TextureDeclaration::Descriptor const&descriptor)
      : TextureDeclaration()
      , ResourceDescriptorAdapter<TextureDeclaration::Descriptor>(descriptor)
    {}

  }
}