#include "graphicsapi/resources/types/depthstencilstate.h"

namespace engine
{
    namespace gfxapi
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CDepthStencilStateDeclaration::SDescriptor::SDescriptor()
            : name("")
            , enableDepth(true)
            , depthMask(EDepthWriteMask::All)
            , depthFunc(EComparison::LessEqual)
            , enableStencil(false)
            , stencilReadMask(0x00)
            , stencilWriteMask(0x00)
            , stencilFrontfaceCriteria()
            , stencilBackfaceCriteria()
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        std::string CDepthStencilStateDeclaration::SDescriptor::toString() const
        {
            std::stringstream ss;
            ss << "DepthStencilStateDescriptor('" << name << "'):\n";

            return ss.str();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CDepthStencilStateDeclaration::CCreationRequest::CCreationRequest(SDescriptor const &aDescriptor)
            : CBaseDeclaration::CCreationRequestBase<SDescriptor>(aDescriptor)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        std::string CDepthStencilStateDeclaration::CCreationRequest::toString() const
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
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CDepthStencilState::CDepthStencilState(CDepthStencilState::SDescriptor const &aDescriptor)
            : CDepthStencilStateDeclaration()
            , CResourceDescriptorAdapter<CDepthStencilStateDeclaration::SDescriptor>(aDescriptor)
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
