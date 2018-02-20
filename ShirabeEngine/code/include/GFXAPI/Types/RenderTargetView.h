#ifndef __SHIRABE_RESOURCETYPES_RENDERTARGET_H__
#define __SHIRABE_RESOURCETYPES_RENDERTARGET_H__

#include "Resources/System/Core/EResourceType.h"
#include "Resources/System/Core/IResource.h"
#include "Resources/System/Core/Handle.h"
#include "Resources/System/Core/ResourceDomainTransfer.h"
#include "Resources/System/Core/ResourceTraits.h"
#include "Resources/Subsystems/GFXAPI/GFXAPI.h"

#include "GFXAPI/Definitions.h"

#include "TextureNDDefinition.h"
#include "RequestDefaultImplementation.h"

namespace Engine {
	namespace GFXAPI {
    
		/**********************************************************************************************//**
		 * \struct	RenderTargetDescriptorImpl
		 *
		 * \brief	A render target descriptor implementation.
		 **************************************************************************************************/
		
    class RenderTargetViewDeclaration {
    public:
      static const constexpr EResourceType    resource_type    = EResourceType::GAPI_VIEW;
      static const constexpr EResourceSubType resource_subtype = EResourceSubType::RENDER_TARGET_VIEW;


      struct Descriptor
        : public DescriptorImplBase<EResourceType::GAPI_VIEW, EResourceSubType::RENDER_TARGET_VIEW> 
      {
        std::string             name;
        Format                  textureFormat;
        unsigned int            dimensionNb;
        TextureArrayDescriptor  array;
        TextureMipMapDescriptor mipMap;

        Descriptor();

        std::string toString() const;
      };

      struct CreationRequest 
        : public BaseDeclaration::CreationRequestBase<Descriptor> 
      {
      public:
        CreationRequest(
          Descriptor     const&desc,
          ResourceHandle const&underlyingTextureHandle);

        ResourceHandle const& underlyingTextureHandle() const;

        std::string toString() const;

      private:
        ResourceHandle m_underlyingTextureHandle;
      };
      
      struct UpdateRequest 
        : public BaseDeclaration::UpdateRequestBase
      {
      public:
        UpdateRequest(ResourceHandle const&);
      };

      struct DestructionRequest
        : public BaseDeclaration::DestructionRequestBase
      {
      public:
        DestructionRequest(ResourceHandle const&);
      };

      struct Query
        : public BaseDeclaration::QueryBase
      {
      public:
        Query(ResourceHandle const&);
      };
      
      struct Binding
        : public BaseDeclaration::BindingBase {
        ResourceHandle internalUnderlyingTextureHandle;

        Binding();
      };
    };

		/**********************************************************************************************//**
		 * \class	GFXAPIRenderTarget
		 *
		 * \brief	A gfxapi render target.
		 **************************************************************************************************/
		class RenderTargetView
			: public RenderTargetViewDeclaration
      , public ResourceDescriptorAdapter<RenderTargetViewDeclaration::Descriptor>
			, public ResourceBindingAdapter<RenderTargetViewDeclaration::Binding>
		{
		public:
			using my_type = RenderTargetView;

      RenderTargetView(
        const RenderTargetView::Descriptor &descriptor,
        const RenderTargetView::Binding    &binding);
		};

		DeclareSharedPointerType(RenderTargetView);
    DefineTraitsPublicTypes(RenderTargetView);
	}
}

#endif