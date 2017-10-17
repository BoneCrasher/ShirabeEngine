#ifndef __SHIRABE_GFXAPIRESOURCESUBSYSTEM_H__
#define __SHIRABE_GFXAPIRESOURCESUBSYSTEM_H__

#include <type_traits>
#include <typeinfo>
#include <optional>
#include <stdint.h>

#include "Core/EngineTypeHelper.h"
#include "Core/EngineStatus.h"
#include "IOC/Observer.h"

#include "Resources/Subsystems/GFXAPI/GFXAPI.h"

namespace Engine {
	namespace GFXAPI {

		using namespace Engine::Resources;

		DeclareInterface(IGFXAPIResourceSubsystem);

		

		DeclareInterfaceEnd(IGFXAPIResourceSubsystem);
		DeclareSharedPointerType(IGFXAPIResourceSubsystem);
	}
}

#endif