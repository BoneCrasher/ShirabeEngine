#ifndef __SHIRABE_RESOURCETYPES_SHADERRESOURCE_H__
#define __SHIRABE_RESOURCETYPES_SHADERRESOURCE_H__

#include "Resources/Core/EResourceType.h"
#include "Resources/Core/IResource.h"
#include "Resources/Core/ResourceDTO.h"
#include "Resources/Core/ResourceDomainTransfer.h"
#include "Resources/Core/ResourceTraits.h"

#include "Resources/Subsystems/GFXAPI/GFXAPI.h"

#include "GFXAPI/Definitions.h"

#include "TextureNDDefinition.h"
#include "RequestDefaultImplementation.h"


namespace Engine {
  namespace Resources {
    using namespace GFXAPI;

    /**********************************************************************************************//**
     * \class ShaderResourceViewDeclaration
     *
     * \brief A shader resource view declaration.
     **************************************************************************************************/
    class ShaderResourceViewDeclaration {
    public:
      static const constexpr EResourceType    resource_type    = EResourceType::GAPI_VIEW;
      static const constexpr EResourceSubType resource_subtype = EResourceSubType::SHADER_RESOURCE_VIEW;

      /**********************************************************************************************//**
       * \struct  Texture
       *
       * \brief A texture.
       **************************************************************************************************/
      struct Texture {
        unsigned int            dimensionNb; // 1..3
        TVector3D<uint32_t>     dimensions;
        bool                    isCube; // Implies the dimensions[2] to be 6
        TextureArrayDescriptor  array;
        TextureMipMapDescriptor mipMap;

        Texture();          
      };

      /**********************************************************************************************//**
       * \struct  StructuredBuffer
       *
       * \brief Buffer for structured.
       **************************************************************************************************/
      struct StructuredBuffer {
        unsigned int firstElementOffset;
        unsigned int elementWidthInBytes;

        StructuredBuffer();
      };

      /**********************************************************************************************//**
       * \union ShaderResourceDimension
       *
       * \brief A shader resource dimension.
       **************************************************************************************************/
      union ShaderResourceDimension {
        Texture          texture;
        StructuredBuffer structuredBuffer;

        ShaderResourceDimension();
      };

      /**********************************************************************************************//**
       * \struct	ShaderResourceDescriptorImpl
       *
       * \brief	A render target descriptor implementation.
       **************************************************************************************************/
      struct Descriptor 
        : public DescriptorImplBase<EResourceType::GAPI_VIEW, EResourceSubType::SHADER_RESOURCE_VIEW>
      {
        enum class EShaderResourceDimension {
          Texture,
          StructuredBuffer
        };

        Format                   format;
        EShaderResourceDimension srvType;
        ShaderResourceDimension  shaderResourceDimension;

        Descriptor();

        std::string toString() const;
      };

      /**********************************************************************************************//**
       * \struct  CreationRequest
       *
       * \brief A creation request.
       **************************************************************************************************/
      struct CreationRequest
        : public BaseDeclaration::CreationRequestBase<Descriptor>
      {
      public:
        CreationRequest(
          Descriptor         const&desc,
          PublicResourceId_t const&underlyingBufferHandle);

        PublicResourceId_t const& underlyingBufferHandle() const;

        std::string toString() const;

      private:
        PublicResourceId_t m_underlyingBufferHandle;
      };

      class UpdateRequest
        : public BaseDeclaration::UpdateRequestBase
      {
      public:
        UpdateRequest(
          PublicResourceId_t    const& inPublicResourceId,
          SubjacentResourceId_t const& inSubjacentResourceId);
      };

      class DestructionRequest
        : public BaseDeclaration::DestructionRequestBase
      {
      public:
        DestructionRequest(
          PublicResourceId_t    const& inPublicResourceId,
          SubjacentResourceId_t const& inSubjacentResourceId);
      };

      class Query
        : public BaseDeclaration::QueryBase
      {
      public:
        Query(
          PublicResourceId_t    const& inPublicResourceId,
          SubjacentResourceId_t const& inSubjacentResourceId);
      };
    };
    
    /**********************************************************************************************//**
     * \class	GFXAPIShaderResource
     *
     * \brief	A gfxapi render target.
     **************************************************************************************************/
    class ShaderResourceView
      : public ShaderResourceViewDeclaration
      , public ResourceDescriptorAdapter<ShaderResourceViewDeclaration::Descriptor>
    {
    public:
      using my_type = ShaderResourceView;

      ShaderResourceView(
        const ShaderResourceView::Descriptor &descriptor);
    };

    DeclareSharedPointerType(ShaderResourceView);
    DefineTraitsPublicTypes(ShaderResourceView);
  }
}

#endif

