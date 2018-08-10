#ifndef __SHIRABE_RESOURCETYPES_TEXTUREVIEW_H__
#define __SHIRABE_RESOURCETYPES_TEXTUREVIEW_H__

#include "resources/core/eresourcetype.h"
#include "resources/core/iresource.h"

#include "resources/core/resourcedomaintransfer.h"
#include "Resources/Core/ResourceTraits.h"
#include "Resources/Core/RequestDefaultImplementation.h"

#include "GraphicsAPI/Resources/GFXAPI.h"
#include "GraphicsAPI/Resources/Types/Definition.h"
#include "GraphicsAPI/Resources/Types/Texture.h"

namespace engine {
	namespace gfxapi {
    
		/**********************************************************************************************//**
		 * \struct	RenderTargetDescriptorImpl
		 *
		 * \brief	A render target descriptor implementation.
		 **************************************************************************************************/
		
    class SHIRABE_TEST_EXPORT TextureViewDeclaration {
    public:
      static const constexpr EResourceType    resource_type    = EResourceType::GAPI_VIEW;
      static const constexpr EResourceSubType resource_subtype = EResourceSubType::TEXTURE_VIEW;


      struct SHIRABE_TEST_EXPORT Descriptor
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

      struct SHIRABE_TEST_EXPORT CreationRequest
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

      class SHIRABE_TEST_EXPORT UpdateRequest
        : public BaseDeclaration::UpdateRequestBase
      {
      public:
        UpdateRequest(
          PublicResourceId_t const& inPublicResourceId);
      };

      class SHIRABE_TEST_EXPORT DestructionRequest
        : public BaseDeclaration::DestructionRequestBase
      {
      public:
        DestructionRequest(
          PublicResourceId_t const& inPublicResourceId);
      };

      class SHIRABE_TEST_EXPORT Query
        : public BaseDeclaration::QueryBase
      {
      public:
        Query(
          PublicResourceId_t const& inPublicResourceId);
      };
    };

		/**********************************************************************************************//**
		 * \class	GFXAPIRenderTarget
		 *
		 * \brief	A gfxapi render target.
		 **************************************************************************************************/
		class SHIRABE_TEST_EXPORT TextureView
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