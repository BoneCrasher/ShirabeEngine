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

		/**********************************************************************************************//**
		 * \enum	EGFXAPI
		 *
		 * \brief	Identifies a specific graphics API
		 **************************************************************************************************/
		enum class EGFXAPI {
			DirectX = 1,
			OpenGL  = 2,
			Vulkan  = 4
		};

		/**********************************************************************************************//**
		 * \enum	EGFXAPIVersion
		 *
		 * \brief   Identifies a specific graphics API version requirement
		 **************************************************************************************************/
		enum class EGFXAPIVersion {
			DirectX_11_0,
			DirectX_11_1,
			DirectX_11_2,
			DirectX_12_0,
			OpenGL_,
			Vulkan_
		};

		enum class ERequestMode {
			Async = 1,
			Sync  = 2
		};

		/**********************************************************************************************//**
		 * \struct	DeferredResourceCreationHandle
		 *
		 * \brief	Handle class to hold an asynchronous request.
		 **************************************************************************************************/
		struct DeferredResourceCreationHandle {
			std::future<GFXAPIResourceHandle_t>& futureHandle;
		};

		/**********************************************************************************************//**
		 * \class	GFXAPIResourceSubSystem
		 *
		 * \brief	The GFXAPIResourceSubSystem is responsible for spawning a resource task based on 
		 * 			 the specific request, executing it (a-)synchronuously and store the results.
		 * 			The task itself will be spawned by a TaskBuilder, determined during runtime. 
		 * 			The subsystem holds a mapping of GFXAPIResourceHandle_t to GFXAPIResourceHolder to 
		 * 			 store and manage the resources.
		 **************************************************************************************************/
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
			EEngineStatus load(
				const ResourceDescriptor<TResource> &inDescriptor,
				const GFXAPIResourceHandleMap       &inResourceDependencyHandles,
				const ERequestMode                  &inRequestMode,
				const Ptr<IAsyncLoadCallback>       &inCallback,
				GFXAPIResourceHandle_t              &outResourceHandle);

			template <typename TResource>
			EEngineStatus unload(
				GFXAPIResourceHandle_t &outResourceHandle,
				const ERequestMode     &inRequestMode);


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

		/**********************************************************************************************//**
		 * \fn	template <typename TResource> EEngineStatus GFXAPIResourceSubSystem::load( const ResourceDescriptor<TResource> &inDescriptor, const GFXAPIResourceHandleMap &inResourceDependencyHandles, const ERequestMode &inRequestMode, const Ptr<IAsyncLoadCallback> &inCallback, GFXAPIResourceHandle_t &outResourceHandle)
		 *
		 * \brief	Loads
		 *
		 * \tparam	TResource	Type of the resource.
		 * \param 		  	inDescriptor			   	Information describing the in.
		 * \param 		  	inResourceDependencyHandles	The in resource dependency handles.
		 * \param 		  	inRequestMode			   	The in request mode.
		 * \param 		  	inCallback				   	The in callback.
		 * \param [in,out]	outResourceHandle		   	Handle of the out resource.
		 *
		 * \return	The EEngineStatus.
		 **************************************************************************************************/
		template <typename TResource>
		EEngineStatus GFXAPIResourceSubSystem::load(
			const ResourceDescriptor<TResource> &inDescriptor,
			const GFXAPIResourceHandleMap       &inResourceDependencyHandles,
			const ERequestMode                  &inRequestMode,
			const Ptr<IAsyncLoadCallback>       &inCallback,
			GFXAPIResourceHandle_t              &outResourceHandle)
		{
			DeferredResourceCreationHandle handle;

			GFXAPIResourceHandle_t resourceHandle = 0;

			EEngineStatus status = loadImpl(descriptor, inResourceDependencyHandles, handle);
			if( !CheckEngineError(status) ) {
				switch( inRequestMode ) {
				default:
				case ERequestMode::Sync:

					resourceHandle = handle.futureHandle.get(); // Wait for it...
					if( resourceHandle == 0 )
						// Invalid
						status = EEngineStatus::GFXAPI_SubsystemResourceCreationFailed;
					else {
						outResourceHandle = resourceHandle;
						status = EEngineStatus::Ok;
					}
					break;
				case ERequestMode::Async:
					break;
				}
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
			status = enqueue<TResource>(task, future);
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