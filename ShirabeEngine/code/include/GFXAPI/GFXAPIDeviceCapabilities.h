#ifndef __SHIRABE_GAPI_DEVICECAPABILITIES_H__
#define __SHIRABE_GAPI_DEVICECAPABILITIES_H__

#include "Core/BasicTypes.h"
#include "Core/EngineStatus.h"
#include "Core/EngineTypeHelper.h"

#include "Log/Log.h"

#include "GFXAPI/Definitions.h"

namespace Engine {
	namespace GFXAPI {
		using namespace Engine::Resources;

		struct GAPIOutputMode {
			Format   format;
			Vec2Dui  size;
			Vec2Dui  refreshRate;
		};
		DeclareListType(GAPIOutputMode, GAPIOutputMode);

		struct GAPIOutput {
			unsigned int       outputIndex;
			GAPIOutputModeList outputModes;
		};
		DeclareListType(GAPIOutput, GAPIOutput);

		struct GAPIAdapterDescription {
			std::string   name;
			unsigned long dedicatedGPUMemory;
			unsigned long dedicatedSysMemory;

		};

		struct GAPIAdapter {
			unsigned int           adapterIndex;
			GAPIAdapterDescription description;
			GAPIOutputList         outputs;
		};
		DeclareListType(GAPIAdapter, GAPIAdapter);

		struct GAPIDeviceCapabilities {
			GAPIAdapterList adapters;
		};

	}
}
#endif 