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
             * @brief CGFXAPIResourceBackend
             */
            CGFXAPIResourceBackend();

        public_methods:
            /**
             * @brief initialize
             * @return
             */
            bool initialize();
            /**
             * @brief deinitialize
             * @return
             */
            bool deinitialize();

            /**
             * @brief load
             * @param aRequest
             * @param aDependencies
             * @param aSyncMode
             * @param aCallback
             * @return
             */
            template <typename TResource>
            EEngineStatus load(
                    typename TResource::CreationRequest  const &aRequest,
                    PublicResourceIdList_t               const &aDependencies,
                    ETaskSynchronization                 const &aSyncMode,
                    CStdSharedPtr_t<IAsyncLoadCallback>  const &aCallback);

            /**
             * @brief unload
             * @param aRequest
             * @return
             */
            template <typename TResource>
            EEngineStatus unload(
                    typename TResource::CDestructionRequest const &aRequest);

            /**
             * @brief registerResource
             * @param aId
             * @param aResouce
             * @return
             */
            EEngineStatus registerResource(
                    PublicResourceId_t    const &aId,
                    CStdSharedPtr_t<void> const &aResouce);

            void setResourceTaskBackend(CStdSharedPtr_t<ResourceTaskBackend_t> const &aBackend);

        private_methods:
            /**
             * @brief loadImpl
             * @param aRequest
             * @param aResolvedDependencies
             * @param aOutHandle
             * @return
             */
            template <typename TResource>
            EEngineStatus loadImpl(
                    typename TResource::CreationRequest const &aRequest,
                    ResolvedDependencyCollection_t      const &aResolvedDependencies,
                    SDeferredResourceOperationHandle          &aOutHandle);

            /**
             * @brief unloadImpl
             * @param aRequest
             * @param aAssignment
             * @param aResolvedDependencies
             * @param aOutHandle
             * @return
             */
            template <typename TResource>
            EEngineStatus unloadImpl(
                    typename TResource::CDestructionRequest const &aRequest,
                    SGFXAPIResourceHandleAssignment        const &aAssignment,
                    ResolvedDependencyCollection_t         const &aResolvedDependencies,
                    SDeferredResourceOperationHandle             &aOutHandle);

            /**
             * @brief enqueue
             * @param aTask
             * @param aOutSharedFuture
             * @return
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
                typename TResource::CreationRequest  const &aRequest,
                PublicResourceIdList_t               const &aDependencies,
                ETaskSynchronization                 const &aSyncMode,
                CStdSharedPtr_t<IAsyncLoadCallback>  const &aCallback)
        {
            ResourceTaskFn_t::result_type    resourceHandle ={};
            SDeferredResourceOperationHandle handle         = {};

            EEngineStatus status = EEngineStatus::Ok;

            // Resolve dependencies...
            ResolvedDependencyCollection_t resolvedDependencies = {};
            for(PublicResourceIdList_t::value_type const &dependency : aDependencies)
                resolvedDependencies[dependency] = mStorage[dependency];

            try
            {
                status = loadImpl<TResource>(aRequest, resolvedDependencies, handle);
                if(!CheckEngineError(status))
                {
                    switch(aSyncMode)
                    {
                    default:
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
            ResourceTaskFn_t::result_type resourceHandle ={ };

            ResolvedDependencyCollection_t resolvedDependencies={}; // Guarding the public API by passing in this empty map.

            SDeferredResourceOperationHandle handle;

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
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        EEngineStatus CGFXAPIResourceBackend::loadImpl(
                typename TResource::CreationRequest const &aRequest,
                ResolvedDependencyCollection_t      const &aResolvedDependencies,
                SDeferredResourceOperationHandle          &aOutHandle)
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
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        EEngineStatus CGFXAPIResourceBackend::unloadImpl(
                typename TResource::CDestructionRequest const &aRequest,
                SGFXAPIResourceHandleAssignment        const &aAssignment,
                ResolvedDependencyCollection_t         const &aResolvedDependencies,
                SDeferredResourceOperationHandle             &aOutHandle)
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
        //<-----------------------------------------------------------------------------
    }
}

#endif
