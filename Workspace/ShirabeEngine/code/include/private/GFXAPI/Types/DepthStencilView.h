#ifndef __SHIRABE_RESOURCETYPES_DEPTHSTENCIL_VIEW_H__
#define __SHIRABE_RESOURCETYPES_DEPTHSTENCIL_VIEW_H__

#include "Resources/Core/EResourceType.h"
#include "Resources/Core/IResource.h"

#include "Resources/Core/ResourceDomainTransfer.h"
#include "Resources/Core/ResourceTraits.h"
#include "Resources/Types/Texture.h"
#include "Resources/Subsystems/GFXAPI/GFXAPI.h"

#include "GFXAPI/Definitions.h"

#include "RequestDefaultImplementation.h"

namespace Engine {
  namespace GFXAPI {
    using namespace Engine::Resources;

    class DepthStencilViewDeclaration {
    public:

      static const constexpr EResourceType    resource_type    = EResourceType::GAPI_VIEW;
      static const constexpr EResourceSubType resource_subtype = EResourceSubType::DEPTH_STENCIL_VIEW;

      /**********************************************************************************************//**
       * \struct	ShaderResourceDescriptorImpl
       *
       * \brief	A render target descriptor implementation.
       **************************************************************************************************/
      struct Descriptor
        : public DescriptorImplBase<EResourceType::GAPI_VIEW, EResourceSubType::DEPTH_STENCIL_VIEW>
      {
        std::string name;
        Format      format;
        TextureInfo subjacentTexture;
        ArraySlices arraySlices;
        MipSlices   mipSlices;

        Descriptor();

        std::string toString() const;
      };

      class CreationRequest
        : public BaseDeclaration::CreationRequestBase<Descriptor>
      {
      public:
        CreationRequest(
          Descriptor         const&desc,
          PublicResourceId_t const&underlyingTextureHandle);

        PublicResourceId_t const& underlyingTextureHandle() const;

        std::string toString() const;

      private:
        PublicResourceId_t m_underlyingTextureHandle;
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
     * \class	DepthStencilView
     *
     * \brief	A gfxapi render target.
     **************************************************************************************************/
    class DepthStencilView
      : public DepthStencilViewDeclaration
      , public ResourceDescriptorAdapter<DepthStencilViewDeclaration::Descriptor>
    {
    public:
      using my_type = DepthStencilView;

      DepthStencilView(
        const DepthStencilView::Descriptor &descriptor);
    };

    DeclareSharedPointerType(DepthStencilView);
    DefineTraitsPublicTypes(DepthStencilView);

  }
}

#endif
