#ifndef __SHIRABE_GFXAPI_RESOURCE_TASK_BACKEND_H__
#define __SHIRABE_GFXAPI_RESOURCE_TASK_BACKEND_H__

#include <Functional>
#include <map>

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

#include "Resources/Core/ResourceDomainTransfer.h"
#include "Resources/Subsystems/GFXAPI/GFXAPI.h"

#include "Resources/Subsystems/GFXAPI/Types/All.h"

namespace Engine {
  namespace GFXAPI {

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

    using ResolvedDependencyCollection = std::map<GFXAPIResourceHandle_t, Ptr<void>>;

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

    template <typename... TSupportedTypes>
    class GFXAPIResourceTaskBackend
      : public GFXAPIResourceTaskBackendModule<TSupportedTypes>...
    {
    };

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