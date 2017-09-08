#ifndef __SHIRABE_GAPI_DEVICECAPABILITIES_H__
#define __SHIRABE_GAPI_DEVICECAPABILITIES_H__

#include "Core/BasicTypes.h"
#include "Core/EngineStatus.h"
#include "Core/EngineTypeHelper.h"
#include "Log/Log.h"
#include "Resources/ResourceDescriptors.h"

namespace Engine {
	namespace GAPI {
		using namespace Engine::Resources;

		struct GAPIOutputMode {
			Format   _format;
			Vec2Dui  _size;
			Vec2Dui  _refreshRate;
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