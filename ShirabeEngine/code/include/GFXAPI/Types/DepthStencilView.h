#ifndef __SHIRABE_RESOURCETYPES_DEPTHSTENCIL_H__
#define __SHIRABE_RESOURCETYPES_DEPTHSTENCIL_H__

#include "Resources/System/Core/EResourceType.h"
#include "Resources/System/Core/IResource.h"
#include "Resources/System/Core/Handle.h"
#include "Resources/System/Core/ResourceDomainTransfer.h"

#include "Resources/Subsystems/GFXAPI/GFXAPI.h"

#include "GFXAPI/Definitions.h"
#include "GFXAPI/Types/TextureND.h"

namespace Engine {
	namespace GFXAPI {
		using namespace Engine::Resources;

    class DepthStencilView;
    class DepthStencilState;

		/**********************************************************************************************//**
		 * \struct	ShaderResourceDescriptorImpl
		 *
		 * \brief	A render target descriptor implementation.
		 **************************************************************************************************/
		struct DepthStencilViewDescriptorImpl {

			/**********************************************************************************************//**
			 * \struct	Texture
			 *
			 * \brief	Internal texture description to be filled according to the
			 * 			underlying resource properties.
			 **************************************************************************************************/
			struct Texture {
				VecND<uint32_t, 2>      dimensions;
				TextureArrayDescriptor  array;
				TextureMipMapDescriptor mipMap;

				inline Texture()
					: dimensions({ 0, 0 })
					, array()
					, mipMap()
				{}
			};

			std::string  name;
			Format       format;
			Texture      texture;

			DepthStencilViewDescriptorImpl()
				: name("")
				, format(Format::UNKNOWN)
				, texture()
			{}

			std::string toString() const {
				std::stringstream ss;

				ss
					<< "RenderTargetDescriptor ('" << name << "'):                             \n"
					<< " Format:            " << (uint8_t)format << "\n,"
					<< " Dimensions:        " << 2 << "\n,"
					<< " Array:             " << (uint8_t)texture.array.size << "\n,"
					<< " First array index: " << (uint8_t)texture.array.firstArraySlice << "\n,"
					<< " MipMap:            " << (uint8_t)texture.mipMap.mipLevels << "\n,"
					<< " Most Detailed MIP: " << (uint8_t)texture.mipMap.firstMipMapLevel << ";";

				return ss.str();
			}
		};

    struct DepthStencilViewCreationRequestImpl {
    public:
      inline const DepthStencilViewDescriptorImpl& resourceDescriptor() const { return _resourceDescriptor; }

      std::string toString() const {
        std::stringstream ss;

        ss
          << "DepthStencilViewCreationRequest: \n"
          << "[\n"
          << _resourceDescriptor.toString() << "\n"
          << "]"
          << std::endl;

        return ss.str();
      }
    private:
      DepthStencilViewDescriptorImpl _resourceDescriptor;
    };


    struct DepthStencilViewDestructionRequestImpl {

    };

    struct DepthStencilViewQueryRequestImpl {

    };

    struct DepthStencilViewResourceBinding {
      ResourceHandle handle;
    };

    DeclareResourceTraits(DepthStencilView,
                          DepthStencilView,
                          EResourceType::GAPI_VIEW,
                          EResourceSubType::DEPTH_STENCIL_VIEW,
                          DepthStencilViewResourceBinding,
                          DepthStencilViewDescriptorImpl,
                          DepthStencilViewCreationRequestImpl,
                          void,
                          DepthStencilViewQueryRequestImpl,
                          DepthStencilViewDestructionRequestImpl);

    DefineTraitsPublicTypes(DepthStencilView, DepthStencilViewTraits);

		/**********************************************************************************************//**
		 * \class	DepthStencilView
		 *
		 * \brief	A gfxapi render target.
		 **************************************************************************************************/
		class DepthStencilView
			: public DepthStencilViewTraits
      , public ResourceDescriptorAdapter<DepthStencilViewTraits>
			, public ResourceBindingAdapter<DepthStencilViewTraits>
		{
		public:
			using my_type = DepthStencilView;

			DepthStencilView(
				const DepthStencilViewDescriptor &descriptor,
				const DepthStencilViewBinding    &binding)
				: DepthStencilViewTraits()
        , ResourceDescriptorAdapter<DepthStencilViewTraits>(descriptor)
				, ResourceBindingAdapter<DepthStencilViewTraits>(binding)
			{}
		};
		DeclareSharedPointerType(DepthStencilView);

		typedef ResourceDescriptor<DepthStencilView> DepthStencilViewDescriptor;

		/**********************************************************************************************//**
		 * \struct	DepthStencilStateDescriptorImpl
		 *
		 * \brief	A depth stencil state descriptor implementation.
		 **************************************************************************************************/
		struct DepthStencilStateDescriptorImpl {
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

			DepthStencilStateDescriptorImpl()
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

			std::string toString() const {
				std::stringstream ss;

				ss
					<< "DepthStencilStateDescriptor('" << name << "'):\n";

				return ss.str();
			}
		};

		struct DepthStencilStateInfo {
		};
		

    struct DepthStencilStateCreationRequestImpl {
    public:
      inline const DepthStencilStateDescriptorImpl& resourceDescriptor() const { return _resourceDescriptor; }

      std::string toString() const {
        std::stringstream ss;

        ss
          << "DepthStencilStateCreationRequest: \n"
          << "[\n"
          << _resourceDescriptor.toString() << "\n"
          << "]"
          << std::endl;

        return ss.str();
      }
    private:
      DepthStencilStateDescriptorImpl _resourceDescriptor;
    };


    struct DepthStencilStateDestructionRequestImpl {

    };

    struct DepthStencilStateQueryRequestImpl {

    };

    struct DepthStencilStateResourceBinding {
      ResourceHandle handle;
    };

    DeclareResourceTraits(DepthStencilState,
                          DepthStencilState,
                          EResourceType::GAPI_STATE,
                          EResourceSubType::DEPTH_STENCIL_STATE,
                          DepthStencilStateResourceBinding,
                          DepthStencilStateDescriptorImpl,
                          DepthStencilStateCreationRequestImpl,
                          void,
                          DepthStencilStateQueryRequestImpl,
                          DepthStencilStateDestructionRequestImpl);

    DefineTraitsPublicTypes(DepthStencilState, DepthStencilStateTraits);

		/**********************************************************************************************//**
		 * \class	DepthStencilView
		 *
		 * \brief	A gfxapi render target.
		 **************************************************************************************************/
		class DepthStencilState
			: public DepthStencilStateTraits
      , public ResourceDescriptorAdapter<DepthStencilStateTraits> 
		  , public ResourceBindingAdapter<DepthStencilStateTraits> 
		{
		public:
			using my_type         = DepthStencilState;

			DepthStencilState(
				const DepthStencilStateDescriptor &descriptor,
				const DepthStencilStateBinding    &binding)
				: DepthStencilStateTraits()
        , ResourceDescriptorAdapter<DepthStencilStateTraits>(descriptor)
				, ResourceBindingAdapter<DepthStencilStateTraits>(binding)
			{}
		};
		DeclareSharedPointerType(DepthStencilState);

		typedef ResourceDescriptor<DepthStencilState> DepthStencilStateDescriptor;

		/**********************************************************************************************//**
		 * \fn	static D3D11_DEPTH_WRITE_MASK convertToDX11DepthWriteMask(const DepthStencilStateDescriptorImpl::DepthWriteMask& mask)
		 *
		 * \brief	Converts a mask to a dx 11 depth write mask
		 *
		 * \param	mask	The mask.
		 *
		 * \return	The given data converted to a dx 11 depth write mask.
		 **************************************************************************************************/
		static D3D11_DEPTH_WRITE_MASK convertToDX11DepthWriteMask(const DepthStencilStateDescriptorImpl::DepthWriteMask& mask) {
			switch( mask ) {
			default:
			case DepthStencilStateDescriptorImpl::DepthWriteMask::All:
				return D3D11_DEPTH_WRITE_MASK_ALL;
			case DepthStencilStateDescriptorImpl::DepthWriteMask::Zero:
				return D3D11_DEPTH_WRITE_MASK_ZERO;
			}
		}

		/**********************************************************************************************//**
		 * \fn	static D3D11_COMPARISON_FUNC convertToDX11ComparisonFunc(const Comparison& op)
		 *
		 * \brief	Converts an op to a dx 11 comparison function
		 *
		 * \param	op	The operation.
		 *
		 * \return	The given data converted to a dx 11 comparison function.
		 **************************************************************************************************/
		static D3D11_COMPARISON_FUNC convertToDX11ComparisonFunc(const Comparison& op) {
			switch( op ) {
			default:
			case Comparison::Never:
				return D3D11_COMPARISON_NEVER;
			case Comparison::Less:
				return D3D11_COMPARISON_LESS;
			case Comparison::LessEqual:
				return D3D11_COMPARISON_LESS_EQUAL;
			case Comparison::Greater:
				return D3D11_COMPARISON_GREATER;
			case Comparison::NotEqual:
				return D3D11_COMPARISON_NOT_EQUAL;
			case Comparison::GreaterEqual:
				return D3D11_COMPARISON_GREATER_EQUAL;
			case Comparison::Always:
				return D3D11_COMPARISON_ALWAYS;
			}
		}

		/**********************************************************************************************//**
		 * \fn	static D3D11_STENCIL_OP convertToDX11StencilOp(const DepthStencilStateDescriptorImpl::StencilOp& op)
		 *
		 * \brief	Converts an op to a dx 11 stencil operation
		 *
		 * \param	op	The operation.
		 *
		 * \return	The given data converted to a dx 11 stencil operation.
		 **************************************************************************************************/
		static D3D11_STENCIL_OP convertToDX11StencilOp(const DepthStencilStateDescriptorImpl::StencilOp& op) {
			switch( op ) {
			default:
			case DepthStencilStateDescriptorImpl::StencilOp::Keep:
				return D3D11_STENCIL_OP_KEEP;
			case DepthStencilStateDescriptorImpl::StencilOp::Zero:
				return D3D11_STENCIL_OP_ZERO;
			case DepthStencilStateDescriptorImpl::StencilOp::Replace:
				return D3D11_STENCIL_OP_REPLACE;
			case DepthStencilStateDescriptorImpl::StencilOp::IncrementSaturate:
				return D3D11_STENCIL_OP_INCR_SAT;
			case DepthStencilStateDescriptorImpl::StencilOp::DecrementSaturate:
				return D3D11_STENCIL_OP_DECR_SAT;
			case DepthStencilStateDescriptorImpl::StencilOp::Invert:
				return D3D11_STENCIL_OP_INVERT;
			case DepthStencilStateDescriptorImpl::StencilOp::Increment:
				return D3D11_STENCIL_OP_INCR;
			case DepthStencilStateDescriptorImpl::StencilOp::Decrement:
				return D3D11_STENCIL_OP_DECR;
			}
		}
	}
}

#endif
