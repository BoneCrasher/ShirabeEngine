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

#include "Core/EngineTypeHelper.h"
#include "Core/EngineStatus.h"
#include "Core/IOC/Observer.h"
#include "Core/Threading/Looper.h"

#include "Resources/Core/ResourceDomainTransfer.h"
#include "GraphicsAPI/Resources/GFXAPI.h"
#include "GraphicsAPI/Resources/Types/All.h"

namespace Engine {
  namespace GFXAPI {
    using namespace Resources;

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

    using ResolvedDependencyCollection = std::map<PublicResourceId_t, Ptr<void>>;

    template <typename T>
    class GFXAPIResourceTaskBackendModule
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

    class GFXAPIResourceTaskBackend
    {
      template <typename TResource>
      using CreatorFn_t = std::function<EEngineStatus(typename TResource::CreationRequest const&, ResolvedDependencyCollection const&, ResourceTaskFn_t&)>;

      template <typename TResource>
      using UpdaterFn_t = std::function<EEngineStatus(typename TResource::UpdateRequest const&, GFXAPIResourceHandleAssignment const&, ResolvedDependencyCollection const&, ResourceTaskFn_t&)>;

      template <typename TResource>
      using DestructorFn_t = std::function<EEngineStatus(typename TResource::DestructionRequest const&, GFXAPIResourceHandleAssignment const&, ResolvedDependencyCollection const&, ResourceTaskFn_t&)>;

      template <typename TResource>
      using QueryFn_t = std::function<EEngineStatus(typename TResource::Query const&, GFXAPIResourceHandleAssignment const&, ResourceTaskFn_t&)>;

    public:
      template <typename TResource>
      EEngineStatus creationTask(
        typename TResource::CreationRequest const&inRequest,
        ResolvedDependencyCollection        const&inDependencies,
        ResourceTaskFn_t                         &outTask);

      template <typename TResource>
      EEngineStatus updateTask(
        typename TResource::UpdateRequest const&inRequest,
        GFXAPIResourceHandleAssignment    const&assignment,
        ResolvedDependencyCollection      const&inDependencies,
        ResourceTaskFn_t                       &outTask);

      template <typename TResource>
      EEngineStatus destructionTask(
        typename TResource::DestructionRequest const&inRequest,
        GFXAPIResourceHandleAssignment         const&assignment,
        ResolvedDependencyCollection           const&inDependencies,
        ResourceTaskFn_t                            &outTask);

      template <typename TResource>
      EEngineStatus queryTask(
        typename TResource::Query const&inRequest,
        ResourceTaskFn_t               &outTask);

    protected:
      template <typename TResource>
      bool addCreator(CreatorFn_t<TResource> const&fn);

      template <typename TResource>
      bool addUpdater(UpdaterFn_t<TResource> const&fn);

      template <typename TResource>
      bool addDestructor(DestructorFn_t<TResource> const&fn);

      template <typename TResource>
      bool addQuery(QueryFn_t<TResource> const&fn);

    private:
      Map<std::type_index, Any> m_creatorFn;
      Map<std::type_index, Any> m_updateFn;
      Map<std::type_index, Any> m_queryFn;
      Map<std::type_index, Any> m_destroyFn;
    };
    
    template <typename TResource>
    bool GFXAPIResourceTaskBackend::addCreator(CreatorFn_t<TResource> const&fn)
    {
      std::type_index tindex = std::type_index(typeid(TResource));
      if(m_creatorFn.find(tindex) != m_creatorFn.end())
        return false;

      m_creatorFn[tindex] = fn;
      return true;
    }

    template <typename TResource>
    bool GFXAPIResourceTaskBackend::addUpdater(UpdaterFn_t<TResource> const&fn)
    {
      std::type_index tindex = std::type_index(typeid(TResource));
      if(m_updateFn.find(tindex) != m_updateFn.end())
        return false;

      m_updateFn[tindex] = fn;
      return true;
    }
    
    template <typename TResource>
    bool GFXAPIResourceTaskBackend::addDestructor(DestructorFn_t<TResource> const&fn)
    {
      std::type_index tindex = std::type_index(typeid(TResource));
      if(m_destroyFn.find(tindex) != m_destroyFn.end())
        return false;

      m_destroyFn[tindex] = fn;
      return true;
    }
    
    template <typename TResource>
    bool GFXAPIResourceTaskBackend::addQuery(QueryFn_t<TResource> const&fn)
    {
      std::type_index tindex = std::type_index(typeid(TResource));
      if(m_queryFn.find(tindex) != m_queryFn.end())
        return false;

      m_queryFn[tindex] = fn;
      return true;
    }

    template <typename TResource>
    EEngineStatus
      GFXAPIResourceTaskBackend::creationTask(
        typename TResource::CreationRequest const&inRequest,
        ResolvedDependencyCollection        const&inDependencies,
        ResourceTaskFn_t                         &outTask)
    {
      std::type_index tindex = std::type_index(typeid(TResource));
      if(m_creatorFn.find(tindex) == m_creatorFn.end())
        return EEngineStatus::ResourceTaskBackend_FunctionNotFound;

      Any fn = m_creatorFn.at(tindex);
      try {
        CreatorFn_t<TResource> f = std::any_cast<CreatorFn_t<TResource>>(fn);
        return f(inRequest, inDependencies, outTask);
      }
      catch(std::bad_any_cast const&) {
        return EEngineStatus::ResourceTaskBackend_FunctionTypeInvalid;
      }
    }

    template <typename TResource>
    EEngineStatus
      GFXAPIResourceTaskBackend::updateTask(
        typename TResource::UpdateRequest const&inRequest,
        GFXAPIResourceHandleAssignment    const&assignment,
        ResolvedDependencyCollection      const&inDependencies,
        ResourceTaskFn_t                       &outTask)
    {
      std::type_index tindex = std::type_index(typeid(TResource));
      if(m_updateFn.find(tindex) == m_updateFn.end())
        return EEngineStatus::ResourceTaskBackend_FunctionNotFound;

      Any fn = m_updateFn.at(tindex);
      try {
        UpdaterFn_t<TResource> f = std::any_cast<UpdaterFn_t<TResource>>(fn);
        return f(inRequest, assignment, inDependencies, outTask);
      }
      catch(std::bad_any_cast const&) {
        return EEngineStatus::ResourceTaskBackend_FunctionTypeInvalid;
      }
    }

    template <typename TResource>
    EEngineStatus
      GFXAPIResourceTaskBackend::destructionTask(
        typename TResource::DestructionRequest const&inRequest,
        GFXAPIResourceHandleAssignment         const&assignment,
        ResolvedDependencyCollection           const&inDependencies,
        ResourceTaskFn_t                            &outTask)
    {
      std::type_index tindex = std::type_index(typeid(TResource));
      if(m_destroyFn.find(tindex) == m_destroyFn.end())
        return EEngineStatus::ResourceTaskBackend_FunctionNotFound;

      Any fn = m_destroyFn.at(tindex);
      try {
        DestructorFn_t<TResource> f = std::any_cast<DestructorFn_t<TResource>>(fn);
        return f(inRequest, assignment, inDependencies, outTask);
      }
      catch(std::bad_any_cast const&) {
        return EEngineStatus::ResourceTaskBackend_FunctionTypeInvalid;
      }
    }

    template <typename TResource>
    EEngineStatus
      GFXAPIResourceTaskBackend::queryTask(
        typename TResource::Query const&inRequest,
        ResourceTaskFn_t               &outTask)
    {
      std::type_index tindex = std::type_index(typeid(TResource));
      if(m_queryFn.find(tindex) == m_queryFn.end())
        return EEngineStatus::ResourceTaskBackend_FunctionNotFound;

      Any fn = m_queryFn.at(tindex);
      try {
        QueryFn_t<TResource> f = std::any_cast<QueryFn_t<TResource>>(fn);
        return f(inRequest, inDependencies, outTask);
      }
      catch(std::bad_any_cast const&) {
        return EEngineStatus::ResourceTaskBackend_FunctionTypeInvalid;
      }
    }

    /**********************************************************************************************//**
      * \struct	DeferredResourceOperationHandle
      *
      * \brief	Handle class to hold an asynchronous request.
      **************************************************************************************************/
    struct DeferredResourceOperationHandle {
      std::future<ResourceTaskFn_t::result_type> futureHandle;
    };

  }
}

#endif