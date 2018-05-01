#ifndef __SHIRABE_RESOURCES_CORE_DESCRIPTORS_H__
#define __SHIRABE_RESOURCES_CORE_DESCRIPTORS_H__

#include <sstream>

#include "Platform/Platform.h"
#include "GFXAPI/Config.h"

#ifdef GAPI_USE_DX
#include "GFXAPI/DirectX/DX11/Common.h"
#else 

#endif

#include "Core/EngineTypeHelper.h"
#include "Core/BasicTypes.h"

#include "Resources/Core/EResourceType.h"
#include "Resources/Core/ResourceDomainTransfer.h"

namespace Engine {
	namespace GFXAPI {

		enum class ResourceUsage
      : uint8_t
    {
			CPU_None_GPU_ReadWrite      = 1, // i.e. DX11::D3D11_USAGE::Default
			CPU_InitConst_GPU_Read      = 2, // i.e. DX11::D3D11_USAGE::Immutable
			CPU_Write_GPU_Read          = 4, // i.e. DX11::D3D11_USAGE::Dynamic
			CPU_ReadWrite_GPU_ReadWrite = 8  // i.e. DX11::D3D11_USAGE::Staging
		};
		// No bitflags-type defined, since there's only one type of usage at once per resource.

		enum class BufferBinding 
      : uint8_t
    {
			VertexBuffer                 = 1,
			IndexBuffer                  = 2,
			ConstantBuffer               = 4,
			ShaderResource               = 8,
			ShaderOutput_RenderTarget    = 16,
			ShaderOutput_StreamOutput    = 32,
			ShaderOutput_DepthStencil    = 64,
			UnorderedMultithreadedAccess = 128
		};

		enum class Format
      : uint8_t 
    {
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


		



		
	}
}


#endif