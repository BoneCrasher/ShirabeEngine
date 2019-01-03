#ifndef __SHIRABE_GFXAPI_RESOURCE_TASK_BACKEND_H__
#define __SHIRABE_GFXAPI_RESOURCE_TASK_BACKEND_H__

#include <functional>
#include <map>
#include <typeindex>
#include <typeinfo>

#include <type_traits>
#include <typeinfo>
#include <optional>
#include <stdint.h>
#include <functional>
#include <future>
#include <assert.h>
#include <map>

#include <core/enginetypehelper.h>
#include <core/enginestatus.h>
#include <core/patterns/observer.h>
#include <core/threading/looper.h>

#include <resources/core/resourcedomaintransfer.h>
#include "graphicsapi/resources/gfxapi.h"
#include "graphicsapi/resources/types/all.h"

namespace engine
{
    namespace gfxapi
    {
        using namespace resources;

        /**
         * The ETaskSynchronization enum describes how resource tasks should be dispatched.
         */
        enum class ETaskSynchronization
        {
            Async = 1,
            Sync  = 2
        };

        /**
         * The EResourceTaskType enum describes the type of task to be created and dispatched
         * for a resource.
         */
        enum class EResourceTaskType
        {
            Create  = 1,
            Query   = 2, // = Read Info
            Update  = 4,
            Destroy = 8
        };

        /**
         * Describes the structure of a resource task function.
         */
        using ResourceTaskFn_t = std::function<SGFXAPIResourceHandleAssignment()>;

        /**
         * Describes the structure of a collection of resolved resource dependencies.
         */
        using ResolvedDependencyCollection_t = Map<PublicResourceId_t, CStdSharedPtr_t<void>>;

        /**
         * The CGFXAPIResourceTaskBackendModule describes the basic interface for task creator modules.
         *
         * @tparam T The resource type for which a task creator should be declared/defined.
         */
        template <typename T>
        class IGFXAPIResourceTaskBackendModule
        {
        public:
            /**
             * Spawn a creation task for a given request and it's dependencies.
             *
             * @param aRequest      Creation request for a specific resource.
             * @param aDependencies List of resolved dependencies of the resource to be created.
             * @param aOutTask      A task instance to be executed.
             * @return              EEngineStatus::Ok, if successful. Error flags otherwise.
             */
            virtual CEngineResult<> creationTask(
                    typename T::CCreationRequest   const&aRequest,
                    ResolvedDependencyCollection_t const&aDependencies,
                    ResourceTaskFn_t                    &aOutTask) = 0;

            /**
             * Spawn an update task for a given request and it's dependencies.
             *
             * @param aRequest      Creation request for a specific resource.
             * @param aDependencies List of resolved dependencies of the resource to be created.
             * @param aOutTask      A task instance to be executed.
             * @return              EEngineStatus::Ok, if successful. Error flags otherwise.
             */
            virtual CEngineResult<> updateTask(
                    typename T::CUpdateRequest     const&aRequest,
                    ResolvedDependencyCollection_t const&aDependencies,
                    ResourceTaskFn_t                    &aOutTask) = 0;

            /**
             * Spawn an update task for a given request and it's dependencies.
             *
             * @param aRequest      Creation request for a specific resource.
             * @param aDependencies List of resolved dependencies of the resource to be created.
             * @param aOutTask      A task instance to be executed.
             * @return              EEngineStatus::Ok, if successful. Error flags otherwise.
             * @return
             */
            virtual CEngineResult<> destructionTask(
                    typename T::CDestructionRequest const&aRequest,
                    ResolvedDependencyCollection_t  const&aDependencies,
                    ResourceTaskFn_t                     &aOutTask) = 0;

            /**
             * Spawn a query task for a given request.
             *
             * @param aRequest      Creation request for a specific resource.
             * @param aOutTask      A task instance to be executed.
             * @return              EEngineStatus::Ok, if successful. Error flags otherwise.
             */
            virtual CEngineResult<> queryTask(
                    typename T::CQuery const &aRequest,
                    ResourceTaskFn_t         &aOutTask) = 0;
        };

        /**
         * The CGFXAPIResourceTaskBackend class implements a flexible container for resource
         * task creation for various resource types. Therefore, it supports registering
         * creator functions for CRUD-operations individually.
         */
        class CGFXAPIResourceTaskBackend
        {
        public_typedefs:
            /**
             * Type-alias describing the structure of a creator function.
             *
             * @tparam TResource The type of the resource to be created.
             */
            template <typename TResource>
            using CreatorFn_t = std::function<
                                    CEngineResult<>(
                                        typename TResource::CCreationRequest const &,
                                        ResolvedDependencyCollection_t       const &,
                                        ResourceTaskFn_t                           &)>;

            /**
             * Type-alias describing the structure of an updater function.
             *
             * @tparam TResource The type of the resource to be updated.
             */
            template <typename TResource>
            using UpdaterFn_t = std::function<
                                    CEngineResult<>(
                                        typename TResource::CUpdateRequest const &,
                                        SGFXAPIResourceHandleAssignment   const &,
                                        ResolvedDependencyCollection_t    const &,
                                        ResourceTaskFn_t                        &)>;

            /**
             * Type-alias describing the structure of a destructor function.
             *
             * @tparam TResource The type of the resource to be destroyed.
             */
            template <typename TResource>
            using DestructorFn_t = std::function<
                                    CEngineResult<>(
                                        typename TResource::CDestructionRequest const &,
                                        SGFXAPIResourceHandleAssignment        const &,
                                        ResolvedDependencyCollection_t         const &,
                                        ResourceTaskFn_t                             &)>;

            /**
             * Type-alias describing the structure of a query function.
             *
             * @tparam TResource The type of the resource to be queried.
             */
            template <typename TResource>
            using QueryFn_t = std::function<
                                    CEngineResult<>(
                                        typename TResource::CQuery      const &,
                                        SGFXAPIResourceHandleAssignment const &,
                                        ResourceTaskFn_t                      &)>;

        public_destructors:
            /**
             * Destroy and run...
             */
            virtual ~CGFXAPIResourceTaskBackend() = default;

        public_methods:
            /**
             * Templated function to spawn a creation task for a specific resource type.
             *
             * @tparam TResource    The type of the resource to be created.
             * @param aRequest      Resource creation request required for creation.
             * @param aDependencies Resolved list of dependencies of the resource to be created.
             * @param aOutTask      Task instance to be executed for creation.
             * @return              EEngineStatus::Ok, if successful. False otherwise.
             */
            template <typename TResource>
            CEngineResult<> creationTask(
                    typename TResource::CCreationRequest const &aRequest,
                    ResolvedDependencyCollection_t       const &aDependencies,
                    ResourceTaskFn_t                           &aOutTask);

            /**
             * Templated function to spawn a update task for a specific resource type.
             *
             * @tparam TResource    The type of the resource to be updated.
             * @param aAssignment   The currently available assignment from public resource id to backend resource.
             * @param aRequest      Resource update request required for the update.
             * @param aDependencies Resolved list of dependencies of the resource to be updated.
             * @param aOutTask      Task instance to be executed for the update.
             * @return              EEngineStatus::Ok, if successful. False otherwise.
             */
            template <typename TResource>
            CEngineResult<> updateTask(
                    typename TResource::CUpdateRequest const&aRequest,
                    SGFXAPIResourceHandleAssignment    const&aAssignment,
                    ResolvedDependencyCollection_t     const&aDependencies,
                    ResourceTaskFn_t                        &aOutTask);

            /**
             * Templated function to spawn a destruction task for a specific resource type.
             *
             * @tparam TResource    The type of the resource to be destroyed.
             * @param aAssignment   The currently available assignment from public resource id to backend resource.
             * @param aRequest      Resource destruction request required for the destruction.
             * @param aDependencies Resolved list of dependencies of the resource to be destroyed.
             * @param aOutTask      Task instance to be executed for the destruction.
             * @return              EEngineStatus::Ok, if successful. False otherwise.
             */
            template <typename TResource>
            CEngineResult<> destructionTask(
                    typename TResource::CDestructionRequest const &aRequest,
                    SGFXAPIResourceHandleAssignment         const &aAssignment,
                    ResolvedDependencyCollection_t          const &aDependencies,
                    ResourceTaskFn_t                              &aOutTask);

            /**
             * Templated function to spawn a query task for a specific resource type.
             *
             * @tparam TResource    The type of the resource to be queried.
             * @param aAssignment   The currently available assignment from public resource id to backend resource.
             * @param aRequest      Resource query request required for the query.
             * @return              EEngineStatus::Ok, if successful. False otherwise.
             */
            template <typename TResource>
            CEngineResult<> queryTask(
                    typename TResource::CQuery      const &aRequest,
                    SGFXAPIResourceHandleAssignment const &aAssignment,
                    ResourceTaskFn_t                      &aOutTask);

            /**
             * Initialize this task backend instance.
             *
             * @return EEngineStatus::Ok, if successful. False otherwise.
             */
            virtual CEngineResult<> initialize() = 0;

            /**
             * Deinitialize this task backend instance.
             *
             * @return EEngineStatus::Ok, if successful. False otherwise.
             */
            virtual CEngineResult<> deinitialize() = 0;

        protected_methods:
            template <typename TResource>
            CEngineResult<> addCreator(CreatorFn_t<TResource> const &aCreator);

            template <typename TResource>
            CEngineResult<> addUpdater(UpdaterFn_t<TResource> const &aUpdater);

            template <typename TResource>
            CEngineResult<> addDestructor(DestructorFn_t<TResource> const &aDestructor);

            template <typename TResource>
            CEngineResult<> addQuery(QueryFn_t<TResource> const &aQuery);

        private_members:
            Map<std::type_index, Any_t> mCreatorFunctions;
            Map<std::type_index, Any_t> mUpdateFunctions;
            Map<std::type_index, Any_t> mQueryFunctions;
            Map<std::type_index, Any_t> mDestructorFunctions;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource, typename TFunction>
        static CEngineResult<> addFunctor(TFunction const &aFunction, Map<std::type_index, Any_t> &aRegistry)
        {
            std::type_index const typeIndex = std::type_index(typeid(TResource));
            if(aRegistry.end() != aRegistry.find(typeIndex))
            {
                return { EEngineStatus::Error };
            }

            aRegistry[typeIndex] = aFunction;
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<> CGFXAPIResourceTaskBackend::addCreator(CreatorFn_t<TResource> const &aFunction)
        {
            return addFunctor<TResource, CreatorFn_t<TResource>>(aFunction, mCreatorFunctions);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<> CGFXAPIResourceTaskBackend::addUpdater(UpdaterFn_t<TResource> const &aFunction)
        {
            return addFunctor<TResource, UpdaterFn_t<TResource>>(aFunction, mUpdateFunctions);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<> CGFXAPIResourceTaskBackend::addDestructor(DestructorFn_t<TResource> const &aFunction)
        {
            return addFunctor<TResource, DestructorFn_t<TResource>>(aFunction, mDestructorFunctions);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<> CGFXAPIResourceTaskBackend::addQuery(QueryFn_t<TResource> const &aFunction)
        {
            return addFunctor<TResource, QueryFn_t<TResource>>(aFunction, mQueryFunctions);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<> CGFXAPIResourceTaskBackend::creationTask(
                typename TResource::CCreationRequest const &aRequest,
                ResolvedDependencyCollection_t       const &aDependencies,
                ResourceTaskFn_t                           &aOutTask)
        {
            std::type_index const typeIndex = std::type_index(typeid(TResource));
            if(mCreatorFunctions.end() == mCreatorFunctions.find(typeIndex))
            {
                return { EEngineStatus::ResourceTaskBackend_FunctionNotFound };
            }

            Any_t function = mCreatorFunctions.at(typeIndex);
            try
            {
                CreatorFn_t<TResource> f = std::any_cast<CreatorFn_t<TResource>>(function);
                CEngineResult<> const status = f(aRequest, aDependencies, aOutTask);

                return { status };
            }
            catch(std::bad_any_cast const &)
            {
                return { EEngineStatus::ResourceTaskBackend_FunctionTypeInvalid };
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<> CGFXAPIResourceTaskBackend::updateTask(
                typename TResource::CUpdateRequest const&aRequest,
                SGFXAPIResourceHandleAssignment    const&aAssignment,
                ResolvedDependencyCollection_t     const&aDependencies,
                ResourceTaskFn_t                        &aOutTask)
        {
            std::type_index const typeIndex = std::type_index(typeid(TResource));
            if(mUpdateFunctions.end() == mUpdateFunctions.find(typeIndex))
            {
                return { EEngineStatus::ResourceTaskBackend_FunctionNotFound };
            }

            Any_t function = mUpdateFunctions.at(typeIndex);
            try
            {
                UpdaterFn_t<TResource> f = std::any_cast<UpdaterFn_t<TResource>>(function);
                CEngineResult<> const status = f(aRequest, aAssignment, aDependencies, aOutTask);

                return {status};
            }
            catch(std::bad_any_cast const &)
            {
                return {EEngineStatus::ResourceTaskBackend_FunctionTypeInvalid};
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<> CGFXAPIResourceTaskBackend::destructionTask(
                typename TResource::CDestructionRequest const &aRequest,
                SGFXAPIResourceHandleAssignment         const &aAssignment,
                ResolvedDependencyCollection_t          const &aDependencies,
                ResourceTaskFn_t                              &aOutTask)
        {
            std::type_index const typeIndex = std::type_index(typeid(TResource));
            if(mDestructorFunctions.end() == mDestructorFunctions.find(typeIndex))
            {
                return {EEngineStatus::ResourceTaskBackend_FunctionNotFound};
            }

            Any_t function = mDestructorFunctions.at(typeIndex);
            try
            {
                DestructorFn_t<TResource> f = std::any_cast<DestructorFn_t<TResource>>(function);
                CEngineResult<> const status = f(aRequest, aAssignment, aDependencies, aOutTask);

                return {status};
            }
            catch(std::bad_any_cast const &)
            {
                return {EEngineStatus::ResourceTaskBackend_FunctionTypeInvalid};
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<> CGFXAPIResourceTaskBackend::queryTask(
                typename TResource::CQuery      const &aRequest,
                SGFXAPIResourceHandleAssignment const &aAssignment,
                ResourceTaskFn_t                      &aOutTask)
        {
            std::type_index const typeIndex = std::type_index(typeid(TResource));
            if(mQueryFunctions.end() == mQueryFunctions.find(typeIndex))
            {
                return {EEngineStatus::ResourceTaskBackend_FunctionNotFound};
            }

            Any_t function = mQueryFunctions.at(typeIndex);
            try
            {
                QueryFn_t<TResource> f = std::any_cast<QueryFn_t<TResource>>(function);
                CEngineResult<> const status = f(aRequest, aAssignment, aOutTask);

                return {status};
            }
            catch(std::bad_any_cast const&)
            {
                return {EEngineStatus::ResourceTaskBackend_FunctionTypeInvalid};
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------

        /**
         * @brief The DeferredResourceOperationHandle struct
         */
        struct SDeferredResourceOperationHandle
        {
        public_members:
            std::future<ResourceTaskFn_t::result_type> futureHandle;
        };
        //<-----------------------------------------------------------------------------
    }
}

#endif
