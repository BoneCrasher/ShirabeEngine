#ifndef __SHIRABE_IRESOURCETYPE_H__
#define __SHIRABE_IRESOURCETYPE_H__

#include <ostream>

#include "core/enginetypehelper.h"

namespace engine {
	namespace Resources {
		enum class EResourceType 
			: uint16_t
		{
			UNKNOWN        = 0,
			MESH           = 100,
			TEXTURE        = 200,
			SHADER         = 300,
			BUFFER         = 400,
			GAPI_COMPONENT = 500,
			GAPI_VIEW      = 600,
			GAPI_STATE     = 700
		};

		enum class EResourceSubType
			: uint16_t
		{
			UNKNOWN               = 0,
			MESH_STATIC           = 101,
			MESH_SKINNED          = 102,
			TEXTURE_1D            = 201,
			TEXTURE_2D            = 202,
			TEXTURE_3D            = 203,
			TEXTURE_CUBE          = 204,
			SHADER_VS             = 301,
			SHADER_HS             = 302,
			SHADER_DS             = 303,
			SHADER_GS             = 304,
			SHADER_PS             = 305,
			SHADER_CS             = 306,
			SHADER_FX             = 307, // Effects11 .fx-file
			BUFFER_CONSTANT       = 401,
			BUFFER_TEXTURE        = 402,
			BUFFER_STRUCTURED     = 403,
			BUFFER_VERTEX         = 404,
			BUFFER_INDEX          = 405,
			DEVICE                = 501,
			DEVICE_CONTEXT        = 502,
			SWAP_CHAIN            = 503,
      SWAP_CHAIN_BUFFER     = 504,
			DEPTH_STENCIL_VIEW    = 601,
			SHADER_RESOURCE_VIEW  = 602,
			RENDER_TARGET_VIEW    = 603,
			UNORDERED_ACCESS_VIEW = 604,
      TEXTURE_VIEW          = 605,
			DEPTH_STENCIL_STATE   = 701,
			RASTERIZER_STATE      = 702,
			BLEND_STATE           = 703
		};

    static std::ostream& operator<<(std::ostream& s, EResourceType const&rt) {
      s << static_cast<std::underlying_type<EResourceType>::type>(rt);
      return s;
    }

    static std::ostream& operator<<(std::ostream& s, EResourceSubType const&rt) {
      s << static_cast<std::underlying_type<EResourceSubType>::type>(rt);
      return s;
    }
	}
}

#endif