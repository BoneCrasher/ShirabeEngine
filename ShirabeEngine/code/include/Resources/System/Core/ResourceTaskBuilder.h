#ifndef __SHIRABE_RESOURCE_TASK_H__
#define __SHIRABE_RESOURCE_TASK_H__

#include <type_traits>

#include "Core/EngineTypeHelper.h"

#include "Resources/System/Core/ResourceDomainTransfer.h"
#include "Resources/System/Core/ResourceTask.h"
#include "Resources/Subsystems/GFXAPI/GFXAPIResourceBackend.h"

namespace Engine {
	namespace Resources {
    using namespace Engine::GFXAPI;

		enum class EResourceTaskType {
			Creation    = 1,
			Update      = 2,
			Destruction = 4,
			Query       = 8 // Needed?
		};

		class Task {
			
		};
	}
}

#endif