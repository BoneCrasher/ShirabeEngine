#ifndef __SHIRABE_RESOURCETYPES_DEPTHSTENCIL_VIEW_H__
#define __SHIRABE_RESOURCETYPES_DEPTHSTENCIL_VIEW_H__

#include "Resources/System/Core/EResourceType.h"
#include "Resources/System/Core/IResource.h"
#include "Resources/System/Core/Handle.h"
#include "Resources/System/Core/ResourceDomainTransfer.h"

#include "Resources/Subsystems/GFXAPI/GFXAPI.h"

#include "GFXAPI/Definitions.h"
#include "GFXAPI/Types/TextureNDDefinition.h"

namespace Engine {
  namespace GFXAPI {
    using namespace Engine::Resources;

    class DepthStencilViewDeclaration {
    public:

      static const constexpr EResourceType    resource_type    = EResourceType::GAPI_VIEW;
      static const constexpr EResourceSubType resource_subtype = EResourceSubType::DEPTH_STENCIL_VIEW;
      
      /**********************************************************************************************//**
       * \struct	Texture
       *
       * \brief	Internal texture description to be filled according to the
       * 			underlying resource properties.
       **************************************************************************************************/
      struct Texture {
        uint8_t                 dimensionNb;
        TextureArrayDescriptor  array;
        TextureMipMapDescriptor mipMap;

        Texture();
      };

      /**********************************************************************************************//**
       * \struct	ShaderResourceDescriptorImpl
       *
       * \brief	A render target descriptor implementation.
       **************************************************************************************************/
      struct Descriptor {
        std::string name;
        Format      format;
        Texture     texture;

        Descriptor();

        std::string toString() const;
      };

      struct CreationRequest {
      public:
        CreationRequest(
          const Descriptor &desc);

        const Descriptor& resourceDescriptor() const;

        std::string toString() const;

      private:
        Descriptor _resourceDescriptor;
      };

      struct UpdateRequest {

      };


      struct DestructionRequest {

      };

      struct Query {

      };

      struct Binding {
        ResourceHandle handle;

        Binding();
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
      , public ResourceBindingAdapter<DepthStencilViewDeclaration::Binding>
    {
    public:
      using my_type = DepthStencilView;

      DepthStencilView(
        const DepthStencilView::Descriptor &descriptor,
        const DepthStencilView::Binding    &binding);
    };

    DeclareSharedPointerType(DepthStencilView);
    DefineTraitsPublicTypes(DepthStencilView);

  }
}

#endif
