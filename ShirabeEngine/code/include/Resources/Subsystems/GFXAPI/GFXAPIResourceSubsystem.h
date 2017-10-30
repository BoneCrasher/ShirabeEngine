#ifndef __SHIRABE_GFXAPIRESOURCESUBSYSTEM_H__
#define __SHIRABE_GFXAPIRESOURCESUBSYSTEM_H__

#include <type_traits>
#include <typeinfo>
#include <optional>
#include <stdint.h>
#include <functional>
#include <future>
#include <assert.h>

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

		enum class ETaskSynchronization {
			Async = 1,
			Sync  = 2
		};

		enum class EResourceTaskType {
			Create  = 1,
			Query   = 2, // = Read Info
			Update  = 4,
			Destroy = 8
		};

		using ResourceTaskFn_t = std::function<GFXAPIResourceHandle_t()>;

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
				const ETaskSynchronization          &inRequestMode,
				const Ptr<IAsyncLoadCallback>       &inCallback,
				GFXAPIResourceHandle_t              &outResourceHandle);

			template <typename TResource>
			EEngineStatus unload(
				GFXAPIResourceHandle_t     &inResourceHandle,
				const ETaskSynchronization &inSynchronization);


		private:
			template <typename TResource>
			EEngineStatus loadImpl(
				const ResourceDescriptor<TResource> &inDescriptor,
				GFXAPIResourceHandleMap             &inResourceDependencyHandles,
				DeferredResourceCreationHandle      &outHandle);

			template <typename TResource>
			EEngineStatus createTask(
				const ResourceDescriptor<TResource> &inDesc,
				const GFXAPIResourceHandleMap       &inResourceDependencyHandles,
				const EResourceTaskType             &inTaskType,
				ResourceTaskFn_t                    &outTask);
			
			template <typename TResource>
			EEngineStatus enqueue(
				const ResourceTaskFn_t                     &inTask,
				std::shared_future<GFXAPIResourceHandle_t> &outSharedFuture);

		private:
			Threading::Looper::Handler m_resourceThreadHandler;
		};

		/**********************************************************************************************//**
		 * \fn	template <typename TResource> EEngineStatus GFXAPIResourceSubSystem::load( const ResourceDescriptor<TResource> &inDescriptor, const GFXAPIResourceHandleMap &inResourceDependencyHandles, const ETaskSynchronization &inRequestMode, const Ptr<IAsyncLoadCallback> &inCallback, GFXAPIResourceHandle_t &outResourceHandle)
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
			const ETaskSynchronization          &inSynchronization,
			const Ptr<IAsyncLoadCallback>       &inCallback,
			GFXAPIResourceHandle_t              &outResourceHandle)
		{
			GFXAPIResourceHandle_t resourceHandle = 0;

			DeferredResourceCreationHandle handle;
			EEngineStatus status = loadImpl(inDescriptor, inResourceDependencyHandles, handle);
			if( !CheckEngineError(status) ) {
				switch( inSynchronization ) {
				default:
				case ETaskSynchronization::Sync:
					resourceHandle = handle.futureHandle.get(); // Wait for it...
					if( resourceHandle == 0 )
						// Invalid
						status = EEngineStatus::GFXAPI_SubsystemResourceCreationFailed;
					else {
						outResourceHandle = resourceHandle;
						status = EEngineStatus::Ok;
					}
					break;
				case ETaskSynchronization::Async:
					break;
				}
			}				

			return status;
		}

		/**********************************************************************************************//**
		 * \fn	template <typename TResource> EEngineStatus GFXAPIResourceSubSystem::loadImpl( const ResourceDescriptor<TResource> &inDescriptor, GFXAPIResourceHandleMap &inResourceDependencyHandles, DeferredResourceCreationHandle &outHandle)
		 *
		 * \brief	Loads an implementation
		 *
		 * \tparam	TResource	Type of the resource.
		 * \param 		  	inDescriptor			   	Information describing the in.
		 * \param [in,out]	inResourceDependencyHandles	The in resource dependency handles.
		 * \param [in,out]	outHandle				   	Handle of the out.
		 *
		 * \return	The implementation.
		 **************************************************************************************************/
		template <typename TResource>
		EEngineStatus GFXAPIResourceSubSystem::loadImpl(
			const ResourceDescriptor<TResource> &inDescriptor,
			GFXAPIResourceHandleMap             &inResourceDependencyHandles,
			DeferredResourceCreationHandle      &outHandle)
		{
			EEngineStatus status = EEngineStatus::Ok;

		    ResourceTaskFn_t task = nullptr;
			status = createTask<TResource>(inDescriptor, inResourceDependencyHandles, task);
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

		/**********************************************************************************************//**
		 * \fn	template <typename TResource> EEngineStatus GFXAPIResourceSubSystem::createTask( const ResourceDescriptor<TResource> &inDescriptor, const GFXAPIResourceHandleMap &inResourceDependencyHandles, const EResourceTaskType &inTaskType, ResourceTaskFn_t &outTask)
		 *
		 * \brief	Creates a task
		 *
		 * \tparam	TResource	Type of the resource.
		 * \param 		  	inDescriptor			   	Information describing the in.
		 * \param 		  	inResourceDependencyHandles	The in resource dependency handles.
		 * \param 		  	inTaskType				   	Type of the in task.
		 * \param [in,out]	outTask					   	The out task.
		 *
		 * \return	The new task.
		 **************************************************************************************************/
		template <typename TResource>
		EEngineStatus GFXAPIResourceSubSystem::createTask(
			const ResourceDescriptor<TResource> &inDescriptor,
			const GFXAPIResourceHandleMap       &inResourceDependencyHandles,
			const EResourceTaskType             &inTaskType,
			ResourceTaskFn_t                    &outTask)
		{
			// Determine the specific graphics API and access the task builder
			// to create the appropriate task type on the API.
		}

		using namespace Threading;
		class FutureAssignmentTask 
			: public Looper::ITask 
		{
		public:
			FutureAssignmentTask(
				const ResourceTaskFn_t                     &inUnderlyingTaskFn,
				const Looper::Priority                     &inPriority,
				std::shared_future<GFXAPIResourceHandle_t> &inOutSharedFuture)
				: m_priority(inPriority)
			{
				assert(inUnderlyingTaskFn != nullptr);

				std::function<bool()> fn =
					[=] () -> bool 
				{
					GFXAPIResourceHandle_t id = inUnderlyingTaskFn();
					if( id == 0 ) {
						inOutSharedFuture.
						return false;
					} else {

					}
				};

				m_bindingFunction = fn;
			}

			inline Looper::Priority priority() { return m_priority; }

			bool run() {
				

				return true;
			}

		private:
			std::function<bool()>  m_bindingFunction;
			Looper::Priority       m_priority;
		};

		/**********************************************************************************************//**
		 * \fn	template <typename TResource> EEngineStatus GFXAPIResourceSubSystem::enqueue( const ResourceTaskFn_t &inTask, std::shared_future<GFXAPIResourceHandle_t> &outSharedFuture)
		 *
		 * \brief	Adds an object onto the end of this queue
		 *
		 * \tparam	TResource	Type of the resource.
		 * \param 		  	inTask		   	The in task.
		 * \param [in,out]	outSharedFuture	The out shared future.
		 *
		 * \return	The EEngineStatus.
		 **************************************************************************************************/
		template <typename TResource>
		EEngineStatus GFXAPIResourceSubSystem::enqueue(
			const ResourceTaskFn_t                     &inTask,
			std::shared_future<GFXAPIResourceHandle_t> &outSharedFuture) {
			// Here it becomes interesting: 
			// The looper itself should not be tied to whatever return type. 
			// In addition, resource task creation should not be tied to 
			// the looper or any other subsystem.
			//
			// Solution:
			// Wrap the resource task in another and implement the assignment 
			// of the shared-future there.		

			FutureAssignmentTaskPtr looperTask
				= MakeSharedPointerType<FutureAssignmentTask>(inTask, Looper::Priority::Normal, outSharedFuture);

			m_resourceThreadHandler.post(looperTask);
		}
	}
}

#endif