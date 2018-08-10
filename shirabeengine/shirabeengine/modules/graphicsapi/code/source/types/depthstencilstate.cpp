#include "GraphicsAPI/Resources/Types/DepthStencilState.h"

namespace engine {
  namespace gfxapi {

    DepthStencilStateDeclaration::Descriptor
      ::Descriptor()
      : name("")
      , enableDepth(true)
      , depthMask(DepthWriteMask::All)
      , depthFunc(Comparison::LessEqual)
      , enableStencil(false)
      , stencilReadMask(0x00)
      , stencilWriteMask(0x00)
      , stencilFrontfaceCriteria()
      , stencilBackfaceCriteria()
    {}

    std::string
      DepthStencilStateDeclaration::Descriptor
      ::toString() const {
      std::stringstream ss;

      ss
        << "DepthStencilStateDescriptor('" << name << "'):\n";

      return ss.str();
    }

    DepthStencilStateDeclaration::CreationRequest
      ::CreationRequest(Descriptor const&desc)
      : BaseDeclaration::CreationRequestBase<Descriptor>(desc)
    {}

    std::string
      DepthStencilStateDeclaration::CreationRequest
      ::toString() const
    {
      std::stringstream ss;

      ss
        << "DepthStencilStateCreationRequest: \n"
        << "[\n"
        << resourceDescriptor().toString() << "\n"
        << "]"
        << std::endl;

      return ss.str();
    }

    DepthStencilStateDeclaration::UpdateRequest
      ::UpdateRequest(
        PublicResourceId_t const& inPublicResourceId)
      : BaseDeclaration::UpdateRequestBase(inPublicResourceId)
    {}

    DepthStencilStateDeclaration::DestructionRequest
      ::DestructionRequest(
        PublicResourceId_t const& inPublicResourceId)
      : BaseDeclaration::DestructionRequestBase(inPublicResourceId)
    {}

    DepthStencilStateDeclaration::Query
      ::Query(
        PublicResourceId_t const& inPublicResourceId)
      : BaseDeclaration::QueryBase(inPublicResourceId)
    {}

    DepthStencilState
      ::DepthStencilState(
        DepthStencilState::Descriptor const &descriptor)
      : DepthStencilStateDeclaration()
      , ResourceDescriptorAdapter<DepthStencilStateDeclaration::Descriptor>(descriptor)
    {}

    ///**********************************************************************************************//**
    //* \fn	static D3D11_DEPTH_WRITE_MASK convertToDX11DepthWriteMask(const DepthStencilStateDescriptorImpl::DepthWriteMask& mask)
    //*
    //* \brief	Converts a mask to a dx 11 depth write mask
    //*
    //* \param	mask	The mask.
    //*
    //* \return	The given data converted to a dx 11 depth write mask.
    //**************************************************************************************************/
    //D3D11_DEPTH_WRITE_MASK convertToDX11DepthWriteMask(const DepthStencilState::Descriptor::DepthWriteMask& mask) {
    //  switch(mask) {
    //  default:
    //  case DepthStencilState::Descriptor::DepthWriteMask::All:
    //    return D3D11_DEPTH_WRITE_MASK_ALL;
    //  case DepthStencilState::Descriptor::DepthWriteMask::Zero:
    //    return D3D11_DEPTH_WRITE_MASK_ZERO;
    //  }
    //}

    ///**********************************************************************************************//**
    // * \fn	static D3D11_COMPARISON_FUNC convertToDX11ComparisonFunc(const Comparison& op)
    // *
    // * \brief	Converts an op to a dx 11 comparison function
    // *
    // * \param	op	The operation.
    // *
    // * \return	The given data converted to a dx 11 comparison function.
    // **************************************************************************************************/
    //D3D11_COMPARISON_FUNC convertToDX11ComparisonFunc(const Comparison& op) {
    //  switch(op) {
    //  default:
    //  case Comparison::Never:
    //    return D3D11_COMPARISON_NEVER;
    //  case Comparison::Less:
    //    return D3D11_COMPARISON_LESS;
    //  case Comparison::LessEqual:
    //    return D3D11_COMPARISON_LESS_EQUAL;
    //  case Comparison::Greater:
    //    return D3D11_COMPARISON_GREATER;
    //  case Comparison::NotEqual:
    //    return D3D11_COMPARISON_NOT_EQUAL;
    //  case Comparison::GreaterEqual:
    //    return D3D11_COMPARISON_GREATER_EQUAL;
    //  case Comparison::Always:
    //    return D3D11_COMPARISON_ALWAYS;
    //  }
    //}

    ///**********************************************************************************************//**
    // * \fn	static D3D11_STENCIL_OP convertToDX11StencilOp(const DepthStencilStateDescriptorImpl::StencilOp& op)
    // *
    // * \brief	Converts an op to a dx 11 stencil operation
    // *
    // * \param	op	The operation.
    // *
    // * \return	The given data converted to a dx 11 stencil operation.
    // **************************************************************************************************/
    //D3D11_STENCIL_OP convertToDX11StencilOp(const DepthStencilState::Descriptor::StencilOp& op) {
    //  switch(op) {
    //  default:
    //  case DepthStencilState::Descriptor::StencilOp::Keep:
    //    return D3D11_STENCIL_OP_KEEP;
    //  case DepthStencilState::Descriptor::StencilOp::Zero:
    //    return D3D11_STENCIL_OP_ZERO;
    //  case DepthStencilState::Descriptor::StencilOp::Replace:
    //    return D3D11_STENCIL_OP_REPLACE;
    //  case DepthStencilState::Descriptor::StencilOp::IncrementSaturate:
    //    return D3D11_STENCIL_OP_INCR_SAT;
    //  case DepthStencilState::Descriptor::StencilOp::DecrementSaturate:
    //    return D3D11_STENCIL_OP_DECR_SAT;
    //  case DepthStencilState::Descriptor::StencilOp::Invert:
    //    return D3D11_STENCIL_OP_INVERT;
    //  case DepthStencilState::Descriptor::StencilOp::Increment:
    //    return D3D11_STENCIL_OP_INCR;
    //  case DepthStencilState::Descriptor::StencilOp::Decrement:
    //    return D3D11_STENCIL_OP_DECR;
    //  }
    //}
  }
}