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
#include "Resources/System/Core/ResourceTask.h"
#include "Resources/Subsystems/GFXAPI/GFXAPI.h"

namespace Engine {
	namespace GFXAPI {
		using namespace Engine::Resources;

		enum class EGFXAPI {
			DirectX = 1,
			OpenGL  = 2,
			Vulkan  = 4
		};

		struct DeferredResourceCreationHandle {
			std::future<GFXAPIResourceHandle_t>& futureHandle;
		};

		class GFXAPIResourceSubSystem {		
			DeclareLogTag(GFXAPIResourceSubSystem);
		public:
			DeclareInterface(IAsyncLoadCallback);
			/**********************************************************************************************//**
			 * \fn	virtual void GFXAPIResourceSubSystem::onResourceLoaded(const GFXAPIResourceHandle_t handle) = 0;
			 *
			 * \brief	Invoked once the resource was loaded. If the handle is 0, loading failed. If > 0, successful.
			 *
			 * \param	handle	The handle.
			 **************************************************************************************************/
			virtual void onResourceLoaded(const GFXAPIResourceHandle_t handle) = 0;
			DeclareInterfaceEnd(IAsyncLoadCallback);

			template <typename TResource>
			EEngineStatus loadSync(
				const ResourceDescriptor<TResource> &descriptor,
				GFXAPIResourceHandleMap             &inResourceDependencyHandles,
				GFXAPIResourceHandle_t              &outResourceHandle);

			template <typename TResource>
			EEngineStatus loadAsync(
				const ResourceDescriptor<TResource> &descriptor,
				Ptr<IAsyncLoadCallback> &callback);

		private:
			template <typename TResource>
			EEngineStatus loadImpl(
				const ResourceDescriptor<TResource> &descriptor,
				GFXAPIResourceHandleMap             &inResourceDependencyHandles,
				DeferredResourceCreationHandle      &outHandle);

			template <typename TResource>
			EEngineStatus createBuilder(
				const ResourceDescriptor<TResource> &desc,
				GFXAPIResourceHandleMap             &inResourceDependencyHandles,
				ResourceBuilder<TResource>          &outBuilder);
			
			template <typename TResource>
			EEngineStatus enqueue(
				const ResourceBuilder<TResource>           &builder,
				std::shared_future<GFXAPIResourceHandle_t> &outSharedFuture);

		private:
			Threading::Looper<
		};


		template <typename TResource>
		EEngineStatus GFXAPIResourceSubSystem::loadSync(
			const ResourceDescriptor<TResource> &descriptor,
			GFXAPIResourceHandleMap             &inResourceDependencyHandles,
			GFXAPIResourceHandle_t              &outResourceHandle)
		{
			DeferredResourceCreationHandle handle;

			EEngineStatus status = loadImpl(descriptor, inResourceDependencyHandles, handle);
			if( !CheckEngineError(status) ) {
				GFXAPIResourceHandle_t resourceHandle = handle.futureHandle.get(); // Wait for it...
				if( resourceHandle == 0 )
					// Invalid
					status = EEngineStatus::GFXAPI_SubsystemResourceCreationFailed;
				else {
					outResourceHandle = resourceHandle;
					status = EEngineStatus::Ok;
				}
			}

			return status;
		}

		template <typename TResource>
		EEngineStatus GFXAPIResourceSubSystem::loadAsync(
			const ResourceDescriptor<TResource> &descriptor,
			Ptr<IAsyncLoadCallback> &callback)
		{
			DeferredResourceCreationHandle handle;

			EEngineStatus status = loadImpl(descriptor, handle);
			if( !CheckEngineError(status) ) {
				// Won't be further implemented for now!!
				// If continued:
				//   Create task UID
				//   Store handle w/ future and callback together
				//   Have separate timed task run in intervals to verify the resource states and invoke the callbacks, where necessary.
			}

			return status;
		}

		template <typename TResource>
		EEngineStatus GFXAPIResourceSubSystem::loadImpl(
			const ResourceDescriptor<TResource> &descriptor,
			GFXAPIResourceHandleMap             &inResourceDependencyHandles,
			DeferredResourceCreationHandle      &outHandle)
		{
			EEngineStatus status = EEngineStatus::Ok;

			ResourceBuilder<TResource> builder;
			status = createBuilder<TResource>(descriptor, inResourceDependencyHandles, builder);
			if( CheckEngineError(status) ) {
				Log::Error(logTag(), String::format("Failed to create builder for resource '%0'", descriptor.name()));
				return status;
			}

			std::shared_future<GFXAPIResourceHandle_t> future;
			status = enqueue<TResource>(builder, future);
			if( CheckEngineError(status) ) {
				Log::Error(logTag(), String::format("Failed to enqueue resource creation task for resource '%0'", descriptor.name()));
				return status;
			}

			return status;
		}

		template <typename TResource>
		EEngineStatus GFXAPIResourceSubSystem::createBuilder(
			const ResourceDescriptor<TResource> &desc,
			GFXAPIResourceHandleMap             &inResourceDependencyHandles,
			ResourceBuilder<TResource>          &outBuilder)
		{
			// Invoke task builder for the desired GFX-API.
		}

		template <typename TResource>
		EEngineStatus GFXAPIResourceSubSystem::enqueue(
			const ResourceBuilder<TResource>           &builder,
			std::shared_future<GFXAPIResourceHandle_t> &outSharedFuture) {

		}
	}
}

#endif