#ifndef __SHIRABE_RESOURCETYPES_DEPTHSTENCIL_STATE_H__
#define __SHIRABE_RESOURCETYPES_DEPTHSTENCIL_STATE_H__

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
    using namespace Engine::Resources;

    class DepthStencilStateDeclaration {
    public:
      static const constexpr EResourceType    resource_type    = EResourceType::GAPI_STATE;
      static const constexpr EResourceSubType resource_subtype = EResourceSubType::DEPTH_STENCIL_STATE;

      /**********************************************************************************************//**
       * \struct	DepthStencilStateDescriptorImpl
       *
       * \brief	A depth stencil state descriptor implementation.
       **************************************************************************************************/
      struct Descriptor 
        : public DescriptorImplBase<EResourceType::GAPI_STATE, EResourceSubType::DEPTH_STENCIL_STATE>
      {
        enum class DepthWriteMask
          : uint8_t
        {
          Zero = 0,
          All
        };

        enum class StencilOp
          : uint8_t
        {
          Keep = 1,          // Do nothing
          Zero,              // Write 0
          Replace,           // Write RefValue
          IncrementSaturate, // Increment and clamp to MaxValue, if exceeded
          DecrementSaturate, // Decrement and clamp to 0, if exceeded
          Invert,            // Invert the bits
          Increment,         // Increment and wrap to 0, if exceeded
          Decrement          // Decrement and wrap to MaxValue, if exceeded
        };

        struct StencilCriteria {
          StencilOp   failOp;
          StencilOp   depthFailOp;
          StencilOp   passOp;
          Comparison  stencilFunc;
        };

        std::string name;

        bool             enableDepth;
        DepthWriteMask   depthMask;
        Comparison       depthFunc;
        bool             enableStencil;
        uint8_t          stencilReadMask;
        uint8_t          stencilWriteMask;
        StencilCriteria  stencilFrontfaceCriteria;
        StencilCriteria  stencilBackfaceCriteria;

        Descriptor();
        
        std::string toString() const;       
      };

      struct DepthStencilStateInfo {
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
    class DepthStencilState
      : public DepthStencilStateDeclaration
      , public ResourceDescriptorAdapter<DepthStencilStateDeclaration::Descriptor>
      , public ResourceBindingAdapter<DepthStencilStateDeclaration::Binding>
    {
    public:
      DepthStencilState(
        DepthStencilState::Descriptor const &descriptor,
        DepthStencilState::Binding    const &binding);
    };

    DeclareSharedPointerType(DepthStencilState);
    DefineTraitsPublicTypes(DepthStencilState);

    /**********************************************************************************************//**
     * \fn	static D3D11_DEPTH_WRITE_MASK convertToDX11DepthWriteMask(const DepthStencilStateDescriptorImpl::DepthWriteMask& mask)
     *
     * \brief	Converts a mask to a dx 11 depth write mask
     *
     * \param	mask	The mask.
     *
     * \return	The given data converted to a dx 11 depth write mask.
     **************************************************************************************************/
    static D3D11_DEPTH_WRITE_MASK convertToDX11DepthWriteMask(const DepthStencilState::Descriptor::DepthWriteMask& mask);
    static D3D11_COMPARISON_FUNC  convertToDX11ComparisonFunc(const Comparison& op);
    static D3D11_STENCIL_OP       convertToDX11StencilOp(const DepthStencilState::Descriptor::StencilOp& op);
  }
}

#endif
