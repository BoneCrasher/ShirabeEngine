#include "GFXAPI/DirectX/DX11/DX11ResourceTaskBuilder.h"

namespace Engine {
  namespace GFXAPI {
    using namespace Engine::Resources;
    using namespace Engine::DX::_11;

    EEngineStatus
      GFXAPIResourceTaskBackendImpl<DepthStencilState>::
      creationTask(
        DepthStencilState::CreationRequest  const&request,
        ResolvedDependencyCollection        const&resolvedDependencies,
        ResourceTaskFn_t                         &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }

    EEngineStatus
      GFXAPIResourceTaskBackendImpl<DepthStencilState>::
      updateTask(
        DepthStencilState::UpdateRequest    const&request,
        ResolvedDependencyCollection        const&resolvedDependencies,
        ResourceTaskFn_t                         &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }

    EEngineStatus
      GFXAPIResourceTaskBackendImpl<DepthStencilState>::
      destructionTask(
        DepthStencilState::DestructionRequest const&request,
        ResolvedDependencyCollection          const&resolvedDependencies,
        ResourceTaskFn_t                           &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }

    EEngineStatus
      GFXAPIResourceTaskBackendImpl<DepthStencilState>::
      queryTask(
        DepthStencilState::Query const&request,
        ResourceTaskFn_t              &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }

  }
}
