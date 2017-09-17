#ifndef __SHIRABE_IRESOURCETYPE_H__
#define __SHIRABE_IRESOURCETYPE_H__

#include "Core/EngineTypeHelper.h"

namespace Engine {
	namespace Resources {
		enum class EResourceType 
			: uint16_t
		{
			UNKNOWN        = 0,
			MESH           = 100,
			TEXTURE        = 200,
			SHADER         = 300,
			GAPI_COMPONENT = 500,
			GAPI_VIEW      = 600,
			GAPI_STATE     = 700
		};

		enum class EResourceSubType
			: uint16_t 
		{
			UNKNOWN              = 0,
			MESH_STATIC          = 101,
			MESH_SKINNED         = 102,
			TEXTURE_1D           = 201,
			TEXTURE_2D           = 202,
			TEXTURE_3D           = 203,
			TEXTURE_CUBE         = 204,
			SHADER_VS            = 301,
			SHADER_HS            = 302,
			SHADER_DS            = 303,
			SHADER_GS            = 304,
			SHADER_PS            = 305,
			SHADER_CS            = 306,
			SHADER_FX            = 307, // Effects11 .fx-file
			SWAP_CHAIN           = 501,
			DEPTH_STENCIL_VIEW   = 601,
			SHADER_RESOURCE_VIEW = 602,
			RENDER_TARGET_VIEW   = 603,
			DEPTH_STENCIL_STATE  = 701
		};
	}
}

#endif