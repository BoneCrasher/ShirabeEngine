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

#include "resources/core/resourcedomaintransfer.h"
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
            virtual EEngineStatus creationTask(
                    typename T::CreationRequest    const&aRequest,
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
            virtual EEngineStatus updateTask(
                    typename T::UpdateRequest      const&aRequest,
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
            virtual EEngineStatus destructionTask(
                    typename T::DestructionRequest const&aRequest,
                    ResolvedDependencyCollection_t const&aDependencies,
                    ResourceTaskFn_t                    &aOutTask) = 0;

            /**
             * Spawn a query task for a given request.
             *
             * @param aRequest      Creation request for a specific resource.
             * @param aOutTask      A task instance to be executed.
             * @return              EEngineStatus::Ok, if successful. Error flags otherwise.
             */
            virtual EEngineStatus queryTask(
                    typename T::Query const &aRequest,
                    ResourceTaskFn_t        &aOutTask) = 0;
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
                                    EEngineStatus(
                                        typename TResource::CreationRequest const &,
                                        ResolvedDependencyCollection_t      const &,
                                        ResourceTaskFn_t                          &)>;

            /**
             * Type-alias describing the structure of an updater function.
             *
             * @tparam TResource The type of the resource to be updated.
             */
            template <typename TResource>
            using UpdaterFn_t = std::function<
                                    EEngineStatus(
                                        typename TResource::UpdateRequest const &,
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
                                    EEngineStatus(
                                        typename TResource::DestructionRequest const &,
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
                                    EEngineStatus(
                                        typename TResource::Query       const &,
                                        SGFXAPIResourceHandleAssignment const &,
                                        ResourceTaskFn_t                      &)>;

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
            EEngineStatus creationTask(
                    typename TResource::CreationRequest const &aRequest,
                    ResolvedDependencyCollection_t      const &aDependencies,
                    ResourceTaskFn_t                          &aOutTask);

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
            EEngineStatus updateTask(
                    typename TResource::UpdateRequest const&aRequest,
                    SGFXAPIResourceHandleAssignment   const&aAssignment,
                    ResolvedDependencyCollection_t    const&aDependencies,
                    ResourceTaskFn_t                       &aOutTask);

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
            EEngineStatus destructionTask(
                    typename TResource::DestructionRequest const &aRequest,
                    SGFXAPIResourceHandleAssignment        const &aAssignment,
                    ResolvedDependencyCollection_t         const &aDependencies,
                    ResourceTaskFn_t                             &aOutTask);

            /**
             * Templated function to spawn a query task for a specific resource type.
             *
             * @tparam TResource    The type of the resource to be queried.
             * @param aAssignment   The currently available assignment from public resource id to backend resource.
             * @param aRequest      Resource query request required for the query.
             * @return              EEngineStatus::Ok, if successful. False otherwise.
             */
            template <typename TResource>
            EEngineStatus queryTask(
                    typename TResource::Query       const &aRequest,
                    SGFXAPIResourceHandleAssignment const &aAssignment,
                    ResourceTaskFn_t                      &aOutTask);

        protected_methods:
            template <typename TResource>
            bool addCreator(CreatorFn_t<TResource> const &aCreator);

            template <typename TResource>
            bool addUpdater(UpdaterFn_t<TResource> const &aUpdater);

            template <typename TResource>
            bool addDestructor(DestructorFn_t<TResource> const &aDestructor);

            template <typename TResource>
            bool addQuery(QueryFn_t<TResource> const &aQuery);

        private_members:
            Map<std::type_index, Any> mCreatorFunctions;
            Map<std::type_index, Any> mUpdateFunctions;
            Map<std::type_index, Any> mQueryFunctions;
            Map<std::type_index, Any> mDestructorFunctions;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        bool CGFXAPIResourceTaskBackend::addCreator(CreatorFn_t<TResource> const &aFunction)
        {
            std::type_index const typeIndex = std::type_index(typeid(TResource));
            if(mCreatorFunctions.find(typeIndex) != mCreatorFunctions.end())
                return false;

            mCreatorFunctions[typeIndex] = aFunction;
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        bool CGFXAPIResourceTaskBackend::addUpdater(UpdaterFn_t<TResource> const &aFunction)
        {
            std::type_index const typeIndex = std::type_index(typeid(TResource));
            if(mUpdateFunctions.find(typeIndex) != mUpdateFunctions.end())
                return false;

            mUpdateFunctions[typeIndex] = aFunction;
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        bool CGFXAPIResourceTaskBackend::addDestructor(DestructorFn_t<TResource> const &aFunction)
        {
            std::type_index const typeIndex = std::type_index(typeid(TResource));
            if(mDestructorFunctions.find(typeIndex) != mDestructorFunctions.end())
                return false;

            mDestructorFunctions[typeIndex] = aFunction;
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        bool CGFXAPIResourceTaskBackend::addQuery(QueryFn_t<TResource> const &aFunction)
        {
            std::type_index const typeIndex = std::type_index(typeid(TResource));
            if(mQueryFunctions.find(typeIndex) != mQueryFunctions.end())
                return false;

            mQueryFunctions[typeIndex] = aFunction;
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        EEngineStatus CGFXAPIResourceTaskBackend::creationTask(
                typename TResource::CreationRequest const &aRequest,
                ResolvedDependencyCollection_t      const &aDependencies,
                ResourceTaskFn_t                          &aOutTask)
        {
            std::type_index const typeIndex = std::type_index(typeid(TResource));
            if(mCreatorFunctions.find(typeIndex) == mCreatorFunctions.end())
                return EEngineStatus::ResourceTaskBackend_FunctionNotFound;

            Any function = mCreatorFunctions.at(typeIndex);
            try
            {
                CreatorFn_t<TResource> f = std::any_cast<CreatorFn_t<TResource>>(function);
                EEngineStatus const status = f(aRequest, aDependencies, aOutTask);

                return status;
            }
            catch(std::bad_any_cast const &)
            {
                return EEngineStatus::ResourceTaskBackend_FunctionTypeInvalid;
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        EEngineStatus CGFXAPIResourceTaskBackend::updateTask(
                typename TResource::UpdateRequest const&aRequest,
                SGFXAPIResourceHandleAssignment   const&aAssignment,
                ResolvedDependencyCollection_t    const&aDependencies,
                ResourceTaskFn_t                       &aOutTask)
        {
            std::type_index const typeIndex = std::type_index(typeid(TResource));
            if(mUpdateFunctions.find(typeIndex) == mUpdateFunctions.end())
                return EEngineStatus::ResourceTaskBackend_FunctionNotFound;

            Any function = mUpdateFunctions.at(typeIndex);
            try
            {
                UpdaterFn_t<TResource> f = std::any_cast<UpdaterFn_t<TResource>>(function);
                EEngineStatus const status = f(aRequest, aAssignment, aDependencies, aOutTask);

                return status;
            }
            catch(std::bad_any_cast const &)
            {
                return EEngineStatus::ResourceTaskBackend_FunctionTypeInvalid;
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        EEngineStatus CGFXAPIResourceTaskBackend::destructionTask(
                typename TResource::DestructionRequest const &aRequest,
                SGFXAPIResourceHandleAssignment        const &aAssignment,
                ResolvedDependencyCollection_t         const &aDependencies,
                ResourceTaskFn_t                             &aOutTask)
        {
            std::type_index const typeIndex = std::type_index(typeid(TResource));
            if(mDestructorFunctions.find(typeIndex) == mDestructorFunctions.end())
                return EEngineStatus::ResourceTaskBackend_FunctionNotFound;

            Any function = mDestructorFunctions.at(typeIndex);
            try
            {
                DestructorFn_t<TResource> f = std::any_cast<DestructorFn_t<TResource>>(function);
                EEngineStatus const status = f(aRequest, aAssignment, aDependencies, aOutTask);

                return status;
            }
            catch(std::bad_any_cast const &)
            {
                return EEngineStatus::ResourceTaskBackend_FunctionTypeInvalid;
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        EEngineStatus CGFXAPIResourceTaskBackend::queryTask(
                typename TResource::Query       const &aRequest,
                SGFXAPIResourceHandleAssignment const &aAssignment,
                ResourceTaskFn_t                      &aOutTask)
        {
            std::type_index const typeIndex = std::type_index(typeid(TResource));
            if(mQueryFunctions.find(typeIndex) == mQueryFunctions.end())
                return EEngineStatus::ResourceTaskBackend_FunctionNotFound;

            Any function = mQueryFunctions.at(typeIndex);
            try
            {
                QueryFn_t<TResource> f = std::any_cast<QueryFn_t<TResource>>(function);
                EEngineStatus const status = f(aRequest, aAssignment, aOutTask);

                return status;
            }
            catch(std::bad_any_cast const&)
            {
                return EEngineStatus::ResourceTaskBackend_FunctionTypeInvalid;
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
