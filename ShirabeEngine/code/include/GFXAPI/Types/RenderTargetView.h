#ifndef __SHIRABE_RESOURCETYPES_RENDERTARGET_H__
#define __SHIRABE_RESOURCETYPES_RENDERTARGET_H__

#include "Resources/System/Core/EResourceType.h"
#include "Resources/System/Core/IResource.h"
#include "Resources/System/Core/Handle.h"
#include "Resources/System/Core/ResourceDomainTransfer.h"

#include "GFXAPI/Definitions.h"
#include "GFXAPI/Types/TextureNDDefinition.h"

#include "Resources/Subsystems/GFXAPI/GFXAPI.h"


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


      struct Descriptor {
        std::string             name;
        Format                  textureFormat;
        unsigned int            dimensionNb;
        TextureArrayDescriptor  array;
        TextureMipMapDescriptor mipMap;

        Descriptor();

        std::string toString() const;
      };

      struct CreationRequest {
      public:
        CreationRequest(
          Descriptor const&desc);

        const Descriptor& resourceDescriptor() const;

        std::string toString() const;
      private:
        Descriptor _resourceDescriptor;
      };
      
      struct UpdateRequest { };

      struct DestructionRequest {

      };

      struct Query {

      };

      struct Binding {
        ResourceHandle handle;

        inline Binding();
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