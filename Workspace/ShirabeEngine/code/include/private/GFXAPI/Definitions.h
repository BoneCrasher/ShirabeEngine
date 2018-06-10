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
#include "Resources/Subsystems/GFXAPI/Types/Definition.h"

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
      : uint16_t
    {
			VertexBuffer                 = 1,
			IndexBuffer                  = 2,
			UniformBuffer                = 4,
			ShaderInput                  = 8,
      InputAttachement             = 16,
			ColorAttachement             = 32,
			DepthAttachement             = 64,
      ShaderOutput_StreamOutput    = 128,
			UnorderedMultithreadedAccess = 256,
      CopySource                   = 512,
      CopyTarget                   = 1024
		};

    using Format = Resources::Format;

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