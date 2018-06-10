#ifndef __SHIRABE_RESOURCETYPES_TEXTUREVIEW_H__
#define __SHIRABE_RESOURCETYPES_TEXTUREVIEW_H__

#include "Resources/Core/EResourceType.h"
#include "Resources/Core/IResource.h"

#include "Resources/Core/ResourceDomainTransfer.h"
#include "Resources/Core/ResourceTraits.h"

#include "Resources/Subsystems/GFXAPI/GFXAPI.h"
#include "Resources/Subsystems/GFXAPI/Types/Texture.h"

#include "GFXAPI/Definitions.h"

#include "Resources/Core/RequestDefaultImplementation.h"

namespace Engine {
	namespace GFXAPI {
    
		/**********************************************************************************************//**
		 * \struct	RenderTargetDescriptorImpl
		 *
		 * \brief	A render target descriptor implementation.
		 **************************************************************************************************/
		
    class TextureViewDeclaration {
    public:
      static const constexpr EResourceType    resource_type    = EResourceType::GAPI_VIEW;
      static const constexpr EResourceSubType resource_subtype = EResourceSubType::TEXTURE_VIEW;


      struct Descriptor
        : public DescriptorImplBase<EResourceType::GAPI_VIEW, EResourceSubType::TEXTURE_VIEW>
      {
        std::string name;
        TextureInfo subjacentTexture;
        Format      textureFormat;
        ArraySlices arraySlices;
        MipSlices   mipMapSlices;
        // TODO: Distinguish binding and read/write mode

        Descriptor();

        std::string toString() const;
      };

      struct CreationRequest 
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
		 * \class	GFXAPIRenderTarget
		 *
		 * \brief	A gfxapi render target.
		 **************************************************************************************************/
		class TextureView
			: public TextureViewDeclaration
      , public ResourceDescriptorAdapter<TextureViewDeclaration::Descriptor>
		{
		public:
			using my_type = TextureView;

      TextureView(
        const TextureView::Descriptor &descriptor);
		};

		DeclareSharedPointerType(TextureView);
    DefineTraitsPublicTypes(TextureView);
	}
}

#endif