#ifndef __SHIRABE_RESOURCEDESCRIPTORS_H__
#define __SHIRABE_RESOURCEDESCRIPTORS_H__

#include <sstream>

#include "Platform/Platform.h"
#include "GAPI/Config.h"

#ifdef GAPI_USE_DX
#include "GAPI/DirectX/DX11/DX11Common.h"
#else 

#endif

#include "Core/EngineTypeHelper.h"
#include "Core/BasicTypes.h"

#include "Resources/EResourceType.h"
#include "Resources/IResourceDescriptor.h"

namespace Engine {
	namespace Resources {

		enum class ResourceUsage {
			CPU_None_GPU_ReadWrite = 1, // i.e. DX11::D3D11_USAGE::Default
			CPU_InitConst_GPU_Read = 2, // i.e. DX11::D3D11_USAGE::Immutable
			CPU_Write_GPU_Read = 4, // i.e. DX11::D3D11_USAGE::Dynamic
			CPU_ReadWrite_GPU_ReadWrite = 8  // i.e. DX11::D3D11_USAGE::Staging
		};
		// No bitflags-type defined, since there's only one type of usage at once per resource.

		enum class BufferBinding {
			VertexBuffer = 1,
			IndexBuffer = 2,
			ConstantBuffer = 4,
			ShaderResource = 8,
			ShaderOutput_RenderTarget = 16,
			ShaderOutput_StreamOutput = 32,
			ShaderOutput_DepthStencil = 64,
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

		struct TextureMipMapDescriptor {
			bool    _useMipMaps;
			uint8_t _mipLevels;
		};

		struct TextureArrayDescriptor {
			bool    _isTextureArray;
			uint8_t _textureCount;
		};

		struct TextureMultisapmlingDescriptor {
			bool    _useMultisampling;
			uint8_t _count;
			uint8_t _quality;
		};

		struct ShaderResourceDescriptorImpl {

		};

		struct RenderTargetDescriptorImpl {
			std::string             _name;
			Format                  _textureFormat;
			unsigned int            _dimensions;
			TextureArrayDescriptor  _array;
			TextureMipMapDescriptor _mipMap;

			RenderTargetDescriptorImpl()
				: _name("")
				, _textureFormat(Format::UNKNOWN)
				, _dimensions(0)
			{}

			std::string toString() const {
				std::stringstream ss;

				ss
					<< "RenderTargetDescriptor ('" << _name << "'): "
					<< " Format: "        << (uint8_t) _textureFormat <<  ";";

				return ss.str();
			}
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
			enum class PassCriteria 
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
				StencilOp    _failOp;
				StencilOp    _depthFailOp;
				StencilOp    _passOp;
				PassCriteria _stencilFunc;
			};

			bool             _enableDepth;
			DepthWriteMask   _depthMask;
			PassCriteria     _depthFunc;
			bool             _enableStencil;
			uint8_t          _stencilReadMask;
			uint8_t          _stencilWriteMask;
			StencilCriteria  _stencilFrontfaceCriteria;
			StencilCriteria  _stencilBackfaceCriteria;
		};

		static D3D11_DEPTH_WRITE_MASK convertToDX11DepthWriteMask(const DepthStencilStateDescriptorImpl::DepthWriteMask& mask) {
			switch (mask) {
			default:
			case DepthStencilStateDescriptorImpl::DepthWriteMask::All:
				return D3D11_DEPTH_WRITE_MASK_ALL;
			case DepthStencilStateDescriptorImpl::DepthWriteMask::Zero:
				return D3D11_DEPTH_WRITE_MASK_ZERO;
			}
		}

		static D3D11_COMPARISON_FUNC convertToDX11ComparisonFunc(const DepthStencilStateDescriptorImpl::PassCriteria& op) {
			switch (op) {
			default:
			case DepthStencilStateDescriptorImpl::PassCriteria::Never:
				return D3D11_COMPARISON_NEVER;
			case DepthStencilStateDescriptorImpl::PassCriteria::Less:
				return D3D11_COMPARISON_LESS;
			case DepthStencilStateDescriptorImpl::PassCriteria::LessEqual:
				return D3D11_COMPARISON_LESS_EQUAL;
			case DepthStencilStateDescriptorImpl::PassCriteria::Greater:
				return D3D11_COMPARISON_GREATER;
			case DepthStencilStateDescriptorImpl::PassCriteria::NotEqual:
				return D3D11_COMPARISON_NOT_EQUAL;
			case DepthStencilStateDescriptorImpl::PassCriteria::GreaterEqual:
				return D3D11_COMPARISON_GREATER_EQUAL;
			case DepthStencilStateDescriptorImpl::PassCriteria::Always:
				return D3D11_COMPARISON_ALWAYS;
			}
		}

		static D3D11_STENCIL_OP convertToDX11StencilOp(const DepthStencilStateDescriptorImpl::StencilOp& op) {
			switch (op) {
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

		template <uint8_t N>
		struct TextureDescriptorImpl {
			static const uint8_t Dimensions = N;

			std::string                    _name;
			Format                         _textureFormat;
			VecND<uint32_t, N>             _dimensions;
			TextureMipMapDescriptor        _mipMap;
			TextureArrayDescriptor         _array;
			TextureMultisapmlingDescriptor _multisampling;
			ResourceUsage                  _cpuGpuUsage;
			BufferBindingFlags_t           _gpuBinding;

			std::string toString() const {
				std::stringstream ss; 

				ss
					<< "TextureDescriptor<" << N << ">"
					<< " ('" << _name << "'): "
					<< " Dimensions: "    << N << ", "
					<< " Format: "        << (uint8_t) _textureFormat << ", "
					<< " MipMaps: "       << (_mipMap._useMipMaps              ? "true" : "false") << "; Levels: " << _mipMap._mipLevels   << ", "
					<< " Array: "         << (_array._isTextureArray           ? "true" : "false") << "; Layers: " << _array._textureCount << ", "
					<< " Multisampling: " << (_multisampling._useMultisampling ? "true" : "false")
					  << "; Count/Quality:  " << _multisampling._count << "/" << _multisampling._quality << ", "
					<< " CPU-GPU-Usage: "   << (uint8_t)_cpuGpuUsage << ", "
					<< " GPU-Binding:  "    << (uint8_t)_gpuBinding  << ";";

				return ss.str();
			}
		};

		template <uint8_t N>
		std::ostream& operator<<(std::ostream& s, const TextureDescriptorImpl<N>& d) {
			return (s << d.toString());
		}


		struct SwapChainDescriptorImpl {
			std::string              _name;
			TextureDescriptorImpl<2> _texture;
			bool                     _vsyncEnabled;
			bool                     _fullscreen;
			unsigned int             _windowHandle;
			unsigned int             _backBufferCount;
			unsigned int             _refreshRateNumerator;
			unsigned int             _refreshRateDenominator;

			std::string toString() const {
				std::stringstream ss;

				ss
					<< "SwapChainDescriptor ('" << _name << "'): ";

				return ss.str();
			}
		};


		template <>
		struct ResourceDescriptor<EResourceType::GAPI_COMPONENT, EResourceSubType::SWAP_CHAIN>
			: public SwapChainDescriptorImpl
		{
			typedef SwapChainDescriptorImpl type;

			type _instance;
		};
		typedef ResourceDescriptor<EResourceType::GAPI_COMPONENT, EResourceSubType::SWAP_CHAIN> SwapChainDescriptor;

		template <>
		struct ResourceDescriptor<EResourceType::TEXTURE, EResourceSubType::TEXTURE_1D> 
			: public TextureDescriptorImpl<1>
		{
			typedef TextureDescriptorImpl<1> type;

			type _instance;
		};
		typedef ResourceDescriptor<EResourceType::TEXTURE, EResourceSubType::TEXTURE_1D> Texture1DDescriptor;

		template <>
		struct ResourceDescriptor<EResourceType::TEXTURE, EResourceSubType::TEXTURE_2D>
			: public TextureDescriptorImpl<2>
		{
			typedef TextureDescriptorImpl<2> type;

			type _instance;
		};
		typedef ResourceDescriptor<EResourceType::TEXTURE, EResourceSubType::TEXTURE_2D> Texture2DDescriptor;

		template <>
		struct ResourceDescriptor<EResourceType::TEXTURE, EResourceSubType::TEXTURE_3D>
			: public TextureDescriptorImpl<3>
		{
			typedef TextureDescriptorImpl<3> type;

			type _instance;
		};
		typedef ResourceDescriptor<EResourceType::TEXTURE, EResourceSubType::TEXTURE_3D> Texture3DDescriptor;

		template <>
		struct ResourceDescriptor<EResourceType::GAPI_VIEW, EResourceSubType::RENDER_TARGET_VIEW>
			: public RenderTargetDescriptorImpl
		{
			typedef RenderTargetDescriptorImpl type;

			type _instance;
		};
		typedef ResourceDescriptor<EResourceType::GAPI_VIEW, EResourceSubType::RENDER_TARGET_VIEW> RenderTargetDescriptor;
	}
}


#endif