#ifndef __SHIRABE_GFXAPIRESOURCEBACKEND_H__
#define __SHIRABE_GFXAPIRESOURCEBACKEND_H__

#include <core/threading/looper.h>
#include <core/string.h>

#include "graphicsapi/resources/gfxapiresourcetaskbackend.h"

namespace engine
{
    namespace gfxapi
    {
        using namespace resources;

        /**
         * The EGFXAPI enum identifies a specific graphics API
         */
        enum class EGFXAPI
        {
            DirectX = 1,
            OpenGL  = 2,
            Vulkan  = 4
        };

        /**
         * The EGFXAPIVersion enum identifies a specific graphics API version requirement.
         */
        enum class EGFXAPIVersion
        {
            DirectX_11_0,
            DirectX_11_1,
            DirectX_11_2,
            DirectX_12_0,
            OpenGL_,
            Vulkan_1_0,
            Vulkan_1_1
        };

        /**
         * The IAsyncLoadCallback interface describes callback signatures required for objects
         * that need to receive asynchronous graphics API backend operation results.
         */
        class IAsyncLoadCallback
        {
            SHIRABE_DECLARE_INTERFACE(IAsyncLoadCallback);

            public_api:
            /**
             * Invoked once the resource was loaded.
             * If the handle is 0, loading failed. If > 0, successful.
             *
             * @param aHandle The resource handle of the resource successfully loaded.
             */
            virtual void onResourceLoaded(PublicResourceId_t const &aHandle) = 0;
        };

        /**
         * The GFXAPIResourceBackend is responsible for spawning a resource task based on
         * the specific request, executing it (a-)synchronuously and store the results.
         * The task itself will be spawned by a TaskBuilder, determined during runtime.
         * The resourceBackend holds a mapping of GFXAPIResourceHandle_t to GFXAPIResourceHolder to
         * store and manage the resources.
         */
        class SHIRABE_TEST_EXPORT CGFXAPIResourceBackend
        {
            SHIRABE_DECLARE_LOG_TAG(CGFXAPIResourceBackend)

        public_typedefs:
            using ResourceTaskBackend_t = CGFXAPIResourceTaskBackend;

        public_constructors:
            /**
             * Construct an empty resource task backend.
             */
            CGFXAPIResourceBackend();

        public_methods:
            /**
             * Initialize the resource task backend, which will create and start
             * a separate resource operation thread.
             *
             * @return True, if initialized successfully. False otherwise.
             */
            bool initialize();
            /**
             * Deinitialize the resource task backend, stopping the resource
             * operation thread and joining it.
             * All resource will be removed beforehand.
             *
             * @return True, if successful. False, otherwise.
             */
            bool deinitialize();

            /**
             * Create/Load a specific resource specified by its creation request
             * and provide the result to 'aCallback'.
             *
             * @param aRequest      The resource creation request for the resource to be created.
             * @param aDependencies Dependency UIDs of the resource required for resource creation.
             * @param aSyncMode     Async/Sync creation?
             * @param aCallback     Callback to be invoked on successful resource creation.
             * @return              EEngineStatus::Ok, if successful. An error code otherwise.
             */
            template <typename TResource>
            EEngineStatus load(
                    typename TResource::CCreationRequest const &aRequest,
                    PublicResourceIdList_t               const &aDependencies,
                    ETaskSynchronization                 const &aSyncMode,
                    CStdSharedPtr_t<IAsyncLoadCallback>  const &aCallback);

            /**
             * Unload a specific resource specified by its destruction request.
             *
             * @param aRequest The destruction request used to destroy the resource.
             * @return         EEngineStatus::Ok, if successful. An error code otherwise.
             */
            template <typename TResource>
            EEngineStatus unload(
                    typename TResource::CDestructionRequest const &aRequest);

            /**
             * Register a resource created externally, which has to be accessible through the backend.
             *
             * @param aId       The resource id of the resource.
             * @param aResource The resource to be registered.
             * @return          EEngineStatus::Ok, if successful. An error code otherwise.
             */
            EEngineStatus registerResource(
                    PublicResourceId_t    const &aId,
                    CStdSharedPtr_t<void> const &aResource);

            /**
             * Register a resource task backend used for creating specific op-tasks based
             * on the selected graphics API.
             *
             * @param aBackend The backend pointer. Musn't be nullptr.
             */
            void setResourceTaskBackend(CStdSharedPtr_t<ResourceTaskBackend_t> const &aBackend);

        private_methods:
            /**
             * Implementation of the load operation.
             *
             * @param aRequest              Creation request used to create the resource.
             * @param aResolvedDependencies Dependency UIDs required to create the resource/
             * @param aOutHandle            The result of an async/sync resource operation.
             * @return                      EEngineStatus::Ok if successful. An error code otherwise.
             */
            template <typename TResource>
            EEngineStatus loadImpl(
                    typename TResource::CCreationRequest const &aRequest,
                    ResolvedDependencyCollection_t       const &aResolvedDependencies,
                    SDeferredResourceOperationHandle           &aOutHandle);

            /**
             * Implementation of the unload operation.
             *
             * @param aRequest              Creation request used to create the resource.
             * @param aAssignment           Existing public/internal resource handle assignment for the
             *                              unload operation.
             * @param aResolvedDependencies Dependency UIDs required to create the resource/
             * @param aOutHandle            The result of an async/sync resource operation.
             * @return                      EEngineStatus::Ok if successful. An error code otherwise.
             */
            template <typename TResource>
            EEngineStatus unloadImpl(
                    typename TResource::CDestructionRequest const &aRequest,
                    SGFXAPIResourceHandleAssignment         const &aAssignment,
                    ResolvedDependencyCollection_t          const &aResolvedDependencies,
                    SDeferredResourceOperationHandle              &aOutHandle);

            /**
             * Enqueues a new resource operation task into the resource operation thread.
             *
             * @param aTask            The task to be enqueued.
             * @param aOutSharedFuture The future handle for the tasks to fetch the results.
             * @return                 EEngineStatus::Ok, if successful. An error code otherwise.
             */
            EEngineStatus enqueue(
                    ResourceTaskFn_t                           &aTask,
                    std::future<ResourceTaskFn_t::result_type> &aOutSharedFuture);

        private_members:
            CStdSharedPtr_t<ResourceTaskBackend_t>                       mResourceTaskBackend;
            ResolvedDependencyCollection_t                               mStorage;
            threading::CLooper<ResourceTaskFn_t::result_type>            mResourceThread;
            threading::CLooper<ResourceTaskFn_t::result_type>::CHandler &mResourceThreadHandler;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        EEngineStatus CGFXAPIResourceBackend::load(
                typename TResource::CCreationRequest const &aRequest,
                PublicResourceIdList_t               const &aDependencies,
                ETaskSynchronization                 const &aSyncMode,
                CStdSharedPtr_t<IAsyncLoadCallback>  const &aCallback)
        {
            ResourceTaskFn_t::result_type    resourceHandle = {};
            SDeferredResourceOperationHandle handle         = {};

            EEngineStatus status = EEngineStatus::Ok;

            // Resolve dependencies...
            ResolvedDependencyCollection_t resolvedDependencies = {};
            for(PublicResourceIdList_t::value_type const &dependency : aDependencies)
            {
                resolvedDependencies[dependency] = mStorage[dependency];
            }

            try
            {
                status = loadImpl<TResource>(aRequest, resolvedDependencies, handle);
                if(!CheckEngineError(status))
                {
                    switch(aSyncMode)
                    {
                    case ETaskSynchronization::Sync:
                        resourceHandle = handle.futureHandle.get(); // Wait for it...
                        if(!resourceHandle.valid())
                        {
                            status = EEngineStatus::GFXAPI_SubsystemResourceCreationFailed;
                        }
                        else
                        {
                            // Store the internal handle and return the public handle
                            registerResource(resourceHandle.publicResourceHandle, resourceHandle.internalResourceHandle);

                            status = EEngineStatus::Ok;
                        }
                        break;
                    case ETaskSynchronization::Async:
                        // Unsupported so far...
                        break;
                    }
                }

                HandleEngineStatusError(status, CString::format("Failed to create and/or enqueue resource creation task."));
            }
            catch(std::future_error const&fe)
            {
                CLog::Error(logTag(), CString::format("Failed to access future shared state. Error: %0", fe.what()));
            }

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        EEngineStatus CGFXAPIResourceBackend::unload(
                typename TResource::CDestructionRequest const &aRequest)
        {
            ResourceTaskFn_t::result_type    resourceHandle       ={};
            ResolvedDependencyCollection_t   resolvedDependencies ={}; // Guarding the public API by passing in this empty map.
            SDeferredResourceOperationHandle handle               ={};

            EEngineStatus status = EEngineStatus::Ok;
            try
            {
                CStdSharedPtr_t<void> resource = mStorage[aRequest.publicResourceId()];
                status = unloadImpl<TResource>(
                            aRequest,
                            {
                                aRequest.publicResourceId(),
                                resource
                            },
                            resolvedDependencies,
                            handle);

                if(!CheckEngineError(status))
                {
                    resourceHandle = handle.futureHandle.get(); // Wait for it ALWAYS!
                    mStorage.erase(resourceHandle.publicResourceHandle);
                    status = EEngineStatus::Ok;
                }
            }
            catch(std::future_error const&fe)
            {
                CLog::Error(logTag(), CString::format("Failed to access future shared state. Error: %0", fe.what()));
            }

            HandleEngineStatusError(status, CString::format("Failed to create and/or enqueue resource destruction task."));
            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        EEngineStatus CGFXAPIResourceBackend::loadImpl(
                typename TResource::CCreationRequest const &aRequest,
                ResolvedDependencyCollection_t       const &aResolvedDependencies,
                SDeferredResourceOperationHandle           &aOutHandle)
        {
            ResourceTaskFn_t task = nullptr;

            EEngineStatus status = mResourceTaskBackend->creationTask<TResource>(aRequest, aResolvedDependencies, task);
            if(CheckEngineError(status))
            {
                CLog::Error(logTag(), CString::format("Failed to create build task for resource '%0'", "..."));
                return status;
            }

            std::future<ResourceTaskFn_t::result_type> future{};
            status = enqueue(task, future);
            if(CheckEngineError(status))
            {
                CLog::Error(logTag(), CString::format("Failed to enqueue resource creation task for resource '%0'", "..."));
                return status;
            }

            aOutHandle.futureHandle = std::move(future);

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        EEngineStatus CGFXAPIResourceBackend::unloadImpl(
                typename TResource::CDestructionRequest const &aRequest,
                SGFXAPIResourceHandleAssignment         const &aAssignment,
                ResolvedDependencyCollection_t          const &aResolvedDependencies,
                SDeferredResourceOperationHandle              &aOutHandle)
        {
            ResourceTaskFn_t task = nullptr;

            EEngineStatus status = mResourceTaskBackend->destructionTask<TResource>(aRequest, aAssignment, aResolvedDependencies, task);
            if(CheckEngineError(status))
            {
                CLog::Error(logTag(), CString::format("Failed to create destruction task for resource '%0'", "..."));
                return status;
            }

            std::future<ResourceTaskFn_t::result_type> future;
            status = enqueue(task, future);
            if(CheckEngineError(status))
            {
                CLog::Error(logTag(), CString::format("Failed to enqueue resource creation task for resource '%0'", "..."));
                return status;
            }

            aOutHandle.futureHandle = std::move(future);

            return status;
        }
        //<-----------------------------------------------------------------------------
    }
}

#endif
