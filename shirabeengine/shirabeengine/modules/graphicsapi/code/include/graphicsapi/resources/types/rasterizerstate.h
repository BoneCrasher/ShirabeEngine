#ifndef __SHIRABE_RESOURCETYPES_RASTERIZER_STATE_H__
#define __SHIRABE_RESOURCETYPES_RASTERIZER_STATE_H__

#include "resources/core/eresourcetype.h"
#include "resources/core/iresource.h"

#include "resources/core/resourcedomaintransfer.h"
#include "resources/core/resourcetraits.h"
#include "resources/core/requestdefaultimplementation.h"

#include "GraphicsAPI/Definitions.h"
#include "GraphicsAPI/Resources/GFXAPI.h"

namespace engine {
  namespace gfxapi {
    using namespace engine::Resources;

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

      class CUpdateRequest
        : public BaseDeclaration::CUpdateRequestBase
      {
      public:
        CUpdateRequest(
          PublicResourceId_t const& inPublicResourceId);
      };

      class CDestructionRequest
        : public BaseDeclaration::CDestructionRequestBase
      {
      public:
        CDestructionRequest(
          PublicResourceId_t const& inPublicResourceId);
      };

      class Query
        : public BaseDeclaration::QueryBase
      {
      public:
        Query(
          PublicResourceId_t const& inPublicResourceId);
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
    {
    public:
      RasterizerState(
        RasterizerState::Descriptor const &descriptor);
    };

    DeclareSharedPointerType(RasterizerState);
    DefineTraitsPublicTypes(RasterizerState);
  }
}

#endif
