#ifndef __SHIRABE_GFXAPIRESOURCEBACKEND_H__
#define __SHIRABE_GFXAPIRESOURCEBACKEND_H__

#include "GraphicsAPI/Resources/GFXAPIResourceTaskBackend.h"

namespace Engine {
  namespace GFXAPI {
    using namespace Resources;

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
      Vulkan_1_0,
      Vulkan_1_1
    };

    DeclareInterface(IAsyncLoadCallback);
    /**********************************************************************************************//**
     * \fn	virtual void GFXAPIResourceBackend::onResourceLoaded(const GFXAPIResourceHandle_t handle) = 0;
     *
     * \brief	Invoked once the resource was loaded. If the handle is 0, loading failed. If > 0, successful.
     *
     * \param	handle	The handle.
     **************************************************************************************************/
    virtual void onResourceLoaded(const PublicResourceId_t handle) = 0;
    DeclareInterfaceEnd(IAsyncLoadCallback);

    /**********************************************************************************************//**
     * \class	GFXAPIResourceBackend
     *
     * \brief	The GFXAPIResourceBackend is responsible for spawning a resource task based on
     * 			   the specific request, executing it (a-)synchronuously and store the results.
     * 			  The task itself will be spawned by a TaskBuilder, determined during runtime.
     * 			  The resourceBackend holds a mapping of GFXAPIResourceHandle_t to GFXAPIResourceHolder to
     * 			   store and manage the resources.
     **************************************************************************************************/
    class SHIRABE_TEST_EXPORT GFXAPIResourceBackend {
      DeclareLogTag(GFXAPIResourceBackend);

    public:
      typedef GFXAPIResourceTaskBackend  ResourceTaskBackend_t;
      typedef CStdSharedPtr_t<ResourceTaskBackend_t> ResourceTaskBackendPtr;

      GFXAPIResourceBackend();

      bool initialize();
      bool deinitialize();

      template <typename TResource>
      EEngineStatus load(
        typename TResource::CreationRequest const&inRequest,
        PublicResourceIdList                const&dependencies,
        const ETaskSynchronization               &inRequestMode,
        const CStdSharedPtr_t<IAsyncLoadCallback>            &inCallback);

      template <typename TResource>
      EEngineStatus unload(
        typename TResource::DestructionRequest const&inRequest);

      EEngineStatus registerResource(
        PublicResourceId_t const&id,
        CStdSharedPtr_t<void>                resouce);

      void setResourceTaskBackend(ResourceTaskBackendPtr const& backend);

    private:
      template <typename TResource>
      EEngineStatus loadImpl(
        typename TResource::CreationRequest const&inRequest,
        ResolvedDependencyCollection        const&resolvedDependencies,
        DeferredResourceOperationHandle          &outHandle);

      template <typename TResource>
      EEngineStatus unloadImpl(
        typename TResource::DestructionRequest const&inRequest,
        GFXAPIResourceHandleAssignment         const&assignment,
        ResolvedDependencyCollection           const&resolvedDependencies,
        DeferredResourceOperationHandle             &outHandle);

      EEngineStatus enqueue(
        ResourceTaskFn_t                           &inTask,
        std::future<ResourceTaskFn_t::result_type> &outSharedFuture);

      ResourceTaskBackendPtr       m_resourceTaskBackend;
      ResolvedDependencyCollection m_storage;

      Threading::Looper<ResourceTaskFn_t::result_type>           m_resourceThread;
      Threading::Looper<ResourceTaskFn_t::result_type>::Handler &m_resourceThreadHandler;
    };


    /**********************************************************************************************//**
     * \fn	template <typename TResource> EEngineStatus GFXAPIResourceBackend::load( const ResourceDescriptor<TResource> &inDescriptor, const GFXAPIResourceHandleMap &inResourceDependencyHandles, const ETaskSynchronization &inRequestMode, const CStdSharedPtr_t<IAsyncLoadCallback> &inCallback, GFXAPIResourceHandle_t &outResourceHandle)
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
    EEngineStatus
      GFXAPIResourceBackend
      ::load(
        typename TResource::CreationRequest const&inRequest,
        PublicResourceIdList                const&dependencies,
        const ETaskSynchronization               &inSynchronization,
        const CStdSharedPtr_t<IAsyncLoadCallback>            &inCallback)
    {
      ResourceTaskFn_t::result_type resourceHandle ={};

      DeferredResourceOperationHandle handle;

      EEngineStatus status = EEngineStatus::Ok;

      // Resolve dependencies...
      ResolvedDependencyCollection resolvedDependencies={};
      for(PublicResourceIdList::value_type const&h : dependencies)
        resolvedDependencies[h] = m_storage[h];

      try {

        status = loadImpl<TResource>(inRequest, resolvedDependencies, handle);
        if(!CheckEngineError(status)) {
          switch(inSynchronization) {
          default:
          case ETaskSynchronization::Sync:
            resourceHandle = handle.futureHandle.get(); // Wait for it...
            if(!resourceHandle.valid())
              status = EEngineStatus::GFXAPI_SubsystemResourceCreationFailed;
            else {
              // Store the internal handle and return the public handle
              registerResource(resourceHandle.publicHandle, resourceHandle.internalHandle);

              status = EEngineStatus::Ok;
            }
            break;
          case ETaskSynchronization::Async:
            // Unsupported so far...
            break;
          }
        }

        HandleEngineStatusError(status, String::format("Failed to create and/or enqueue resource creation task."));
      }
      catch(std::future_error const&fe) {
        Log::Error(logTag(), String::format("Failed to access future shared state. Error: %0", fe.what()));
      }

      return status;
    }

    template <typename TResource>
    EEngineStatus
      GFXAPIResourceBackend
      ::unload(
        typename TResource::DestructionRequest const&inRequest)
    {
      ResourceTaskFn_t::result_type resourceHandle ={ };

      ResolvedDependencyCollection resolvedDependencies={}; // Guarding the public API by passing in this empty map.

      DeferredResourceOperationHandle handle;

      EEngineStatus status = EEngineStatus::Ok;
      try {
        status = unloadImpl<TResource>(inRequest, { inRequest.publicResourceId(), m_storage[inRequest.publicResourceId()] }, resolvedDependencies, handle);
        if(!CheckEngineError(status)) {
          resourceHandle = handle.futureHandle.get(); // Wait for it ALWAYS!
          m_storage.erase(resourceHandle.publicHandle);
          status = EEngineStatus::Ok;
        }
      }
      catch(std::future_error const&fe) {
        Log::Error(logTag(), String::format("Failed to access future shared state. Error: %0", fe.what()));
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
    template <typename TResource>
    EEngineStatus GFXAPIResourceBackend
      ::loadImpl(
        typename TResource::CreationRequest  const&inRequest,
        ResolvedDependencyCollection         const&resolvedDependencies,
        DeferredResourceOperationHandle           &outHandle)
    {
      EEngineStatus status = EEngineStatus::Ok;

      ResourceTaskFn_t task = nullptr;
      status = m_resourceTaskBackend->creationTask<TResource>(inRequest, resolvedDependencies, task);
      if(CheckEngineError(status)) {
        Log::Error(logTag(), String::format("Failed to create build task for resource '%0'", "..."));
        return status;
      }

      std::future<ResourceTaskFn_t::result_type> future;
      status = enqueue(task, future);
      if(CheckEngineError(status)) {
        Log::Error(logTag(), String::format("Failed to enqueue resource creation task for resource '%0'", "..."));
        return status;
      }

      outHandle.futureHandle = std::move(future);

      return status;
    }

    template <typename TResource>
    EEngineStatus GFXAPIResourceBackend
      ::unloadImpl(
        typename TResource::DestructionRequest const&inRequest,
        GFXAPIResourceHandleAssignment         const&assignment,
        ResolvedDependencyCollection           const&resolvedDependencies,
        DeferredResourceOperationHandle             &outHandle)
    {
      EEngineStatus status = EEngineStatus::Ok;

      ResourceTaskFn_t task = nullptr;
      status = m_resourceTaskBackend->destructionTask<TResource>(inRequest, assignment, resolvedDependencies, task);
      if(CheckEngineError(status)) {
        Log::Error(logTag(), String::format("Failed to create destruction task for resource '%0'", "..."));
        return status;
      }

      std::future<ResourceTaskFn_t::result_type> future;
      status = enqueue(task, future);
      if(CheckEngineError(status)) {
        Log::Error(logTag(), String::format("Failed to enqueue resource creation task for resource '%0'", "..."));
        return status;
      }

      outHandle.futureHandle = std::move(future);

      return status;
    }
  }
}

#endif