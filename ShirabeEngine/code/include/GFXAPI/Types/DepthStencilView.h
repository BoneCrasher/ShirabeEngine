#ifndef __SHIRABE_RESOURCETYPES_DEPTHSTENCIL_H__
#define __SHIRABE_RESOURCETYPES_DEPTHSTENCIL_H__

#include "Resources/System/Core/EResourceType.h"
#include "Resources/System/Core/IResource.h"
#include "Resources/System/Core/Handle.h"
#include "Resources/System/Core/IResourceDescriptor.h"

#include "Resources/Subsystems/GFXAPI/GFXAPI.h"

#include "GFXAPI/Definitions.h"
#include "GFXAPI/Types/TextureND.h"

namespace Engine {
	namespace GFXAPI {
		using namespace Engine::Resources;

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

		/**********************************************************************************************//**
		 * \struct	ResourceDescriptor<EResourceType::GAPI_VIEW,EResourceSubType::DEPTH_STENCIL_VIEW>
		 *
		 * \brief	A depth stencil view>.
		 **************************************************************************************************/
	}

	namespace Resources {
		using namespace Engine::GFXAPI;

		template <>
		struct ResourceDescriptor<EResourceType::GAPI_VIEW, EResourceSubType::DEPTH_STENCIL_VIEW>
			: public DepthStencilViewDescriptorImpl
		{
			typedef DepthStencilViewDescriptorImpl type;
		};
		typedef ResourceDescriptor<EResourceType::GAPI_VIEW, EResourceSubType::DEPTH_STENCIL_VIEW> DepthStencilViewDescriptor;			
	}

	namespace GFXAPI {

		struct DepthStencilViewInfo {
			static const EResourceType    resource_type    = EResourceType::GAPI_VIEW;
			static const EResourceSubType resource_subtype = EResourceSubType::DEPTH_STENCIL_VIEW;
		};

		/**********************************************************************************************//**
		 * \class	DepthStencilView
		 *
		 * \brief	A gfxapi render target.
		 **************************************************************************************************/
		class DepthStencilView
			: public ResourceDescriptorAdapterBase<DepthStencilViewInfo::resource_type, DepthStencilViewInfo::resource_subtype>
		{
		public:
			using my_type         = DepthStencilView;
			using descriptor_type = ResourceDescriptor<DepthStencilViewInfo::resource_type, DepthStencilViewInfo::resource_subtype>;

			DepthStencilView(
				const descriptor_type &descriptor)
				: ResourceDescriptorAdapterBase<DepthStencilViewInfo::resource_type, DepthStencilViewInfo::resource_subtype>(descriptor)
			{}

		private:
			ResourceHandle _underlyingTexture; // Reference to the underlying texture resource used for the rendertarget.
		};
		DeclareSharedPointerType(DepthStencilView);

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

	}

	namespace Resources {

		/**********************************************************************************************//**
		 * \struct	ResourceDescriptor<EResourceType::GAPI_STATE,EResourceSubType::DEPTH_STENCIL_STATE>
		 *
		 * \brief	A depth stencil state>.
		 **************************************************************************************************/
		template <>
		struct ResourceDescriptor<EResourceType::GAPI_STATE, EResourceSubType::DEPTH_STENCIL_STATE>
			: public DepthStencilStateDescriptorImpl
		{
			typedef DepthStencilStateDescriptorImpl type;
		};
		typedef ResourceDescriptor<EResourceType::GAPI_STATE, EResourceSubType::DEPTH_STENCIL_STATE> DepthStencilStateDescriptor;
	}

	namespace GFXAPI {

		struct DepthStencilStateInfo {
			static const EResourceType    resource_type    = EResourceType::GAPI_STATE;
			static const EResourceSubType resource_subtype = EResourceSubType::DEPTH_STENCIL_STATE;
		};

		/**********************************************************************************************//**
		 * \class	DepthStencilView
		 *
		 * \brief	A gfxapi render target.
		 **************************************************************************************************/
		class DepthStencilState
			: public ResourceDescriptorAdapterBase<DepthStencilStateInfo::resource_type, DepthStencilStateInfo::resource_subtype> {
		public:
			using my_type         = DepthStencilState;
			using descriptor_type = ResourceDescriptor<DepthStencilStateInfo::resource_type, DepthStencilStateInfo::resource_subtype>;

			DepthStencilState(
				const descriptor_type &descriptor)
				: ResourceDescriptorAdapterBase<DepthStencilStateInfo::resource_type, DepthStencilStateInfo::resource_subtype>(descriptor)
			{}
		};
		DeclareSharedPointerType(DepthStencilState);

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