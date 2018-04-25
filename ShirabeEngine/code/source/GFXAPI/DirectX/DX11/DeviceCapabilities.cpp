#include "GFXAPI/DirectX/DX11/Common.h"
#include "GFXAPI/DirectX/DX11/DeviceCapabilities.h"

#include <functional>

namespace Engine {
	namespace DX {
		namespace _11 {
			DXGI_FORMAT DX11DeviceCapsHelper::convertFormatGAPI2DXGI(const Format& fmt) {
				switch (fmt) {
				case Format::RGBA_32_UINT:             return DXGI_FORMAT_R32G32B32A32_UINT;
				case Format::RGBA_32_SINT:             return DXGI_FORMAT_R32G32B32A32_SINT;
				case Format::RGBA_32_FLOAT:            return DXGI_FORMAT_R32G32B32A32_FLOAT;
				case Format::RGBA_32_TYPELESS:         return DXGI_FORMAT_R32G32B32A32_TYPELESS;
				case Format::RGBA_16_UNORM:            return DXGI_FORMAT_R16G16B16A16_UNORM;
				case Format::RGBA_16_SNORM:            return DXGI_FORMAT_R16G16B16A16_SNORM;
				case Format::RGBA_16_UINT:             return DXGI_FORMAT_R16G16B16A16_UINT;
				case Format::RGBA_16_SINT:             return DXGI_FORMAT_R16G16B16A16_SINT;
				case Format::RGBA_16_FLOAT:            return DXGI_FORMAT_R16G16B16A16_FLOAT;
				case Format::RGBA_16_TYPELESS:         return DXGI_FORMAT_R16G16B16A16_TYPELESS;
				case Format::RGBA_8_UNORM:             return DXGI_FORMAT_R8G8B8A8_UNORM;
				case Format::RGBA_8_UNORM_SRGB:        return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
				case Format::RGBA_8_SNORM:             return DXGI_FORMAT_R8G8B8A8_SNORM;
				case Format::RGBA_8_UINT:              return DXGI_FORMAT_R8G8B8A8_UINT;
				case Format::RGBA_8_SINT:              return DXGI_FORMAT_R8G8B8A8_SINT;
				case Format::RGBA_8_TYPELESS:          return DXGI_FORMAT_R8G8B8A8_TYPELESS;
				case Format::RGB_32_UINT:              return DXGI_FORMAT_R32G32B32_UINT;
				case Format::RGB_32_SINT:              return DXGI_FORMAT_R32G32B32_SINT;
				case Format::RGB_32_FLOAT:             return DXGI_FORMAT_R32G32B32_FLOAT;
				case Format::RGB_32_TYPELESS:          return DXGI_FORMAT_R32G32B32_TYPELESS; 
				case Format::D24_UNORM_S8_UINT:        return DXGI_FORMAT_D24_UNORM_S8_UINT;
				case Format::D32_FLOAT:                return DXGI_FORMAT_D32_FLOAT;
				case Format::R32_FLOAT:                return DXGI_FORMAT_R32_FLOAT;
				case Format::R32_UINT:                 return DXGI_FORMAT_R32_UINT;
				case Format::R32_SINT:                 return DXGI_FORMAT_R32_SINT;
				case Format::D32_FLOAT_S8X24_UINT:     return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
				case Format::R32_FLOAT_X8X24_TYPELESS: return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
				case Format::BC1_UNORM:                return DXGI_FORMAT_BC1_UNORM;
				case Format::BC1_UNORM_SRGB:           return DXGI_FORMAT_BC1_UNORM_SRGB;
				case Format::BC1_TYPELESS:             return DXGI_FORMAT_BC1_TYPELESS;
				case Format::BC2_UNORM:                return DXGI_FORMAT_BC2_UNORM;
				case Format::BC2_UNORM_SRGB:           return DXGI_FORMAT_BC2_UNORM_SRGB;
				case Format::BC2_TYPELESS:             return DXGI_FORMAT_BC2_TYPELESS;
				case Format::BC3_UNORM:                return DXGI_FORMAT_BC3_UNORM;
				case Format::BC3_UNORM_SRGB:           return DXGI_FORMAT_BC3_UNORM_SRGB;
				case Format::BC3_TYPELESS:             return DXGI_FORMAT_BC3_TYPELESS;
				case Format::BC4_UNORM:                return DXGI_FORMAT_BC4_UNORM;
				case Format::BC4_SNORM:                return DXGI_FORMAT_BC4_SNORM;
				case Format::BC4_TYPELESS:             return DXGI_FORMAT_BC4_TYPELESS;
				case Format::BC5_UNORM:                return DXGI_FORMAT_BC5_UNORM;
				case Format::BC5_SNORM:                return DXGI_FORMAT_BC5_SNORM;
				case Format::BC5_TYPELESS:             return DXGI_FORMAT_BC5_TYPELESS;
				case Format::BC6H_SF16:                return DXGI_FORMAT_BC6H_SF16;
				case Format::BC6H_UF16:                return DXGI_FORMAT_BC6H_UF16;
				case Format::BC6H_TYPELESS:            return DXGI_FORMAT_BC6H_TYPELESS;
				case Format::BC7_UNORM:                return DXGI_FORMAT_BC7_UNORM;
				case Format::BC7_UNORM_SRGB:           return DXGI_FORMAT_BC7_UNORM_SRGB;
				case Format::BC7_TYPELESS:             return DXGI_FORMAT_BC7_TYPELESS;
				}
				return DXGI_FORMAT_UNKNOWN;
			}

			Format DX11DeviceCapsHelper::convertFormatDXGI2GAPI(const DXGI_FORMAT& fmt) {
				switch (fmt) {
				case DXGI_FORMAT_R32G32B32A32_UINT:		   return Format::RGBA_32_UINT;
				case DXGI_FORMAT_R32G32B32A32_SINT:		   return Format::RGBA_32_SINT;
				case DXGI_FORMAT_R32G32B32A32_FLOAT:	   return Format::RGBA_32_FLOAT;
				case DXGI_FORMAT_R32G32B32A32_TYPELESS:	   return Format::RGBA_32_TYPELESS;
				case DXGI_FORMAT_R16G16B16A16_UNORM:	   return Format::RGBA_16_UNORM;
				case DXGI_FORMAT_R16G16B16A16_SNORM:	   return Format::RGBA_16_SNORM;
				case DXGI_FORMAT_R16G16B16A16_UINT:		   return Format::RGBA_16_UINT;
				case DXGI_FORMAT_R16G16B16A16_SINT:		   return Format::RGBA_16_SINT;
				case DXGI_FORMAT_R16G16B16A16_FLOAT:	   return Format::RGBA_16_FLOAT;
				case DXGI_FORMAT_R16G16B16A16_TYPELESS:	   return Format::RGBA_16_TYPELESS;
				case DXGI_FORMAT_R8G8B8A8_UNORM:		   return Format::RGBA_8_UNORM;
				case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:	   return Format::RGBA_8_UNORM_SRGB;
				case DXGI_FORMAT_R8G8B8A8_SNORM:		   return Format::RGBA_8_SNORM;
				case DXGI_FORMAT_R8G8B8A8_UINT:			   return Format::RGBA_8_UINT;
				case DXGI_FORMAT_R8G8B8A8_SINT:			   return Format::RGBA_8_SINT;
				case DXGI_FORMAT_R8G8B8A8_TYPELESS:		   return Format::RGBA_8_TYPELESS;
				case DXGI_FORMAT_R32G32B32_UINT:		   return Format::RGB_32_UINT;
				case DXGI_FORMAT_R32G32B32_SINT:		   return Format::RGB_32_SINT;
				case DXGI_FORMAT_R32G32B32_FLOAT:		   return Format::RGB_32_FLOAT;
				case DXGI_FORMAT_R32G32B32_TYPELESS:	   return Format::RGB_32_TYPELESS;
				case DXGI_FORMAT_D24_UNORM_S8_UINT:        return Format::D24_UNORM_S8_UINT;
				case DXGI_FORMAT_D32_FLOAT:				   return Format::D32_FLOAT;
				case DXGI_FORMAT_R32_FLOAT:				   return Format::R32_FLOAT;
				case DXGI_FORMAT_R32_UINT:				   return Format::R32_UINT;
				case DXGI_FORMAT_R32_SINT:				   return Format::R32_SINT;
				case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:	   return Format::D32_FLOAT_S8X24_UINT;
				case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS: return Format::R32_FLOAT_X8X24_TYPELESS;
				case DXGI_FORMAT_BC1_UNORM:				   return Format::BC1_UNORM;
				case DXGI_FORMAT_BC1_UNORM_SRGB:		   return Format::BC1_UNORM_SRGB;
				case DXGI_FORMAT_BC1_TYPELESS:			   return Format::BC1_TYPELESS;
				case DXGI_FORMAT_BC2_UNORM:				   return Format::BC2_UNORM;
				case DXGI_FORMAT_BC2_UNORM_SRGB:		   return Format::BC2_UNORM_SRGB;
				case DXGI_FORMAT_BC2_TYPELESS:			   return Format::BC2_TYPELESS;
				case DXGI_FORMAT_BC3_UNORM:				   return Format::BC3_UNORM;
				case DXGI_FORMAT_BC3_UNORM_SRGB:		   return Format::BC3_UNORM_SRGB;
				case DXGI_FORMAT_BC3_TYPELESS:			   return Format::BC3_TYPELESS;
				case DXGI_FORMAT_BC4_UNORM:				   return Format::BC4_UNORM;
				case DXGI_FORMAT_BC4_SNORM:				   return Format::BC4_SNORM;
				case DXGI_FORMAT_BC4_TYPELESS:			   return Format::BC4_TYPELESS;
				case DXGI_FORMAT_BC5_UNORM:				   return Format::BC5_UNORM;
				case DXGI_FORMAT_BC5_SNORM:				   return Format::BC5_SNORM;
				case DXGI_FORMAT_BC5_TYPELESS:			   return Format::BC5_TYPELESS;
				case DXGI_FORMAT_BC6H_SF16:				   return Format::BC6H_SF16;
				case DXGI_FORMAT_BC6H_UF16:				   return Format::BC6H_UF16;
				case DXGI_FORMAT_BC6H_TYPELESS:			   return Format::BC6H_TYPELESS;
				case DXGI_FORMAT_BC7_UNORM:				   return Format::BC7_UNORM;
				case DXGI_FORMAT_BC7_UNORM_SRGB:		   return Format::BC7_UNORM_SRGB;
				case DXGI_FORMAT_BC7_TYPELESS:			   return Format::BC7_TYPELESS;
				}
				return Format::UNKNOWN;
			}		

			D3D11_USAGE DX11DeviceCapsHelper::convertResourceUsageGAPI2D3D11(
				const ResourceUsage& f
			) {
				switch (f) {
				case  ResourceUsage::CPU_None_GPU_ReadWrite:
					return D3D11_USAGE_DEFAULT;
				case  ResourceUsage::CPU_Write_GPU_Read:
					return D3D11_USAGE_DYNAMIC;
				case ResourceUsage::CPU_InitConst_GPU_Read:
					return D3D11_USAGE_IMMUTABLE;
				case ResourceUsage::CPU_ReadWrite_GPU_ReadWrite:
					return D3D11_USAGE_STAGING;
				}
			}
			ResourceUsage DX11DeviceCapsHelper::convertResourceUsageD3D112GAPI(
				const D3D11_USAGE& u
			) {
				switch (u) {
				default:
				case D3D11_USAGE_DEFAULT:
					return ResourceUsage::CPU_None_GPU_ReadWrite;
				case D3D11_USAGE_DYNAMIC:
					return ResourceUsage::CPU_Write_GPU_Read;
				case D3D11_USAGE_IMMUTABLE:
					return ResourceUsage::CPU_InitConst_GPU_Read;
				case D3D11_USAGE_STAGING:
					return ResourceUsage::CPU_ReadWrite_GPU_ReadWrite;
				}
			}

			UINT DX11DeviceCapsHelper::convertBufferBindingGAPI2D3D11(
				const BufferBindingFlags_t& f
			) {
				UINT d3d11BindFlags = 0;
				if (checkFlag(f, BufferBinding::VertexBuffer))
					d3d11BindFlags |= D3D11_BIND_VERTEX_BUFFER;
				if (checkFlag(f, BufferBinding::IndexBuffer))
					d3d11BindFlags |= D3D11_BIND_INDEX_BUFFER;
				if (checkFlag(f, BufferBinding::ConstantBuffer))
					d3d11BindFlags |= D3D11_BIND_CONSTANT_BUFFER;
				if (checkFlag(f, BufferBinding::ShaderResource))
					d3d11BindFlags |= D3D11_BIND_SHADER_RESOURCE;
				if (checkFlag(f, BufferBinding::ShaderOutput_RenderTarget))
					d3d11BindFlags |= D3D11_BIND_RENDER_TARGET;
				if (checkFlag(f, BufferBinding::ShaderOutput_StreamOutput))
					d3d11BindFlags |= D3D11_BIND_STREAM_OUTPUT;
				if (checkFlag(f, BufferBinding::ShaderOutput_DepthStencil))
					d3d11BindFlags |= D3D11_BIND_DEPTH_STENCIL;
				if (checkFlag(f, BufferBinding::UnorderedMultithreadedAccess))
					d3d11BindFlags |= D3D11_BIND_UNORDERED_ACCESS;

				return d3d11BindFlags;
			}
			BufferBindingFlags_t DX11DeviceCapsHelper::convertBufferBindingD3D112GAPI(
				const UINT& f) {
				BufferBindingFlags_t gapiBindFlags = 0;
				if (checkFlag(f, D3D11_BIND_VERTEX_BUFFER))
					gapiBindFlags = EnumClassFlags<BufferBinding>::append(gapiBindFlags, BufferBinding::VertexBuffer);
				if (checkFlag(f, D3D11_BIND_INDEX_BUFFER))
					gapiBindFlags = EnumClassFlags<BufferBinding>::append(gapiBindFlags, BufferBinding::IndexBuffer);
				if (checkFlag(f, D3D11_BIND_CONSTANT_BUFFER))
					gapiBindFlags = EnumClassFlags<BufferBinding>::append(gapiBindFlags, BufferBinding::ConstantBuffer);
				if (checkFlag(f, D3D11_BIND_SHADER_RESOURCE))
					gapiBindFlags = EnumClassFlags<BufferBinding>::append(gapiBindFlags, BufferBinding::ShaderResource);
				if (checkFlag(f, D3D11_BIND_RENDER_TARGET))
					gapiBindFlags = EnumClassFlags<BufferBinding>::append(gapiBindFlags, BufferBinding::ShaderOutput_RenderTarget);
				if (checkFlag(f, D3D11_BIND_STREAM_OUTPUT))
					gapiBindFlags = EnumClassFlags<BufferBinding>::append(gapiBindFlags, BufferBinding::ShaderOutput_StreamOutput);
				if (checkFlag(f, D3D11_BIND_DEPTH_STENCIL))
					gapiBindFlags = EnumClassFlags<BufferBinding>::append(gapiBindFlags, BufferBinding::ShaderOutput_DepthStencil);
				if (checkFlag(f, D3D11_BIND_UNORDERED_ACCESS))
					gapiBindFlags = EnumClassFlags<BufferBinding>::append(gapiBindFlags, BufferBinding::UnorderedMultithreadedAccess);

				return gapiBindFlags;
			}

			DeclareSharedPointerType(IDXGIFactory);
			DeclareSharedPointerType(IDXGIAdapter);
			DeclareSharedPointerType(IDXGIOutput);
			DeclareListType(IDXGIAdapterPtr, IDXGIAdapterPtr);
			
		}
	}
}