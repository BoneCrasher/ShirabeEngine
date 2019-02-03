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

        public_enums:
            /**
             * The EImportStorageMode enum describes flags for resource imports in case
             * a resource with an equal identifier was already stored.
             */
            enum class EImportStorageMode
            {
                NoOverwrite = 1,
                Overwrite   = 2
            };

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
             * @return A result containing EEngineStatus::Ok, if successful.
             * @return A result containing an EEngineStatus error value on error.
             */
            CEngineResult<> initialize();
            /**
             * Deinitialize the resource task backend, stopping the resource
             * operation thread and joining it.
             * All resource will be removed beforehand.
             *
             * @return A result containing EEngineStatus::Ok, if successful.
             * @return A result containing an EEngineStatus error value on error.
             */
            CEngineResult<> deinitialize();

            /**
             * Create/Load a specific resource specified by its creation request
             * and provide the result to 'aCallback'.
             *
             * @param aRequest      The resource creation request for the resource to be created.
             * @param aDependencies Dependency UIDs of the resource required for resource creation.
             * @param aSyncMode     Async/Sync creation?
             * @param aCallback     Callback to be invoked on successful resource creation.
             * @return A result containing EEngineStatus::Ok, if successful.
             * @return A result containing an EEngineStatus error value on error.
             */
            template <typename TResource>
            CEngineResult<> load(
                    typename TResource::CCreationRequest const &aRequest,
                    PublicResourceIdList_t               const &aDependencies,
                    ETaskSynchronization                 const &aSyncMode,
                    CStdSharedPtr_t<IAsyncLoadCallback>  const &aCallback);

            /**
             * Unload a specific resource specified by its destruction request.
             *
             * @param aRequest The destruction request used to destroy the resource.
             * @return A result containing EEngineStatus::Ok, if successful.
             * @return A result containing an EEngineStatus error value on error.
             */
            template <typename TResource>
            CEngineResult<> unload(
                    typename TResource::CDestructionRequest const &aRequest);

            /**
             * Register a resource created externally, which has to be accessible through the backend.
             *
             * @param aId       The resource id of the resource.
             * @param aResource The resource to be registered.
             * @return A result containing EEngineStatus::Ok, if successful.
             * @return A result containing an EEngineStatus error value on error.
             */
            CEngineResult<> registerResource(
                    PublicResourceId_t    const &aId,
                    CStdSharedPtr_t<void> const &aResource,
                    EImportStorageMode    const &aImportStorageMode = EImportStorageMode::NoOverwrite);

            /**
             * Register a resource task backend used for creating specific op-tasks based
             * on the selected graphics API.
             *
             * @param aBackend The backend pointer. Musn't be nullptr.
             */
            CEngineResult<> setResourceTaskBackend(CStdSharedPtr_t<ResourceTaskBackend_t> const &aBackend);

        private_methods:
            /**
             * Implementation of the load operation.
             *
             * @param aRequest              Creation request used to create the resource.
             * @param aResolvedDependencies Dependency UIDs required to create the resource/
             * @param aOutHandle            The result of an async/sync resource operation.
             * @return A result containing EEngineStatus::Ok, if successful.
             * @return A result containing an EEngineStatus error value on error.
             */
            template <typename TResource>
            CEngineResult<> loadImpl(
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
             * @return A result containing EEngineStatus::Ok, if successful.
             * @return A result containing an EEngineStatus error value on error.
             */
            template <typename TResource>
            CEngineResult<> unloadImpl(
                    typename TResource::CDestructionRequest const &aRequest,
                    SGFXAPIResourceHandleAssignment         const &aAssignment,
                    ResolvedDependencyCollection_t          const &aResolvedDependencies,
                    SDeferredResourceOperationHandle              &aOutHandle);

            /**
             * Enqueues a new resource operation task into the resource operation thread.
             *
             * @param aTask            The task to be enqueued.
             * @param aOutSharedFuture The future handle for the tasks to fetch the results.
             * @return A result containing EEngineStatus::Ok, if successful.
             * @return A result containing an EEngineStatus error value on error.
             */
            CEngineResult<> enqueue(
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
        CEngineResult<> CGFXAPIResourceBackend::load(
                typename TResource::CCreationRequest const &aRequest,
                PublicResourceIdList_t               const &aDependencies,
                ETaskSynchronization                 const &aSyncMode,
                CStdSharedPtr_t<IAsyncLoadCallback>  const &aCallback)
        {
            SHIRABE_UNUSED(aCallback);

            ResourceTaskFn_t::result_type    resourceHandleFetch = {};
            SDeferredResourceOperationHandle handle             = {};

            CEngineResult<> loadOp = { EEngineStatus::Ok };

            // Resolve dependencies...
            ResolvedDependencyCollection_t resolvedDependencies = {};
            for(PublicResourceIdList_t::value_type const &dependency : aDependencies)
            {
                resolvedDependencies[dependency] = mStorage[dependency];
            }

            try
            {
                loadOp = loadImpl<TResource>(aRequest, resolvedDependencies, handle);
                if(loadOp.successful())
                {
                    switch(aSyncMode)
                    {
                    case ETaskSynchronization::Sync:
                        resourceHandleFetch = handle.futureHandle.get(); // Wait for it...
                        if(not resourceHandleFetch.successful() && not resourceHandleFetch.data().valid())
                        {
                            loadOp = CEngineResult<>(EEngineStatus::GFXAPI_SubsystemResourceCreationFailed);
                        }
                        else
                        {
                            SGFXAPIResourceHandleAssignment const &assignment = resourceHandleFetch.data();

                            // Store the internal handle and return the public handle
                            registerResource(assignment.publicResourceHandle, assignment.internalResourceHandle);

                            loadOp = CEngineResult<>(EEngineStatus::Ok);
                        }
                        break;
                    case ETaskSynchronization::Async:
                        // Unsupported so far...
                        break;
                    }
                }

                return CEngineResult<>(loadOp);
            }
            catch(std::future_error const &fe)
            {
                CLog::Error(logTag(), CString::format("Failed to access future shared state. Error: %0", fe.what()));
                return CEngineResult<>(EEngineStatus::Error);
            }
            catch(std::exception const &e)
            {
                CLog::Error(logTag(), CString::format("Error while trying to request a task result in the resource backend. Error: %0", e.what()));
                return CEngineResult<>(EEngineStatus::Error);

            }
            catch(...)
            {
                CLog::Error(logTag(), CString::format("Unknown error while trying to request a task result in the resource backend."));
                return CEngineResult<>(EEngineStatus::Error);
            }

            return loadOp;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<> CGFXAPIResourceBackend::unload(
                typename TResource::CDestructionRequest const &aRequest)
        {
            ResourceTaskFn_t::result_type    resourceHandleFetch  ={};
            ResolvedDependencyCollection_t   resolvedDependencies ={}; // Guarding the public API by passing in this empty map.
            SDeferredResourceOperationHandle handle               ={};

            CEngineResult<> unloadOp = EEngineStatus::Ok;
            try
            {
                CStdSharedPtr_t<void> resource = mStorage[aRequest.publicResourceId()];
                unloadOp = unloadImpl<TResource>(
                            aRequest,
                            {
                                aRequest.publicResourceId(),
                                resource
                            },
                            resolvedDependencies,
                            handle);

                if(unloadOp.successful())
                {
                    resourceHandleFetch = handle.futureHandle.get(); // Wait for it ALWAYS!
                    if(resourceHandleFetch.successful())
                    {
                        mStorage.erase(resourceHandleFetch.data().publicResourceHandle);
                        unloadOp = CEngineResult<>(EEngineStatus::Ok);
                    }
                }
            }
            catch(std::future_error const&fe)
            {
                CLog::Error(logTag(), CString::format("Failed to access future shared state. Error: %0", fe.what()));
            }

            EngineStatusPrintOnError(unloadOp.result(), logTag(), CString::format("Failed to create and/or enqueue resource destruction task."));
            return unloadOp;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<> CGFXAPIResourceBackend::loadImpl(
                typename TResource::CCreationRequest const &aRequest,
                ResolvedDependencyCollection_t       const &aResolvedDependencies,
                SDeferredResourceOperationHandle           &aOutHandle)
        {
            ResourceTaskFn_t task = nullptr;

            CEngineResult<> creationOp = mResourceTaskBackend->creationTask<TResource>(aRequest, aResolvedDependencies, task);
            if(not creationOp.successful())
            {
                CLog::Error(logTag(), CString::format("Failed to create build task for resource '%0'", "..."));
                return creationOp;
            }

            std::future<ResourceTaskFn_t::result_type> future{};
            CEngineResult<> enqueueing = enqueue(task, future);
            if(not enqueueing.successful())
            {
                CLog::Error(logTag(), CString::format("Failed to enqueue resource creation task for resource '%0'", "..."));
                return enqueueing;
            }

            aOutHandle.futureHandle = std::move(future);

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<> CGFXAPIResourceBackend::unloadImpl(
                typename TResource::CDestructionRequest const &aRequest,
                SGFXAPIResourceHandleAssignment         const &aAssignment,
                ResolvedDependencyCollection_t          const &aResolvedDependencies,
                SDeferredResourceOperationHandle              &aOutHandle)
        {
            ResourceTaskFn_t task = nullptr;

            CEngineResult<> destructionOp = mResourceTaskBackend->destructionTask<TResource>(aRequest, aAssignment, aResolvedDependencies, task);
            if(not destructionOp.successful())
            {
                CLog::Error(logTag(), CString::format("Failed to create destruction task for resource '%0'", "..."));
                return destructionOp;
            }

            std::future<ResourceTaskFn_t::result_type> future;
            CEngineResult<> enqueueing = enqueue(task, future);
            if(not enqueueing.successful())
            {
                CLog::Error(logTag(), CString::format("Failed to enqueue resource creation task for resource '%0'", "..."));
                return enqueueing;
            }

            aOutHandle.futureHandle = std::move(future);

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------
    }
}

#endif
