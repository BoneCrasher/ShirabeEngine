#ifndef __SHIRABE_RESOURCETYPES_RASTERIZER_STATE_H__
#define __SHIRABE_RESOURCETYPES_RASTERIZER_STATE_H__

#include "Resources/System/Core/EResourceType.h"
#include "Resources/System/Core/IResource.h"
#include "Resources/System/Core/Handle.h"
#include "Resources/System/Core/ResourceDomainTransfer.h"
#include "Resources/System/Core/ResourceTraits.h"

#include "Resources/Subsystems/GFXAPI/GFXAPI.h"

#include "GFXAPI/Definitions.h"

#include "RequestDefaultImplementation.h"

namespace Engine {
  namespace GFXAPI {
    using namespace Engine::Resources;

    class RasterizerStateDeclaration {
    public:
      static const constexpr EResourceType    resource_type    = EResourceType::GAPI_STATE;
      static const constexpr EResourceSubType resource_subtype = EResourceSubType::RASTERIZER_STATE;

      enum class FillMode {
        WireFrame,
        Solid
      };

      enum class CullMode {
        None,
        Front, 
        Back
      }; // In any case, we expect front to be counter clockwise!   

      /**********************************************************************************************//**
       * \struct	DepthStencilStateDescriptorImpl
       *
       * \brief	A depth stencil state descriptor implementation.
       **************************************************************************************************/
      struct Descriptor 
        : public DescriptorImplBase<EResourceType::GAPI_STATE, EResourceSubType::RASTERIZER_STATE>
      {
        std::string name;

        FillMode fillMode;
        CullMode cullMode;

        bool
          antialiasRasterLines,
          multisamplingEnabled, // Requires respective MS format
          scissorEnabled,
          depthClipEnabled;

        Descriptor();
        
        std::string toString() const;       
      };
      
      struct CreationRequest
        : public BaseDeclaration::CreationRequestBase<Descriptor> {
      public:
        CreationRequest(Descriptor const&desc);

        std::string toString() const;
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
        : public BaseDeclaration::BindingBase
      {
        Binding();
      };
    };

    /**********************************************************************************************//**
     * \class	DepthStencilView
     *
     * \brief	A gfxapi render target.
     **************************************************************************************************/
    class RasterizerState
      : public RasterizerStateDeclaration
      , public ResourceDescriptorAdapter<RasterizerStateDeclaration::Descriptor>
      , public ResourceBindingAdapter<RasterizerStateDeclaration::Binding>
    {
    public:
      RasterizerState(
        RasterizerState::Descriptor const &descriptor,
        RasterizerState::Binding    const &binding);
    };

    DeclareSharedPointerType(RasterizerState);
    DefineTraitsPublicTypes(RasterizerState);
  }
}

#endif
