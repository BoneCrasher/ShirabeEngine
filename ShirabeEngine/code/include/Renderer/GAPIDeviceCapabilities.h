#ifndef __SHIRABE_GAPI_DEVICECAPABILITIES_H__
#define __SHIRABE_GAPI_DEVICECAPABILITIES_H__

#include "Core/BasicTypes.h"
#include "Core/EngineStatus.h"
#include "Core/EngineTypeHelper.h"
#include "Log/Log.h"

namespace Engine {
	namespace GAPI {
		enum class GAPIFormat {
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

		struct GAPIOutputMode {
			GAPIFormat _format;
			Vec2Dui    _size;
			Vec2Dui    _refreshRate;
		};
		DeclareListType(GAPIOutputMode, GAPIOutputMode);

		struct GAPIOutput {
			unsigned int       _outputIndex;
			GAPIOutputModeList _outputModes;
		};
		DeclareListType(GAPIOutput, GAPIOutput);

		struct GAPIAdapterDescription {
			std::string   _name;
			unsigned long _dedicatedGPUMemory;
			unsigned long _dedicatedSysMemory;

		};

		struct GAPIAdapter {
			unsigned int           _adapterIndex;
			GAPIAdapterDescription _description;
			GAPIOutputList         _outputs;
		};
		DeclareListType(GAPIAdapter, GAPIAdapter)

			struct GAPIDeviceCapabilities {
			GAPIAdapterList _adapters;
		};

	}
}
#endif 