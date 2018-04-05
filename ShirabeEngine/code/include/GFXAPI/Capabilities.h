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
      uint32_t modeIndex;
			Format   format;
			TVector3D<uint32_t> 
        size,
        refreshRate;
		};
		DeclareListType(GAPIOutputMode, GAPIOutputMode);

		struct GAPIOutput {
			uint32_t           outputIndex;
			GAPIOutputModeList outputModes;
		};
		DeclareListType(GAPIOutput, GAPIOutput);

		struct GAPIAdapterDescription {
			std::string   name;
			unsigned long dedicatedGPUMemory;
			unsigned long dedicatedSysMemory;

		};

		struct GAPIAdapter {
      uint32_t               adapterIndex;
			GAPIAdapterDescription description;
			GAPIOutputList         outputs;
		};
		DeclareListType(GAPIAdapter, GAPIAdapter);

    struct GAPIAdapterOutputConfig {
      int32_t adapterIndex;
      int32_t outputIndex;
      int32_t modeIndex;
    };

		struct GAPIDeviceCapabilities {
			GAPIAdapterList         adapters;
      GAPIAdapterOutputConfig selectedConfig;
		};

	}
}
#endif 