#include "GFXAPI/Vulkan/Resources/ResourceTaskBackend.h"

namespace Engine {
  namespace Vulkan {
    using namespace Engine::Resources;

    EEngineStatus
      VulkanResourceTaskBackend::
      creationTask(
        DepthStencilState::CreationRequest  const&request,
        ResolvedDependencyCollection        const&resolvedDependencies,
        ResourceTaskFn_t                         &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }

    EEngineStatus
      VulkanResourceTaskBackend::
      updateTask(
        DepthStencilState::UpdateRequest    const&request,
        ResolvedDependencyCollection        const&resolvedDependencies,
        ResourceTaskFn_t                         &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }

    EEngineStatus
      VulkanResourceTaskBackend::
      destructionTask(
        DepthStencilState::DestructionRequest const&request,
        ResolvedDependencyCollection          const&resolvedDependencies,
        ResourceTaskFn_t                           &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }

    EEngineStatus
      VulkanResourceTaskBackend::
      queryTask(
        DepthStencilState::Query const&request,
        ResourceTaskFn_t              &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }


  }
}
