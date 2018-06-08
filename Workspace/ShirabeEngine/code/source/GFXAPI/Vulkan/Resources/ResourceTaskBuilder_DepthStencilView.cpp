#include "GFXAPI/Vulkan/Resources/ResourceTaskBackend.h"


namespace Engine {
  namespace Vulkan {

    using namespace Engine::Resources;
    using namespace Engine::DX::_11;

    EEngineStatus
      VulkanResourceTaskBackend::
      creationTask(
        DepthStencilView::CreationRequest   const&request,
        ResolvedDependencyCollection        const&resolvedDependencies,
        ResourceTaskFn_t                         &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }

    EEngineStatus
      VulkanResourceTaskBackend::
      updateTask(
        DepthStencilView::UpdateRequest     const&request,
        ResolvedDependencyCollection        const&resolvedDependencies,
        ResourceTaskFn_t                         &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }

    EEngineStatus
      VulkanResourceTaskBackend::
      destructionTask(
        DepthStencilView::DestructionRequest const&request,
        ResolvedDependencyCollection         const&resolvedDependencies,
        ResourceTaskFn_t                          &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }

    EEngineStatus
      VulkanResourceTaskBackend::
      queryTask(
        DepthStencilView::Query const&request,
        ResourceTaskFn_t             &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }


  }
}