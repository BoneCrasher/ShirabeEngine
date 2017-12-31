#ifndef __SHIRABE_GFXAPIRESOURCEBACKEND_H__
#define __SHIRABE_GFXAPIRESOURCEBACKEND_H__

#include <type_traits>
#include <typeinfo>
#include <optional>
#include <stdint.h>
#include <functional>
#include <future>
#include <assert.h>
#include <map>

#include "Core/EngineTypeHelper.h"
#include "Core/EngineStatus.h"
#include "IOC/Observer.h"
#include "Threading/Looper.h"

#include "Resources/System/Core/ResourceDomainTransfer.h"
#include "Resources/System/Core/ResourceTask.h"
#include "Resources/Subsystems/GFXAPI/GFXAPI.h"

#include "GFXAPI/Types/All.h"

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

    using ResourceTaskFn_t = std::function<GFXAPIResourceHandleAssignment()>;

    using ResolvedDependencyCollection      = std::map<ResourceHandle, Ptr<void>>;
    using PublicToPrivateBackendResourceMap = std::map<GFXAPIResourceHandle_t, Ptr<void>>;

    template <typename T>
    class IGFXAPIResourceTaskBackendDecl
    {
    public:
      virtual EEngineStatus creationTask(
        typename T::CreationRequest  const&inRequest,
        ResolvedDependencyCollection const&inDependencies,
        ResourceTaskFn_t                  &outTask) = 0;

      virtual EEngineStatus updateTask(
        typename T::UpdateRequest    const&inRequest,
        ResolvedDependencyCollection const&inDependencies,
        ResourceTaskFn_t                  &outTask) = 0;

      virtual EEngineStatus destructionTask(
        typename T::DestructionRequest const&inRequest,
        ResolvedDependencyCollection   const&inDependencies,
        ResourceTaskFn_t                    &outTask) = 0;

      virtual EEngineStatus queryTask(
        const typename T::Query &inRequest,
        ResourceTaskFn_t        &outTask) = 0;
    };

    template <typename... TSupportedTypes>
    class IGFXAPIResourceTaskBackend
      : public IGFXAPIResourceTaskBackendDecl<TSupportedTypes>...
    {};

    /**********************************************************************************************//**
     * \struct	DeferredResourceOperationHandle
     *
     * \brief	Handle class to hold an asynchronous request.
     **************************************************************************************************/
    struct DeferredResourceOperationHandle {
      std::future<ResourceTaskFn_t::result_type>& futureHandle;
    };

    DeclareInterface(IAsyncLoadCallback);
    /**********************************************************************************************//**
     * \fn	virtual void GFXAPIResourceBackend::onResourceLoaded(const GFXAPIResourceHandle_t handle) = 0;
     *
     * \brief	Invoked once the resource was loaded. If the handle is 0, loading failed. If > 0, successful.
     *
     * \param	handle	The handle.
     **************************************************************************************************/
    virtual void onResourceLoaded(const GFXAPIResourceHandle_t handle) = 0;
    DeclareInterfaceEnd(IAsyncLoadCallback);

    /**********************************************************************************************//**
     * \class	GFXAPIResourceBackend
     *
     * \brief	The GFXAPIResourceBackend is responsible for spawning a resource task based on
     * 			 the specific request, executing it (a-)synchronuously and store the results.
     * 			The task itself will be spawned by a TaskBuilder, determined during runtime.
     * 			The resourceBackend holds a mapping of GFXAPIResourceHandle_t to GFXAPIResourceHolder to
     * 			 store and manage the resources.
     **************************************************************************************************/
    template <typename... TSupportedResourceTypes>
    class GFXAPIResourceBackend {
      DeclareLogTag(GFXAPIResourceBackend);

    public:
      typedef IGFXAPIResourceTaskBackend<TSupportedResourceTypes...> ResourceTaskBackend_t;
      typedef Ptr<ResourceTaskBackend_t>                             ResourceTaskBackendPtr;

      GFXAPIResourceBackend();

      template <typename TResource>
      EEngineStatus load(
        typename TResource::CreationRequest const&inRequest,
        GFXAPIResourceHandleMap             const&dependencies,
        const ETaskSynchronization               &inRequestMode,
        const Ptr<IAsyncLoadCallback>            &inCallback,
        GFXAPIResourceHandle_t                   &outResourceHandle);

      template <typename TResource>
      EEngineStatus unload(
        typename TResource::DestructionRequest const& inRequest);

      template <typename TUnderlyingType>
      EEngineStatus getUnderlyingHandle(
        GFXAPIResourceHandle_t const&handle,
        Ptr<TUnderlyingType>        &outHandlePtr)
      {
        assert(CheckValidHandle(handle));

        Ptr<void> anything = findUnderlyingHandle(handle);
        assert(anything != nullptr);

        Ptr<TUnderlyingType> p = std::dynamic_pointer_cast<TUnderlyingType>(anything);
        assert(p != nullptr);

        return p;
      }

      void setResourceTaskBackend(ResourceTaskBackendPtr const& backend) {
        assert(backend != nullptr);

        m_resourceTaskBackend = backend;
      }

    private:
      template <typename TResource>
      EEngineStatus loadImpl(
        typename TResource::CreationRequest const&inRequest,
        ResolvedDependencyCollection        const&resolvedDependencies,
        DeferredResourceOperationHandle          &outHandle);

      template <typename TResource>
      EEngineStatus unloadImpl(
        typename TResource::DestructionRequest const&inRequest,
        ResolvedDependencyCollection           const&resolvedDependencies,
        DeferredResourceOperationHandle             &outHandle);

      EEngineStatus enqueue(
        ResourceTaskFn_t                           &inTask,
        std::future<ResourceTaskFn_t::result_type> &outSharedFuture);

      ResourceTaskBackendPtr m_resourceTaskBackend;
      

      Threading::Looper<ResourceTaskFn_t::result_type>           m_resourceThread;
      Threading::Looper<ResourceTaskFn_t::result_type>::Handler &m_resourceThreadHandler;
    };

    template <typename... TSupportedResourceTypes>
    GFXAPIResourceBackend<TSupportedResourceTypes...>
      ::GFXAPIResourceBackend()
      : m_resourceThread()
      , m_resourceThreadHandler(m_resourceThread.getHandler())
    {}

    /**********************************************************************************************//**
     * \fn	template <typename TResource> EEngineStatus GFXAPIResourceBackend::load( const ResourceDescriptor<TResource> &inDescriptor, const GFXAPIResourceHandleMap &inResourceDependencyHandles, const ETaskSynchronization &inRequestMode, const Ptr<IAsyncLoadCallback> &inCallback, GFXAPIResourceHandle_t &outResourceHandle)
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
    template <typename... TSupportedResourceTypes>
    template <typename TResource>
    EEngineStatus
      GFXAPIResourceBackend<TSupportedResourceTypes...>
      ::load(
        typename TResource::CreationRequest const&inRequest,
        GFXAPIResourceHandleMap             const&dependencies,
        const ETaskSynchronization               &inSynchronization,
        const Ptr<IAsyncLoadCallback>            &inCallback,
        GFXAPIResourceHandle_t                   &outResourceHandle)
    {
      ResourceTaskFn_t::result_type resourceHandle ={};

      DeferredResourceOperationHandle handle={};

      // Resolve dependencies...
      ResolvedDependencyCollection resolvedDependencies={};
      for(GFXAPIResourceHandleMap::value_type const&h : dependencies)
        resolvedDependencies[h.first] = m_store[h.second];

      EEngineStatus status = loadImpl<TResource>(inRequest, resolvedDependencies, handle);
      if(!CheckEngineError(status)) {
        switch(inSynchronization) {
        default:
        case ETaskSynchronization::Sync:
          resourceHandle = handle.futureHandle.get(); // Wait for it...
          if(!resourceHandle.valid())
            status = EEngineStatus::GFXAPI_SubsystemResourceCreationFailed;
          else {
            // Store the internal handle and return the public handle
            m_storage[resourceHandle.publicHandle] = resourceHandle.internalHandle;
            outResourceHandle = resourceHandle.publicHandle;

            status = EEngineStatus::Ok;
          }
          break;
        case ETaskSynchronization::Async:
          // Unsupported so far...
          break;
        }
      }

      HandleEngineStatusError(status, String::format("Failed to create and/or enqueue resource creation task."));

      return status;
    }

    template <typename... TSupportedResourceTypes>
    template <typename TResource>
    EEngineStatus
      GFXAPIResourceBackend<TSupportedResourceTypes...>
      ::unload(
        typename TResource::DestructionRequest const&inRequest)
    {
      EEngineStatus status = EEngineStatus::Ok;

      ResolvedDependencyCollection resolvedDependencies={}; // Guarding the public API by passing in this empty map.

      DeferredResourceOperationHandle handle={};
      EEngineStatus status = unloadImpl<TResource>(inRequest, resolvedDependencies, handle);
      if(!CheckEngineError(status)) {
        GFXAPIResourceHandle_t resourceHandle = handle.futureHandle.get(); // Wait for it ALWAYS!
        if(resourceHandle.valid()) {
          status = EEngineStatus::GFXAPI_SubsystemResourceDestructionFailed;
        }
        else {
          RemoveTheFuckOutOfIt(resourceHandle);

          status = EEngineStatus::Ok;
        }
      }

      HandleEngineStatusError(status, String::format("Failed to create and/or enqueue resource destruction task."));

      return status;
    }

    /**********************************************************************************************//**
     * \fn	template <typename TResource> EEngineStatus GFXAPIResourceBackend::loadImpl( const ResourceDescriptor<TResource> &inDescriptor, GFXAPIResourceHandleMap &inResourceDependencyHandles, DeferredResourceOperationHandle &outHandle)
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
    template <typename... TSupportedResourceTypes>
    template <typename TResource>
    EEngineStatus GFXAPIResourceBackend<TSupportedResourceTypes...>
      ::loadImpl(
        typename TResource::CreationRequest  const&inRequest,
        ResolvedDependencyCollection         const&resolvedDependencies,
        DeferredResourceOperationHandle           &outHandle)
    {
      EEngineStatus status = EEngineStatus::Ok;

      ResourceTaskFn_t task = nullptr;
      status = (*m_resourceTaskBackend).IGFXAPIResourceTaskBackend<TResource>::creationTask(inRequest, resolvedDependencies, task);
      if(CheckEngineError(status)) {
        Log::Error(logTag(), String::format("Failed to create build task for resource '%0'", "..."));
        return status;
      }

      std::future<ResourceTaskFn_t::result_type> future;
      status = enqueue<TResource>(task, future);
      if(CheckEngineError(status)) {
        Log::Error(logTag(), String::format("Failed to enqueue resource creation task for resource '%0'", "..."));
        return status;
      }

      outHandle.futureHandle = std::move(future);

      return status;
    }

    template <typename... TSupportedResourceTypes>
    template <typename TResource>
    EEngineStatus GFXAPIResourceBackend<TSupportedResourceTypes...>
      ::unloadImpl(
        typename TResource::DestructionRequest const&inRequest,
        ResolvedDependencyCollection           const&resolvedDependencies,
        DeferredResourceOperationHandle             &outHandle)
    {
      EEngineStatus status = EEngineStatus::Ok;

      ResourceTaskFn_t task = nullptr;
      status = (*m_resourceTaskBackend).IGFXAPIResourceTaskBackend<TResource>::destructionTask(inRequest, resolvedDependencies, task);
      if(CheckEngineError(status)) {
        Log::Error(logTag(), String::format("Failed to create destruction task for resource '%0'", "..."));
        return status;
      }

      std::future<ResourceTaskFn_t::result_type> future;
      status = enqueue<TResource>(task, future);
      if(CheckEngineError(status)) {
        Log::Error(logTag(), String::format("Failed to enqueue resource creation task for resource '%0'", "..."));
        return status;
      }

      outHandle.futureHandle = std::move(future);

      return status;
    }

    /**********************************************************************************************//**
     * \fn	template <typename TResource> EEngineStatus GFXAPIResourceBackend::enqueue( const ResourceTaskFn_t &inTask, std::future<GFXAPIResourceHandle_t> &outSharedFuture)
     *
     * \brief	Adds an object onto the end of this queue
     *
     * \tparam	TResource	Type of the resource.
     * \param 		  	inTask		   	The in task.
     * \param [in,out]	outSharedFuture	The out shared future.
     *
     * \return	The EEngineStatus.
     **************************************************************************************************/
    template <typename... TSupportedResourceTypes>
    EEngineStatus
      GFXAPIResourceBackend<TSupportedResourceTypes...>
      ::enqueue(
        ResourceTaskFn_t                           &inTask,
        std::future<ResourceTaskFn_t::result_type> &outSharedFuture)
    {
      using namespace Threading;

      std::future<ResourceTaskFn_t::result_type>   looperTaskFuture;
      ILooper<ResourceTaskFn_t::result_type>::Task looperTask;
      looperTask.setPriority(Priority::Normal);

      looperTaskFuture = looperTask.bind(inTask);
      outSharedFuture  = std::move(looperTaskFuture);

      bool enqueued = m_resourceThreadHandler.post(std::move(looperTask));

      return (enqueued ? EEngineStatus::Ok : EEngineStatus::GFXAPI_SubsystemThreadEnqueueFailed);
    }

    using BasicGFXAPIResourceBackend = GFXAPIResourceBackend<EngineTypes>;
  }
}

#endif