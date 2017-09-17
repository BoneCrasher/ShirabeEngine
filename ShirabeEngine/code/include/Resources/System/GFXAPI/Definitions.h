#ifndef __SHIRABE_RESOURCES_CORE_DESCRIPTORS_H__
#define __SHIRABE_RESOURCES_CORE_DESCRIPTORS_H__

#include <sstream>

#include "Platform/Platform.h"
#include "GAPI/Config.h"

#ifdef GAPI_USE_DX
#include "GAPI/DirectX/DX11/DX11Common.h"
#else 

#endif

#include "Core/EngineTypeHelper.h"
#include "Core/BasicTypes.h"

#include "Resources/System/Core/EResourceType.h"
#include "Resources/System/Core/IResourceDescriptor.h"

namespace Engine {
	namespace Resources {

		enum class ResourceUsage {
			CPU_None_GPU_ReadWrite      = 1, // i.e. DX11::D3D11_USAGE::Default
			CPU_InitConst_GPU_Read      = 2, // i.e. DX11::D3D11_USAGE::Immutable
			CPU_Write_GPU_Read          = 4, // i.e. DX11::D3D11_USAGE::Dynamic
			CPU_ReadWrite_GPU_ReadWrite = 8  // i.e. DX11::D3D11_USAGE::Staging
		};
		// No bitflags-type defined, since there's only one type of usage at once per resource.

		enum class BufferBinding {
			VertexBuffer                 = 1,
			IndexBuffer                  = 2,
			ConstantBuffer               = 4,
			ShaderResource               = 8,
			ShaderOutput_RenderTarget    = 16,
			ShaderOutput_StreamOutput    = 32,
			ShaderOutput_DepthStencil    = 64,
			UnorderedMultithreadedAccess = 128
		};
		DeclareEnumClassUnderlyingType(BufferBinding, BufferBindingFlags_t);

		enum class Format {
			UNKNOWN,
			RGBA_32_UINT,
			RGBA_32_SINT,
			RGBA_32_FLOAT,
			RGBA_32_TYPELESS,
			RGBA_16_UINT,
			RGBA_16_SINT,
			RGBA_16_UNORM,
			RGBA_16_SNORM,
			RGBA_16_FLOAT,
			RGBA_16_TYPELESS,
			RGBA_8_UINT,
			RGBA_8_SINT,
			RGBA_8_UNORM,
			RGBA_8_UNORM_SRGB,
			RGBA_8_SNORM,
			RGBA_8_TYPELESS,
			RGB_32_UINT,
			RGB_32_SINT,
			RGB_32_FLOAT,
			RGB_32_TYPELESS,
			D24_UNORM_S8_UINT,
			D32_FLOAT,
			R32_FLOAT,
			R32_UINT,
			R32_SINT,
			D32_FLOAT_S8X24_UINT,
			R32_FLOAT_X8X24_TYPELESS,
			BC1_UNORM,
			BC1_UNORM_SRGB,
			BC1_TYPELESS,
			BC2_UNORM,
			BC2_UNORM_SRGB,
			BC2_TYPELESS,
			BC3_UNORM,
			BC3_UNORM_SRGB,
			BC3_TYPELESS,
			BC4_UNORM,
			BC4_SNORM,
			BC4_TYPELESS,
			BC5_UNORM,
			BC5_SNORM,
			BC5_TYPELESS,
			BC6H_SF16,
			BC6H_UF16,
			BC6H_TYPELESS,
			BC7_UNORM,
			BC7_UNORM_SRGB,
			BC7_TYPELESS
		};

		enum class Comparison
			: uint8_t
		{
			Never        =  1,
			Less         =  2,
			Equal        =  4,
			LessEqual    =  6, // Less | Equal,
			Greater      =  8,
			NotEqual     = 16,
			GreaterEqual = 12, // Greater | Equal,
			Always       = 32
		};

		struct ShaderResourceDescriptorImpl {

		};


		struct DepthStencilViewDescriptorImpl {

		};

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
				StencilOp  _failOp;
				StencilOp  _depthFailOp;
				StencilOp  _passOp;
				Comparison _stencilFunc;
			};

			bool             _enableDepth;
			DepthWriteMask   _depthMask;
			Comparison       _depthFunc;
			bool             _enableStencil;
			uint8_t          _stencilReadMask;
			uint8_t          _stencilWriteMask;
			StencilCriteria  _stencilFrontfaceCriteria;
			StencilCriteria  _stencilBackfaceCriteria;
		};

		static D3D11_DEPTH_WRITE_MASK convertToDX11DepthWriteMask(const DepthStencilStateDescriptorImpl::DepthWriteMask& mask) {
			switch( mask ) {
			default:
			case DepthStencilStateDescriptorImpl::DepthWriteMask::All:
				return D3D11_DEPTH_WRITE_MASK_ALL;
			case DepthStencilStateDescriptorImpl::DepthWriteMask::Zero:
				return D3D11_DEPTH_WRITE_MASK_ZERO;
			}
		}

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