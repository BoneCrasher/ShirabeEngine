#include "GFXAPI/DirectX/DX11/DX11ResourceTaskBuilder.h"


namespace Engine {
  namespace GFXAPI {
    using namespace Engine::Resources;
    using namespace Engine::DX::_11;

    EEngineStatus
      GFXAPIResourceTaskBackendImpl<RasterizerState>::
      creationTask(
        RasterizerState::CreationRequest  const&request,
        ResolvedDependencyCollection      const&resolvedDependencies,
        ResourceTaskFn_t                       &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }

    EEngineStatus
      GFXAPIResourceTaskBackendImpl<RasterizerState>::
      updateTask(
        RasterizerState::UpdateRequest    const&request,
        ResolvedDependencyCollection      const&resolvedDependencies,
        ResourceTaskFn_t                       &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }

    EEngineStatus
      GFXAPIResourceTaskBackendImpl<RasterizerState>::
      destructionTask(
        RasterizerState::DestructionRequest const&request,
        ResolvedDependencyCollection        const&resolvedDependencies,
        ResourceTaskFn_t                         &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }

    EEngineStatus
      GFXAPIResourceTaskBackendImpl<RasterizerState>::
      queryTask(
        RasterizerState::Query const&request,
        ResourceTaskFn_t            &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }

  }
}
