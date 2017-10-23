#ifndef __SHIRABE_GFXAPIRESOURCESUBSYSTEM_H__
#define __SHIRABE_GFXAPIRESOURCESUBSYSTEM_H__

#include <type_traits>
#include <typeinfo>
#include <optional>
#include <stdint.h>
#include <functional>
#include <future>

#include "Core/EngineTypeHelper.h"
#include "Core/EngineStatus.h"
#include "IOC/Observer.h"
#include "Threading/Looper.h"

#include "Resources/System/Core/IResourceDescriptor.h"
#include "Resources/System/Core/ResourceBuilder.h"
#include "Resources/Subsystems/GFXAPI/GFXAPI.h"

namespace Engine {
	namespace GFXAPI {

		using namespace Engine::Resources;

		DeclareInterface(IGFXAPIResourceSubsystem);
		// API
		DeclareInterfaceEnd(IGFXAPIResourceSubsystem);
		DeclareSharedPointerType(IGFXAPIResourceSubsystem);

		struct DeferredResourceCreationHandle {
			std::future<GFXAPIResourceHandle_t>& futureHandle;
		};

		template <typename TResource, typename... TRunParams>
		std::function<void(TRunParams... params)> bindTask(
			const ResourceDescriptor<TResource>  &descriptor,
			const ResourceBuilder<TResource>     &builder,
			Threading::param_pack<TRunParams...>  pack) {
			return [&] (TRunParams... params) {
				//
			}
		}

		template <typename TSubsystem, typename TResource>
		EEngineStatus bind(
			const ResourceDescriptor<TResource> &descriptor, 
			Ptr<TSubsystem>                     &subsystem,
			DeferredResourceCreationHandle      &outHandle) 
		{
			ResourceBuilder<TResource> builder;
			
			EEngineStatus status = TSubsystem::createBuilder<TResource>(descriptor, builder);

			std::future<GFXAPIResourceHandle_t>& task = subsystem->enqueue(
				bindTask < TResource, typename TSubsystem::looper_type::params >
			);

			return status;
		}

	}
}

#endif